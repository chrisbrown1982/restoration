#include "farm.h"
 
void my_lock( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
 
   do {
      // use an xchg to obtain a lock
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;

      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
	break;

      // otherwise wait until signalled
      pthread_cond_wait(&tq->lockcond,&tq->lockmutex);

      // don't assume we have the lock yet...
      
   } while ( 1 ) ;
}
 

void my_unlock( struct tq *tq )
{
   tq->lock = 0 ;

   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);

   // this is safer, but may still be unnecessary
   pthread_cond_broadcast(&tq->lockcond);
}
