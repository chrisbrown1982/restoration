// A simplem but slightly broken, stateful four stage pipeline, derived
// from simplePipeB.c. Each stage is slightly different.
// Data is manipulated in place, in the array, and barriers
// are used to enforce the correct pipelined access pattern
//
// Broken because worker2 (the third stage) "incorrectly" accesses
// data[i+2] in its i'th iteration, putting it in a data race
// with worker0 (the first stage).
//
//     gcc -o nearPipeB -lpthread nearPipeB.c
//     ./nearPipeB dataSize 
//  
// Possible annotations
// This is NEARLY a 4 stage pipeline
// Data is processed in place in array "data"
// Element-wise stage work for stage 0 consists
// of the middle two lines of the second loop in function "Worker0"
// Element-wise stage work for stage 1 consists
// of the middle two lines of the second loop in function "Worker1"
// Element-wise stage work for stage 2  consists
// of the middle two lines of the second loop in function "Worker2"
// HAZARD Stage 2 is not a valid pipeline stage:
//   LINE "if ((i+2)<dataSize) data[i+2] = 0;" creates a data race
// Element-wise stage work for stage 3 consists
// of the middle two lines of the second loop in function "Worker3"
// All stages are stateful, via variable "mystate"

#define _REENTRANT
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SHARED 1
#define MAXDATA 1000    /* maximum grid size, including boundaries */
#define NWORKERS 4

void *Worker(void *);
void InitializeData();
void Barrier();

pthread_mutex_t barrier;  /* mutex semaphore for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numArrived = 0;       /* count of the number who have arrived */

int dataSize;

int data[MAXDATA];


void *Worker0(void *arg) {
  int myid = 0;
  int i;
  int mystate = myid;

  // tick over while the first item "reaches" me
  for (i=0; i<myid; i++) {
    Barrier();
  }

  // process items one by one
  for (i=0; i<dataSize; i++) {
    mystate += data[i]+1;       // I am stateful
    data[i] = data[i]+mystate;  // mystate affects output
    Barrier();
  }

  // tick over while the pipeline drains
  for (i=0; i<NWORKERS-myid-1; i++) {
    Barrier();
  }
}

void *Worker1(void *arg) {
  int myid = 1;
  int i;
  int mystate = myid;

  // tick over while the first item "reaches" me
  for (i=0; i<myid; i++) {
    Barrier();
  }

  // process items one by one
  for (i=0; i<dataSize; i++) {
    mystate += data[i]+2;       // I am stateful
    data[i] = data[i]+mystate;  // mystate affects output
    Barrier();
  }

  // tick over while the pipeline drains
  for (i=0; i<NWORKERS-myid-1; i++) {
    Barrier();
  }
}

void *Worker2(void *arg) {
  int myid = 2;
  int i;
  int mystate = myid;

  // tick over while the first item "reaches" me
  for (i=0; i<myid; i++) {
    Barrier();
  }

  // process items one by one
  for (i=0; i<dataSize; i++) {
    mystate += data[i]-2;       // I am stateful
    if ((i+2)<dataSize) data[i+2] = 0;
    data[i] = data[i]+mystate;  // mystate affects output
    Barrier();
  }

  // tick over while the pipeline drains
  for (i=0; i<NWORKERS-myid-1; i++) {
    Barrier();
  }
}

void *Worker3(void *arg) {
  int myid = 3;
  int i;
  int mystate = myid;

  // tick over while the first item "reaches" me
  for (i=0; i<myid; i++) {
    Barrier();
  }

  // process items one by one
  for (i=0; i<dataSize; i++) {
    mystate += data[i]-1;       // I am stateful
    data[i] = data[i]+mystate;  // mystate affects output
    Barrier();
  }

  // tick over while the pipeline drains
  for (i=0; i<NWORKERS-myid-1; i++) {
    Barrier();
  }
}

void InitializeData() {

  int i;
  for (i = 0; i < dataSize; i++) {
    data[i] = 0;
  }
}

void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == NWORKERS) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

int main(int argc, char *argv[]) {
  /* thread ids and attributes */

  pthread_t workerid[NWORKERS];
  pthread_attr_t attr;
  long i, junk;
  FILE *results;

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line and initialize data */
  dataSize   = atoi(argv[1]);
  InitializeData();

  /* create the workers, then wait for them to finish */
  pthread_create(&workerid[0], &attr, Worker0, &junk);
  pthread_create(&workerid[1], &attr, Worker1, &junk);
  pthread_create(&workerid[2], &attr, Worker2, &junk);
  pthread_create(&workerid[3], &attr, Worker3, &junk);

  for (i = 0; i < NWORKERS; i++)
    pthread_join(workerid[i], NULL);

  results = fopen("results", "w");
  fprintf(results, "number of workers:  %d\n",NWORKERS);
  for (i = 0; i < dataSize; i++) {
      fprintf(results, "%d ", data[i]);
  }
  fprintf(results, "\n");
}


