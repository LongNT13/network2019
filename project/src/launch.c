#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Global.h"
#include <sys/poll.h>
#include <sys/select.h>
#include <time.h>

int initServer(unsigned short portIn){
	//-----------------------
	//initialize as a server
	FILE *fp;
	int countDone =0;
	char portChar[5];
	int sockfd, clientfd;
	socklen_t clen;
	int clientfds[10];
	memset(clientfds, 0, sizeof(clientfds));
	struct sockaddr_in saddr, caddr;
	unsigned short port = portIn;


	sprintf(portChar, "%d", port);
	char* fileName = strcat( portChar, ".txt");
	printf("%s\n", fileName);
	fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		printf("error\n");
		exit(-1);
	}

	if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0){
		printf("Error set socket option\n");
		exit(-1);
	}
	int fl = fcntl(sockfd, F_GETFL, 0);
	fl |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, fl);
	
	memset(&saddr, 0, sizeof(saddr)); 
	saddr.sin_family = AF_INET; 
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	saddr.sin_port = htons(port);
	
	if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		printf("Error binding\n");
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, 5) < 0) { 
		printf("Error listening\n");
		exit(1);
	}

	int countSize = 0;//delete after
	clen=sizeof(caddr);
	while (1) {
		fd_set set; // declaration of the set
		FD_ZERO(&set); //// clear the set
		FD_SET(sockfd, &set); // add listening sockfd to set
		int i, maxfd;
		char s[1000];
		FD_SET(1, &set);
		
		maxfd = sockfd;
		for (i = 0; i < 10; i++){
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
				for (int i=0; i< 10; i++){
					if (clientfds[i] == 0){
						clientfds[i] = clientfd;
						break;
					}
				}

				printf("Successfully accepted a client\n");
				
			}
		}
		for (int i=0; i< 10; i++){
			if(clientfds[i]>0 && FD_ISSET(clientfds[i], &set)){
				if(read(clientfds[i], s, sizeof(s)) > 0){
					// printf("%d Client %d said: %s\n", countSize, clientfds[i], s);
					// printf("%s\n", s);
										
					countSize++;
					if (strcmp(s, "<done>") == 0)
					{
						printf("received everything from map %d\n", clientfds[i]);
						char *doneMap = "d";
						countDone++;
						write(clientfds[i], doneMap, sizeof(doneMap));
					}else{
						fprintf(fp, "%s\n", s);					
					}
					
					if (countDone == 3)
					{
						// printf("first break\n");
						break;
					}
				}
				else{
					// some error, remove it from the "active" fd array
					printf("client %d has disconnected.\n", clientfds[i]);
					clientfds[i] = 0;
				}

			}
		}
		if (countDone == 3)
		{
			// printf("second break;\n");
			break;
		}
	}
	fclose(fp);
	
}

int main(int argc, char const *argv[])
{
	clock_t t;
    double time_taken;
	int fds[2],i;
	pipe(fds);
	int pid = fork();
	if (pid == 0)
	{
		printf("this is child\n");
		close(fds[0]);
		
		char fdsChar[64];
		sprintf(fdsChar, "%d", fds[1]);
		
		execl("binary/split","split", fdsChar, NULL);
	}else{
		printf("this is father\n");
		close(fds[1]);
		char c[100];
		bool firstReceive = true;
		int noPort = 0, curPort = 0;
		short *port;
		while(1)
		{

			if(read(fds[0], c, sizeof(c)) > 0){
				if (firstReceive)
				{
					firstReceive = false;
					noPort = atoi(c);
					port = (short *) malloc(noPort * sizeof(short));
					continue;
				}
				port[curPort] = atoi(c);
				curPort++;
				if (curPort == noPort)
				{
					break;
				}
			}
		}
		while(1){
			char s;
			printf("Enter 's' to begin map : \n");
			scanf("%c", &s);
			if (s == 's')
			{
				//start counting
				// t = clock();
				printf("start time %f\n", (double)t);
				int sockfds[noPort];
				for (i = 0; i < noPort; ++i)
				{
					sockfds[i] = connectSoc(port[i]);
					char *ch = "testing send map";
					write(sockfds[i], ch, sizeof(ch));
				}
				//close socket
				for (i = 0; i < noPort; ++i)
				{
					shutdown(sockfds[i], SHUT_RDWR);
					char c;
					while((read(sockfds[i], &c, sizeof(c))) > 0);
					close(sockfds[i]);	
				}
				break;
			}
			
		}

		
		//close socket

		//init as server
		unsigned short portIn = 7000; 
		initServer(portIn);
		//execute reduce
		// t = clock() - t;
		// printf("end time %ld\n", (double)t);
		// printf("clock %lu\n", CLOCKS_PER_SEC);
		// time_taken = ((double) t) / CLOCKS_PER_SEC;
		// printf("%f seconds to execute first \n", time_taken); 
		execl("binary/reduce","reduce",NULL);
	}

	return 0;
}