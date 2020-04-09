#include <pthread.h>
#include <stdio.h>
#define MAX_TASKS 100

struct tq {
  volatile char lock;

  // used to avoid spinlocks
  volatile unsigned lockcount;
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

void inline my_lock( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;

   // track how many threads are blocked
   my_increment( tq );

   do {

      // use an xchg to obtain a lock
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;

      // do we have the lock?  If so, exit the loop
      if ( 0 == old_value)
	break;

      // otherwise wait until signalled
      pthread_cond_wait(&tq->lockcond,&tq->lockmutex);

      // don't assume we have the lock yet - we need to redo the xchg...
      
   } while ( 1 ) ;

   // we are not blocked, so decrement the block count
   my_decrement( tq );
}
 

void inline my_unlock( struct tq *tq )
{
   tq->lock = 0 ;

   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);

   // this is safer, but may still be unnecessary
   // pthread_cond_broadcast(&tq->lockcond);

   // this checks that there are blocked threads, before unblocking one of them
   if( tq->lockcount > 0 )
     pthread_cond_signal(&tq->lockcond);
}

inline void my_increment( struct tq *tq )
{
  __asm__ __volatile__ ( "lock ; inc %0\n\t" :
			"+m" (tq->lockcount) :
			"cc" ) ;

  //   unsigned old_value ;
   //   unsigned add_value = 1 ;
 
   //   __asm__ __volatile__( "lock ; xaddl %0,%1\n\t" :
   //                         "=r"(old_value), "+m" (tq->lockcount) :
   //                         "0" (add_value) :
   //                         "cc" ) ;
}

inline void my_decrement( struct tq *tq )
{
  __asm__ __volatile__( "lock ; inc %0\n\t" :
			"+m" (tq->lockcount) :
			"cc" ) ;
}
 

#if 0
#define makefarm(worker,n,tqi,tqo)			\
  definefarmworker(worker,tqi,tqo);			\
  createfarm(farm_ ## worker,n);

#define makepipe(worker1,worker2,tqi,tqo)		\
  struct tq *tq = newtq();				\
  definefarmworker(worker1,tqi,tq);			\
  definefarmworker(worker2,tq,tq2);			\
  createpipe(farm_ ## worker1,farm_ ## worker2);
#endif
