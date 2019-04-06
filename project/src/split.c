#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "Global.h"

int main(int argc, char const *argv[])
{
	//check argc
	//connect to several node
	int finalCheck;
	int numNode = 3;
	short port[numNode];
	port[0] = 8784;
	int sockfds[numNode];
	int i=0;
	while(i<numNode){
		int temp = connectSoc(port[i]);
		if (temp<0)
		{
			port[i]++;
			continue;
		}else{
			sockfds[i] = temp;
			i++;
			port[i]=port[i-1]+1;
		}
	}
	//end connect to several node

	char ch, file_name[25];
	FILE *fp;
	
	//need to modify in order to receive user's input
	if (argc == 2)
	{
		finalCheck = atoi(argv[1]);
		printf("final check : %d\n", finalCheck);
		// fp = fopen(argv[1], "r");
		fp = fopen("binary/data/randomTextFile.txt", "r"); // read mode
	}else{
		// printf("Enter name of a file you wish to see\n");
		// fgets(file_name, sizeof(file_name), stdin);
		fp = fopen("binary/data/randomTextFile.txt", "r"); // read mode
	}
	//

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	int line = 0, currentMem, j, whichSoc;
	whichSoc = numNode;
	char *val = NULL;
	bool newLine = true;
	while(1){
		//check end of file or not
		if ((ch = fgetc(fp)) != EOF)
		{
			// printf("%c", ch);
			if (ch == '\n')
			{
				//send mess to node
				whichSoc %= numNode;
				int checkErr = write(sockfds[whichSoc], val, currentMem);
				if (checkErr<=0)
				{
					printf("error writing\n");
				}
				whichSoc++;

				printf("%d %s\n", line+1, val);

				// sleep(1);
				usleep(40000);			

				free(val);
				//prevemt double free
				val = NULL;
				line++;
				newLine = true;
				continue;
			}else{//ch != '\n'
				if (newLine)
				{
					free(val);
					val = (char *) malloc(2* sizeof(char));
					currentMem = 2;
					newLine = false;
					j=0;
				}else{
					currentMem++;
					val = realloc(val, currentMem*(sizeof(char)));
				}
				val[j]=ch;
				j++;
			}
		}else{
			printf("%d %s\n", line+1, val);

			//send mess to node
			whichSoc %= numNode;
			int checkErr = write(sockfds[whichSoc], val, currentMem);
			if (checkErr<=0)
			{
				printf("error writing\n");
			}			

			usleep(100000);	
			char *doneChar = "<done>";
			for (i = 0; i < numNode; ++i)
			{
				printf("%s\n", doneChar);
				write(sockfds[i], doneChar, sizeof(doneChar));
			}
			free(val);
			//prevemt double free
			val = NULL;
			printf("end of file\n");
			break;
		}
	}

	fclose(fp);
	// int count =0;
	while(1){

		char c;
		read(sockfds[whichSoc], &c, sizeof(c));
		if (c == 'd')
		{
			printf("Node has received everything\n");
			break;
		}
	}
	for (i = 0; i < numNode; ++i)
	{
		shutdown(sockfds[i], SHUT_RDWR);
		char c;
		while((read(sockfds[i], &c, sizeof(c))) > 0);
		close(sockfds[i]);	
	}

	char c[3];
	sprintf(c, "%d", numNode);
	write(finalCheck, c, sizeof(c));
	usleep(100000);
	for (i = 0; i < numNode; ++i)
	{
		char portChar[5];
		sprintf(portChar, "%d", port[i]);
		write(finalCheck, portChar, sizeof(portChar));	
		usleep(100000);
	}
	return 0;
}