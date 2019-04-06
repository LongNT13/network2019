#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Global.h"
#include <unistd.h>

int main(int argc, char const *argv[])
{
	char file_name[25];
	FILE *fp;
	if (argc == 2)
	{
		fp = fopen(argv[1], "r");
	}else{
		// printf("Enter name of a file you wish to see\n");
		// fgets(file_name, sizeof(file_name), stdin);
		fp = fopen("binary/data/randomTextFile.txt", "r"); // read mode
	}


	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}


	char ch;
	valuenumber *listWord;
	listWord = (valuenumber *) malloc(24* sizeof(valuenumber));
	int i = 0,j =0, currentSize = 0, currentMem = 0;
	char *val = NULL;
	bool first = true, newWord = true, last;
	while(1){
		if ((ch = fgetc(fp)) != EOF)
		{
			// printf("%c", ch);
			if (ch == ' ' || ch == '\n'){
			//access val when there is nothing (free before)
			// printf("space\n");
			//first word
				if (first)
				{
					listWord[0].value = (char *) malloc((strlen(val)+1) * sizeof(char));
					strcpy(listWord[0].value, val);
					listWord[0].number = 1;
					currentSize =1;
					first = false;
				}else{//second word onward
					int isExsist = 0,z;
					if (val != NULL){
						for (z = 0; z < currentSize; ++z)
						{
							//compare every string in the struct with new string
							// printf("%d\n", z);
							// printf("%s\n", listWord[z].value);
							// printf("%s\n", val);
							
							if (strcmp(listWord[z].value, val) == 0)
							{
								listWord[z].number++;
								isExsist = 1;
								break;
							}

						}
						//check if there is a existing word in a list
						if (isExsist == 0)
						{
							listWord[currentSize].value = (char *) malloc((strlen(val)+1) * sizeof(char));
							strcpy(listWord[currentSize].value, val);
							listWord[currentSize].number = 1;
							currentSize++;
							if (currentSize>=24)
							{
								listWord = realloc(listWord, (currentSize+1)*sizeof(valuenumber));
							}
						}
					}
					
				}
				free(val);
				//prevent double free
				val = NULL;
				newWord = true;
				continue;

			}else{
				if (newWord)
				{
					val = (char *) malloc(2* sizeof(char));
					newWord = false;
					j=0;
					currentMem = 2;
				}else{
					currentMem++;
					val = realloc(val, currentMem);
				}
				val[j]=ch;
				j++;
			}
		}else{
			int check = 0,z;
			if (val != NULL){
				for (z = 0; z < currentSize; ++z)
				{
							//compare every string in the struct with new string
					if (strcmp(listWord[z].value, val) == 0)
					{
						listWord[z].number++;
						check = 1;
						break;
					}						
				}

						//check if there is a existing word in a list
				if (check ==0)
				{
					listWord[currentSize].value = (char *) malloc((strlen(val)+1) * sizeof(char));
					strcpy(listWord[currentSize].value, val);
					listWord[currentSize].number = 1;
					currentSize++;
				}
			}
			break;
		}
	}

	fclose(fp);	

	//read value number
	// printf("%d\n", currentSize);
	// for (i = 0; i < currentSize; ++i)
	// {
	// 	printf("%s,%d\n", listWord[i].value, listWord[i].number);
	// }

	//send to original node
	int sockfd;
	sockfd = connectSoc(7000);
	for (int i = 0; i < currentSize; ++i)
	{
		char numChar[5];
		sprintf(numChar, "%d", listWord[i].number);
		
		char *tempVal = strcat(strcat(listWord[i].value,","), numChar);
		// char *tempVal = "done";
		printf("%s\n", tempVal);
		usleep(50000*20);
		int checkErr = write(sockfd, tempVal, sizeof(tempVal)+1);
		if (checkErr<1)
		{
			printf("error\n");
		}
		// free(tempVal);
	}
	printf("Current size : %d\n", --currentSize);
	usleep(50000*20);
	char *tempVal = "<done>";
	write(sockfd, tempVal, sizeof(tempVal));

	while(1){

		char c;
		read(sockfd, &c, sizeof(c));
		if (c == 'd')
		{
			printf("Original node has received everything\n");
			break;
		}
	}

	char c;
	printf("closing\n");
	shutdown(sockfd, SHUT_RDWR);
	while((read(sockfd, &c, sizeof(c))) > 0);
	close(sockfd);
	printf("done close\n");
	return 0;
}