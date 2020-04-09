volatile char lock_word = 0 ;
 
void my_lock( int tid_ignored )
{
   char old_value ;
   char new_value = 1 ;
 
   do {
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (lock_word) :
                            "0" (new_value) :
                            "cc" ) ;
 
   } while ( 1 == old_value ) ;
}
 

void my_unlock( int tid_ignored )
{
   lock_word = 0 ;
}
