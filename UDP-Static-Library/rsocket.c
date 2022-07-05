/*
	NETWORKS LAB 4
	FILE TRANSFER USING SOCKETS
	HAASITA PINNEPU	19CS30021
	MAJJI DEEPIKA 19CS30027
*/

#include "rsocket.h"


//Dropping message
int dropMessage(float p)
{
	int n=rand()%1000;
	if( (n/1000.0)<p )
		return 1;
	else 
		return 0;
}


// defining datatype for message
typedef struct message
{
	int counter;
	int flag;
	char buffer[100];
	struct sockaddr* dest_addr;
	socklen_t addrlen;
	size_t length;
	time_t time;
} msg;


//content of received message
typedef struct received_message
{
	int counter;
	struct sockaddr_in addr;
} recv_msg;


//content of received buffer
typedef struct received_buffer
{
	char buffer[100];
	struct sockaddr_in addr;
	socklen_t clilen;
	int len;
} recv_buf;



// GLOBAL VARIABLES REQUIRED

msg*  unack_msg_table;
recv_msg* recv_msg_table;
recv_buf* recv_buffer;


//threads
pthread_t R;	//takes care of received message table
pthread_t S;	//checks the unacknowlwdged message table

//mutex lockers
pthread_mutex_t lock_unack_count, lock_recv_buf_count, lock_try_sent_counter; 
pthread_mutex_t lock_unack_msg_table, lock_recv_buffer;

int recv_num;	//no.of messages received
int recv_buf_num;	//no.of messages in the receive buffer
int unack_num;		//no.of unacknowledged sent messages
int send_count;	//header counter
int try_sending_counter;	//no.of all attempts to send
int SockFd;

char* buffer_complete;

int sockfd;
void HandleAckMsgRecv(char *buf)
{
	int this_counter, there=0;
	memcpy(&this_counter, buf + sizeof(int), sizeof(int));
	int i;
	for ( i = 0; i < unack_num; i++)
	{
		int counter_in_ack = unack_msg_table[i].counter;
		if (counter_in_ack== this_counter)
		{
			there = 1;
			break;
		}
	}
	
	if(there)
	{
		pthread_mutex_lock(&lock_unack_msg_table);
		
		int j;
		for ( j = i; j < unack_num - 1; j++)
			unack_msg_table[j] = unack_msg_table[j + 1];
		pthread_mutex_unlock(&lock_unack_msg_table);
		pthread_mutex_lock(&lock_unack_count);
		unack_num--;
		pthread_mutex_unlock(&lock_unack_count);
	}	
}

// Sending Acknowledgment
void sendAck(int i, int header, int sockfd)
{
    char buffT[2* sizeof(int)];
	int pd = 1234;
	memcpy(buffT, &pd, sizeof(int));
	memcpy(buffT + sizeof(int), &header, sizeof(int));
	sendto(sockfd, buffT, 2 * sizeof(int), 0, (struct sockaddr *) &recv_msg_table[i].addr, sizeof(recv_msg_table[i].addr));
}

void HandleAppMsgRecv(char *buf, int n, struct sockaddr_in addr, socklen_t clilen, int sockfd, int header)
{
	int i;
	int there=0;
	
	for (i = 0; i < recv_num; i++)
	{
		if (recv_msg_table[i].counter == header)
		{
			there = 1;
			recv_msg_table[i].addr = addr;
			break;
		}
	}
	
	if (there == 0)
	{
		pthread_mutex_lock(&lock_recv_buffer);
		
		memcpy(recv_buffer[recv_buf_num].buffer, buf + sizeof(int), n - sizeof(int));
		recv_buffer[recv_buf_num].len = n - sizeof(int);
		recv_buffer[recv_buf_num].addr = addr;
		recv_buffer[recv_buf_num].clilen = clilen;
		pthread_mutex_unlock(&lock_recv_buffer);
		pthread_mutex_lock(&lock_recv_buf_count);
		recv_buf_num++;
		pthread_mutex_unlock(&lock_recv_buf_count);
		recv_msg_table[recv_num].counter = header;
		recv_msg_table[recv_num].addr = addr;
		recv_num++;
	}
	sendAck(i, header, sockfd);	
}


