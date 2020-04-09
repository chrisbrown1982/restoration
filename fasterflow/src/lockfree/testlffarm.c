#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#define NUM_THREADS	16
//#define NUM_THREADS	1

// #define MAX_TASKS 100

struct tq *tq1, *tq2;

int payload(int v);

definefarmworkerfun(farm_payload,payload,tq1,tq2)

int main(int argc, char *argv[])
{
  int i, ntasks=0;

  tq1=newtq();
  tq2=newtq();

  createfarm(farm_payload,NUM_THREADS);

#if 1
  //   {int i; for(i=0; i < 1; ) ; }

  //  puttask(tq1,45);
  for(i=0; i < 20; ++i) {
    puttask(tq1,35);
    puttask(tq1,40);
    puttask(tq1,10);
    puttask(tq1,20);
    puttask(tq1,25);
    puttask(tq1,18);
    ntasks += 6;
  }
#endif

  puttask(tq1,40); ++ntasks;
  //  puttask(tq1,40); ++ntasks;

  // {int i; for(i=0; i < 1; ) ; }

  //  fprintf(stderr,"getting from (%x)\n",tq2);

  for(i = 0; i < ntasks; ++i)
    fprintf(stderr,"res=%d\n",gettask(tq2));

  /* Last thing that main() should do */
  //  pthread_exit(NULL);
}


