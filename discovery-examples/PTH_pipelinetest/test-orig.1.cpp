#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM_THREADS 1
#define BUFFER_SIZE 10
#define EOS 102

/*
  Simulates a pipeline where two pthreads push and pop things from a buffer. The buffer is actually simulated by the number of things in a buffer.
  Designed to be a gross simplification of the PGPry example which does this sort of thing.

  The sleep commands represent some workload. StageOne should take slightly less time than StageTwo, meaning whilst they should display ping-pong-like behaviour in the printout, StageOne will generally produce more work than StageTwo consumes it within a certain amount of time.

  As with PGPry, you can't just remove all of the pthread code and hope for the best, since StageOne will keep filling the buffer until nothing works.
*/

volatile int inBuffer = -1;
#pragma discovery pipeline remove_pthread_dec inBuffer
pthread_mutex_t monk;

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
#pragma stage 1
void *StageOne(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageOne, " << id << endl;
  int i = 0;
  while (i < 101) {
    if (!BufferIsFull()) {
      // pthread_mutex_lock(&monk);
      int buff = inBuffer;
      // pthread_mutex_unlock(&monk);
      if (buff < BUFFER_SIZE) {
        buff = buff + 1;  
      }
      // sleep(i*2);
      cout << id << "; Put in buffer, " << buff << endl;
      // pthread_mutex_lock(&monk);
      inBuffer = buff;
      // pthread_mutex_unlock(&monk);
      i = i + 1;
     
      if (!BufferIsEmpty()) {
      // pthread_mutex_lock(&monk);
      cout << id << "; inBuffer : " << inBuffer << endl;
      inBuffer--;
      cout << id << "; inBuffer now: " << inBuffer << endl;
      // pthread_mutex_unlock(&monk);
      i = i + 1;
    }
    }
    
  }
  #pragma discovery pipeline remove_pthread_op
  // pthread_exit(NULL);
  return NULL;
}

void *StageTwo(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageTwo, " << id << endl;
  int i = 0;
  while (inBuffer != EOS) {
    if (!BufferIsEmpty()) {
      // pthread_mutex_lock(&monk);
      cout << id << "; inBuffer : " << inBuffer << endl;
      inBuffer--;
      cout << id << "; inBuffer now: " << inBuffer << endl;
      // pthread_mutex_unlock(&monk);
      i = i + 1;
    }
    // sleep(i*4);
  }
  #pragma discovery pipeline remove_pthread_op
  // pthread_exit(NULL);
  return NULL;
}

int main () {
  pthread_t threads[NUM_THREADS*2];
  int rc;
  int i;

  // for(i = 0; i < NUM_THREADS; i++) {
    int *arg = (int*) malloc(sizeof(*arg));
    cout << "main() : creating first batch thread, " << i << endl;
    // StageOne(&i);
    *arg = i;
    // rc = pthread_create(&threads[i], NULL, StageOne, arg);
    rc = 0;
    StageOne(arg);
    
    if (rc) {
      cout << "Error: unable to create thread, " << rc << endl;
      exit(-1);
    }
  // }
  cout << endl;
  cout << endl;
  // for(i = 0; i < NUM_THREADS; i++) {
    int *arg2 = (int*) malloc(sizeof(*arg2));
    *arg2 = i+NUM_THREADS;
    cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
    // rc = pthread_create(&threads[i+NUM_THREADS], NULL, StageTwo, arg2);
    rc = 0;
    // StageTwo(arg2);

    if (rc) {
      cout << "Error: unable to create thread, " << rc << endl;
      exit(-1);
    }
  // }

  // pthread_join(threads[1], NULL);
}