void HandleReceive(int sockfd)
{
	char buf[BUFFER_SIZE];
    	memset(buf, '\0', sizeof buf);
    	
    	struct sockaddr_in cliaddr;

    	socklen_t addr_len = sizeof(cliaddr);
    	int n = recvfrom(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &addr_len);
    	
    	if (n < 0)
	{
		perror("Error in receipt");
		exit(EXIT_FAILURE);
	}
	
	buf[n] = '\0';
    	if (dropMessage(DROP_PROBALITY))
        	return ;
        
       int header;
	memcpy(&header, buf, sizeof(int));
	
	int clilen = sizeof(cliaddr);
	
	if (header > 50) 
		HandleAckMsgRecv(buf);	
	else
		HandleAppMsgRecv(buf,n, cliaddr,  clilen, sockfd, header);
}

void HandleRetransmission(int sockfd)
{
	if(unack_num>0)
	{
		for(int i=0;i<unack_num; i++)
		{
			msg this=unack_msg_table[i];
			time_t time_now=time(NULL);
			
			if ((int)(time_now- this.time) >= T)
			{
				unack_msg_table[i].time = time_now;
				char buffer_complete[sizeof(int) + 100];
				memcpy(buffer_complete, &(this.counter), sizeof(int));
				memcpy(buffer_complete + sizeof(int), this.buffer, this.length);
				sendto(sockfd, buffer_complete, sizeof(int) + this.length, this.flag, this.dest_addr, this.addrlen);
				pthread_mutex_lock(&lock_try_sent_counter);
				try_sending_counter++;
				pthread_mutex_unlock(&lock_try_sent_counter);
				printf("Retransmitted message-%s\n", this.buffer);
				
			}
		}
	}
}
void *thread_R(void* param)
{
	int s_fd=*((int *) param);
	fd_set sock;
	
	struct timeval tv;
	tv.tv_sec = T;
	
	while(1)
	{
		int r_select = select(s_fd + 1, &sock, NULL, NULL, &tv);
			if (r_select < 0)
			{
			    perror("Selection Failed\n");
			}
			else if (r_select)
			{
			    if (FD_ISSET(s_fd, &sock))
			    { 
				    HandleReceive(s_fd);
			    }
			}
			else
			{
			    HandleRetransmission(s_fd);
			}
	}
}

/*
void *thread_S(void *par)
{
	
}
*/


// sending data via MRP Socket
int r_sendto(int sockfd, const void* buf_here, size_t len, int flag,const struct sockaddr* dest_addr, socklen_t addrlen)
{
	int counter = 0;
	int buf_size = BUFFER_SIZE;
	
	
	while(1)
	{
		if ((int)len <= 0)
			break;
		int stat, amt;
		if (len > buf_size)
			amt = buf_size;
		else
			amt = len;
		memcpy(buffer_complete, &send_count, sizeof(int));	// Add header
		memcpy(buffer_complete + sizeof(int), (char *) (buf_here + (counter * buf_size)), amt); // Add content
		pthread_mutex_lock(&lock_unack_msg_table);
		unack_msg_table[unack_num].time = time(NULL);
		unack_msg_table[unack_num].counter = send_count;
		memcpy(unack_msg_table[unack_num].buffer, buf_here + (counter * buf_size), amt);
		unack_msg_table[unack_num].length = amt;
		unack_msg_table[unack_num].flag = flag;
		unack_msg_table[unack_num].dest_addr = dest_addr;
		unack_msg_table[unack_num].addrlen = addrlen;
		pthread_mutex_unlock(&lock_unack_msg_table);
		pthread_mutex_lock(&lock_unack_count);
		unack_num++;
		pthread_mutex_unlock(&lock_unack_count);
		send_count++;
		stat = sendto(sockfd, buffer_complete, sizeof(int) + amt, flag, dest_addr, addrlen);
		printf("Transmitted %s\n", buffer_complete + sizeof(int));
		pthread_mutex_lock(&lock_try_sent_counter);
		try_sending_counter++;
		pthread_mutex_unlock(&lock_try_sent_counter);
		len -= buf_size;
		
		if (stat < 0)
		{
			unack_num--;
			send_count--;
			return stat;
		}
		counter+=1;
	}
	return 0;
}

