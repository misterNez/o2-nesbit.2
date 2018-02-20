#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct Data
{
   int flag[5];
   char buff[5][200];
} Data;


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

               Data data;
               Data* p_data;
               p_data = &data;
               int key = 123456789;
               int shmid = shmget(key, sizeof(data), IPC_CREAT | 0666);
               p_data = shmat(shmid, NULL, 0);
	      
               p_data->flag[0] = 1;
               strcpy(p_data->buff[0], "Hello");

               printf("%d %s\n", p_data->flag[0], p_data->buff[0]);

               //char (*buffers)[5][200];
               //int (*flags)[5];

               //int flag_id;
               //int flag_key = 12345678;
               //flag_id = shmget(flag_key, sizeof(*flags), IPC_CREAT | 0666);
               //flags = shmat(flag_id, 0, 0);
               //*flags[0] = 55;
               //printf("%d\n", *flags[0]);

               //int buff_id;
               //int buff_key = 87654321;
               //buff_id = shmget(buff_key, sizeof(*buffers), IPC_CREAT | 0666);
               //buffers = shmat(buff_id, 0, 0);
               //strcpy(*buffers[0], "Hello");
               //printf("%s\n", *buffers[0]);

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
  
               //shmdt(flags);
               shmdt(p_data);
               shmctl(shmid, IPC_RMID, 0); 
               //shmctl(buff_id, IPC_RMID, 0);            
               
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
