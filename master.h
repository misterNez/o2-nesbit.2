#ifndef MASTER_H_
#define MASTER_H_


typedef enum {idle, want_in, in_cs} state;

typedef struct Data
{
   int turn;
   state sFlag[100];
   int bFlag[5];
   char buff[5][200];
   int count;
   int lines;
   int done;
} Data;


#endif
