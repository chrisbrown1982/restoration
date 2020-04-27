// #include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include <ff/farm.hpp>
//#include <ff/pipeline.hpp>
//#include <ff/ocl/mem_man.h>
#include "farmConvolution.h"

//#include <ff/ocl/mem_man.h>
#include <iostream>
#include <sstream>
#include <string>

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <png.h>

/* int width; // =2048;
int height; //=2048;
uint maskWidth=8;
uint maskHeight=8;
int worker_width=8;
int worker_height=8;
cl_uint2 inputDimensions = {width    , height};
cl_uint2 maskDimensions  = {maskWidth, maskHeight};
uint inputDimension;
uint maskDimension;
int quiet=1;
int nworkers;
int nworkers2;
int gnworkers;
const int max_strlen=10; */
double get_current_time();
using namespace std;
//using namespace ff;

int lock_char;
int unlock_char;
int lock_tq;
int unlock_tq;
int lock_t_task_t;
int unlock_t_task_t;


extern int payload(int t);
extern void my_lock(struct tq *tq );
extern void my_unlock( struct tq *tq );
extern void exit(int x); 

// per tq?
//pthread_mutex_t mutex;

void my_lock_char( struct t_char *tq )
{
   char old_value ;
   char new_value = 1 ;
 
   /* do {
      // use an xchg to obtain a lock
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;

      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
	break;
     */

    // pthread_mutex_lock(&tq->lockmutex); 
      // otherwise wait until signalled
    // pthread_cond_wait(&tq->lockcond,&tq->lockmutex);
      
      // don't assume we have the lock yet...
      
   // } while ( 1 ) ;
   lock_char++;
}
 

void my_unlock_char( struct t_char *tq )
{
   tq->lock = 0 ;

   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);

   // this is safer, but may still be unnecessary
   // pthread_cond_broadcast(&tq->lockcond);
   unlock_char++;
}

void my_lock_t_task_t( struct t_task_t *tq )
{
   char old_value ;
   char new_value = 1 ;
 
   /* do {
      // use an xchg to obtain a lock
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;

      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
	break;
     */

    // pthread_mutex_lock(&tq->lockmutex); 
      // otherwise wait until signalled
    // pthread_cond_wait(&tq->lockcond,&tq->lockmutex);
      
      // don't assume we have the lock yet...
      
   // } while ( 1 ) ;
   lock_t_task_t++;
}
 

void my_unlock_t_task_t( struct t_task_t *tq )
{
   tq->lock = 0 ;
   unlock_t_task_t++;

   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);

   // this is safer, but may still be unnecessary
   pthread_cond_broadcast(&tq->lockcond);
}

void my_lock_tq( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
 
   /* do {
      // use an xchg to obtain a lock
      __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) :
                            "cc" ) ;

      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
	break;
     */

    // pthread_mutex_lock(&tq->lockmutex); 
      // otherwise wait until signalled
    // pthread_cond_wait(&tq->lockcond,&tq->lockmutex);
      
      // don't assume we have the lock yet...
      
   // } while ( 1 ) ;
   lock_tq++;
}

void my_unlock_tq( struct tq *tq )
{
   tq->lock = 0 ;

   unlock_tq++;

   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);

   // this is safer, but may still be unnecessary
   // pthread_cond_broadcast(&tq->lockcond);
}

struct tq *newtq() {
  struct tq *tq = (struct tq *)malloc(sizeof(struct tq));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  // pthread_cond_init(&tq->notifycond,NULL); // NULL?

  // pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

task_t2* gettask2(struct t_task_t *tq) {
  task_t2* t;

  // fprintf(stderr,"getting\n");

  do {
    my_lock_t_task_t(tq);

    if (tq->removeat == tq->addat) {
      my_unlock_t_task_t(tq);
      //fprintf(stderr,"waiting...\n");
      // pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
     // fprintf (stderr, "%d got past cond wait\n", pthread_self());
      //fprintf(stderr,"finished waiting\n");
    }
  }  while (tq->removeat == tq->addat);

  t = tq->ts[tq->removeat];

  // fprintf (stderr, "%d stage 2 worker got task (%d)\n", pthread_self(), tq->removeat-tq->addat);
  if (++tq->removeat == MAX_TASKS) {
    tq->removeat = 0;
    // fprintf (stderr,"LALALALAALAL\n");
  }
  my_unlock_t_task_t(tq);
  // pthread_mutex_unlock(&tq->notifymutex);
  // fprintf(stderr,"got %d\n",t);

  return(t);
}


void puttask1(struct t_task_t *tq,task_t2* v) {
  int a, r;

  //  fprintf(stderr,"putting %d\n",v);

  do {
    my_lock_t_task_t(tq);

    if (tq->addat == tq->removeat-1) 
      my_unlock_t_task_t(tq); // exit(0);
  } while (tq->addat == tq->removeat-1);

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...
  my_unlock_t_task_t(tq);

  // signal if there are blocked tasks
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    //    fprintf(stderr,"unblocking...\n");
    // pthread_cond_signal(&tq->notifycond);
    // fprintf(stderr,"finished waiting\n");
  }
  //  fprintf(stderr,"put %d\n",v);
}

