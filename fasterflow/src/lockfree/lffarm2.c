/* Lock free queue implementation of "FasterFlow", Kevin Hammond, 18th September 2013
   divider idea taken from a a Dr Dobbs article
   There is currently no way to free queue nodes (probably needs another level of indirection
   in the allocator).  Also, the allocation buffer is a fixed size.

   Finally, there are two successive cas's in qputlf that I'm suspicious of.

   This version uses the GCC sync_* primitive to implement CAS, which is probably more reliable than direct ASM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include "queue2.h"

extern int payload(int t);
extern void exit(int x);
inline void qputlf(Queue q,Value v);
inline Value qgetlf(Queue q);

inline int cas(long int old, long int *ptr,long int new) {
  return(__sync_val_compare_and_swap(ptr,old,new)==old);
  /*
  // !__sync_bool_compare_and_swap(p, 0, 1)) 
  unsigned char res;
  //  return(1);
  __asm__ __volatile__(     "lock\n"
			    "cmpxchgl %2,%1\n"
			    "sete %0\n" :
			    "=q" (res), "=m" (*ptr):
			     "r" (new), "m" (*ptr), "a" (old)  :
			    "memory", "cc" ) ;
  return(res);
  */
}

#if 0
inline int dcas(int old1, int old2, int *ptr,int new1, int new2) {
  unsigned char res;
  __asm__ __volatile__(     "lock\n"
			    "cmpxchg16l %2,%1\n"
			    "sete %0\n" :
			    "=q" (res), "=m" (*ptr):
			     "r" (new1), "m" (*ptr), "a" (old1)  :
			     "memory" ) ;
  return(res);
}

inline void xchg(int *ptr,int new) {
  __asm__ __volatile__(     "xchgl %1,%0\n":
			     "=m" (*ptr):
			    "r" (new), "m" (*ptr) :
			     "memory" ) ;
}

typedef unsigned long int atom_t;

unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
{
  unsigned char
    cas_result;

  __asm__ __volatile__
  (


    "lock;"          // make cmpxchg8b atomic
    "cmpxchg8b %0;"  // cmpxchg8b sets ZF on success
    "setz      %1;"  // if ZF set, set cas_result to 1
    // output
    : "+m" (*destination), "=q" (cas_result)

    // input
    : "m" (*destination), "a" (*compare), "d" (*(compare+1)), "b" (*exchange), "c" (*(exchange+1))
    // clobbered
    : "cc" // , "a", "d"
  );

  return( cas_result );
}

__attribute__((always_inline))
static __inline__
int atomic_cas64(void volatile* dest,
                 void* xcmp,
                 void const* xxchg)
{
   int rc;

   __asm__ __volatile__ (
    "mov %3, %%esi            ;\n" // exchange
    "mov 0(%%esi), %%ebx      ;\n" // exchange low
    "mov 4(%%esi), %%ecx      ;\n" // exchange high

    "mov %2, %%esi            ;\n" // comparand
    "mov 0(%%esi), %%eax      ;\n" // comparand low
    "mov 4(%%esi), %%edx      ;\n" // comparand high

    "mov %1, %%esi            ;\n" // destination
    "lock cmpxchg8b (%%esi)      ;\n"
    "jz   1f                   ;\n"
    "mov %2, %%esi            ;\n" // comparand
    "mov %%eax, 0(%%esi)      ;\n" // comparand low
    "mov %%edx, 4(%%esi)      ;\n" // comparand high
  "1:      mov $0, %0               ;\n"
    "setz %b0                  ;\n" // rc =
    : "=&a" (rc)
    : "m" (dest), "m" (xcmp), "m" (xxchg)
    : "cc", "memory", "edx", "ebx", "ecx", "esi"
    );

   return rc;
}


#define atomic_dwcas atomic_cas64
#endif

int gettask(tq tq) {
  int t;

  //  fprintf(stderr,"getting at (%x)\n",tq);

  t = qgetlf(tq);

  //  fprintf(stderr,"got %d at (%x)\n",tq);

  return(t);
}


void puttask(tq tq,int v) {

  //  fprintf(stderr,"putting %d @ (%x)\n",v,tq);

  qputlf(tq,v);

  // fprintf(stderr,"put %d @ (%x)\n",v,tq);
}

void worker(tq tqi, tq tqo) {
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

void sworker(tq tq1, tq tq2) {
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

struct Alloc {
  int next;
  void *buf[MAX_TASKS*100];
} alloc;

typedef struct Alloc *Alloc;

//Alloc alloc[MAX_THREADS];

void *new(size_t size) {
  //  long tid = pthread_self();
  //  fprintf(stderr,"tid = %d\n",tid);
  //  Alloc a = alloc[tid];
  Alloc a = &alloc;
  int next, next1;

  do {
    next = a->next;
    next1 = next+size;
  } while(!cas(next,&a->next,next1));

  return(&a->buf[next]);
}

Node newNode(Value v) {
  Node n = new(sizeof(*n));
  n->value = v;
  n->next = NULL;
  return(n);
}

//Node newNodePtr(Node n) {
//  return((NodePtr)new(n));
//}

Queue newQueue () {
  Queue q = (Queue)malloc(sizeof(struct Queue));
  //  q->first = q->last = NULL;
  // Create dummy node to indicate end of queue
  q->first = q->last = q->divider = newNode(0);
  q->last->next = NULL;
  return(q);
}

tq newtq() {
  tq tq = newQueue();
  return(tq);
}

// Queue is empty when 
//inline int qempty(Queue q) {
//  return(q->first->next==NULL);
//}


inline Value qgetlf(Queue q) {
  Node divider, last;
  Value result;

  do {
  loop:
    divider = q->divider;
    last = q->last;

    // check for empty queue
    if(divider == last)
      //    continue;
      // This should be equivalent to a continue, but the continue segfaults!
      goto loop;

    //    fprintf(stderr,"divider is %x(%x), last is %x\n",divider,q->divider,last);
    result = divider->next->value;

  }  while (!cas(divider,&q->divider,divider->next));

  //  fprintf(stderr,"result is %d, divider is %x\n",result,divider);
  return(result);
}


inline void qputlf(Queue q,Value v) {
  Node last, next, new;
  //  exit(1);

  do {
    last = q->last;
    next = last->next;
    new  = newNode(v);

    // I'm not convinced by this!  Only works if there's only one producer, surely
    // (splits two cas operations, so we could conceivably overwrite the next node)
    if(!cas(next,&last->next,new))
      continue;
  }
  while(!cas(last,&q->last,last->next));

  return;
  Node divider = q->divider;

  // cleanup.  CAS is needed since there may be multiple producers...
  {
    Node first = q->first;
    while(first != q->divider) {
      if(!cas(first,&q->first,first->next))
	continue;
      // dispose of first node...
      first = q->first;
    }
  }
  
  // last should point to last value in the queue, but this might move so require another CAS. That's Awkward.
  //  xchg(q->last,n);
  /*
  do {
    last = q->last;
  } while(!cas(last,&q->last,n)); // a simple xchg would probably work
  */
}
