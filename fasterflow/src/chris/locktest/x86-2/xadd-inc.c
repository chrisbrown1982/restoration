volatile unsigned foo = 0 ;
 
inline void my_increment()
{
  unsigned old_value ;
  unsigned add_value = 1 ;
 
  __asm__ __volatile__( "lock ; xaddl %0,%1\n\t" :
			"=r"(old_value), "+m" (foo) :
			"0" (add_value) :
			"cc" ) ;
  }
 
void my_lock( int tid_ignored )
{
}
 
void my_unlock( int tid_ignored )
{
}
