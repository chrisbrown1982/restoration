#include <iostream>
#include <cstdlib>
// #include <pthread.h>
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

// Assumes a perfect scheduling -- so it won't necessarily have the same behaviour as all runs of the original parallel version.

void* f (void* threadarg1, void* threadarg2) {

  int id1 = *((int*)threadarg1);
  cout << "StageOne, " << id1 << endl;
  int i1 = 0;

  int id2 = *((int*)threadarg2);
  cout << "StageTwo, " << id2 << endl;
  int i2 = 0;


  while (i1 < 10 || i2 < 20 ) {
    bool state1 = (inBuffer >= BUFFER_SIZE);
    if (!state1) {
      int buff;
   
      buff = inBuffer;
   
      if (buff < BUFFER_SIZE) {
        buff = buff + 1;  
      }
      cout << id1 << "; Put in buffer, " << buff << endl;
      
      inBuffer = buff;
      
      i1 = i1 + 1;
    }
    
    bool state2 = (inBuffer < 0);
    bool x = state2;
    if (!x) {
      int buff;
      
      // Read 1
      buff = inBuffer;

      cout << id2 << "; inBuffer : " << inBuffer << endl;
      buff--;
      cout << id2 << "; inBuffer now: " << inBuffer << endl;

      inBuffer = buff;
      i2 = i2 + 1;
    }
  }

  return NULL;
}

int main () {
  // pthread_t threads[NUM_THREADS*2];
  int rc;
  int i;

  // for(i = 0; i < NUM_THREADS; i++) {
  int *arg1 = (int*) malloc(sizeof(*arg1));
  cout << "main() : creating first batch thread, " << i << endl;
  // StageOne(&i);
  *arg1 = i;
  
  // rc = pthread_create(&threads[i], NULL, StageOne, arg);
  rc = 0;
  // StageOne(arg1);

  if (rc) {
    cout << "Error: unable to create thread, " << rc << endl;
    exit(-1);
  }
  // }
  
  // for(i = 0; i < NUM_THREADS; i++) {
  int *arg2 = (int*) malloc(sizeof(*arg2));
  *arg2 = i+NUM_THREADS;
  cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
  
  // rc = pthread_create(&threads[i+NUM_THREADS], NULL, StageTwo, arg);
  rc = 0;
  // StageTwo(arg2);
  f(arg1, arg2);

  if (rc) {
    cout << "Error: unable to create thread, " << rc << endl;
    exit(-1);
  }
  // }

  // pthread_join(threads[1], NULL);
}