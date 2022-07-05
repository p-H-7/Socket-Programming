/*
    FTP server
    Assignment 3

    Haasita Pinnepu 19CS30021
    Majji Deepika 19CS30027
*/

// Assumption: if any wrong command input is given which is not in the list, the server returns an error code of 650.

#include <stdio.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<limits.h>
#include<dirent.h>
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

#define BUF_SIZE 8

// #define PORTX 70000

long int PORTX,PORTY;


int main()
{
	int			sockfd, newsockfd, sockfd_get, sockfd_put ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;
	char buffer[100];	
    char cmd[200];

    char cmdsplit[20][20] = {"\0"};
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd< 0) 
	{
		perror("sorry....Socket cannot be created\n");
		exit(0);
	}
	
	printf(" Enter the port number(Same as client): ");
	scanf("%ld",&PORTX);
	printf("\n");
	
	if(PORTX<20000 || PORTX>65535)
	{
		perror("Invalid Port number\n");
		exit(0);
	}
	
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(PORTX);
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	{
		perror("Unable to reuse the port......");
		exit(EXIT_FAILURE);
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address......");
		exit(0);
	} 
	
	printf("!Serving Running!\n");
    printf("!!Press ctrl+c to exit!!\n");
	
	listen(sockfd, 5);

    char user[36];
    char pass[36];
	
	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen) ;

		if (newsockfd < 0) 
		{
			perror("Accept error\n");
			exit(0);
		}
		
		printf("Connection established..\n");
		//send(newsockfd, buffer, strlen(buffer) + 1, 0);

        int flag = 1; //first flag
        int flag_2 = 1;
        //int flag_pass = 1;
        int return_code[] = {0};

        while(1)
        {
		
            recv(newsockfd, cmd, 100, 0);
            printf("Command from Client: %s\n", cmd);

            int j=0; int ctr=0;
            for(int pos=0;pos<=(strlen(cmd));pos++)
            {
                // if space or NULL found, assign NULL into newString[ctr]
                if(cmd[pos]==' '|| cmd[pos]=='\0' || cmd[pos]=='\n')
                {
                    cmdsplit[ctr][j]='\0';
                    ctr++;  //for next word
                    j=0;    //for next word, init index to 0
                }
                else
                {
                    cmdsplit[ctr][j]=cmd[pos];
                    j++;
                }
            }
            //printf("%s", cmdsplit[0]);
            int user_flag = 0;
            int pass_flag = 0;

            if(flag)
            {
                if(strcmp(cmdsplit[0],"user")!=0)
                {
                    return_code[0] = htonl(600);
                    send(newsockfd, return_code, sizeof(int), 0);
                    continue;
                }
                else
                {
                    FILE *fp;
                    fp = fopen("user.txt", "r");

                    if (!fp)
                    {
                        fprintf(stderr, "Error opening file '%s'\n", fp);
                        return EXIT_FAILURE;
                    }

                    char line[256];
                    char linesplit[36][36];

                    while (fgets(line, sizeof(line), fp))
                    {
                        j=0;
                        ctr = 0;
                        for(int pos=0;pos<=(strlen(line));pos++)
                        {
                            // if space or NULL found, assign NULL into newString[ctr]
                            if(line[pos]==' '|| line[pos]=='\0' || line[pos]=='\n')
                            {
                                linesplit[ctr][j]='\0';
                                ctr++;  //for next word
                                j=0;    //for next word, init index to 0
                            }
                            else
                            {
                                linesplit[ctr][j]=line[pos];
                                j++;
                            }
                        }

                        //printf("%s %s", linesplit[0], cmdsplit[1]);

                        if(strcmp(linesplit[0], cmdsplit[1]) == 0)
                        {
                            return_code[0] = htonl(200);
                            //printf("sending 200");
                            send(newsockfd, return_code, sizeof(int), 0);
                            user_flag = 1;
                            flag = 0;
                            strcpy(user, linesplit[0]);
                            break;
                        }

                    }

                    if(user_flag == 0)
                    {
                        return_code[0] = htonl(500);
                        send(newsockfd, return_code, sizeof(int), 0);
                        continue;
                   }
                }

            }

            else if(flag_2)
            {
                printf("%s", user);
                if(strcmp(cmdsplit[0],"pass")!=0)
                {
                    return_code[0] = htonl(600);
                    send(newsockfd, return_code, sizeof(int), 0);
                    continue;
                }

                else
                {
                    FILE *fp;
                    fp = fopen("user.txt", "r");

                    if (!fp)
                    {
                        fprintf(stderr, "Error opening file '%s'\n", fp);
                        return EXIT_FAILURE;
                    }

                    char line[256];
                    char linesplit[36][36];

                    while (fgets(line, sizeof(line), fp))
                    {
                        j=0;
                        ctr = 0;
                        for(int pos=0;pos<=(strlen(line));pos++)
                        {
                            // if space or NULL found, assign NULL into newString[ctr]
                            if(line[pos]==' '|| line[pos]=='\0' || line[pos]=='\n')
                            {
                                linesplit[ctr][j]='\0';
                                ctr++;  //for next word
                                j=0;    //for next word, init index to 0
                            }
                            else
                            {
                                linesplit[ctr][j]=line[pos];
                                j++;
                            }
                        }

                        //printf("%s %s", linesplit[0], cmdsplit[1]);

                        if((strcmp(linesplit[1], cmdsplit[1]) == 0) && (strcmp(linesplit[0], user) == 0))
                        {
                            return_code[0] = htonl(200);
                            //printf("sending 200");
                            send(newsockfd, return_code, sizeof(int), 0);
                            pass_flag = 1;
                            flag_2 = 0;
                            strcpy(user, linesplit[0]);
                            break;
                        }  
                    }

                    if(pass_flag == 0)
                    {
                        return_code[0] = htonl(500);
                        flag = 1; //enter username again
                        send(newsockfd, return_code, sizeof(int), 0);
                        continue;
                    }
                }
            }

            else
            {
                if (strcmp(cmdsplit[0], "cd")==0)	
                {
                    int x = chdir(cmdsplit[1]);
                    if(x==0)
                    {
                        return_code[0] = htonl(200);
                        send(newsockfd, return_code, sizeof(int), 0);
                        //printf("Successfully changed the directory\n");		
                    }
                    else
                    {
                        perror("ERROR: changing directory was  UNSUCCESSFUL\n");
                        return_code[0] = htonl(500);
                        send(newsockfd, return_code, sizeof(int), 0);
                    }  
                }

                else if(strcmp(cmdsplit[0],"lcd")==0)
                {
                    //printf("lcd executed\n");
                }

                else if(strcmp(cmdsplit[0],"dir")==0)
                {
                    char cwd[PATH_MAX];
                    getcwd(cwd, sizeof(cwd));
                    
                    DIR *dp = NULL;
                    struct dirent *dptr;
                        
                    char buff[128];
                    dp=opendir(cwd);
                    dptr=readdir(dp);
                        
                    char returnData[PATH_MAX] = "";

                    if (dp)
                    {
                        while ((dptr = readdir(dp)) != NULL)
                        {
                            //printf("%s\n", dptr->d_name);
                            strcpy (returnData, dptr->d_name);
                            send(newsockfd, returnData, sizeof(returnData),0);
                        }
                        if((dptr = readdir(dp)) == NULL)
                        {
                            strcpy (returnData, "\0");
                            send(newsockfd, returnData, sizeof(returnData),0);
                        }
                        closedir(dp);
                    }
                        
                }

                else if(strcmp(cmdsplit[0],"quit")==0)
                {   
                    printf("Connection CLosed\n\n");
                    close(newsockfd);
                    break;
                }

                else if(strcmp(cmdsplit[0],"get")==0)
                {
                    int fp;
                    fp = open(cmdsplit[1], O_RDONLY);
                    //char ch[1];

                    if (fp==-1)
                    {
                        perror("Error in File opening\n");
                        return_code[0] = htonl(500);
                        send(newsockfd, return_code, sizeof(int), 0);
                        continue;
                    }

                    else
                    {   
                        return_code[0] = htonl(200);
                        send(newsockfd, return_code, sizeof(int), 0);
                        int l;
                        char filebuffer[10], buff[13], ch[1];
                        for(int i=0;i<13;i++)buff[i]='\0';
                        while((l=read(fp,filebuffer,10))>0)
                        {
                                    filebuffer[l]='\0';
                                    if(l==10)
                                        ch[0]='M';
                                    else
                                        ch[0]='L';
                                    int con=htonl(l);
                                    // Send character
                                    send(newsockfd,ch,sizeof(ch),0);
                                    // Send bytes read
                                    send(newsockfd,&con,sizeof(con),0);
                                    // Send filebuffer
                                    send(newsockfd,filebuffer,strlen(filebuffer),0);
                                    // If file is completely read break
                                    if(l<10)
                                        break;
                        }
                    }   
                }

                else if(strcmp(cmdsplit[0],"put")==0)
                {
                    int filehandle;
			        filehandle = open(cmdsplit[2], O_CREAT | O_WRONLY |O_TRUNC, 0666);

                    if(filehandle==-1)
                    {
                        printf("Cannot open File\n");
                        return_code[0] = htonl(500);
                        send(newsockfd, return_code, sizeof(int), 0);
                        continue;
                    }

                    else
                    {
                        return_code[0] = htonl(200);
                        send(newsockfd, return_code, sizeof(int), 0);
                        char filebuff[5];
						int size1=0;
						// File receiving module
						while(1){
							char ch[1];
							// Receive character
							int u = recv(newsockfd,ch,sizeof(ch),0);
							ch[u]='\0';
							int p;
							// Receive number of bytes
							recv(newsockfd,&p,sizeof(int),0);
							p=ntohl(p);
							char filebuff[p];
							// Receive filebuffer
							p = recv(newsockfd,filebuff,sizeof(filebuff),0);
							filebuff[p]='\0';
							// Write data to file
							write(filehandle,filebuff,strlen(filebuff));
							if(ch[0]=='L')
								break;
						}
                    }
						
					close(filehandle);
                }

                else if(strcmp(cmdsplit[0],"mget")==0)
                {   
                    int i = 1;
                    while(1)
                    {
                        if(strcmp(cmdsplit[i],"\0")==0)
                        {
                            break;
                        }

                        else if(i>2)
                            break;

                        else
                        {
                            int fp;
                            fp = open(cmdsplit[i], O_RDONLY);
                            //char ch[1];
                            printf("%s\n", cmdsplit[i]);

                            if (fp==-1)
                            {
                                perror("Error in File opening\n");
                                return_code[0] = htonl(500);
                                send(newsockfd, return_code, sizeof(int), 0);
                                continue;
                            }

                            else
                            {   
                                return_code[0] = htonl(200);
                                send(newsockfd, return_code, sizeof(int), 0);
                                int l;
                                char filebuffer[10], buff[13], ch[1];
                                for(int i=0;i<13;i++)buff[i]='\0';
                                while((l=read(fp,filebuffer,10))>0)
                                {
                                            filebuffer[l]='\0';
                                            if(l==10)
                                                ch[0]='M';
                                            else
                                                ch[0]='L';
                                            int con=htonl(l);
                                            // Send character
                                            send(newsockfd,ch,sizeof(ch),0);
                                            // Send bytes read
                                            send(newsockfd,&con,sizeof(con),0);
                                            // Send filebuffer
                                            send(newsockfd,filebuffer,strlen(filebuffer),0);
                                            // If file is completely read break
                                            if(l<10)
                                                break;
                                }
                            }  
                        }
                        i++;
                    }
                    continue;
                }

                else if(strcmp(cmdsplit[0],"mput")==0)
                {   
                    int i = 1;
                    while(1)
                    {
                        if(strcmp(cmdsplit[i],"\0")==0)
                        {
                            break;
                        }
                        
                        if(i>2)
                            break;

                        else
                        {
                            int filehandle;
                            filehandle = open(cmdsplit[i], O_CREAT | O_WRONLY |O_TRUNC, 0666);

                            if(filehandle==-1)
                            {
                                printf("Cannot open File\n");
                                return_code[0] = htonl(500);
                                send(newsockfd, return_code, sizeof(int), 0);
                                continue;
                            }

                            else
                            {
                                return_code[0] = htonl(200);
                                send(newsockfd, return_code, sizeof(int), 0);
                                char filebuff[5];
                                int size1=0;
                                // File receiving module
                                while(1){
                                    char ch[1];
                                    // Receive character
                                    int u = recv(newsockfd,ch,sizeof(ch),0);
                                    ch[u]='\0';
                                    int p;
                                    // Receive number of bytes
                                    recv(newsockfd,&p,sizeof(int),0);
                                    p=ntohl(p);
                                    char filebuff[p];
                                    // Receive filebuffer
                                    p = recv(newsockfd,filebuff,sizeof(filebuff),0);
                                    filebuff[p]='\0';
                                    // Write data to file
                                    write(filehandle,filebuff,strlen(filebuff));
                                    if(ch[0]=='L')
                                        break;
                                }
                            }
                                
                            close(filehandle);
                        }
                    }
                    i++;
                }

                else 
                {
                    return_code[0] = htonl(650);
                    send(newsockfd, return_code, sizeof(int), 0);
                }
            }

        }

        close(newsockfd);

	}
	return 0;
	 
}