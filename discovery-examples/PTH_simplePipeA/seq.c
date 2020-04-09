// A very simple stateful pipeline. All stages are the same.
// Data is manipulated in place, in the array, and barriers
// are used to enforce the correct pipelined access pattern
//
//     gcc -o simplePipeA -lpthread simplePipeA.c
//     ./simplepipeA dataSize numWorkers

// Possible annotations
// This is a "numWorkers" stage pipeline
// Data is processed in place in array "data"
// The element-wise stage work for all stages consists
// of the middle two lines of the second loop in function "Worker"
// All stages are stateful, via variable "mystate"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SHARED 1
#define MAXDATA 1000    /* maximum data array size */
#define MAXWORKERS 16   /* maximum number of worker threads */


void Worker(void *);
void InitializeData();
void Barrier();

int numArrived = 0;       /* count of the number who have arrived */

int dataSize, numWorkers;

int data[MAXDATA];


int main(int argc, char *argv[]) {
  long i;
  FILE *results;

  /* read command line and initialize data */
  dataSize   = atoi(argv[1]);
  numWorkers = atoi(argv[2]);
  InitializeData();

  /* create the workers, then wait for them to finish */
  for (i = 0; i < numWorkers; i++)
    Worker((void *) i);

  results = fopen("results", "w");
  fprintf(results, "number of workers:  %d\n",numWorkers);
  for (i = 0; i < dataSize; i++) {
      fprintf(results, "%d ", data[i]);
  }
  fprintf(results, "\n");
}


void Worker(void *arg) {
  long myid = (long) arg;
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
  for (i=0; i<numWorkers-myid-1; i++) {
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
  if (numArrived == numWorkers) {
    numArrived = 0;
  }
}