char* gettask1(struct t_char *tq) {
  char* t;

  //  fprintf(stderr,"getting\n");

  do {
    my_lock_char(tq);

    if (tq->removeat == tq->addat) {
      my_unlock_char(tq);
      // fprintf(stderr," stage1 waiting...\n");
      // pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
      // fprintf(stderr,"stage1 finished waiting\n");
    }
  }  while (tq->removeat == tq->addat);

  t = tq->ts[tq->removeat];
  // fprintf(stderr, "%d stage 1 worker got task\n", pthread_self());

  if (++tq->removeat == MAX_TASKS)
    tq->removeat = 0;

  my_unlock_char(tq);
 // pthread_mutex_unlock(&tq->notifymutex);
  //  fprintf(stderr,"got %d\n",t);

  return(t);
}


int gettasktq(struct tq *tq) {
  int t;

  //  fprintf(stderr,"getting\n");

  do {
    my_lock_tq(tq);

    if (tq->removeat == tq->addat) {
      my_unlock_tq(tq);
      //      fprintf(stderr,"waiting...\n");
      // pthread_cond_wait(&tq->notifycond,&tq->notifymutex);
      // fprintf(stderr,"finished waiting\n");
    }
  }  while (tq->removeat == tq->addat);

  t = tq->ts[tq->removeat];

  if (++tq->removeat == MAX_TASKS)
    tq->removeat = 0;

  my_unlock_tq(tq);
 // pthread_mutex_unlock(&tq->notifymutex);
  //  fprintf(stderr,"got %d\n",t);

  return(t);
}



// The notification process is highly time sensitive -- there could be races in here...  KH
void puttask(struct tq *tq,int v) {
  int a, r;

  //  fprintf(stderr,"putting %d\n",v);

  do {
    my_lock_tq(tq);

    if (tq->addat == tq->removeat-1) 
      my_unlock_tq(tq); // exit(0);
  } while (tq->addat == tq->removeat-1);

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...
  my_unlock_tq(tq);

  // signal if there are blocked tasks
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    //    fprintf(stderr,"unblocking...\n");
    // pthread_cond_signal(&tq->notifycond);
    // fprintf(stderr,"finished waiting\n");
  }
  //  fprintf(stderr,"put %d\n",v);
}

// The notification process is highly time sensitive -- there could be races in here...  KH
void puttask_char(struct t_char *tq,char * v) {
  int a, r;
   // printf("BAZ\n");
  //  fprintf(stderr,"putting %d\n",v);

  do {
    my_lock_char(tq);

    if (tq->addat == tq->removeat-1) 
      my_unlock_char(tq); // exit(0);
  } while (tq->addat == tq->removeat-1);

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS) 
  {
//	printf("FOOBAR\n");
    tq->addat = 0;
}

  
  // unlock BEFORE signalling...
  my_unlock_char(tq);

  // signal if there are blocked tasks
  if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    //    fprintf(stderr,"unblocking...\n");
    // pthread_cond_signal(&tq->notifycond);
    // fprintf(stderr,"finished waiting\n");
  }
  //  fprintf(stderr,"put %d\n",v);
}




/* void *Worker(void *threadid) */
/* { */
/*   long tid,tid2; */
/*   tid = (long)threadid; */
/*   tid2 = pthread_self(); */
/*   // printf("Hello World! It's me, thread #%ld (%x)!\n", tid, tid2); */
 
/*   while(1) { */
/*     worker(tq1,tq2); */
/*   } */
/*   pthread_exit(NULL); */
/* } */

// pthread_t threads[NUM_THREADS];

// use definefarmworker to create a suitable worker to be farmed

