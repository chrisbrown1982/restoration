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


task_t2 *workerStage1(int i)
{

    char* images = (char *) malloc (sizeof(char)*1000);
    sprintf(images,"/Users/chrisb/Documents/Papers/hlpp2020/restoration/fasterflow/src/chris/convolution/examples/Simple_Convolution/Heterogeneous_FF/images/image%d.png", i);

    task_t2 *res;
    res = create_input(images);
    return res;
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

void workerStage3(int task)
{
    //do nothing!
}








int main(int argc, char* argv[])
{
  int NIMGS;
  char **images;
 
  int nworkers1 = atoi(argv[1]);
  int nworkers2 = atoi(argv[2]);
  NIMGS = atoi(argv[3]);
  
  width = 1024;
  height = 1024;  
  
  // printf("here\n");

   double cpu_start_time = get_current_time();
  
  images = (char **) malloc (sizeof(char *)*NIMGS);
    int rc;
      rc = 0; 
  for (int i=0; i<NIMGS; i++) {
        workerStage3(workerStage2(workerStage1(i)));
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
