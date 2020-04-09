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
#pragma discovery pipeline remove_pthread_dec inBuffer
pthread_mutex_t monk;

bool BufferIsEmpty() {
  pthread_mutex_lock(&monk);
  bool state = (inBuffer < 0);
  pthread_mutex_unlock(&monk);
  return state;
}

bool BufferIsFull() {
  pthread_mutex_lock(&monk);
  bool state = (inBuffer >= BUFFER_SIZE);
  pthread_mutex_unlock(&monk);
  return state;
}

void *StageOne(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageOne, " << id << endl;
  int i = 0;
  while (i < 10) {
    if (!BufferIsFull()) {
      int buff;
      pthread_mutex_lock(&monk);
      buff = inBuffer;
      pthread_mutex_unlock(&monk);

      if (buff < BUFFER_SIZE) {
        buff = buff + 1;  
      }
      sleep(1);
      cout << id << "; Put in buffer, " << buff << endl;
      
      pthread_mutex_lock(&monk);
      inBuffer = buff;
      pthread_mutex_unlock(&monk);
      i = i + 1;
    }
  }
  pthread_exit(NULL);
}

void *StageTwo(void *threadarg) {
  int id = *((int*)threadarg);
  cout << "StageTwo, " << id << endl;
  int i = 0;
  while (i < 20) {
    if (!BufferIsEmpty()) {
      int buff;
      pthread_mutex_lock(&monk);
      buff = inBuffer;
      pthread_mutex_unlock(&monk);


      cout << id << "; inBuffer : " << inBuffer << endl;
      buff--;
      cout << id << "; inBuffer now: " << inBuffer << endl;

      pthread_mutex_lock(&monk);
      inBuffer = buff;
      pthread_mutex_unlock(&monk);
      i = i + 1;
    }
    sleep(1);
  }
  pthread_exit(NULL);
}

int main () {
  pthread_t threads[NUM_THREADS*2];
  int rc;
  int i;

  for(i = 0; i < NUM_THREADS; i++) {
    int *arg = (int*) malloc(sizeof(*arg));
    cout << "main() : creating first batch thread, " << i << endl;
    // StageOne(&i);
    *arg = i;
    rc = pthread_create(&threads[i], NULL, StageOne, arg);

    if (rc) {
      cout << "Error: unable to create thread, " << rc << endl;
      exit(-1);
    }
  }
  cout << endl;
  cout << endl;
  for(i = 0; i < NUM_THREADS; i++) {
    int *arg = (int*) malloc(sizeof(*arg));
    *arg = i+NUM_THREADS;
    cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
    rc = pthread_create(&threads[i+NUM_THREADS], NULL, StageTwo, arg);

    if (rc) {
      cout << "Error: unable to create thread, " << rc << endl;
      exit(-1);
    }
  }

  pthread_join(threads[1], NULL);
}