#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include "master.h"

//Start of producer program
int main (int argc, char* argv[]) {

   //File variables
   FILE* fpRead;
   FILE* fpWrite;
 
   //Open the log file
   fpWrite = fopen("prod.log", "a");

   //Shared memory
   Data data;
   Data* b_data;
   b_data = &data;
   int key = 123456789;
   int shmid = shmget(key, sizeof(data), 0666);
   b_data = shmat(shmid, NULL, 0);

   char line[200];   //Array to hold line read
   int len;          //Int to hold length of line.
   int i;	     //Int used to iterate over buffer array.

   //Time stuff
   time_t timer;
   char tString[9];
   struct tm* tm_info;
 
   time(&timer);
   tm_info = localtime(&timer);
   strftime(tString, 9, "%H:%M:%S", tm_info);
   fprintf(fpWrite, "%s Started\n", tString);

   //Seed rand()
   srand(time(0));

   //Open the read file
   fpRead = fopen("text.txt", "r");

   //Try to fill a buffers
   fprintf(stdout, "%s: Computing...\n", argv[0]);
   do {
      time(&timer);
      tm_info = localtime(&timer);
      strftime(tString, 9, "%H:%M:%S", tm_info);
      fprintf(fpWrite, "%s Check\n", tString);
      //Traverse buffers
      for (i = 0; i < 5; i++) {
         //Check if empty
         if (b_data->bFlag[i] == 0) {
            //If not end of file:
            if ( fgets(line, sizeof(line), fpRead) != NULL ) {
               //Do work
               b_data->lines++;
               time(&timer);
	       tm_info = localtime(&timer);
               strftime(tString, 9, "%H:%M:%S", tm_info);
               fprintf(fpWrite, "%s Write    %d    %s", tString, i, line);
               strcpy(b_data->buff[i], line);
               b_data->bFlag[i] = 1;
            }
            //Else end the program
            else {
               b_data->done = 1;
               time(&timer);
	       tm_info = localtime(&timer);
               strftime(tString, 9, "%H:%M:%S", tm_info);
               fprintf(fpWrite, "%s Terminated\tNormal\n", tString);
               fclose(fpWrite);
               fclose(fpRead);
               shmdt(b_data);
               return 0;
            }
         }
      }

      //Sleep
      int r = (rand()%5 + 1);
      time(&timer);
      tm_info = localtime(&timer);
      strftime(tString, 9, "%H:%M:%S", tm_info);
      fprintf(fpWrite, "%s Sleep    %d\n", tString, r);
      sleep(r);
   
   } while (1);

   return 1;
}
//End of producer program
