#ifndef FARMCONVOLUTION_H
#define FARMCONVOLUTION_H

/* #include <pthread.h>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <ff/ocl/mem_man.h>
*/

#include <pthread.h>
#include <semaphore.h>

#define MAX_TASKS 1000000

int width; // =2048;
int height; //=2048;
int maskWidth=8;
int maskHeight=8;
int worker_width=8;
int worker_height=8;
// cl_uint2 inputDimensions = {width    , height};
// cl_uint2 maskDimensions  = {maskWidth, maskHeight};
unsigned int inputDimension;
unsigned int maskDimension;
int quiet=1;
int nworkers;
int nworkers2;
int gnworkers;
const int max_strlen=10;
//using namespace ff;

struct task_t2{
  task_t2(){
    //inpt= new cl_uint[ width * height];
    outpt= new unsigned short[width*height];
    msk=new float[maskWidth * maskHeight];
  }
  //cl_uint* inpt;
 //cl_uint* outpt;
  unsigned short *inpt;
  unsigned short *outpt;
  float* msk;
};

struct t_char {
  volatile char lock;

  // used to avoid spinlocks
  pthread_cond_t lockcond;
  pthread_mutex_t lockmutex;

  // used for notification locks
  pthread_cond_t notifycond;
  pthread_mutex_t notifymutex;
  
  sem_t sem;
  sem_t locksem;

  int addat;
  int removeat;
  char* ts[MAX_TASKS];
};

struct t_task_t {
  volatile char lock;

  // used to avoid spinlocks
  pthread_cond_t lockcond;
  pthread_mutex_t lockmutex;

  // used for notification locks
  pthread_cond_t notifycond;
  pthread_mutex_t notifymutex;
  
  sem_t sem;
  sem_t locksem;

  int addat;
  int removeat;
  task_t2* ts[MAX_TASKS];
};


struct tq {
  volatile char lock;
  pthread_cond_t lockcond;
  pthread_mutex_t lockmutex;
  
  // used for notification locks
  //  pthread_condattr_t notifycondattr;
  pthread_cond_t notifycond;
  pthread_mutex_t notifymutex;
  
  sem_t sem;
  sem_t locksem;
  
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
extern char* gettask1(struct t_char *tq );
extern task_t2* gettask2(struct t_task_t *tq );
extern void puttask1(struct t_task_t *tq,task_t2* v);
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
  
#endif
