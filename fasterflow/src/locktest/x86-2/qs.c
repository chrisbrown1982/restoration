#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define MAX_TASKS 10000


//FastFlow task type
typedef struct {
    int i;
    int j;
    int k;
} ff_task;


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
  task_t* ts[MAX_TASKS+1];
};

struct tq* tq;
struct tq* tq2;
struct tq* tq3;

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

void createfarm(void (*Worker1)(), void (*Worker2)(), int n1, int n2) {
  int rc;
  long t;

  for(t=0;t<n1;t++)
    makeThread(Worker1,t);
    
  for(t=n1;t<n2;t++)
    makeThread(Worker2,t);  
}

void my_lock_tq( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
   
   do {
      // __sync_val_compare_and_swap(&tq->lock,old_value,new_value);
      old_value = __sync_lock_test_and_set(&tq->lock,1);
      if ( 0 == old_value)
        break;
      sem_wait(&tq->locksem);

   } while ( 1 ) ;
   // lock++;
}

void my_unlock_tq( struct tq *tq )
{
   // tq->lock = 0 ;
   __sync_lock_release(&tq->lock);

   sem_post(&tq->locksem);
   // unlock++;
}

task_t *gettask(struct tq *tq) {
  void *t;


  sem_wait(&tq->sem);
  my_lock_tq(tq);
  // fprintf(stderr, "gettask2 progressed\n"); 
  t = tq->ts[tq->removeat];
  //
  if (++tq->removeat == MAX_TASKS)
     tq->removeat = 0;
  //
  my_unlock_tq(tq);

  return(t);
}

void puttask(struct tq *tq, task_t* v) {
  int a, r;

  // fprintf(stderr,"putting2 %d\n",v);

  while(1) {
    my_lock_tq(tq);
    if (tq->addat == tq->removeat-1) 
      my_unlock_tq(tq);
    else
      break;
  }

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

    sem_post(&tq->sem);

  my_unlock_tq(tq);
}


// some globals 
unsigned size    = 0;
int thresh       = 0;
unsigned int   *A= NULL;

void print_array() {
  int j=0;
  
  for(unsigned int i=0;i<size;i++) {
      if (j == 15) {
	  printf("\n");
	  j=0;
      }
      j++;
      printf("%d ", A[i]);
  }
  printf("\n\n");
}


inline void swap(int i, int j) {
  register int tmp;
  tmp = A[i]; A[i] = A[j]; A[j] = tmp;
}

/* Return the largest of first two keys. */
inline int FindPivot(int i, int j) {
    register int pivot = (i+(j-i))/2;
    if (A[i]>A[pivot]) {
	if (A[i]>A[j]) return i;
	else return j;
    } else {
	if (A[pivot]>A[j]) return pivot;
	else return j;
    }
}


/* Partition the array between elements i and j around the specified pivot. */
inline int Partition(int i, int j, unsigned int pivot) {
    int left = i;
    int right = j;
    
    do {
        swap(left,right);
        while (A[left]  <  pivot) left++;
        while (A[right] >= pivot) right--;
    } while (left <= right);
    
    return(left);
}


inline void QuickSort(int i, int j) {
    if (j-i <= 1) {
	if (A[i]>A[j]) swap(i,j);
	return;
    } 
    int pivot = FindPivot(i,j);
    int k     = Partition(i,j,A[pivot]);
    QuickSort(i, k-1);
    QuickSort(k,j);
}

void initArray() {
    /* All of the elements are unique. */
    for (unsigned int i = 0; i < size; i++)	A[i] = i;
    
    /* Shuffle them randomly. */
    srandom(0);
    for (unsigned int i = 0; i < size; i++)	
	swap(i, (random() % (size-i)) + i);
    
}

void usage() {
    fprintf(stderr,"Usage: ff_qs <sz> <threshold> <nworkers>\n\n");
    fprintf(stderr,"       sz                : size of unsorted array\n");
    fprintf(stderr,"       bubble-threashold : threashold for sequential sorting\n");
    fprintf(stderr,"       nworkers          : the n. of FastFlow worker threads\n");
}


task_t * worker(task_t * t) {
    // ff_task * task = (ff_task*)t;
    int i=task->i, j=task->j;
        
    if (j - i <= thresh) {
        QuickSort(i,j);
        task->k = -1; // reset the value
        return task;
    } 
    int pivot = FindPivot(i,j);
    task->k   = Partition(i,j,A[pivot]);
        
    return task;
}

void Stage2()
{
    task_t* t;
    task_t* r;
    while(1)
    {
        t = gettask(tq2);
        r = worker(t);
        puttask(tq1, r);
    }
}


int emitter(int, streamlen, task_t * t) {	
    // ff_task * task = (ff_task*)t;
    if (task == NULL) {
        int pivot = FindPivot(0,size-1);
        int k     = Partition(0,size-1,A[pivot]);
         
        // task = new ff_task;
        task = (task_t*)malloc(sizeof(task_t));
        task->i=0; task->j=k-1;
        //ff_send_out(task);
        puttask(tq2, task)
      
        // task = new ff_task;
        task = (task_t*)malloc(sizeof(task_t));
        task->i=k; task->j=size-1;
        //ff_send_out(task);
        puttask(tq2, task);    
            
        streamlen=2;
            
        return streamlen;       
    }
        
        
    int i=task->i, j=task->j, k=task->k;
    --streamlen;
    if (k==-1) {
        if (streamlen == 0) {
            puttask(tq3, task);
            // delete task;
            return streamlen;
        }
        // delete task;
        return streamlen;
    }
        
    task->i=i; task->j=k-1;	task->k=-1;
    // ff_send_out(task);
    puttask(tq2, task);
        
    task = (task_t*)malloc(sizeof(task_t));
    task->i=k; task->j=j; task->k=-1;
    // ff_send_out(task);
    puttask(tq2, task);
        
    streamlen +=2;
        
    return streamlen;
}

void Stage1()
{

    int streamlen = 0;
    streamlen = emmitter(NULL);
    task_t *t;
    while(1)
    {
        t = gettask(streamlen, tq1);
        streamlen = emitter(t)
    }
}

int main(int argc, char *argv[]) {
    bool check_result=false;

    if (argc<4 || argc>5) {
        usage();
        return -1;
    } 
    
    size   = atoi(argv[1]);
    thresh = atoi(argv[2]);
    int nworkers=atoi(argv[3]);
    if (argc==5) check_result=true;
    
    if (nworkers > ff::ff_farm<>::DEF_MAX_NUM_WORKERS) {
        fprintf(stderr, "too many number of workers\n");
        return -1;
    }
    if ((unsigned)thresh > (size/2)) { // just a simple check
        fprintf(stderr, "threshold too high\n");
        return -1;        
    }
    
    A = new unsigned int[size];
    if (!A) {
        fprintf(stderr,"Not enough memory for A\n");
        exit(1);
    }
    
    initArray();
    
    tq = newtq();
    tq2 = newtq();
    tq3 = newtq();
    
    createfarm(stage1, stage2, 1, nworkers);
    
    gettask(tq3);
    
        if (1) {
        for(unsigned int i=0;i<size;i++) 
            if (A[i]!=i) {
                error("wrong result\n");
                return -1;
            }
        printf("Ok\n");
    }
    
    return 0;
}


