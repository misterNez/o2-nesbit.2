#include <stdio.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
  printf("I am producer %ld. n = %d, index = %d\n", (long)getpid(), atoi(argv[1]), atoi(argv[2]));
  sleep(5);
  return 0;
}
