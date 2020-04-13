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

#define BUFSIZE 1000
#define MAXDATA 5000
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


/* First stage just emits data */
void *Stage1(void *arg) {
  int my_output, i = MAXDATA;
  
  pipeline_stage_queues_t *myQueues = (pipeline_stage_queues_t *)arg;
  queue_t *myOutputQueue = myQueues->outputQueue;

  do {
    my_output = i;
    i--;
    add_to_queue(myOutputQueue, my_output);
  } while(i>=0);
  
  return NULL;
}

/* Second stage reads an element from the input queue, adds 1 to it and writes it to the output queue */
void *Stage2(void *arg) {
  int my_input;
  int my_output;

  pipeline_stage_queues_t *myQueues = (pipeline_stage_queues_t *)arg;
  queue_t *myOutputQueue = myQueues->outputQueue;
  queue_t *myInputQueue = myQueues->inputQueue;

  do {
    my_input = read_from_queue(myInputQueue);
    if (my_input > 0)
      my_output = my_input + 1;
    else /* 0 is a terminating token...If we get it, we just pass it on... */
      my_output = 0;
    add_to_queue(myOutputQueue, my_output);
  } while (my_input>0);

  return NULL;
}

/* Third stage reads an element from the input queue, multiplies it by 2 and writes it to the output queue */
void *Stage3(void *arg) {
  int my_input;
  int my_output;

  pipeline_stage_queues_t *myQueues = (pipeline_stage_queues_t *)arg;
  queue_t *myOutputQueue = myQueues->outputQueue;
  queue_t *myInputQueue = myQueues->inputQueue;

  do {
    my_input = read_from_queue(myInputQueue);
    if (my_input >= 0)
      my_output = my_input * 2;
    else
      my_output = -1;
    add_to_queue(myOutputQueue, my_output);
  } while (my_input>0);

  return NULL;
}

void InitialiseQueue(queue_t *queue, int capacity) {
  queue->elements = (int*) malloc (sizeof(int) * capacity);
  queue->readFrom = 0;
  queue->addTo = 0;
  queue->nr_elements = 0;
  queue->capacity = capacity;
}

void Pipe(void* a1, void* a2, void* a3) {
  Stage1(a1);
  Stage2(a2);
  Stage3(a3);
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


