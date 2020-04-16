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

#define BUFSIZE 100//0
#define MAXDATA 500//0
#define NRSTAGES 3

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

struct PipeStruct {
  // INPUTS
  int* i_1;
  // OUTPUTS
  queue_t* myOutputQueue_3;
  // INTER-STAGE TEMP VARS; USED IN LOOP COND
  int* my_output_1;
  int* my_output_2;
};

PipeStruct S1(PipeStruct arg) {
  if (*arg.i_1 >= 0) {
    *arg.my_output_1 = *arg.i_1;
    *arg.i_1 = *arg.i_1-1;
  } 
  return arg;
}

PipeStruct S2(PipeStruct arg) {
  if (*arg.my_output_1 >= 0) {
    if (*arg.my_output_1 > 0)
      *arg.my_output_2 = *arg.my_output_1 + 1;
    else // 0 is a terminating token...If we get it, we just pass it on...
      *arg.my_output_2 = 0;
  }
  return arg;
}

PipeStruct S3(PipeStruct arg) {
  int my_output_3;
  if (*arg.my_output_2 >= -1) {
      if (*arg.my_output_2 >= 0)
        my_output_3 = *arg.my_output_2 * 2;
      else
        my_output_3 = -1;
      arg.myOutputQueue_3->elements[arg.myOutputQueue_3->addTo] = my_output_3;
      arg.myOutputQueue_3->addTo = (arg.myOutputQueue_3->addTo + 1) % arg.myOutputQueue_3->capacity;
      arg.myOutputQueue_3->nr_elements++;
    }
  return arg;
}


void Pipe(void* a1, void* a2, void* a3) {
  // STAGE ONE
  int my_output_1, i_1 = MAXDATA;
  
  pipeline_stage_queues_t *myQueues_1 = (pipeline_stage_queues_t *)a1;
  queue_t *myOutputQueue_1 = myQueues_1->outputQueue;

  // STAGE TWO
  int my_input_2;
  int my_output_2;

  pipeline_stage_queues_t *myQueues_2 = (pipeline_stage_queues_t *)a2;
  queue_t *myOutputQueue_2 = myQueues_2->outputQueue;
  queue_t *myInputQueue_2 = myQueues_2->inputQueue;

  // STAGE THREE
  int my_input_3;
  int my_output_3;

  pipeline_stage_queues_t *myQueues_3 = (pipeline_stage_queues_t *)a3;
  queue_t *myOutputQueue_3 = myQueues_3->outputQueue;
  queue_t *myInputQueue_3 = myQueues_3->inputQueue;

  do {
    PipeStruct arg = PipeStruct {&i_1, myOutputQueue_3, &my_output_1, &my_output_2};
    PipeStruct r = S3(S2(S1(arg)));
    // my_output_1 = r.my_output_1;
    // my_output_2 = r.my_output_2;
  } while (i_1 >= 0 || my_output_1 > 0 || my_output_2 > 0);
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
  
  Pipe((void *)&stage_queues[0], (void *)&stage_queues[1], (void *)&stage_queues[2]);

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


