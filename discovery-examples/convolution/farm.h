#include <pthread.h>

#define MAX_TASKS 100

struct tq {
  volatile char lock;

  // used to avoid spinlocks
  pthread_cond_t lockcond;
  pthread_mutex_t lockmutex;

  // used for notification locks
  pthread_cond_t notifycond;
  pthread_mutex_t notifymutex;

  int addat;
  int removeat;
  int ts[MAX_TASKS];
};

#define definefarmworkerfun(name,worker,tqi,tqo)	\
  /* void farm_ ## worker ( ) { int t, r; */		\
  void name ( ) { int t, r; 	    \
    while (1) { \
      t = gettask(tqi);				\
      r = worker(t);				\
      puttask(tqo,r);				\
    }						\
  }

struct tq *newtq();
int gettask(struct tq *tq);
void puttask(struct tq *tq,int v);
void createfarm(void (*Worker)(),int n);
void createpipe(void (*Worker1)(),void (*Worker2)());

/*
void inline my_lock( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
 
   do {
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;
 
   } while ( 1 == old_value ) ;
}
 

void inline my_unlock( struct tq *tq )
{
   tq->lock = 0 ;
}
*/

#define makefarm(worker,n,tqi,tqo)			\
  definefarmworker(worker,tqi,tqo);			\
  createfarm(farm_ ## worker,n);

#define makepipe(worker1,worker2,tqi,tqo)		\
  struct tq *tq = newtq();				\
  definefarmworker(worker1,tqi,tq);			\
  definefarmworker(worker2,tq,tq2);			\
  createpipe(farm_ ## worker1,farm_ ## worker2);