void makeThread(void (*Worker)(),long t) {
  int rc;

  pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

  rc = 0; // pthread_create(pt, NULL, (void *(*)(void *))Worker, (void *)t);
  Worker();
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

void createpipe(void (*Worker1)(),void (*Worker2)()) {
    makeThread(Worker1,0);
    makeThread(Worker2,1);
}

void createpipefarm(void (*Worker1)(), void (*Worker2)(), int n1, int n2) {
  //  for (long i=0; i< n1; i++) {
 //       printf("Making thread Stage1 %d\n", i);
        // makeThread(Worker1, i);

        int rc;

        // pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

        rc = 0; // pthread_create(pt, NULL, (void *(*)(void *))Worker1, (void *)i);
        Worker1();
        //  rc = pthread_create(pt, NULL, Worker, (void *)t);

        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }


 //   }
    
    // for (long i=0; i<n2; i++) {
 //	printf("Making thread Stage2 %d\n", i+n1);
        //makeThread(Worker2, i+n1);

        // int rc;

        // pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

        int temp = 0 + n1;

        rc = 0 ; // pthread_create(pt, NULL, (void *(*)(void *))Worker2, (void *)temp);
        Worker2();
        //  rc = pthread_create(pt, NULL, Worker, (void *)t);

        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

    // }

}


/*struct task_t{
  task_t(){
    //inpt= new cl_uint[ width * height];
    outpt= new ushort[width*height];
    msk=new float[maskWidth * maskHeight];
  }
  //cl_uint* inpt;
 //cl_uint* outpt;
  ushort *inpt;
  ushort *outpt;
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

  int addat;
  int removeat;
  task_t* ts[MAX_TASKS];
}; */


struct t_char* tq1;
struct t_task_t* tq2;
struct tq* tq3;


struct t_char *newtchar() {
  struct t_char *tq = (struct t_char *)malloc(sizeof(struct t_char));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  // pthread_cond_init(&tq->notifycond,NULL); // NULL?

  // pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

struct t_task_t *newttaskt() {
  struct t_task_t *tq = (struct t_task_t *)malloc(sizeof(struct t_task_t));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  // pthread_cond_init(&tq->notifycond,NULL); // NULL?

  // pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

unsigned short *read_image(const char *fileName, png_uint_32 height) {
  int i, header_size = 8, is_png;
  char header[8];
  FILE *fp = fopen(fileName,"rb");
  png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr), end_info = png_create_info_struct(png_ptr);
  png_bytep raw_data; 
  png_bytepp row_pointers;
  png_uint_32 row_bytes;

  fread (header, 1, header_size, fp);
  is_png = !png_sig_cmp((png_bytep)header,0,header_size);
  if (!is_png) { printf("not a png\n"); return(NULL);}
  png_init_io(png_ptr,fp);
  png_set_sig_bytes(png_ptr,header_size);
  png_read_info(png_ptr, info_ptr);
  row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  raw_data = (png_bytep) png_malloc(png_ptr, height*row_bytes);
  row_pointers = (png_bytepp) png_malloc(png_ptr, height*sizeof(png_bytep));
  for (i=0; i<height; i++)
    row_pointers[i] = raw_data+i*row_bytes;
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_read_rows(png_ptr,row_pointers,NULL,height);

