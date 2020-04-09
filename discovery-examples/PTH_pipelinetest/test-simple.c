#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

/*
  Simulates a simple producer-consumer pipeline where the producer stage
  increments a counter and the consumer stage prints the counter.
*/

/* Number of firings of the pipeline. */
#define N 10
/* Whether to print debug information. */
#define DEBUG 0

sem_t producer_sem, consumer_sem;

int counter = 0;

void *producer(void *arg) {
  int i;
  for (i = 0; i < N; i++) {
    sem_wait(&producer_sem);
    counter++;
    if (DEBUG) printf("producer: %d\n", counter);
    sem_post(&consumer_sem);
  }
  pthread_exit(NULL);
}

void *consumer(void *arg) {
  int i;
  for (i = 0; i < N; i++) {
    sem_wait(&consumer_sem);
    printf("%d\n", counter);
    sem_post(&producer_sem);
  }
  pthread_exit(NULL);
}

int main(void) {
  sem_init(&producer_sem, 0, 1);
  sem_init(&consumer_sem, 0, 0);
  pthread_t producer_thread, consumer_thread;
  pthread_create(&producer_thread, NULL, producer, NULL);
  pthread_create(&consumer_thread, NULL, consumer, NULL);
  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);
  return 0;
}
