#define MAX_TASKS 100
#define MAX_THREADS 16

// void my_decrement( struct tq *tq );
// void my_increment( struct tq *tq );

typedef int Value;

typedef struct Node *Node;

struct Node {
  Value value;
  Node next;
};


typedef struct NodePtr *NodePtr;

struct NodePtr {
  int count;
  Node p;
};

typedef struct Queue *Queue;

struct Queue {
  Node first, divider, last;
};

typedef Queue tq;

#define definefarmworkerfun(name,worker,tqi,tqo)	\
  /* void farm_ ## worker ( ) { int t, r; */		\
  void name ( ) { int t, r; 	    \
    while (1) { \
      t = gettask(tqi);				\
      r = worker(t);				\
      puttask(tqo,r);				\
    }						\
  }

tq newtq();
int gettask(tq tq);
void puttask(tq tq,int v);
void createfarm(void (*Worker)(),int n);
void createpipe(void (*Worker1)(),void (*Worker2)());
