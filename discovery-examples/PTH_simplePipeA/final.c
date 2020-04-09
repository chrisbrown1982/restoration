// A very simple stateful pipeline. All stages are the same.
// Data is manipulated in place, in the array.
//
//     gcc -o final .final.c
//     ./final dataSize 
//  

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXDATA 1000    /* maximum input size*/

#define INPLACE  0
#define STATEFUL 0

void InitializeData();

int dataSize;

int data[MAXDATA];

// Simple type for a stateful, in-place pipeline stage function
// Proper one would be templated etc

typedef void (*stage) (void*, void*);

// An actual stage function, processes *data in place
// and may use/change *state

void Worker(int *state, int *data) {
 
  *state += *data + 1;       // I am stateful
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
	    (stage) &Worker, &startstates[0],
	    (stage) &Worker, &startstates[1],
	    (stage) &Worker, &startstates[2],
	    (stage) &Worker, &startstates[3]);

  results = fopen("results", "w");
  for (i = 0; i < dataSize; i++) {
      fprintf(results, "%d ", data[i]);
  }
  fprintf(results, "\n");
}


void InitializeData() {

  int i;
  for (i = 0; i < dataSize; i++) {
    data[i] = 0;
  }
}

/*
 * Process:
 *
 * 1. Remove all existing parallelism, start from main()
 *    Note names of all deleted declarations
 *    - Delete pthread_t and pthread_attr_t declarations
 *    - Delete statements setting up attributes
 *    - Delete mutex and cond init statements
 *    - dataSize and numWorkers are left alone since they're not in the pthread
 *      library and do not use any deleted variables.
 *    - Descend into InitialiseData
 *      - No change here since it does not use any deleted vars or pthread ops
 *    - Replace pthread_create with a call to the function (Worker) passed as
 *      argument, using the arguments (i) that were passed.
 *    - Descend into the function now called
 *      - Change its type from pointer to value
 *      - Descend into Barrier()
 *        - remove pthread_ functions
 *          I think we can simply delete these.
 *          It would be nice to be able to know to delete Barrier() entirety
 *    - Remove second for loop in main since pthread_join is deleted
 * 2. The pipe is the for loop on lines 45--46 in seq.c
 *    - Need to know that Worker updates shared state (& what it is)
 *    - Introduce pipe stage (contents of for loop + initial mystate var)
 *    - Replicate pipe stage (+ increment mystate) numWorkers times
 *    - For loop should be replaced by call to pipe
 *
 */

