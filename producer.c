#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

typedef struct Data
{
   int flag[5];
   char buff[5][200];
} Data;

int main(int argc, char* argv[]) {

   FILE* fp;

   Data data;
   Data* p_data;
   p_data = &data;
   int key = 123456789;
   int shmid = shmget(key, sizeof(data), 0666);
   p_data = shmat(shmid, NULL, 0);

   printf("I am producer %ld. n = %d, index = %d, buff[0] = %s, flag[0] = %d\n", 
                  (long)getpid(), atoi(argv[1]), atoi(argv[2]), p_data->buff[0], p_data->flag[0]);
   sleep(5);
   return 0;
}
