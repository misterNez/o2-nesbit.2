#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include "master.h"

//Start of consumer program
int main (int argc, char* argv[]) {
   //File variables
   FILE* fp1;
   FILE* fp2;

   //Shared memory
   Data data;
   Data* b_data;
   b_data = &data;
   int key = 123456789;
   int shmid = shmget(key, sizeof(data), 0666);
   b_data = shmat(shmid, NULL, 0);

   //Local variables
   int r = 0;                //Used for random time
   int c = 0;                //used to iterate over buffer array
   int j = 0;                //Used for concurrency
   int n = atoi(argv[1]);    //Total number of consumers
   int i = atoi(argv[2]);    //Index number of this consumer

   //Create file names
   char file2[16];
   snprintf(file2, sizeof(file2), "cons%d.log", i);
   fp2 = fopen(file2, "a");

   char* file1 = "mast.log";

   //Time stuff
   time_t timer;
   char tString[9];
   struct tm* tm_info;

   //Write
   time(&timer);
   tm_info = localtime(&timer);
   strftime(tString, 9, "%H:%M:%S", tm_info);
   fprintf(fp2, "%s Started\n", tString);

   //Seed rand
   srand(time(0));

   //Start of multiple process solution
   fprintf(stdout, "%s, %d: Computing...\n", argv[0], i);
   do {
      //Sleep
      r = (rand()%5 + 1);
      time(&timer);
      tm_info = localtime(&timer);
      strftime(tString, 9, "%H:%M:%S", tm_info);
      fprintf(fp2, "%s Sleep\t%d\n", tString, r);
      sleep(r);

      do {
	 b_data->sFlag[i] = want_in;   //Raise flag
	 j = b_data->turn;            //Set local variable

         //Wait for turn
         while (j != i)
            j = (b_data->sFlag[j] != idle) ? b_data->turn : (j + 1) % n;
            
         //Declare intention to enter critical section
         b_data->sFlag[i] = in_cs;

         //Check that no one else is in critical section
         for ( j = 0; j < n; j++)
            if ( (j != i) && (b_data->sFlag[j] == in_cs) )
               break;

      } while ( (j < n) || (b_data->turn != i && b_data->sFlag[b_data->turn] != idle) );

      //Assign turn to self
      b_data->turn = i;
 
      //Critical section
      for (c = 0; c < 5; c++) {
         if (b_data->bFlag[c] == 1) {
            //Write to logs
            time(&timer);
            tm_info = localtime(&timer);
            strftime(tString, 9, "%H:%M:%S", tm_info);
            fprintf(fp2, "%s Read\t%d\t%s", tString, c,  b_data->buff[c]);
            fprintf(fp2, "%s Attempt to write to master log\n", tString);
            fp1 = fopen(file1, "a");
            fprintf(fp1, "%ld\t%d\t%s", getpid(), i, b_data->buff[c]);
            fclose(fp1);
            fprintf(fp2, "%s Wrote master log\n", tString);

            //Set buffer to empty and increment count
            b_data->bFlag[c] = 0;        
            b_data->count++;
            break;
         }
      }

      //Exit section
      j = (b_data->turn + 1) % n;
      while (b_data->sFlag[i] == idle)
         j = (j + 1) % n;

      //Assign turn to next waiting process; change own flag to idle
      b_data->turn = j;
      b_data->sFlag[i] = idle;
   
   } while( (b_data->done == 0) || (b_data->count < b_data->lines) );
   //While there are still lines to written

   time(&timer);
   tm_info = localtime(&timer);
   strftime(tString, 9, "%H:%M:%S", tm_info);
   fprintf(fp2, "%s: Terminated  Normal\n", tString);

   //Close local log file
   fclose(fp2);

   shmdt(b_data);

   return 0;
}
//End of consumer program
