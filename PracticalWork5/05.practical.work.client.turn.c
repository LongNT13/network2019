#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h> 
#include <unistd.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in saddr;
	struct hostent *h;
	int sockfd;
	short port = 8784;

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(-1);
	}

	if (argc == 2)  {
		h = gethostbyname(argv[1]);
	}else{
		char hostName[256];
		printf("Enter host name : \n");
		scanf("%s",hostName);
		h = gethostbyname(hostName);
	}

	if(h == NULL) {
		printf("Unknown host\n");
		exit(-1);
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
		exit(-1);
	}else{

		while(1){
			//send message
			char mess[1000];
			printf("client >  ");
			fgets(mess, 1000, stdin);
			write(sockfd, mess, sizeof(mess));
			char mess2[1000];
			read(sockfd, mess2, sizeof(mess2));
			printf("server > %s", mess2);
		}
	}
	return 0;

}