#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	2

#define MAX_TASKS 100

int tq1[MAX_TASKS+2];
int tq2[MAX_TASKS+2];

void *Worker(void *threadid) {
  extern void worker(int tqi[],int tqo[]);

  while(1) {
    worker(tq1,tq2);
  }
}

int main(int argc, char *argv[])
{
  int i, ntasks=0;

  inittq(tq1);
  inittq(tq2);

  createthreads(Worker,NUM_THREADS);
  //  puttask(tq1,45);
  /*
    for(i=0; i < 10; ++i) {
    puttask(tq1,35);
    puttask(tq1,10);
    puttask(tq1,20);
    puttask(tq1,25);
    puttask(tq1,18);
    ntasks += 5;
  }
  */

  puttask(tq1,40); ++ntasks;
  puttask(tq1,40); ++ntasks;

  //  {int i; for(i=0; i < 1; ) ; }

  for(i = 0; i < ntasks; ++i)
    fprintf(stderr,"res=%d\n",gettask(tq2));

  /* Last thing that main() should do */
  //  pthread_exit(NULL);
}
