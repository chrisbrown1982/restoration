//Jacobi iteration using pthreads by Greg Andrews
//http://www.cs.arizona.edu/people/greg/mpdbook/programs/jacobi.c
//
//   usage on DICE:
//     gcc -O2 -o simplejacobi simplejacobi.c
//     ./simplejacobi gridSize numIters
//

// A simple parallel stencil program
//
// Possible annotations
//    This is a stencil
//    Double buffered (or two stencils in use?)
//    Pointwise code is the innermost body of the two loops in function "Worker"
//    Data is grid1=> grid2  and grid2 => grid1 (ie  double buffering)
//    Stencil(s) is/are iterated - termination is by iteration count

#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <limits.h>
#define SHARED 1
#define MAXGRID 10000   /* maximum grid size, including boundaries */

void Worker();
void InitializeGrids();
/*void Barrier();*/

struct tms buffer;        /* used for timing */
clock_t start, finish;

int gridSize, numWorkers, numIters, stripSize;

double maxDiff;
double grid1[MAXGRID][MAXGRID], grid2[MAXGRID][MAXGRID];

typedef void (*callback)(int*, int*, int*, int*);
typedef double (*scallback)(int, int);

// Iteration pattern, counts down to 0
// Should have a list of arguments to pass to f
void iter(const callback f, int* arg1, int* arg2, int* arg3, int* arg4, int n) {
  while (n > 0) {
    f(arg1, arg2, arg3, arg4);
    n--;
  }
}

// GrPPI stencil would require two stages: gather neighbourhood & reduction
// Neighbourhood: list of elements from grid1
// Reduction: in this case, sum with a (* 0.25) to the result
void stencil(const scallback f, int* arg1, int* arg2, int* arg3, int* arg4, double grid[MAXGRID][MAXGRID]) {
  int i = *arg1;
  int j = *arg2;
  int first = *arg3;
  int last = *arg4;
  // gridsize is global, but should be passed in as an argument
  // grid1 & grid2 should also be passed in -- separate due to situations like this
  for (i = first; i <= last; i++) {
    for (j = 1; j <= gridSize; j++) {
      grid[i][j] = f(i, j);
    }
  }
}

/* main() -- read command line, initialize grids, and create threads
             when the threads are done, print the results */

int main(int argc, char *argv[]) {
  /* thread ids and attributes */
  double maxdiff = maxDiff = 0.0;
  FILE *results;

  /* read command line and initialize grids */
  gridSize = atoi(argv[1]);
  numIters = atoi(argv[2]);
  InitializeGrids();

  start = times(&buffer);
  /* create the workers, then wait for them to finish */
  Worker();

  finish = times(&buffer);
  /* print the results */
  printf("elapsed time:  %d\n", finish-start);
}

double Worker1(int i, int j) {
  return (grid1[i-1][j] + grid1[i+1][j] +
      grid1[i][j-1] + grid1[i][j+1]) * 0.25;
}

double Worker2(int i, int j) {
  return (grid2[i-1][j] + grid2[i+1][j] +
      grid2[i][j-1] + grid2[i][j+1]) * 0.25;
}

void Worker0(int* iref, int* jref, int* firstref, int* lastref) {
  int i = *iref;
  int j = *jref;
  int first = *firstref;
  int last = *lastref;

  scallback w1 = Worker1;
  scallback w2 = Worker2;

  stencil(w1, iref, jref, firstref, lastref, grid2);
  stencil(w2, iref, jref, firstref, lastref, grid1);
};


/* Each Worker computes values in one strip of the grids.
   The main worker loop does two computations to avoid copying from
   one grid to the other.  */

void Worker() {
  double maxdiff, temp;
  int i, j, iters;
  int first, last;

  //printf("worker %d (pthread id %d) has started\n", myid, pthread_self());

  /* determine first and last rows of my strip of the grids */
  /*first = myid*stripSize + 1;*/
  /*last = first + stripSize - 1;*/
  first = 1;
  last = gridSize-1;

  callback w0 = Worker0;

  iter(w0, &i, &j, &first, &last, numIters);

  /* compute the maximum difference in my strip and set global variable */
  maxdiff = 0.0;
  for (i = first; i <= last; i++) {
    for (j = 1; j <= gridSize; j++) {
      temp = grid1[i][j]-grid2[i][j];
      if (temp < 0)
        temp = -temp;
      if (maxdiff < temp)
        maxdiff = temp;
    }
  }
  maxDiff = maxdiff;
}

void InitializeGrids() {
  /* initialize the grids (grid1 and grid2)
     set boundaries to 1.0 and interior points to 0.0  */
  int i, j;

  for (i = 0; i <= gridSize+1; i++)
    for (j = 0; j <= gridSize+1; j++) {
      grid1[i][j] = 0.0;
      grid2[i][j] = 0.0;
    }
  for (i = 0; i <= gridSize+1; i++) {
    grid1[i][0] = 1.0;
    grid1[i][gridSize+1] = 1.0;
    grid2[i][0] = 1.0;
    grid2[i][gridSize+1] = 1.0;
  }
  for (j = 0; j <= gridSize+1; j++) {
    grid1[0][j] = 1.0;
    grid2[0][j] = 1.0;
    grid1[gridSize+1][j] = 1.0;
    grid2[gridSize+1][j] = 1.0;
  }
}
