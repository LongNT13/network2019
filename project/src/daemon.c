#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
	FILE* fp;
	bool isSplit = true;
	int sockfd, clen, clientfd;
	int clientfds[5];
	memset(clientfds, 0, sizeof(clientfds));
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8784;
	char portChar[5];

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	while(1){
		
		if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Error creating socket\n");
			exit(-1);
		}
		//set non block
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
			printf("Error set socket option\n");
			exit(-1);
		}

		int fl = fcntl(sockfd, F_GETFL, 0);
		fl |= O_NONBLOCK;
		fcntl(sockfd, F_SETFL, fl);

		memset(&saddr, 0,sizeof(saddr));
		saddr.sin_family = AF_INET;	
		saddr.sin_addr.s_addr = htonl(INADDR_ANY);
		saddr.sin_port = htons(port);
		
		if((bind(sockfd, (struct sockaddr *) &saddr,sizeof(saddr))) < 0) {
			printf("Error binding\n");
			port++;
			continue;
		}
		//open local file
		sprintf(portChar, "%d", port);
		char* fileName = strcat( portChar, ".txt");
		printf("%s\n", fileName);
		fp = fopen( fileName, "w");
		if (fp == NULL)
		{
			printf("error\n");
			exit(-1);
		}
		break;
	}


	if(listen(sockfd, 5) < 0) {
		printf("Error listening\n");
		exit(-1);
	}

	//error
	printf("listening on %d\n", port);
	clen=sizeof(caddr);
	while (1) {
		fd_set set; // declaration of the set
		FD_ZERO(&set); //// clear the set
		FD_SET(sockfd, &set); // add listening sockfd to set
		int i, maxfd;
		char s[1500];
		FD_SET(1, &set);
		
		maxfd = sockfd;
		for (i = 0; i < 5; i++){
			// add connected client sockets to set
			if(clientfds[i] > 0){
				FD_SET(clientfds[i], &set);
			}

			if(clientfds[i] > maxfd){
				maxfd = clientfds[i];
			}
		}
		select(maxfd + 1, &set, NULL, NULL, NULL);
		
		if (FD_ISSET(sockfd, &set)){
			clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);
			if (clientfd > 0) {
				int fl = fcntl(clientfd, F_GETFL, 0);
				fl |= O_NONBLOCK;
				fcntl(clientfd, F_SETFL, fl);

				// add it to the clientfds array
				for (int i=0; i< 5; i++){
					if (clientfds[i] == 0){
						clientfds[i] = clientfd;
						break;
					}
				}

				printf("Successfully accepted a client\n");
				printf("receiving block ....\n");
			}
		}
		// sleep(0.3);
		for (int i=0; i< 5; i++){
			if(clientfds[i]>0 && FD_ISSET(clientfds[i], &set)){
				if(read(clientfds[i], s, sizeof(s)) > 0){
					if (!isSplit)
					{
						printf("Map signal received\n");
						//call to map
						int pid;
						pid = fork();
						if (pid == 0)
						{
							//this is a child
							sprintf(portChar, "%d", port);
							char* fileName = strcat( portChar, ".txt");
							execl("binary/map","map", fileName, NULL);
						}else{
							//this is a father

						}
					}else{
						if (strcmp(s,"<done>") == 0)
						{
							printf("done receiving\n");
							char *doneReceive = "d";
							for (int i = 0; i < 5; ++i)
					    	{
					    		if (clientfds[i] > 0)
					    		{
									// printf("sending\n");
							      	write(clientfds[i], doneReceive, sizeof(doneReceive));
					    		}
					    	}
						}else{
							// printf("Client %d said: %s\n", clientfds[i], s);
							fprintf(fp, "%s\n", s);						

						}

					}
				}
				else{
					// some error, remove it from the "active" fd array
					printf("client %d has disconnected.\n", clientfds[i]);
					//hard code (can not be extended)
					if (isSplit)
					{
						isSplit = false;
						fclose(fp);//might need to change to cover all the possibilities
					}
					clientfds[i] = 0;
				}
			}
		}
		// printf("poll\n");
		struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
		if (poll(input, 1, 100) > 0) {
      		fgets(s, 1000, stdin);
	      	s[strlen(s) - 1] = 0;
	      	// printf("accepted mess\n");
	    	for (int i = 0; i < 5; ++i)
	    	{
	    		if (clientfds[i] > 0)
	    		{
					// printf("sending\n");
			      	write(clientfds[i], s, strlen(s));
	    		}
	    	}
	    }
		

	}
	return 0;

}