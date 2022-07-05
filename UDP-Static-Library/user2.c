#include "rsocket.h"

#define MAXLINE 100
#define PORTX 50000+(2*0021)
#define PORTY 50000+(2*0021)+1

int main()
{
    int sockfd;
	struct sockaddr_in 	serv_addr, cli_addr;
    char buff[MAXLINE];

    sockfd = r_socket(AF_INET, SOCK_MRP, 0);
    if(sockfd < 0)
	{
		perror("Sorry :(...Unable to create a socket\n");
		exit(0);
	}

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    memset(&cli_addr, 0, sizeof(cli_addr)); 

    //server info
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORTX); 
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //client info
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(PORTY);
    cli_addr.sin_addr.s_addr = INADDR_ANY;

    if(r_bind(sockfd, (const struct sockaddr *)&cli_addr,  sizeof(cli_addr)) < 0)
    {
        perror("Unable to bind\n");
        r_close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t l = sizeof(serv_addr);
    for(int i = 0;i<MAXLINE;i++)
    {  
        int n = r_recvfrom(sockfd, (char *)buff, MAXLINE, 0, (struct sockaddr *) &serv_addr, &l);
        printf("%s\n", buff);
    }

    printf("\n");
    r_close(sockfd); 
    return 0; 
}