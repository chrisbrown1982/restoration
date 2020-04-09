// A very simple stateful four stage pipeline. Each stage is slightly different.
// Data is manipulated in place, in the array, and barriers
// are used to enforce the correct pipelined access pattern
//
//     gcc -o final final .c
//     ./final dataSize 
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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXDATA 1000    /* maximum data size */

void InitializeData();

int dataSize;
int data[MAXDATA];

#define INPLACE  0
#define STATEFUL 0

// Simple type for a stateful, in-place pipeline stage function
// Proper one would be templated etc

typedef void (*stage) (void*, void*);

// Actual stage functions, process *data in place
// and may use/change *state

void Worker0 (int *state, int *data) {
  *state += *data + 1;       // I am stateful
  *data = *data + *state;    // mystate affects output
}

void Worker1 (int *state, int *data) {
  *state += *data + 2;       // I am stateful
  *data = *data + *state;    // mystate affects output
}

void Worker2 (int *state, int *data) {
  *state += *data - 2;       // I am stateful
  *data = *data + *state;    // mystate affects output
}

void Worker3 (int *state, int *data) {
  *state += *data - 1;       // I am stateful
  *data = *data + *state;    // mystate affects output
}

// A very simple pipeline interface and sequential implementation.
// Obviously a proper implementation would be polymorphic, variadic, templated, etc
// but this give the flavour.
// The first two parameters capture the pipeline modes, ie inplace or not, stateful or not
// The "implementation" below doesn't check these, but a real one would :)
// The fourth parameter should really be polymorphic

void Pipeline (int datamode, int statemode, int N, int *data,
	       stage s0, void *state0,
	       stage s1, void *state1,
	       stage s2, void *state2,
	       stage s3, void *state3) {

  // sequential implementation
  // for every stage
  //   process every item

  int i;

  for (i=0; i<N; i++) {
    s0(state0, &data[i]);
  }

  for (i=0; i<N; i++) {
    s1(state1, &data[i]);
  }

  for (i=0; i<N; i++) {
    s2(state2, &data[i]);
  }

  for (i=0; i<N; i++) {
    s3(state3, &data[i]);
  }

}  

void InitializeData() {

  int i;
  for (i = 0; i < dataSize; i++) {
    data[i] = 0;
  }
}

int main(int argc, char *argv[]) {

  long i;
  FILE *results;

  /* read command line and initialize data */
  dataSize   = atoi(argv[1]);
  InitializeData();

  // create the start states for each stage
  // in this state is just an integer,
  // initialised to the stage's position
  int startstates[4] = {0, 1, 2, 3};

  Pipeline (INPLACE, STATEFUL, dataSize, data,
	    (stage) &Worker0, &startstates[0],
	    (stage) &Worker1, &startstates[1],
	    (stage) &Worker2, &startstates[2],
	    (stage) &Worker3, &startstates[3]);

  results = fopen("results", "w");
  for (i = 0; i < dataSize; i++) {
      fprintf(results, "%d ", data[i]);
  }
  fprintf(results, "\n");
}

/*
 * Process:
 *
 * 1. Eliminate parallelism, global vars & main
 *    - Delete pthread_mutex & pthread_cond statements
 *    - Delete global thread attributes
 *    - Delete mutex & condition init
 *    - Enter InitialiseData (nothing to do)
 *    - Replace pthread_create calls with their passed functions
 *    - Enter each worker function
 *      - Worker 0
 *        - Enter Barrier
 *          - Delete pthread calls
 *        - Nothing else changes
 *      - Worker 1 to 3, no change
 *    - Delete pthread_join loop
 *    - No change to rest of main
 * 2. Sequence of four WorkerN calls are the pipe
 *    - Introduce pipeline over these four
 *    - Need to know that they act on the same data
 *    - & that they can't be swapped around? (not commutative?)
 *    - The fact they each have a for loop inside them could indicate that it's
 *      worth putting in a pipe
 *
 */
