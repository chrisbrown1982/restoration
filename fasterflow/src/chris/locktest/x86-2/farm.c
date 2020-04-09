#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>

extern int payload(int t);
extern void my_lock( int *tid );
extern void my_unlock( int *tid );
extern void exit(int x);

#define MAX_TASKS 100

void inittq(int tq[]) {
  tq[0] = 2; tq[1] = 0;
}


int gettask(int tq[]) {
  int t,  n,  ntasks;

  do {
    my_lock(tq);
    n = tq[0];
    ntasks = tq[1];

    if (ntasks == 0)
      my_unlock(tq);
  }  while(ntasks == 0);

  if (n == 2)
    n = MAX_TASKS+2;

  t = tq[--n];

  //  if (n == MAX_TASKS+2)
  //    n = 2;

  tq[0] = n;
  --tq[1];
  my_unlock(tq);
  return(t);
}

void puttask(int tq[],int v) {
  int n, ntasks;

  my_lock(tq);
  n = tq[0];
  ntasks = tq[1];

  if (ntasks == MAX_TASKS) exit(0);

  tq[n++] = v;
  if (n == MAX_TASKS+2)
    n = 2;

  tq[0] = n;
  ++tq[1];
  my_unlock(tq);
}

void worker(int tqi[], int tqo[]) {
  int t,r;
  //    long tid = syscall(SYS_gettid);
  //  long tid = gettid();
   long tid = pthread_self();

  //  while(1) {
  {
    //    fprintf(stderr,"%x: %x, %x\n",tid,tqi,tqo);
    //    fprintf(stderr,"%x: w1\n",tid);
    t = gettask(tqi);
    //    fprintf(stderr,"%x: w2 %d\n",tid, t);
    r = payload(t);
    //    fprintf(stderr,"%x: w3 %d\n",tid, r);
    puttask(tqo,r);
    //    fprintf(stderr,"%x: w4\n",tid);
  }
}

/* void *Worker(void *threadid) { */
/*   while(1) { */
/*     worker(tq1,tq2); */
/*   } */
/* } */

/* void *Worker(void *threadid) */
/* { */
/*   long tid,tid2; */
/*   tid = (long)threadid; */
/*   tid2 = pthread_self(); */
/*   // printf("Hello World! It's me, thread #%ld (%x)!\n", tid, tid2); */
 
/*   while(1) { */
/*     worker(tq1,tq2); */
/*   } */
/*   pthread_exit(NULL); */
/* } */

// pthread_t threads[NUM_THREADS];

void createthreads(void *(*Worker)(),int n) {
  int rc;
  long t;
  for(t=0;t<n;t++){
    pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));
    //    rc = pthread_create(&Threads[t], NULL, bWorker, (void *)t);
    rc = pthread_create(pt, NULL, Worker, (void *)t);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}
