/*
	NETWORKS LAB 4
	FILE TRANSFER USING SOCKETS
	HAASITA PINNEPU	19CS30021
	MAJJI DEEPIKA 19CS30027
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define MESSAGE_SIZE 100
#define TABLE_SIZE 50
#define DROP_PROBALITY 0.4
#define T 2
#define SOCK_MRP 153


int dropMessage(float);

int r_socket(int, int, int);

int r_bind(int, socklen_t, const struct sockaddr* );

int r_sendto(int, const void*, size_t, int, const struct sockaddr*, socklen_t);

int r_recvfrom(int , char *, size_t , int , const struct  sockaddr *, socklen_t*);

int r_close(int);