/* Lock free queue implementation of "FasterFlow", Kevin Hammond, 18th September 2013
   divider idea taken from a a Dr Dobbs article
   There is currently no way to free queue nodes (probably needs another level of indirection
   in the allocator).

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include "queue.h"

extern int payload(int t);
extern void exit(int x);

struct tq *newtq() {
  struct tq *tq = (struct tq *)malloc(sizeof(struct tq));
  tq->addat = 0; tq->removeat = 0;
  return(tq);
}

//#define cas(reg,orig,new) ((reg==orig)? (orig = new, 1): 0)
inline int cas(int old, int *ptr,int new) {
  unsigned char res;
  __asm__ __volatile__(     "lock\n"
			    "cmpxchgl %2,%1\n"
			    "sete %0\n" :
			    "=q" (res), "=m" (*ptr):
			     "r" (new), "m" (*ptr), "a" (old)  :
			     "memory" ) ;
  return(res);
}

int gettask(struct tq *tq) {
  int t, r, r1;

  //  fprintf(stderr,"getting at %d (%x)\n",tq->removeat,tq);

  do {
    // make sure queue is non-empty
    r = tq->removeat;
    if (r == tq->addat)
      continue;

    // extract task
    t = tq->ts[r];

    // calculate new value of r
    r1 = r+1;
    if (r1 == MAX_TASKS)
      r1 = 0;

    // atomically exchange to update queue
    if(cas(r,&tq->removeat,r1))
       break;

  } while(1);

  //  fprintf(stderr,"got %d at %d (%x)\n",t,r,tq);

  return(t);
}


void puttask(struct tq *tq,int v) {
  int a, a1;

  //  fprintf(stderr,"putting %d @ %d (%x)\n",v,tq->addat,tq);

  do {
    a = tq->addat;
    if (a == tq->removeat-1)
      continue;

    a1 = a+1;
    if(a1 == MAX_TASKS)
      a1 = 0;

    // update with the new task
    // there is a race condition here - dcas or a data structure would be better!
    tq->ts[a] = v;
    if(cas(a,&tq->addat,a1)) {
      //      tq->ts[a] = v;
      break;
    }
  } while(1);

  //  fprintf(stderr,"put %d @ %d (%x)\n",v,a,tq);
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

