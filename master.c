#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "master.h"

//State of master process
int main(int argc, char* argv[]) 
{
   int c = 0;     //Option variable
   int n = 10;    //default number of consomers

   //Check for invalid number of arguments
   if (argc != 2 && argc != 3) 
   {
      fprintf(stderr, "%s: Error: Invalid number of arguments\n", argv[0]);
      return -1;
   }

   //Two args:
   else if (argc == 2) 
   {
      //Check options
      while ((c = getopt(argc, argv, "h")) != -1) 
      {
         switch(c) 
         {
            //Display help menu
            case 'h':
	       fprintf(stdout, "%s: Help : ./master -n [NUMBER OF PRODUCERS]", argv[0]);
               break;

            case ':':
               fprintf(stderr, "");
               break;

            case '?':
               fprintf(stderr, "");
         }
      }
   }

   //Three args:
   else if (argc == 3) 
   {
      //Check options
      while ((c = getopt(argc, argv, "n:")) != 1) 
      {
         switch(c) 
         {
            //Start of main computation:
            case 'n':
               //Update number of consumers
               if (atoi(argv[2]) > 0)
                  n = atoi(argv[2]);
               
               //Local int variables
               int index = 0;
	       int counter = 0;
               int status = 0;
               int limit = 17;
               
               //PID variables
               pid_t childpid, donepid = 0;

               //Executable names
               char* ccmd = "./consumer";
               char* pcmd = "./producer";

               //Shared Memory
               Data data;
               Data* b_data;
               b_data = &data;
               int key = 123456789;
               int shmid = shmget(key, sizeof(data), IPC_CREAT | 0666);
               b_data = shmat(shmid, NULL, 0);
	       
               //Initialize the shared memory
               int i = 0; 
               for (i = 0; i < 5; i++) { 
                  b_data->bFlag[i] = 0;
                  strcpy(b_data->buff[i], "\0");
               }

               b_data->done = 0;

               //Create parameters for producer
               char n_str[12];
               char id_str[12];
               sprintf(n_str, "%d", n);
               sprintf(id_str, "%d", index);

               //Fork the producer child
	       childpid = fork();

               switch(childpid)
               {
                  //Check for error
                  case -1:
                     fprintf(stderr, "%s: Error: Failed to fork producer", argv[0]);
                     break;
 
                  //Produder process executes
                  case 0:
                     fprintf(stdout, "%s: Attempting to exec producer %ld\n", argv[0], (long)getpid());
                     if ( execl(pcmd, pcmd, n_str, id_str, NULL) == -1 )
		        fprintf(stderr, "%s: Error: Failed to exec producer", argv[0]);

                     break;
               }

               //Loop for the number of consumers
               for ( ; index < n; index++) 
               {
                  //Check if process limit reached
                  if (counter == limit) 
                  { 
                     fprintf(stdout, "%s: Process limit reached. Waiting...\n", argv[0]);
		     donepid = wait(&status);
                     counter--;
		     fprintf(stdout, "%s: Process %ld finished. %d child processes running.\n", argv[0], (long)donepid, counter);
                  }

                  //Fork a producer
                  childpid = fork();
                  counter++;

                  //Create parameters for consumer
                  char n_str[12];
                  char id_str[12];
                  sprintf(n_str, "%d", n);
                  sprintf(id_str, "%d", index);

                  //Check for error
                  if (childpid == -1)
                     fprintf(stderr, "%s: Error: Failed to fork consumer\n");

                  //Consumer process executes
                  else if (childpid == 0) 
                  {
                     fprintf(stdout, "%s: Attempting to exec consumer %ld\n", argv[0], (long)getpid());
                     if (execl(ccmd, ccmd, n_str, id_str, NULL) == -1)
                        fprintf(stderr, "%s: Error: Failed to exec consumer\n");
                     exit(0);
                  }

	       }

               //Master process waits for all children
               fprintf(stdout, "%s: Waiting...\n", argv[0]);
               while ((donepid = wait(&status)) > 0) 
               {
                  counter--;
                  fprintf(stdout, "%s: Process %ld finished.\n", argv[0], (long)donepid);
               }
  
               //Delete shared memory
               shmdt(b_data);
               shmctl(shmid, IPC_RMID, 0);
               
               //End program
               return 0;;

            case ':':
               fprintf(stderr, "");
               break;

            case '?':
               fprintf(stderr, "");
         } 
         return 1;
      }
   }
   return 2;
}
