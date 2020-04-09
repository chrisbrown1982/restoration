#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
// #include "farmConvolution.h"

//#include <ff/ocl/mem_man.h>

#define MAX_TASKS 5000

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
  int ts[MAX_TASKS+1];
};


struct tq* tq;
struct tq* tq2;

// const int N=1024;

#define N 1024

unsigned long A[N][N];
unsigned long B[N][N];
unsigned long C[N][N];

struct tq *newtq() {
  struct tq *tq = (struct tq *)malloc(sizeof(struct tq));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  pthread_cond_init(&tq->notifycond,NULL); // NULL?
  sem_init (&tq->sem, 0, -1);
  sem_init (&tq->locksem, 0, 0);
  pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

void makeThread(void (*Worker)(),long t) {
  int rc;

  pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

  rc = pthread_create(pt, NULL, (void *(*)(void *))Worker, (void *)t);
  //  rc = pthread_create(pt, NULL, Worker, (void *)t);

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




inline void my_lock_tq( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
   
   do {
      // __sync_val_compare_and_swap(&tq->lock,old_value,new_value);
      old_value = __sync_lock_test_and_set(&tq->lock,1);
      // use an xchg to obtain a lock
      /* __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) : 
                            "cc" ) ;
      */
      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
        break;

      // otherwise wait until signalled
   //    fprintf (stderr, "(%p,%d) waiting for condition\n", tq, pthread_self());
      //pthread_cond_wait(&tq->lockcond,&tq->lockmutex);
      sem_wait(&tq->locksem);

    //  fprintf (stderr, "(%p,%d) finished waiting for condition\n", tq, pthread_self());

      // don't assume we have the lock yet...

   } while ( 1 ) ;
}

inline void my_unlock_tq( struct tq *tq )
{
   // tq->lock = 0 ;
   __sync_lock_release(&tq->lock);
   
   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);
   
   // this is safer, but may still be unnecessary
   // pthread_cond_broadcast(&tq->lockcond);
   // pthread_mutex_unlock(&tq->lockmutex);
   sem_post(&tq->locksem);
}

int gettask1(struct tq *tq) {
  int t;

  // fprintf(stderr,"getting\n");

  do {
    my_lock_tq(tq);

    if (tq->removeat == tq->addat) {
      my_unlock_tq(tq);
      // fprintf(stderr,"waiting...\n");
      pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
     //  fprintf(stderr,"finished waiting\n");
    }
  }  while (tq->removeat == tq->addat);

  t = tq->ts[tq->removeat];

  if (++tq->removeat == MAX_TASKS)
    tq->removeat = 0;

  my_unlock_tq(tq);

  pthread_mutex_unlock(&tq->notifymutex);

    // fprintf(stderr,"gettask1 got %d\n",t);

  return(t);
}

int gettask2(struct tq *tq) {
  int t;

  // fprintf(stderr,"getting\n");
 
  // original version, didn't work
  //do {
  //fprintf(stderr, "gettask2 getting lock\n");
  //my_lock_tq(tq);
  //fprintf(stderr, "gettask2, got lock\n");

  //if (tq->removeat == tq->addat) {
  //  my_unlock_tq(tq);
  // fprintf(stderr,"gettask2 waiting...\n");
  //  pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
  //   fprintf(stderr,"gettask2 finished waiting\n");
  //}
  //}  while (tq->removeat == tq->addat);

  //version 2, didn't work
  //while(1) {
  //fprintf (stderr, "gettask2 trying to obtain lock\n");
  //my_lock_tq(tq);
  //fprintf (stderr, "gettask2 obtained lock\n");
  //if (tq->removeat == tq->addat) {
  //  my_unlock_tq(tq);
  //  fprintf (stderr, "gettask2 waiting\n");
  //  pthread_cond_wait(&tq->notifycond, &tq->notifymutex);
  //  fprintf (stderr, "gettask2 got signal\n");
  //} else
  //  break;
  //}

  while(1) {
    my_lock_tq(tq);
    if (tq->removeat == tq->addat) {
      my_unlock_tq(tq);
      sem_wait(&tq->sem);
    } else
      break;
  }

  // fprintf(stderr, "gettask2 progressed\n"); 
  t = tq->ts[tq->removeat];

  if (++tq->removeat == MAX_TASKS)
    tq->removeat = 0;

  my_unlock_tq(tq);
  // fprintf (stderr, "gettask2 lock released\n");

  //pthread_mutex_unlock(&tq->notifymutex);
   
   // fprintf(stderr,"gettask2 got %d\n",t);

  return(t);
}

void puttask1(struct tq *tq, int v) {
  int a, r;

//  fprintf(stderr,"putting1 %d\n",v);

  do {
    my_lock_tq(tq);

    if (tq->addat == tq->removeat-1)
      my_unlock_tq(tq); // exit(0);
  } while (tq->addat == tq->removeat-1);

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...

  // signal if there are blocked tasks
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
  //    //  fprintf(stderr,"unblocking...\n");
    pthread_cond_signal(&tq->notifycond);
   //   fprintf(stderr,"finished waiting\n");
  }
 // fprintf(stderr,"put %d %d %d\n",v, tq->removeat+1, tq->addat);
  my_unlock_tq(tq);
}

