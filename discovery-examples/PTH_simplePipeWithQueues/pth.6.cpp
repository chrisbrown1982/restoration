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
    // STAGE ONE
    if (i_1 >= 0) {
      my_output_1 = i_1;
      i_1--;
      // add_to_queue(myOutputQueue_1, my_output_1);
      // READ -- addTo
      // WRITE -- elements
      // myOutputQueue_1->elements[myOutputQueue_1->addTo] = my_output_1;
      // READ -- capacity
      // UPDATE -- addTo
      // myOutputQueue_1->addTo = (myOutputQueue_1->addTo + 1) % myOutputQueue_1->capacity;
      // UPDATE -- nr_elements
      // myOutputQueue_1->nr_elements++;
    }

    printf("s1s2 queue : %i\n", myOutputQueue_1->nr_elements);

    // STAGE TWO
    // my_input_2 = read_from_queue(myInputQueue_2);

    // READ
    int elem_2;
    // READ -- readFrom, elements
    // elem_2 = myInputQueue_2->elements[myInputQueue_2->readFrom];
    elem_2 = my_output_1;
    // UPDATE -- nr_elements
    // myInputQueue_2->nr_elements--; // this would be classified as a write?
    // READ -- capacity
    // UPDATE -- readFrom
    // myInputQueue_2->readFrom = (myInputQueue_2->readFrom + 1) % myInputQueue_2->capacity; // this too?
    my_input_2 = elem_2;

    if (my_input_2 >= 0) {
      printf("my_input_2 : %i\n", my_input_2);
      if (my_input_2 > 0)
        my_output_2 = my_input_2 + 1;
      else // 0 is a terminating token...If we get it, we just pass it on...
        my_output_2 = 0;
      // add_to_queue(myOutputQueue_2, my_output_2);

      // READ -- addTo
      // WRITE -- elements
      // myOutputQueue_2->elements[myOutputQueue_2->addTo] = my_output_2;
      // READ -- capacity
      // UPDATE -- addTo
      // myOutputQueue_2->addTo = (myOutputQueue_2->addTo + 1) % myOutputQueue_2->capacity;
      // UPDATE -- nr_elements
      // myOutputQueue_2->nr_elements++;
    }

    // STAGE THREE
    // my_input_3 = read_from_queue(myInputQueue_3);
    // READ
    int elem_3;
    // READ -- readFrom, elements
    // elem_3 = myInputQueue_3->elements[myInputQueue_3->readFrom];
    elem_3 = my_output_2;
    // UPDATE -- nr_elements
    // myInputQueue_3->nr_elements--;
    // READ -- capacity
    // UPDATE -- readFrom
    // myInputQueue_3->readFrom = (myInputQueue_3->readFrom + 1) % myInputQueue_3->capacity;
    my_input_3 = elem_3;
    if (my_input_3 >= -1) {
      if (my_input_3 >= 0)
        my_output_3 = my_input_3 * 2;
      else
        my_output_3 = -1;
      // add_to_queue(myOutputQueue_3, my_output_3);
      // READ -- addTo
      // WRITE -- elements
      myOutputQueue_3->elements[myOutputQueue_3->addTo] = my_output_3;
      // READ -- capacity
      // UPDATE -- addTo
      myOutputQueue_3->addTo = (myOutputQueue_3->addTo + 1) % myOutputQueue_3->capacity;
      // UPDATE -- nr_elements
      myOutputQueue_3->nr_elements++;
    }
  } while (i_1 >= 0 || my_input_2 > 0 || my_input_3 > 0);
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


