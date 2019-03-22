#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
	int sockfd, clen, clientfd;
	struct sockaddr_in saddr, caddr;
	struct pollfd fdsServer[1];
	struct pollfd fdsClient[10];
	unsigned short port = 8784;
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(-1);
	}

	//set reuse address
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
		printf("Error set socket option\n");
		exit(-1);
	}

	//set non block server
	int flag = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

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

	//initialize pollfd
	memset(fdsServer, 0, sizeof(fdsServer));
	memset(fdsClient, 0, sizeof(fdsClient));
	fdsServer[0].fd = sockfd;//check accepting 
	fdsServer[0].events = POLLIN;

	int currentSize = 0;

	clen=sizeof(caddr);
	while(1){
		int temp = poll(fdsServer, 1, 5*1000);
		if (temp > 0)//server's file descriptors has a return event = POLLIN
		{
			while(1){
				clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);
				if (clientfd < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						printf("Error accepting connection\n");
						exit(-1);
					}else{
						break;
					}
				}else{
					//add new file descriptor
					printf("successfully connected\n");
					fdsClient[currentSize].fd = clientfd;
					fdsClient[currentSize].events = POLLIN;
					currentSize++;

					//set nonblock
					int flagClient = fcntl(clientfd, F_GETFL, 0);
					fcntl(clientfd, F_SETFL, flagClient | O_NONBLOCK);

				}
			}
		}else{
			int i;
			for (i = 0; i < currentSize; ++i)
			{
				while(1){
					//read message
					char mess[1000];
					if (read(fdsClient[i].fd, mess, sizeof(mess))<=0)
					{
						// sort array after delete file descriptor, need more time on this
						// currentSize--;
						break;
					}
					printf("client %d > %s", i, mess);	

					int temp2 = poll(fdsServer, 1, 10*1000);
					if (temp2>0)
					{					
						printf("server > ");
						fgets(mess, 1000, stdin);
						if (strcmp("/dc\n",mess) == 0)
						{
							printf("shutting down client ...\n");
							shutdown(clientfd, SHUT_RDWR);
							int count;
							char c;
							while((count = read(clientfd, &c, sizeof(c))) > 0);
							close(clientfd);
							continue;
						}
						//write to all client
						for (int i = 0; i < currentSize; ++i)
						{
							write(fdsClient[i].fd, mess, sizeof(mess));
						}
					}else{
						break;//timeout = 0 or random errors that i dont understand = -1
					}

				}	
			}
			
		}
		
	}


	return 0;

}