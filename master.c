#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[]) 
{
   int c = 0;
   int n = 10;

   if (argc != 2 && argc != 3) 
   {
      fprintf(stderr, "%s: Error: Invalid number of arguments\n", argv[0]);
      return -1;
   }

   else if (argc == 2) 
   {
      while ((c = getopt(argc, argv, "h")) != -1) 
      {
         switch(c) 
         {
            case 'h':
	       fprintf(stdout, "");
               break;

            case ':':
               fprintf(stderr, "");
               break;

            case '?':
               fprintf(stderr, "");
         }
      }
   }

   else if (argc == 3) 
   {
      while ((c = getopt(argc, argv, "n:")) != 1) 
      {
         switch(c) 
         {
            case 'n':
               if (atoi(argv[2]) > 0)
                  n = atoi(argv[2]);
	       int index = 0;
	       int counter = 0;
               int status = 0;
               int limit = 17;
               pid_t childpid, donepid = 0;
               char* ccmd = "./consumer";
               char* pcmd = "./producer";
	       char* buffer[5];
               int bufFlag[5];

               int i = 0;
               for ( ; i < 5; i++) 
               {
                  buffer[0] = NULL;
	          bufFlag[0] = 0;
               } 

               int flag_id;
               int flag_key = 10000;
               //flag_id = shmget(flag_key, sizeof(bufFlag), IPC_CREAT | 0600);
               //bufFlag = shmat(flag_id, NULL, 0);

               int buff_id;
               int buff_key = 20000;
               //buff_id = shmget(buff_key, sizeof(buffer), IPC_CREAT | 0600);
               //buffer = shmat(buff_id, NULL, 0);

               char n_str[12];
               char id_str[12];
               sprintf(n_str, "%d", n);
               sprintf(id_str, "%d", index);

	       childpid = fork();
               switch(childpid)
               {
                  case -1:
                     perror("producer failed to fork");
                     return 1;
 
                  case 0:
                     printf("Attempting to exec producer %ld\n", (long)getpid());
                     execl(pcmd, "producer", n_str, id_str, NULL);
                     break;

                  default:
                     while ((donepid = waitpid(-1, &status, WNOHANG)) > 0)
                        fprintf(stdout, "%s: Process %ld finished(0).\n", argv[0], (long)donepid);
               }

               for ( ; index < n; index++) 
               {
                  if (counter == limit) 
                  { 
                     fprintf(stdout, "%s: Process limit reached. Waiting...\n", argv[0]);
		     donepid = wait(&status);
                     counter--;
		     fprintf(stdout, "%s: Process %ld finished. %d child processes running.\n", argv[0], (long)donepid, counter);
                  }

                  childpid = fork();
                  counter++;
                  char n_str[12];
                  char id_str[12];
                  sprintf(n_str, "%d", n);
                  sprintf(id_str, "%d", index);

                  if (childpid == -1)
                     fprintf(stderr, "fork error\n");

                  else if (childpid == 0) 
                  {
                     printf("Attempting to exec consumer %ld\n", (long)getpid());
                     execl(ccmd, "consumer", n_str, id_str, NULL);
                     exit(0);
                  }

                  else 
                  {
                     while ((donepid = waitpid(-1, &status, WNOHANG)) > 0)
                     {
                        counter--;
                        fprintf(stdout, "%s: Process %ld finished(1).\n", argv[0], (long)donepid);
                     }
                  }
	       }

               while ((donepid = wait(&status)) > 0) 
               {
                  counter--;
                  fprintf(stdout, "%s: Process %ld finished(2).\n", argv[0], (long)donepid);
               }
            
               break;

            case ':':
               fprintf(stderr, "");
               break;

            case '?':
               fprintf(stderr, "");
         }

         return 0;
      }
   }
}
