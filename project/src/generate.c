#include <stdio.h>
#include <stdlib.h> 

int main ()
{
   FILE * fp;
   /* open the file for writing*/
   fp = fopen ("binary/data/randomTextFile.txt","w");
   //97-122 asii
   /* write 10 lines of text into the file stream*/
   int temp;
   unsigned long i, count, count2;
   count =1;
   //858993460 1.1
   int count3 = 0;
   for(i = 0; i < 85899346;i++){
      if (count%4 == 0)
      {
         count3++;
         if (count3 != 80)
         {
            fprintf(fp, " ");
         }
      }
      if (count3==80)
      {
         count3 = 0;
         fprintf(fp, "\n");
      }
      temp = rand()%10;
      temp+=97;
      fprintf (fp, "%c",temp);

      count++;
   }
 
   /* close the file*/  
   fclose (fp);
   return 0;
}