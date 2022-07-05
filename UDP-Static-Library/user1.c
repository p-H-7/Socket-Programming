#include "rsocket.h"

#define PORTX 50000+(2*0021)
#define MAXLINE 100 

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buff[MAXLINE];

    sockfd = r_socket(AF_INET, SOCK_MRP, 0);

    if (sockfd < 0) 
	{
		perror("Sorry, Socket cannot be created\n");
		exit(0);
	}

    printf("Please Enter the Input String: \n");
    scanf("%s",buff);
    memset(&serv_addr, 0, sizeof(serv_addr)); 

    serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		    = htons(PORTX);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	{
		perror("Unable to reuse the port.\n");
		exit(EXIT_FAILURE);
	}

    if (r_bind(sockfd, (const struct sockaddr *)&serv_addr,  sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address.\n");
		exit(0);
	}

    //printf("!Serving Running!\n");

    for(int i = 0; i<strlen(buff); i++)
    { 
        r_sendto(sockfd, (const char *)(buff+i), 1, 0,(const struct sockaddr *) &serv_addr, sizeof(serv_addr));
    }

    r_close(sockfd); 
    return 0; 
}