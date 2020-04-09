// A very simple stateful four stage pipeline. Each stage is slightly different.
// Data is manipulated in place, in the array, and barriers
// are used to enforce the correct pipelined access pattern
//
//     gcc -o simplePipeB -lpthread simplePipeB.c
//     ./simplepipeB dataSize 
//  

// Possible annotations
// This is a 4 stage pipeline
// Data is processed in place in array "data"
// Element-wise stage work for stage 0 consists
// of the middle two lines of the second loop in function "Worker0"
// Element-wise stage work for stage 1 consists
// of the middle two lines of the second loop in function "Worker1"
// Element-wise stage work for stage 2  consists
// of the middle two lines of the second loop in function "Worker2"
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
  numArrived++;
  if (numArrived == NWORKERS) {
    numArrived = 0;
  }
}

int main(int argc, char *argv[]) {
  /* thread ids and attributes */

  long i, junk;
  FILE *results;

  /* read command line and initialize data */
  dataSize   = atoi(argv[1]);
  InitializeData();

  /* create the workers, then wait for them to finish */
  Worker0(&junk);
  Worker1(&junk);
  Worker2(&junk);
  Worker3(&junk);

  results = fopen("results", "w");
  fprintf(results, "number of workers:  %d\n",NWORKERS);
  for (i = 0; i < dataSize; i++) {
      fprintf(results, "%d ", data[i]);
  }
  fprintf(results, "\n");
}


