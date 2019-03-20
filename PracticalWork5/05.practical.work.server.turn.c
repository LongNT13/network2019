#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int sockfd, clen, clientfd;
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8784;
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(-1);
	}

	memset(&saddr, 0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);
	
	if((bind(sockfd, (struct sockaddr *) &saddr,sizeof(saddr))) < 0) {
		printf("Error binding\n");
		exit(-1);
	}

	if(listen(sockfd, 5) < 0) {
		printf("Error listening\n");
		exit(-1);
	}

	//error
	clen=sizeof(caddr);
	if((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
		printf("Error accepting connection\n");
		exit(-1);
	}else{
		printf("successfully connected\n");
		printf("waiting for message .... \n");
		while(1){
			char mess[1000];
			read(clientfd, mess, sizeof(mess));
			
			//receive message
			printf("client > %s", mess);
			
			printf("server > ");
			fgets(mess, 1000, stdin);
			write(clientfd, mess, sizeof(mess));
		}
	}

	return 0;

}