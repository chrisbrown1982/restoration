#define MAX_TASKS 100

struct tq {
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

// void my_decrement( struct tq *tq );
// void my_increment( struct tq *tq );
