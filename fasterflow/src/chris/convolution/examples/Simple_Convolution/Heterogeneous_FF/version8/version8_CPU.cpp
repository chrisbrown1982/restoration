/* -*- Mode: C++; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 Mehdi Goli: m.goli@rgu.ac.uk */

/*
 * Very basic test for the heterogeneous FastFlow pipeline (actually a 3-stage torus).
 *
 */

#include <png.h>

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
// #include <ff/ocl/mem_man.h>

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
#include <malloc.h>
#include <sys/time.h>
//#include "mpi.h"

/* 
 * THRESHOLD is the static estimation of the maximum number of ff_node which can run on GPU. 
 * By default the value is set to 1 ff_node per GPU.
 */
//#define THRESHOLD 4 

int width; // =2048;
int height; //=2048;
int maskWidth=8;
uint maskHeight=8;
int worker_width=8;
int worker_height=8;
// uint inputDimensions = {width    , height};
// uint maskDimensions  = {maskWidth, maskHeight};
uint inputDimension;
uint maskDimension; 
int quiet=1;
int nworkers;
int nworkers2;
int gnworkers;
const int max_strlen=10;
double get_current_time();
using namespace std;
using namespace ff;

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

struct task_worker{
  task_worker(int i,int j, task_t * t):i(i),j(j),t(t){}
  int i;
  int j;
  task_t * t;
};

template<typename T> 
void printArray(std::string header, const T * data,  const int width,const int height)
{
  std::cout<<"\n"<<header<<"\n";
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++)
      std::cout<<data[i*width+j]<<" ";
    std::cout<<"\n";
  }
  std::cout<<"\n";
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

template<typename T>
T roundToPowerOf2(T val)
{
  int bytes = sizeof(T);
  
  val--;
  for(int i = 0; i < bytes; i++)
    val |= val >> (1<<i);  
  val++;
  
  return val;
}

template<typename T>
int isPowerOf2(T val)
{
  long long _val = val;
  if((_val & (-_val))-_val == 0 && _val != 0)
    return 1;
  else
    return 0;
}

void*  create_input(const char *imageName) {
  task_t *t = new task_t;

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
  
  return (void *)t;

}

class CPU_Stage: public ff_node {
  
public:
  
  int svc_init(){
    ct=0;
    return 0;
  }
  
  void * svc(void * task ) {

    if (task==NULL) return NULL;
    else{
      task_t *t = (task_t *)task;
      //double cpu_start_time = get_current_time();
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
      
      return GO_ON;
    }
  }
  
  void svc_end(){printf("worker %d : %f\n",ff_node::get_my_id(),ct);}
private:
  double ct;
  
};


class ff_pipe_first_stage: public ff_node{
public:
  ff_pipe_first_stage(){};
  
  int svc_init(){
    //first_pipe_stage_time = get_current_time();
    streamlen=0;
    return 0;
  }
  
  void* svc(void *task){
    printf ("got task\n");
    //if(streamlen<max_strlen){
    //double start_time = get_current_time();
    //char imageName[30];
    void *res;
    //sprintf(imageName, "images/image%d.png", streamlen);
    //++streamlen;
    res = create_input((char *)task);
    //printf ("image reading time %f\n", get_current_time()-start_time);
    return res;
    //} else 
    //return (void*)FF_EOS;
  }
  
  void svc_end(){
    //end_pipe_stage_time = get_current_time();
    //printf("time spent in the first stage of the pipeline:%f\n", end_pipe_stage_time- first_pipe_stage_time);
  }
private:
  int streamlen;
  double    first_pipe_stage_time;
  double    end_pipe_stage_time;
};

class StreamGen: public ff_node {
public:
  StreamGen(int NIMGS, char* images[]):NIMGS(NIMGS),images(images) {}

  void * svc(void * task) {
    for(int i=0; i<NIMGS; i++) {
      ff_send_out((void *) images[i]);
    }
    return NULL;
  }
protected:
  int NIMGS;
  char** images;
};



class Pipe2: public ff_node {
public:
  Pipe2() {
  // add_task = new AddTask();

  // ff_pipeline pipe2;
  // pipe2.add_stage(&add_task);
    printf ("foobar\n");
  pipe2.add_stage(new ff_pipe_first_stage);
  pipe2.add_stage(new CPU_Stage);
  pipe2.run_then_freeze();
  printf ("baz\n");
  }

  void* svc(void* task) {

    pipe2.offload(task); // .addTask(task);

  }

private:
  // AddTask add_task; // = new AddTask();
  ff_pipeline pipe2;
  //pipe.add_stage(&global_farm);
  //pipe.add_stage(&global_farm);
  //pipe.add_stage(&global_farm2);
  // pipe2.add_stage(new ff_pipe_first_stage);
  // pipe2.add_stage(new CPU_Stage);
};


int main(int argc, char * argv[]) {
  int NIMGS;
  char **images;

  if (argc<5) 
    std::cerr << "use: " << argv[0] << " <noCPUworkers farm1> <noCPUworkers farm2> <noGPUWorkers> <imageSize> <nrImages>\n";
  nworkers = atoi(argv[1]);
  nworkers2 = atoi(argv[2]);
  gnworkers = atoi(argv[3]);
  height = atoi(argv[4]);
  NIMGS = atoi(argv[5]);
  width = height;

  images = (char **) malloc (sizeof(char *)*NIMGS);
  for (int i=0; i<NIMGS; i++) {
    images[i] = (char *) malloc (sizeof(char)*20);
    sprintf(images[i],"images/image%d.png", i);
  }

  //StreamGen streamgen(NIMGS,images);
  
  //ff_pipeline pipe;
  //pipe.add_stage(&streamgen);

  // ff_pipeline pipe2;
  //pipe.add_stage(&global_farm);
  //pipe.add_stage(&global_farm);
  //pipe.add_stage(&global_farm2);
  // pipe2.add_stage(new ff_pipe_first_stage);
  // pipe2.add_stage(new CPU_Stage);

  //printf("1\n");

  ff_farm<> farm;
  farm.add_emitter(new StreamGen(NIMGS,images));
  farm.add_collector(NULL);
  std::vector<ff_node*> pp;
  for(int i=0; i<nworkers; ++i)
  {
       ff_pipeline * pipe2 = new ff_pipeline;

       ff_farm<> * stage2 = new ff_farm<>;
       stage2->add_collector(NULL);
       std::vector<ff_node*> w;
       for(int i=0; i<nworkers2; ++i)
       {
            w.push_back(new CPU_Stage); 
       }

       stage2->add_workers(w);

       // pipe2->add_stage(stage1);
       pipe2->add_stage(new ff_pipe_first_stage);
       pipe2->add_stage(stage2);
       // pipe2->add_stage(new CPU_Stage);
       pp.push_back(pipe2);
  }

  farm.add_workers(pp);

  //pipe.add_stage(&farm);

  //printf("2\n");

  double cpu_start_time = get_current_time();
  if (farm.run_and_wait_end()<0/*pipe.run_and_wait_end()<0*/) {
    error("running pipeline\n");
    return -1;
  }
  // global_farm.ffStats(std::cout);
  double cpu_end_time = get_current_time();
  std::cout<< "total_time:"<< cpu_end_time-cpu_start_time<<std::endl;
  
  return 0;
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

