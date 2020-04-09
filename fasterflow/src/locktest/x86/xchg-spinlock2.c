#include "farm.h"
 
void my_lock( struct tq *tq )
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
 

void my_unlock( struct tq *tq )
{
   tq->lock = 0 ;
}