  return (unsigned short *)raw_data;

}


task_t2*  create_input(const char *imageName) {
  task_t2 *t = new task_t2;
  double start_time = get_current_time();

  t->inpt = read_image(imageName, height);
	
  /* Fill a blurr filter or some other filter of your choice*/
  for(uint i=0; i < maskWidth*maskHeight; i++) t->msk[i] = 0;
  
  float val = 1.0f/(maskWidth * 2.0f - 1.0f);
  
  ushort y = maskHeight/2;
  for(uint i=0; i < maskWidth; i++) 
    t->msk[y*maskWidth + i] = val;
  
  ushort x = maskWidth/2;
  for(uint i=0; i < maskHeight; i++) 
    t->msk[i*maskWidth + x] = val;
  
  return t;

  // printf ("image generation time %f\n", get_current_time() - start_time);
}


task_t2 *workerStage1(const char *task)
{
    task_t2 *res;
    res = create_input(task);
    return res;
}

void stage1()
{
    // printf("Stage1\n");
    char *t;
    task_t2* r;
    while(1)
    {
        t = gettask1(tq1);
        r = workerStage1(t);
        puttask1(tq2, r);
    }
}

int workerStage2(task_t2 *task)
{
      double cpu_start_time = get_current_time();
      int vstep = (maskWidth)/2;
      int hstep = (maskHeight)/2;
      float sumFX;
      int left,right,top,bottom,maskIndex,index;
      for(int x = 0; x < height; x++)
        for(int y = 0; y < width; y++) {
          //find the left, right, top and bottom indices such that the indices do not go beyond image boundaires
          left    = (x           <  vstep) ? 0         : (x - vstep);
          right   = ((x + vstep - 1) >= width) ? width - 1 : (x + vstep - 1); 
          top     = (y           <  hstep) ? 0         : (y - hstep);
          bottom  = ((y + hstep - 1) >= height)? height - 1: (y + hstep - 1); 
          sumFX = 0;
          
          for(int i = left; i <= right; i++)
            for(int j = top ; j <= bottom; j++) {
              //performing weighted sum within the mask boundaries
              maskIndex = (j - (y - hstep)) * maskWidth  + (i - (x - vstep));
              index     = j                 * width      + i;
              sumFX += ((float)task->inpt[index] * task->msk[maskIndex]);
            }
          sumFX += 0.5f;
          //verificationOutput
          task->outpt[y*width + x] = (ushort) sumFX; //int(sumFX);
        }
      
      double cpu_end_time = get_current_time();
      // printf("workerStage2 time taken: %f\n", cpu_end_time-cpu_start_time);
      
      //ct += (cpu_end_time - cpu_start_time);
      return 42;
}

void stage2()
{
    task_t2 *t;
    int r;
    while(1)
    {
      //fprintf(stderr, "%d Stage2 grabbing a task...\n", pthread_self());
        t = gettask2(tq2);
        //fprintf(stderr, "%d Stage2 got a task\n", pthread_self());
        r = workerStage2(t);
        //printf("Stage2 putting a task\n");
        puttask(tq3, r);
        //printf("Stage2 done\n");
    }
}


int main(int argc, char* argv[])
{
  int NIMGS;
  char **images;
  
  tq3 = newtq();
  tq1 = newtchar();
  tq2 = newttaskt();
  

  int nworkers1 = atoi(argv[1]);
  int nworkers2 = atoi(argv[2]);
  NIMGS = atoi(argv[3]);
  
  // NIMGS = 100;
  //
  
  lock_char;
  unlock_char;
  lock_tq;
  unlock_tq;
  lock_t_task_t;
  unlock_t_task_t;

  width = 1024;
  height = 1024;  
  
  // printf("here\n");

   double cpu_start_time = get_current_time();
  
  images = (char **) malloc (sizeof(char *)*NIMGS);
  for (int i=0; i<NIMGS; i++) {
    // printf("here\n");
    images[i] = (char *) malloc (sizeof(char)*1000);
    sprintf(images[i],"/Users/chrisb/Documents/Papers/hlpp2020/restoration/fasterflow/src/chris/convolution/examples/Simple_Convolution/Heterogeneous_FF/images/image%d.png", i);
    
    puttask_char(tq1, images[i]);
  }
printf ("size of tq is %d %d\n", tq1->removeat, tq1->addat);	
  createpipefarm(stage1, stage2, nworkers1, nworkers2);
  
  for (int i=0; i<NIMGS; i++) {
    gettasktq(tq3);
  }

  double cpu_end_time = get_current_time();
  std::cout<< "total_time, "<< cpu_end_time-cpu_start_time<<std::endl; 
  std::cout<< "total lock, "<< lock_char + lock_tq + lock_t_task_t <<std::endl;
  std::cout<< "total unlock, " << unlock_char + unlock_tq + unlock_t_task_t << std::endl;

  std::cout<< "lock char, "<< lock_char <<std::endl;
  std::cout<< "unlock char, "<< unlock_char <<std::endl;

  std::cout<< "lock tq, "<< lock_tq <<std::endl;
std::cout<< "lock tq, "<< lock_tq <<std::endl;

std::cout<< "lock t_task_t, "<< lock_t_task_t <<std::endl;
std::cout<< "lock t_task_t, "<< lock_t_task_t <<std::endl;

}

double get_current_time()
{
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;

  if (gettimeofday(&tval, NULL) == -1)
    result = -1.0;
  else if(!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  }
  else
    result = (double) (tval.tv_sec - start) + 1.0e-6*(tval.tv_usec - startu);

  return result;
}
