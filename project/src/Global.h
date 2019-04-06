#ifndef GLOBAL_H
#define GLOBAL_H

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

typedef struct ValueNumber{
	char *value;
	int number;
} valuenumber;

int connectSoc(short port){
	struct sockaddr_in saddr;
	struct hostent *h;
	int sockfd;
	
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		return -1;
	}

	h = gethostbyname("localhost");
	

	if(h == NULL) {
		printf("Unknown host\n");
		return -1;
	}

    int i=0;
    while ( h -> h_addr_list[i] != NULL) {
		printf("%s\n", inet_ntoa( (struct in_addr) *((struct in_addr *) h->h_addr_list[i])));
		i++;
    }

    printf("\n");
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	saddr.sin_port = htons(port);
	
	if(connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		printf("Cannot connect\n");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
		printf("Error set socket option\n");
		exit(-1);
	}

	printf("success port %d\n", port);

    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);

	return sockfd;
}

#endif