#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "farm.h"

#include <ff/ocl/mem_man.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <png.h>

#define MAX_TASKS 10

int width; // =2048;
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
const int max_strlen=10;

struct task_t{
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
};


struct t_char tq1;
struct t_task_t tq2;
struct tq tq3;


struct t_char *newtchar() {
  struct t_char *tq = (struct t_char *)malloc(sizeof(struct t_char));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  pthread_cond_init(&tq->notifycond,NULL); // NULL?

  pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

struct t_task_t *newttaskt() {
  struct t_task_t *tq = (struct t_task_t *)malloc(sizeof(struct t_task_t));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  pthread_cond_init(&tq->notifycond,NULL); // NULL?

  pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}

ushort *read_image(const char *fileName, png_uint_32 height) {
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

  return (ushort *)raw_data;

}



task_t*  create_input(const char *imageName) {
  task_t *t = new task_t;
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

  printf ("image generation time %f\n", get_current_time() - start_time);
}


task_t *workerStage1(const char *task)
{
    task_t *res;
    res = create_input(task);
    return res;
}



void stage1()
{
    char *t;
    task_t* r;
    while(1)
    {
        t = gettask(tq1);
        r = workerStage1(t);
        puttask(tq2, r);
    }
}


int workerStage3(task_t *task)
{
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
              sumFX += ((float)t->inpt[index] * t->msk[maskIndex]);
            }
          sumFX += 0.5f;
          //verificationOutput
          t->outpt[y*width + x] = (ushort) sumFX; //int(sumFX);
        }
      
      //double cpu_end_time = get_current_time();
      
      //ct += (cpu_end_time - cpu_start_time);
      return 42;
}

void stage2()
{
    task_t *t;
    int r;
    while(1)
    {
        t = gettask(tq2);
        r = workerStage2(t);
        puttask(tq3, r);
    }
}


int main(int argc, char* argv[])
{
  int NIMGS;
  char **images;

  width = 100;
  height = 100;  
  
  images = (char **) malloc (sizeof(char *)*MAX_TASKS);
  for (int i=0; i<NIMGS; i++) {
    images[i] = (char *) malloc (sizeof(char)*20);
    sprintf(images[i],"images/image%d.png", i);
  }

  
  createpipe(stage1, stage2);
}
