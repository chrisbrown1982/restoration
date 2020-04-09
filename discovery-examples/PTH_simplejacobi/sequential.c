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

/* This version has removed all the pthread code, leaving us with a simple
   sequential version. */
/* This sequential version can then be used to introduce the parallel
   (sequential) stencil pattern/skeleton. */

#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <limits.h>
#define SHARED 1
#define MAXGRID 10000   /* maximum grid size, including boundaries */

void Worker();
void InitializeGrids();

struct tms buffer;        /* used for timing */
clock_t start, finish;

int gridSize, numWorkers, numIters, stripSize;

double maxDiff;
double grid1[MAXGRID][MAXGRID], grid2[MAXGRID][MAXGRID];

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


/* Each Worker computes values in one strip of the grids.
   The main worker loop does two computations to avoid copying from
   one grid to the other.  */
/*
   Workers are reduced to a single worker that performs over the entire grid.
*/

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

  for (iters = 1; iters <= numIters; iters++) {
    /* update my points */
    for (i = first; i <= last; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid2[i][j] = (grid1[i-1][j] + grid1[i+1][j] +
                       grid1[i][j-1] + grid1[i][j+1]) * 0.25;
      }
    }
    /* update my points again */
    for (i = first; i <= last; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid1[i][j] = (grid2[i-1][j] + grid2[i+1][j] +
               grid2[i][j-1] + grid2[i][j+1]) * 0.25;
      }
    }
  }
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
