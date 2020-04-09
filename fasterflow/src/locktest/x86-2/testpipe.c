#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "farm.h"
#define NUM_THREADS	4

#define MAX_TASKS 100

struct tq *tq1, *tq2, *tq3;

int payload(int v);

definefarmworkerfun(pipe1,payload,tq1,tq3)
definefarmworkerfun(pipe2,payload,tq3,tq2)

int main(int argc, char *argv[])
{
  int i, ntasks=0;

  tq1=newtq();
  tq2=newtq();
  tq3=newtq();

  createpipe(pipe1,pipe2);

  //  createfarm(farm_payload,NUM_THREADS);

/*   //  puttask(tq1,45); */
/*   for(i=0; i < 10; ++i) { */
/*     puttask(tq1,35); */
/*     puttask(tq1,40); */
/*     puttask(tq1,10); */
/*     puttask(tq1,20); */
/*     puttask(tq1,25); */
/*     puttask(tq1,18); */
/*     ntasks += 6; */
/*   } */

/*   puttask(tq1,40); ++ntasks; */
/*   puttask(tq1,40); ++ntasks; */
   puttask(tq1,2); ++ntasks;
   puttask(tq1,3); ++ntasks;
   puttask(tq1,4); ++ntasks;

  //  {int i; for(i=0; i < 1; ) ; }

  for(i = 0; i < ntasks; ++i)
    fprintf(stderr,"res=%d\n",gettask(tq2));

  /* Last thing that main() should do */
  //  pthread_exit(NULL);
}