int r_recvfrom(int sockfd, char *buf, size_t lenCurr, int flag, const struct  sockaddr * addr, socklen_t* addrlen)
{
    if (sockfd != SockFd)
		return -1;
    int len = (int)lenCurr;
    if (flag != MSG_PEEK && flag != 0)
		return -1;
    while (recv_buf_num == 0)
		sleep(1);
    int toReturn;

    if (recv_buffer[0].len <= len)
		toReturn = recv_buffer[0].len;
	else
		toReturn = len;
    
    memcpy(buf, recv_buffer[0].buffer, toReturn);	// Retrieve content
	memcpy(addr, (struct sockaddr *) &recv_buffer[0].addr, sizeof(recv_buffer[0].addr)); // Retrieve address
	memcpy(addrlen, &recv_buffer[0].clilen, sizeof(socklen_t));

    if (flag != MSG_PEEK)
	{
		pthread_mutex_lock(&lock_recv_buffer);
		for (int k = 0; k < recv_buf_num - 1; k++)
			recv_buffer[k] = recv_buffer[k + 1];
		pthread_mutex_unlock(&lock_recv_buffer);
		pthread_mutex_lock(&lock_recv_buf_count);
		recv_buf_num--;
		pthread_mutex_unlock(&lock_recv_buf_count);
	}
	return toReturn;  
}


int r_bind(int sockfd, socklen_t addrlen, const struct sockaddr* servaddr)
{
	return bind(sockfd, servaddr, addrlen);
}

	
int r_socket( int domain, int type, int protocol)
{
	if(type!=SOCK_MRP)
		return -1;
	
	srand(time(NULL));
	
	
	if ((sockfd = socket(domain, SOCK_DGRAM, protocol)) < 0)
        	return sockfd;
        
        int *arg = malloc(sizeof(*arg));
	*arg = sockfd;
	
	//creating R and S threads
	pthread_create(&R, NULL, thread_R, arg);
	//pthread_create(&S, NULL, thread_S, arg);
	
	unack_msg_table	=(msg*)malloc(TABLE_SIZE*sizeof(msg));
	recv_msg_table=(recv_msg *)malloc(TABLE_SIZE*sizeof(recv_msg));
	recv_buffer= (recv_buf *) malloc(BUFFER_SIZE * sizeof(recv_buffer));
	buffer_complete=(char *) malloc(sizeof(int) + (BUFFER_SIZE*sizeof(char)));
	
	// Initialise counters to zero
	recv_buf_num = 0;	
	unack_num = 0;
	recv_num = 0;
	send_count = 0;
	try_sending_counter = 0;
	
	SockFd = sockfd;
	
	if(pthread_mutex_init(&lock_recv_buf_count, NULL)!=0)
	{
		perror("Error in mutex init");
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_init(&lock_recv_buffer, NULL)!=0)
	{
		perror("Error in mutex init");
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_init(&lock_try_sent_counter, NULL)!=0)
	{
		perror("Error in mutex init");
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_init(&lock_unack_msg_table, NULL)!=0)
	{
		perror("Error in mutex init");
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_init(&lock_unack_count, NULL)!=0)
	{
		perror("Error in mutex init");
		exit(EXIT_FAILURE);
	}
	
	
	return sockfd;
}

//r _close for closing the socket
int r_close(int sockfd)		
{
	while(unack_num);
	
	// Freeing dynamically allocated memory
	free(unack_msg_table);	
	free(recv_msg_table);
	free(recv_buffer);
	
	//closing threads
	pthread_cancel(R);
	pthread_cancel(S);		
	close(sockfd);
	
	pthread_mutex_destroy(&lock_recv_buffer);
	pthread_mutex_destroy(&lock_unack_count);
	pthread_mutex_destroy(&lock_recv_buf_count);
	pthread_mutex_destroy(&lock_try_sent_counter);
	pthread_mutex_destroy(&lock_unack_msg_table);
	
	printf("\nTotal no of sends: %d\n", try_sending_counter);
	return 0;
}

int main()
{
	return 0;
}