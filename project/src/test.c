#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Global.h"
#include <unistd.h>
#include <time.h>
int main(int argc, char const *argv[])
{
	clock_t t;
	t = clock();
	char file_name[25];
	FILE *fp;
	if (argc == 2)
	{
		fp = fopen(argv[1], "r");
	}else{
		// printf("Enter name of a file you wish to see\n");
		// fgets(file_name, sizeof(file_name), stdin);
		printf("open randomTextFile\n");
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

	for (i = 0; i < currentSize; ++i)
	{
		printf("%s,%d\n", listWord[i].value, listWord[i].number);
	}
	t = clock() -t;
	double time_taken = ((double) t) / CLOCKS_PER_SEC;
	printf("%f seconds to execute first \n", time_taken); 
	return 1;
}
//672.034925