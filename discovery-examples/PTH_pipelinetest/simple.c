#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <unistd.h>

pthread_t one;
pthread_t two;

int shared = 0;
int flag = 1;
pthread_mutex_t mut;

void *StageOne(void *threadarg1) {
  for (int i1 = 0; i1 < 5; i1++) {
    // Read shared
    pthread_mutex_lock(&mut);
    int loc_shared = shared;
    pthread_mutex_unlock(&mut);

    sleep(1);

    // Update shared
    pthread_mutex_lock(&mut);
    shared = loc_shared++;
    printf("(1-%i) shared = %i\n", i1, shared);
    pthread_mutex_unlock(&mut);
  }
  
  pthread_exit(NULL);
}

void *StageTwo(void *threadarg2) {
  for (int i2 = 0; i2 < 5; i2++) { // Needs an EOS...
    // Read shared
    pthread_mutex_lock(&mut);
    int loc_shared = shared;
    pthread_mutex_unlock(&mut);

    sleep(1);

    // Update shared
    pthread_mutex_lock(&mut);
    shared = loc_shared--;
    printf("(2-%i) shared = %i\n", i2, shared);
    pthread_mutex_unlock(&mut);
  }

  pthread_exit(NULL);
}

int main () {
  int status;

  // Stage One
  status = pthread_create(&one, NULL, StageOne, 0);
  if (status != 0) {
    exit(-1);
  }

  // Stage Two
  status = pthread_create(&two, NULL, StageTwo, 0);
  if (status != 0) {
    exit(-1);
  }

  pthread_join(one, NULL);
  pthread_join(two, NULL);

  return 0;
}