void puttask2(struct tq *tq, int v) {
  int a, r;

  // fprintf(stderr,"putting2 %d\n",v);

  while(1) {
    my_lock_tq(tq);
    if (tq->addat == tq->removeat-1) 
      my_unlock_tq(tq);
    else
      break;
  }

  //do {
  //my_lock_tq(tq);
  //if (tq->addat == tq->removeat-1) {
  //  my_unlock_tq(tq); // exit(0);
  //}
  //} while (tq->addat == tq->removeat-1);

  // fprintf (stderr, "about to put task2\n");
  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...

  // signal if there are blocked tasks
  // fprintf (stderr, "(%d,%d)\n", tq->removeat, tq->addat);
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    // fprintf(stderr,"unblocking...\n");
    //pthread_cond_signal(&tq->notifycond);
    sem_post(&tq->sem);
    // fprintf(stderr,"finished waiting\n");
  }
 // fprintf(stderr,"put %d %d %d\n",v, tq->removeat+1, tq->addat);
  my_unlock_tq(tq);
}

int workerStage1(int i)
{
        unsigned long _C=0;
        int j=0;
        int k=0;    

        for(j=0;j<N;++j){

            for(k=0;k<N;++k)
               _C += A[i][k]*B[k][j];


        //    printf("C[%d][%d] = %d", i,j,_C);
            C[i][j] = _C;
            _C = 0;
        }

        /*  unsigned long R=0;
            for(j=0;j<N;++j) {
                for(k=0;k<N;++k)
                    R += A[i][k]*B[k][j];
                 printf("(%d,%d) %d %d\n", i,j, C[i][j], R);

        //        if (C[i][j]!=R) {
        //            printf("Wrong result\n");
        //            return -1;
        //        }
                R=0;
            }

        */ 
        return _C;
}


void worker2()
{
        // printf("worker2\n");
	int t;
        int r;
        while(1)
	{
         //  printf("getting a task...\n");
	   t = gettask1(tq);
         //  printf("getting a task, computing\n");
           r = workerStage1(t);
         //  printf("putting a task...\n");
           puttask2(tq2, r);
	}
}

int main(int argc, char * argv[]) {
	int nworkers = atoi(argv[1]);
	// N = atoi(argv[1]);

        tq = newtq();
        tq2 = newtq();

        int i, j;

	for(i=0;i<N;++i)
           for(j=0;j<N;++j) {
             A[i][j] = i+j;
             B[i][j] = i*j;
             C[i][j] = 0;
        }

        //printf("offloading tasks...\n");
	for (i=0; i<N; i++) {
          // offload the tasks to the farm...
          //printf("offloading tasks\n");
          puttask1(tq, i);
        }

	//printf("create the farm...\n");
        // create the farm

        createfarm(worker2, nworkers);
        
        //printf("created the farm %d\n", N);

        for (i=0; i<N; i++) {
           // printf("Getting Task %d\n", i);
           gettask2(tq2);
        }

        unsigned long R=0;
        int k;

        /*for(i=0;i<N;++i)
            for(j=0;j<N;++j) {
                for(k=0;k<N;++k)
                    R += A[i][k]*B[k][j];
                 printf("(%d,%d) %d %d\n", i,j, C[i][j], R);

        //        if (C[i][j]!=R) {
        //            printf("Wrong result\n");
        //            return -1;
        //        }
                R=0;
            }
        printf("OK\n");
*/
}
