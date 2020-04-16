// A very simple three stage pipeline with queues in between stages. 
// The first stage has just an output queue. The last stage has both input and
// output queues, and the output queue of the last stage is written to a result file.
// Queues are also bounded and the total amount of data that flows around is larger than
// the capacity of queues. 
//
//     gcc -o simplePipeWithQueues -lpthread original.c
//     ./original
//  

#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <tbb/pipeline.h>

#define BUFSIZE 1000
#define MAXDATA 5000
#define NRSTAGES 3

#define NTOKEN 5

/* Structure to keep info about queues between stages */
typedef struct {
  int *elements;
  int nr_elements; /* how many elements are currently in the queue */
  int capacity;
  int readFrom; /* position in the array from which to read from */
  int addTo; /* position in the array of the first free slot for adding data */
} queue_t;

void add_to_queue(queue_t *queue, int elem)
{
  queue->elements[queue->addTo] = elem;
  queue->addTo = (queue->addTo + 1) % queue->capacity;
  queue->nr_elements++;
}

int read_from_queue(queue_t *queue)
{
  int elem;
  elem = queue->elements[queue->readFrom];
  queue->nr_elements--;
  queue->readFrom = (queue->readFrom + 1) % queue->capacity;
  return elem;
}


/* Structure to keep information about input and output queue of a pipeline stage */
typedef struct {
queue_t *inputQueue;
queue_t *outputQueue;
} pipeline_stage_queues_t;


void InitialiseQueue(queue_t *queue, int capacity) {
  queue->elements = (int*) malloc (sizeof(int) * capacity);
  queue->readFrom = 0;
  queue->addTo = 0;
  queue->nr_elements = 0;
  queue->capacity = capacity;
}

struct S1Pair {
  int i_1;
  int my_output_1;
  int addTo;
  int nr_elements;
};

long long fib(int n) 
{ 
  /* Declare an array to store Fibonacci numbers. */
  long long f[n+1]; 
  long long i; 
  
  /* 0th and 1st number of the series are 0 and 1*/
  f[0] = 0; 
  f[1] = 1; 

  for (i = 2; i <= n; i++) 
  { 
      /* Add the previous 2 numbers in the series 
         and store it */
      f[i] = f[i-1] + f[i-2]; 
  } 
  
  // printf("f[n]:%i\n",f[n]);
  return f[n]; 
} 

void PipeTBB(int* first, int* elements, int* addTo, int* capacity, int* nr_elements) {
    tbb::parallel_pipeline( /*max_number_of_live_token=*/ 16,
        tbb::make_filter<void,int>(
            tbb::filter::serial,
            [&](tbb::flow_control& fc)-> int{
                // printf("Hello\n");
                if( *first >= 0 ) {
                    // printf("first: %i\n",*first);
                    int my_output = *first;
                    *first = *first-1;
                    return my_output;
                 } else {
                    // printf("Stop\n");
                    fc.stop();
                    return 0;
                }
            }    
        ) &
        tbb::make_filter<int,int>(
            tbb::filter::parallel,
            [](int my_input){
              int my_output;
              // printf("my_input : %i\n", my_input);
              if (my_input > 0) {
                // long long fibn = fib(100);
                // fprintf(stderr, "fib : %lld\n", fibn);
                my_output = my_input + 1;
              } else { // 0 is a terminating token...If we get it, we just pass it on...
                my_output = 0;
              // add_to_queue(myOutputQueue_2, my_output_2);
              // printf("my_output:%i\n",my_output);
              }
              return my_output;
            }
        ) &
        tbb::make_filter<int,void>(
            tbb::filter::serial,
            [&](int my_input) {
              int my_output;
              if (my_input >= 0)
                my_output = my_input * 2;
              else
                my_output = -1;
              // add_to_queue(myOutputQueue, my_output);
              elements[*addTo] = my_output;
              *addTo = (*addTo + 1) % *capacity;
              *nr_elements=*nr_elements+1;
            }
        )
    );
}
     

int main(int argc, char *argv[]) {
  long i;
  FILE *results;

  /* initialize queues */
  queue_t queue[NRSTAGES];
  for (i=0; i<NRSTAGES; i++) {
    int capacity;
    if (i<NRSTAGES-1)
      capacity = BUFSIZE;
    else
      capacity = MAXDATA+1; /* the output queue of the last stage has infinite capacity */
    InitialiseQueue(&queue[i], capacity);
  }

  /* Set input and output queues for each stage */
  pipeline_stage_queues_t stage_queues[NRSTAGES];
  for (i=0; i<NRSTAGES; i++) {
    if (i>0) 
      stage_queues[i].inputQueue = &queue[i-1];
    else
      stage_queues[i].inputQueue = NULL;
    stage_queues[i].outputQueue = &queue[i];
  }
  
  // Pipe((void *)&stage_queues[0], (void *)&stage_queues[1], (void *)&stage_queues[2]);

  int first = MAXDATA;

  PipeTBB(&first,
          queue[NRSTAGES-1].elements,
          &(queue[NRSTAGES-1].addTo),
          &(queue[NRSTAGES-1].capacity),
          &(queue[NRSTAGES-1].nr_elements));

  queue_t output_queue = queue[NRSTAGES-1];

  results = fopen("results", "w");
  fprintf(results, "number of stages:  %d\n",NRSTAGES);
  for (i = 0; i < MAXDATA; i++) {
    fprintf(results, "%d ", output_queue.elements[i]);
  }
  fprintf(results, "\n");
  fclose(results);
  return 0;
}


