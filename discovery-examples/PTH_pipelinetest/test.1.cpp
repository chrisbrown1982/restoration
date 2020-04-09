#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM_THREADS 1
#define BUFFER_SIZE 10

/*
  Simulates a pipeline where two pthreads push and pop things from a buffer. The buffer is actually simulated by the number of things in a buffer.
  Designed to be a gross simplification of the PGPry example which does this sort of thing.

  The sleep commands represent some workload. StageOne should take slightly less time than StageTwo, meaning whilst they should display ping-pong-like behaviour in the printout, StageOne will generally produce more work than StageTwo consumes it within a certain amount of time.

  As with PGPry, you can't just remove all of the pthread code and hope for the best, since StageOne will keep filling the buffer until nothing works.
*/

volatile int inBuffer = -1;
// pthread_mutex_t monk;

bool BufferIsEmpty() {
  // pthread_mutex_lock(&monk);
  bool state = (inBuffer < 0);
  // pthread_mutex_unlock(&monk);
  return state;
}

bool BufferIsFull() {
  // pthread_mutex_lock(&monk);
  bool state = (inBuffer >= BUFFER_SIZE);
  // pthread_mutex_unlock(&monk);
  return state;
}

void *StageOne(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageOne, " << id << endl;
  int i = 0;
  while (i < 101) {
    if (!BufferIsFull()) {
      // pthread_mutex_lock(&monk);
      if (inBuffer < BUFFER_SIZE) {
        inBuffer = inBuffer + 1;  
      }
      cout << id << "; Put in buffer, " << inBuffer << endl;
      // pthread_mutex_unlock(&monk);
      // sleep(i*2);
      i = i + 1;
    }
  }
  // pthread_exit(NULL);
  return NULL;
}

void *StageTwo(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageTwo, " << id << endl;
  int i = 0;
  while (true) {
    if (!BufferIsEmpty()) {
      // pthread_mutex_lock(&monk);
      cout << id << "; inBuffer : " << inBuffer << endl;
      inBuffer--;
      cout << id << "; inBuffer now: " << inBuffer << endl;
      // pthread_mutex_unlock(&monk);
      i = i + 1;
    }
    sleep(i*4);
  }
  // pthread_exit(NULL);
  return NULL;
}

struct tuple1 {int i2; int inbuffer;} ;

struct tuple2 {int id; int inbuffer; int i2;} ;

tuple2 w1 (int i2, int inBuffer, int id, int i) {
  if (i2 < 101) {
      if (!BufferIsFull()) {
        if (inBuffer < BUFFER_SIZE) {
          inBuffer = inBuffer + 1;  
        }
        cout << id << "; Put in buffer, " << inBuffer << endl;
        // sleep(i*2);
        i2 = i2 + 1;
      }
    }
tuple2 x ={.id = id, .inbuffer=inBuffer, .i2=i2};
return x;      
}

tuple2 w2(int id2, int inBuffer, int i3, int x) {
  if (i3<200) {
      if (!BufferIsEmpty()) {
        cout << id2 << "; inBuffer : " << inBuffer << endl;
        inBuffer--;
        cout << id2 << "; inBuffer now: " << inBuffer << endl;
        
        // cout << i3 << endl;
      }
      i3 = i3 + 1;
      // sleep(i3*4);
    }
    tuple2 x42 ={.id=id2, .inbuffer=inBuffer, .i2=i3};
    return x42;
  }


int main () {
  // pthread_t threads[NUM_THREADS*2];
  int rc;
  int i;


  // Merged for loops -- still doesn't work since these loops don't express the right looping behaviour.
  for(i = 0; i < NUM_THREADS; i++) {
    int *arg = (int*) malloc(sizeof(*arg));
    cout << "main() : creating first batch thread, " << i << endl;
    *arg = i;
    // StageOne(arg);
    int id = *((int*)arg);
    cout << "StageOne, " << id << endl;
    int i2 = 0;

    // Reordered definitions & related while loops.
    int *arg2 = (int*) malloc(sizeof(*arg2));
    *arg2 = i+NUM_THREADS;
    cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
    // StageTwo(arg2);
    int id2 = *((int*)arg2);
    cout << "StageTwo, " << id2 << endl;
    int i3 = 0;





    while ((i2 < 101) || (i3 < 200)) {
 
      tuple2 x;
      tuple2 y;

      x = w1(i2, inBuffer, id, i);

      i2 = x.i2;

      y = w2(x.id, x.inbuffer, i3, x.i2);

      i3 = y.i2;

      inBuffer = y.inbuffer;
      // x = w1( ... )
      // y = w2(...., x)



  }

  // pthread_join(threads[1], NULL);
}
}