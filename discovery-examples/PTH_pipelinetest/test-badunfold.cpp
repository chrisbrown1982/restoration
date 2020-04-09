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
}

int main () {
  // pthread_t threads[NUM_THREADS*2];
  int rc;
  int i;

// #pragma pipeline stage one
//   for(i = 0; i < NUM_THREADS; i++) {
//     int *arg = (int*) malloc(sizeof(*arg));
//     cout << "main() : creating first batch thread, " << i << endl;
//     // StageOne(&i);
//     *arg = i;
//     // rc = pthread_create(&threads[i], NULL, StageOne, arg);

//     // if (rc) {
//     //   cout << "Error: unable to create thread, " << rc << endl;
//     //   exit(-1);
//     // }
//     StageOne(arg);
//   }
//   cout << endl;
//   cout << endl;

// #pragma pipeline stage two
//   for(i = 0; i < NUM_THREADS; i++) {
//     int *arg = (int*) malloc(sizeof(*arg));
//     *arg = i+NUM_THREADS;
//     cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
//     // rc = pthread_create(&threads[i+NUM_THREADS], NULL, StageTwo, arg);

//     // if (rc) {
//     //   cout << "Error: unable to create thread, " << rc << endl;
//     //   exit(-1);
//     // }
//     StageTwo(arg);
//   }


  // Merged for loops -- still doesn't work since these loops don't express the right looping behaviour.
  for(i = 0; i < NUM_THREADS; i++) {
    int *arg = (int*) malloc(sizeof(*arg));
    cout << "main() : creating first batch thread, " << i << endl;
    *arg = i;
    // StageOne(arg);
    int id = *((int*)arg);
    cout << "StageOne, " << id << endl;
    int i2 = 0;
    while (i2 < 101) {
      if (!BufferIsFull()) {
        // pthread_mutex_lock(&monk);
        if (inBuffer < BUFFER_SIZE) {
          inBuffer = inBuffer + 1;  
        }
        cout << id << "; Put in buffer, " << inBuffer << endl;
        // pthread_mutex_unlock(&monk);
        // sleep(i*2);
        i2 = i2 + 1;
      }
    }

    int *arg2 = (int*) malloc(sizeof(*arg2));
    *arg2 = i+NUM_THREADS;
    cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
    // StageTwo(arg2);
    int id2 = *((int*)arg2);
    cout << "StageTwo, " << id2 << endl;
    int i3 = 0;
    while (true) {
      if (!BufferIsEmpty()) {
        // pthread_mutex_lock(&monk);
        cout << id2 << "; inBuffer : " << inBuffer << endl;
        inBuffer--;
        cout << id2 << "; inBuffer now: " << inBuffer << endl;
        // pthread_mutex_unlock(&monk);
        i3 = i3 + 1;
      }
      sleep(i3*4);
    }
  }

  // pthread_join(threads[1], NULL);
}