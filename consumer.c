#include <stdio.h>

int main (int argc, char* argv[]) {
   printf("I am consumer %ld. n = %d, index = %d\n", (long)getpid(), atoi(argv[1]), atoi(argv[2]));
   return 0;
}
