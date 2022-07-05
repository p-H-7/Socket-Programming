/* 
    FTP client
    Assignment 3

    Haasita Pinnepu 19CS30021
    Majji Deepika 19CS30027
*/

// Assumption: if any wrong command input is given which is not in the list, the server returns an error code of 650.

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <sys/wait.h>
#include<limits.h>
#include<dirent.h>
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

#define PORTX 50000
#define MAX_CHAR 100
#define MAX_TOKENS 10

int main()
{
	
	int 	sockfd;
	struct sockaddr_in 	serv_addr, cli_addr;
	
	int x;
	char buffer[100];
	
	//int i = 0;

    int openFlag = 1;
	
	while(1)
	{	
		printf("\nmyFTP>");
	
		char cmd[200], cmdsplit[20][20];
		fgets(cmd, 200, stdin);
		
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
		//printf("%s", cmdsplit[1]);
		
		if(openFlag)
		{
			if(strcmp(cmdsplit[0],"open")==0)
			{
                openFlag = 0;
				char * ip = cmdsplit[1];
				int port = atoi(cmdsplit[2]);
				//printf("%d\n",port);
				//printf("%s\n",cmdsplit[2]);
				//printf("%s\n", ip);
				if(port<20000 || port>65535)
				{
					perror("Invalid Port number\n");
					continue;
				}
				
				serv_addr.sin_family	= AF_INET;
				inet_aton(ip, &serv_addr.sin_addr);
				serv_addr.sin_port	= htons(port);
				
				//opening a socket
				sockfd = socket(AF_INET, SOCK_STREAM, 0);
				if(sockfd<0)
				{
					perror("sorry :(...Unable to create a socket\n");
					exit(0);
				}
		
				int con;
				con=connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
		
				//trying to establish connection
				if(con<0)
				{
					perror("Sorry :(....unable to connect to server \n");
					exit(0);
				}
				else
		
					printf(" Connection established successfully :) \n");
				
			}
			else
			{
				printf("Please enter a valid command: First one should be open\n");
				continue;
			}
		}

        else
        {   
            send(sockfd, cmd, strlen(cmd)+1, 0);
            
            if(strcmp(cmd,"quit\n")==0)
            {
                printf("Closing Connection\n");
                break;
            }

            else if(strcmp(cmdsplit[0],"user")==0)
            {
                int return_code[]={0};
				int n = recv(sockfd, return_code, sizeof(int), 0);
                return_code[0] = ntohl(return_code[0]);

                printf("Return code received: %d\n", return_code[0]);
                if (return_code[0] == 200)
			    {
				    printf("Success: Username found\n");
			    }
                else if(return_code[0] == 500)
                {
                    printf("Username Does not exist. Please try again.");
                    continue;
                }
                else if(return_code[0] == 600)
                {
                    printf("Please Enter the Password.\n");
                    continue;
                }
                if (return_code[0] == 650)
			    {
				    printf("Incorrect command\n");
			    }
            }

            else if(strcmp(cmdsplit[0],"pass")==0)
            {
                int return_code[]={0};
				int n = recv(sockfd, return_code, sizeof(int), 0);
                return_code[0] = ntohl(return_code[0]);

                printf("Return code received: %d\n", return_code[0]);
                if (return_code[0] == 200)
			    {
				    printf("Success: Password correct\n");
			    }
                else if(return_code[0] == 500)
                {
                    printf("Password Incorrect. Please enter username and password again.\n");
                    continue;
                }   
                else if(return_code[0] == 600)
                {
                    printf("Please Enter Username and Password First.\n");
                    continue;
                }
                if (return_code[0] == 650)
			    {
				    printf("Incorrect command\n");
			    }
            }

            else if(strcmp(cmdsplit[0],"cd")==0)
            {
                int return_code[]={0};
                
                //send(sockfd, cmd, sizeof(cmd), 0);
                int n = recv(sockfd, return_code, sizeof(int), 0);
                
                //printf("Code returned from server :  %d\n", return_code[0]);
                return_code[0] = ntohl(return_code[0]);

                printf("Code returned from server :  %d\n", return_code[0]);
                
                if (return_code[0] == 200)
                {
                    printf("Directory changing SUCCESSFUL with return code 200...\n");
                }
                else if (return_code[0] == 500)
                {
                    printf("ERROR: Directory change unsuccessful with return code 500...\n");
                }
                else if(return_code[0] == 600)
                {
                    printf("Please Enter Username and Password First.\n");
                    continue;
                }
            }
            else if(strcmp(cmdsplit[0],"lcd")==0)
            {
                int n=chdir(cmdsplit[1]);
                if(n<0)
                    perror("ERROR: Directory change unsuccessful\n");
                else
                    printf("Directory changing SUCCESSFUL\n");   
            }

            else if(strcmp(cmdsplit[0],"dir")==0)
            {
                char contents[PATH_MAX]="example";
                printf("Files in the server directory are: \n");
                while(strcmp(contents, "\0")!=0)
                {
                    recv(sockfd,contents,sizeof(contents),0);
                    printf("%s\n", contents);
                }
                continue;
            }

            else if(strcmp(cmdsplit[0],"get")==0)
            {
                int filehandle,p,u;
				filehandle = open(cmdsplit[2], O_CREAT | O_WRONLY |O_TRUNC, 0666);
                char ch[1];

                if(filehandle==-1)
                {
                    printf("Cannot open given file\n");
                    continue;
                }

                int return_code[]={0};
				int n = recv(sockfd, return_code, sizeof(int), 0);
                return_code[0] = ntohl(return_code[0]);
                printf("Return code received: %d\n", return_code[0]);

                if(return_code[0] == 500)
                {
                    printf("File does not exist at server.\n");
                }

                else if(return_code[0] == 600)
                {
                    printf("Please Enter Username and Password First.\n");
                    continue;
                }

                else if(return_code[0] == 200)
                {
                    printf("File transfer Succesful.\n");
                    while(1)
                    {
                        // Receive character
                        u = recv(sockfd,ch,sizeof(ch),0);
                        ch[u]='\0';
                        // Receive number of bytes
                        recv(sockfd,&p,sizeof(int),0);
                        p=ntohl(p);
                        char filebuff[p];
                        // Receive buffer
                        p = recv(sockfd,filebuff,sizeof(filebuff),0);
                        filebuff[p]='\0';
                        write(filehandle,filebuff,strlen(filebuff));
                        // If last block break
                        if(ch[0]=='L')
                            break;
                    }
                    //continue;
                }
				close(filehandle);
                continue;
            }

            else if(strcmp(cmdsplit[0],"put")==0)
            {
                int filehandle;
			    filehandle = open(cmdsplit[1],O_RDONLY);

                if(filehandle == -1)
                {
                    printf("Cannot open given file\n");
                    continue;
                }

                int return_code[]={0};
				int n = recv(sockfd, return_code, sizeof(int), 0);
                return_code[0] = ntohl(return_code[0]);
                printf("Return code received: %d\n", return_code[0]);

                if(return_code[0] == 500)
                {
                    printf("Server not able to open file.\n");
                }

                else if(return_code[0] == 600)
                {
                    printf("Please Enter Username and Password First.\n");
                    continue;
                }

                else if(return_code[0] == 200)
                {
                    printf("File transfer Succesful.\n");
                    char filebuff[10];
					int l;
					char ch[1];
					// File sending module
				    while((l=read(filehandle,filebuff,10))>0){
						filebuff[l]='\0';
						if(l==10)
							ch[0]='M';
						else
							ch[0]='L';
						int con=htonl(l);
						// Send character
						send(sockfd,ch,sizeof(ch),0);
						// Send number of bytes
						send(sockfd,&con,sizeof(con),0);
						// Send buffer
						send(sockfd,filebuff,strlen(filebuff),0);
						if(l<10)
							break;
					}	
                    //continue;				
                }                
                close(filehandle);
                continue;
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
                            int filehandle,p,u;
                            filehandle = open(cmdsplit[i], O_CREAT | O_WRONLY |O_TRUNC, 0666);
                            char ch[1];

                            if(filehandle==-1)
                            {
                                printf("Cannot open given file\n");
                                //continue;
                            }

                            int return_code[]={0};
                            int n = recv(sockfd, return_code, sizeof(int), 0);
                            return_code[0] = ntohl(return_code[0]);
                            printf("Return code received: %d\n", return_code[0]);

                            if(return_code[0] == 500)
                            {
                                printf("File does not exist at server.\n");
                            }

                            else if(return_code[0] == 600)
                            {
                                printf("Please Enter Username and Password First.\n");
                                //continue;
                            }

                            else if(return_code[0] == 200)
                            {
                                printf("File transfer Succesful.\n");
                                while(1)
                                {
                                    // Receive character
                                    u = recv(sockfd,ch,sizeof(ch),0);
                                    ch[u]='\0';
                                    // Receive number of bytes
                                    recv(sockfd,&p,sizeof(int),0);
                                    p=ntohl(p);
                                    char filebuff[p];
                                    // Receive buffer
                                    p = recv(sockfd,filebuff,sizeof(filebuff),0);
                                    filebuff[p]='\0';
                                    write(filehandle,filebuff,strlen(filebuff));
                                    // If last block break
                                    if(ch[0]=='L')
                                        break;
                                }
                                //continue;
                            }
                            close(filehandle);
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
                            filehandle = open(cmdsplit[i],O_RDONLY);

                            if(filehandle == -1)
                            {
                                printf("Cannot open given file\n");
                                //continue;
                            }

                            int return_code[]={0};
                            int n = recv(sockfd, return_code, sizeof(int), 0);
                            return_code[0] = ntohl(return_code[0]);
                            printf("Return code received: %d\n", return_code[0]);

                            if(return_code[0] == 500)
                            {
                                printf("Server not able to open file.\n");
                            }

                            else if(return_code[0] == 600)
                            {
                                printf("Please Enter Username and Password First.\n");
                                //continue;
                            }

                            else if(return_code[0] == 200)
                            {
                                printf("File transfer Succesful.\n");
                                char filebuff[10];
                                int l;
                                char ch[1];
                                // File sending module
                                while((l=read(filehandle,filebuff,10))>0)
                                {
                                    filebuff[l]='\0';
                                    if(l==10)
                                        ch[0]='M';
                                    else
                                        ch[0]='L';
                                    int con=htonl(l);
                                    // Send character
                                    send(sockfd,ch,sizeof(ch),0);
                                    // Send number of bytes
                                    send(sockfd,&con,sizeof(con),0);
                                    // Send buffer
                                    send(sockfd,filebuff,strlen(filebuff),0);
                                    if(l<10)
                                        break;
                                }	
                                //continue;				
                            }                
                            close(filehandle);
                        }
                        i++;
                    }
                continue;
            }

            else
            {
                int return_code[]={0};
				int n = recv(sockfd, return_code, sizeof(int), 0);
                return_code[0] = ntohl(return_code[0]);
                printf("Return code received: %d\n", return_code[0]);
                if (return_code[0] == 650)
			    {
				    printf("Incorrect command\n");
			    }
                else if(return_code[0] == 600)
                {
                    printf("Please Enter Username and Password First.\n");
                    continue;
                }
            }
        }
        //i++;
	}
    close(sockfd);
    return 0;	
}