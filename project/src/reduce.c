#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "Global.h"
#include <time.h>

int main(int argc, char const *argv[])
{
	clock_t start, end;
    double time_taken;
    start = clock();
	char file_name[25];
	FILE *fp;
	if (argc == 2)
	{
		fp = fopen(argv[1], "r");
	}else{
		// printf("Enter name of a file you wish to see\n");
		// fgets(file_name, sizeof(file_name), stdin);
		fp = fopen("7000.txt", "r"); // read mode
	}


	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	char ch;
	valuenumber *listWord;
	int i = 0,j =0, currentSize = 0, currentMem = 0;
	char *val = NULL;
	int num;
	bool first = true, newWord = true, last, isEOF;
	isEOF = false;
	while(1){
		if ((ch = fgetc(fp)) != EOF)
		{
			if (ch == ',')
			{
				ch = fgetc(fp);
				num = ch - '0';
				// printf("%d\n", num);
				while(1){
					if ((ch = fgetc(fp)) != '\n')
					{
						if (ch == EOF)
						{
							isEOF = true;
							break;
						}
						num *= 10;
						num = num + (ch - '0');
					}else{
						newWord = true;
						break;
					}

				}

				int check = 0,z;
				if (val != NULL){

					if (first)
					{
						
						listWord = (valuenumber *) malloc(1* sizeof(valuenumber));
						first = false;
						listWord[currentSize].value = (char *) malloc((strlen(val)+1) * sizeof(char));
						strcpy(listWord[currentSize].value, val);
						listWord[currentSize].number = num;
						currentSize++;
					}else{
						for (z = 0; z < currentSize; ++z)
						{
							//compare every string in the struct with new string
							// printf("%d %s\n",z ,  listWord[z].value);
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
							int tempSize = currentSize+1;
							listWord = realloc(listWord, tempSize*sizeof(valuenumber));

							listWord[currentSize].value = (char *) malloc((strlen(val)+1) * sizeof(char));
							strcpy(listWord[currentSize].value, val);
							listWord[currentSize].number = num;
							currentSize++;
						}
					}
				}

				if (isEOF)
				{
					break;
				}
			}else{
				if (newWord)
				{
					free(val);
					val = (char *) malloc(2* sizeof(char));
					newWord = false;
					j=0;
					currentMem = 2;
				}else{
					currentMem++;
					val = realloc(val, currentMem);
				}
				val[j]=ch;
				// printf("%s\n", val);
				j++;
			}
		}else{
			break;
		}
	}

	fclose(fp);	

	for (i = 0; i < currentSize; ++i)
	{
		printf("%s,%d\n", listWord[i].value, listWord[i].number);
	}
	end = clock();
	time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("%f seconds to execute second \n", time_taken); 
	return 0;

}