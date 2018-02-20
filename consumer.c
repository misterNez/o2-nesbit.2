#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>

typedef struct Data
{
   int flag[5];
   char buff[5][200];
} Data;

int main (int argc, char* argv[]) 
{
   FILE* fp1;
   FILE* fp2;

   Data data;
   Data* p_data;
   p_data = &data;
   int key = 123456789;
   int shmid = shmget(key, sizeof(data), 0666);
   p_data = shmat(shmid, NULL, 0);

   //char* (*buffers)[5];
   //int (*flags)[5];

   //int flag_id;
   //int flag_key = 12345678;
   //flag_id = shmget(flag_key, sizeof(*flags), 0666);
   //flags = shmat(flag_id, 0, 0);

   //int buff_id;
   //int buff_key = 87654321;
   //buff_id = shmget(buff_key, sizeof(*buffers), 0666);
   //buffers = shmat(buff_id, 0, 0);

   //strcpy(*buffers[0], "World");

   printf("I am consumer %ld. n = %d, index = %d, buff[0] = %s, flag[0] = %d\n", 
                  (long)getpid(), atoi(argv[1]), atoi(argv[2]), p_data->buff[0], p_data->flag[0]);
   
   fp1 = fopen("master.log", "a");
   fprintf(fp1, "%ld   %d   %s\n", getpid(), atoi(argv[2]), p_data->buff[0]);

   return 0;
}
