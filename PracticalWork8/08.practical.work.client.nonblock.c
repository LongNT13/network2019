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
#include <sys/poll.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{

	printf("Initializing socket ....\n");
	struct sockaddr_in saddr;
	struct hostent *h;
	int sockfd;
	short port = 8784;
	struct pollfd fds[1];

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
		getchar();
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

	//initialize pollfd
	fds[0].fd = 0;
	fds[0].events = 0;
	fds[0].events |= POLLIN;
	int timeout = 5*1000; // 5secs refresh every 5

	//connect to server
	if(connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		printf("Cannot connect\n");
		exit(-1);
	}else{

		//set reuse address
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
			printf("Error set socket option\n");
			exit(-1);
		}

		//set non block server
		int flag = fcntl(sockfd, F_GETFL, 0);
		fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

		while(1){
			char mess[1000];
			
			//receive messsage
			if(read(sockfd, mess, sizeof(mess)) > 0){
				printf("server > %s", mess);				
			}
			//send message
			// int temp = poll(fds, 1, timeout);
			// if (temp == 0)
			// {
			// 	continue;
			// }else if(temp==0){
			
				printf("client >  ");	

				fgets(mess, sizeof(mess), stdin);
				if (strcmp("/quit\n",mess) == 0)
				{
					printf("shutting down client ...\n");
					shutdown(sockfd, SHUT_RDWR);
					int count;
					char c;
					while((count = read(sockfd, &c, sizeof(c))) > 0);
					close(sockfd);
					exit(0);
				}
				write(sockfd, mess, sizeof(mess));
			// }
			// else{
			// 	exit(-1);
			// }
			
		}
	}		

	return 0;

}