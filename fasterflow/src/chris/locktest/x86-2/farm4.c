#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <sys/syscall.h>
#include <pthread.h>
#include "farm2.h"

extern int payload(int t);
extern void my_lock( struct tq *tq );
extern void my_unlock( struct tq *tq );
extern void exit(int x);

struct tq *newtq() {
  struct tq *tq = (struct tq *)malloc(sizeof(struct tq));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  pthread_cond_init(&tq->notifycond,NULL); // NULL?

  pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

int gettask(struct tq *tq) {
  int t;

  //  fprintf(stderr,"getting\n");

  do {
    my_lock(tq);

    if (tq->removeat == tq->addat) {
      my_unlock(tq);
      //      fprintf(stderr,"waiting...\n");
      pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
      // fprintf(stderr,"finished waiting\n");
    }
  }  while (tq->removeat == tq->addat);

  t = tq->ts[tq->removeat];

  if (++tq->removeat == MAX_TASKS)
    tq->removeat = 0;

  my_unlock(tq);

  //  fprintf(stderr,"got %d\n",t);

  return(t);
}

// The notification process is highly time sensitive -- there could be races in here...  KH
void puttask(struct tq *tq,int v) {
  int a, r;

  //  fprintf(stderr,"putting %d\n",v);

  do {
    my_lock(tq);

    if (tq->addat == tq->removeat-1) 
      my_unlock(tq); // exit(0);
  } while (tq->addat == tq->removeat-1);

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...
  my_unlock(tq);

  // signal if there are blocked tasks
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    //    fprintf(stderr,"unblocking...\n");

    // wake one of the blocked threads
    pthread_cond_signal(&tq->notifycond);
    // fprintf(stderr,"finished waiting\n");
  }
  //  fprintf(stderr,"put %d\n",v);
}

void worker(struct tq *tqi, struct tq *tqo) {
  int t,r;
  //    long tid = syscall(SYS_gettid);
  //  long tid = gettid();
  long tid = (long) pthread_self();

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

void sworker(struct tq *tq1, struct tq *tq2) {
  while(1) {
    worker(tq1,tq2);
  }
}

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

// use definefarmworker to create a suitable worker to be farmed

void makeThread(void (*Worker)(),long t) {
  int rc;

  pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

  rc = pthread_create(pt, NULL, (void *(*)(void *))Worker, (void *)t);

  if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void createfarm(void (*Worker)(),int n) {
  int rc;
  long t;

  for(t=0;t<n;t++)
    makeThread(Worker,t);
}

void createpipe(void (*Worker1)(),void (*Worker2)()) {
    makeThread(Worker1,0);
    makeThread(Worker2,1);
}

