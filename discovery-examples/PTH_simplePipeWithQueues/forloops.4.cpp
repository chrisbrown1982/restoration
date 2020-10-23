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
// #include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BUFSIZE 100//0
#define MAXDATA 5//0//0//0
#define NRSTAGES 3

/* Structure to keep info about queues between stages */
typedef struct {
  int *elements;
  int nr_elements; /* how many elements are currently in the queue */
  int capacity;
  int readFrom; /* position in the array from which to read from */
  int addTo; /* position in the array of the first free slot for adding data */
  pthread_mutex_t queue_lock;
  pthread_cond_t queue_cond_write; /* signalled when we write something into the queue, wakes up threads waiting to read from an empty queue */
  pthread_cond_t queue_cond_read; /* signalled when we read something from the queue, wakes up threads waiting to write to a full queue */
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


long long fibr(long long n) {
  if (n <= 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return (fibr(n-1) + fibr(n-2));
  }
}

void InitialiseQueue(queue_t *queue, int capacity) {
  queue->elements = (int*) malloc (sizeof(int) * capacity);
  queue->readFrom = 0;
  queue->addTo = 0;
  queue->nr_elements = 0;
  queue->capacity = capacity;
}


void Pipe(void* a0[], void* a1, void* a2, void* a3) {
  int my_output1, i1; // = MAXDATA;
  
  pipeline_stage_queues_t *myQueues1 = (pipeline_stage_queues_t *)a1;
  queue_t *myOutputQueue1 = myQueues1->outputQueue;

  a0[0] = NULL;

  int my_input2;
  int my_output2;

  pipeline_stage_queues_t *myQueues2 = (pipeline_stage_queues_t *)a2;
  queue_t *myOutputQueue2 = myQueues2->outputQueue;
  queue_t *myInputQueue2 = myQueues2->inputQueue;

  a0[1] = NULL;

  int my_input3;
  int my_output3;

  pipeline_stage_queues_t *myQueues3 = (pipeline_stage_queues_t *)a3;
  queue_t *myOutputQueue3 = myQueues3->outputQueue;
  queue_t *myInputQueue3 = myQueues3->inputQueue;

  // for (i1 = MAXDATA ; i1>=-1; i1--) {
  //   printf("S1 : %d\n",i1);
  //   my_output1 = i1;
  //   add_to_queue(myOutputQueue1, my_output1);
  // }
  
  // // my_input2 = read_from_queue(myInputQueue2);
  // for (my_input2 = read_from_queue(myInputQueue2); my_input2>=0; my_input2 = read_from_queue(myInputQueue2)) {
  //   // printf("S2 : %d\n",my_input2);
  //   if (my_input2 > 0) {
  //     // long long fibn = fibr(32);
  //     my_output2 = my_input2 + 1;
  //   } else { /* 0 is a terminating token...If we get it, we just pass it on... */
  //     my_output2 = -1;
  //   }
  //   add_to_queue(myOutputQueue2, my_output2);
  // }

  // // my_input3 ;
  // for (my_input3 = read_from_queue(myInputQueue3); my_input3>=0; my_input3 = read_from_queue(myInputQueue3)) {
  //   printf("S3 : %d\n",my_input3);
  //   if (my_input3 > 0)
  //     my_output3 = my_input3 * 2;
  //   else
  //     my_output3 = -1;
  //   add_to_queue(myOutputQueue3, my_output3);
  // }

  for (i1 = MAXDATA; i1>=-1; i1--) {
    if (i1>=-1) {
      printf("S1 : %d\n",i1);
      my_output1 = i1;
      add_to_queue(myOutputQueue1, my_output1);
    }

    my_input2 = read_from_queue(myInputQueue2);
    if (my_input2>=0) {
      printf("S2 : %d\n",my_input2);
      if (my_input2 > 0) {
        my_output2 = my_input2 + 1;
      } else {
        my_output2 = -1;
      }
      add_to_queue(myOutputQueue2, my_output2);
    }

    my_input3 = read_from_queue(myInputQueue3);
    if (my_input3>=0) {
      printf("S3 : %d\n",my_input3);
      if (my_input3 > 0)
        my_output3 = my_input3 * 2;
      else
        my_output3 = -1;
      add_to_queue(myOutputQueue3, my_output3);
    }
  }

  printf("S3 break\n");
  a0[2] = NULL;
}

int main(int argc, char *argv[]) {
  /* thread ids and attributes */

  void* workerid[NRSTAGES];
  // pthread_attr_t attr;
  long i;
  FILE *results;

  /* set global thread attributes */
  // pthread_attr_init(&attr);
  // pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

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
  
    
  
  /* create the workers, then wait for them to finish */
  // workerid[0] = Stage1((void *)&stage_queues[0]);
  // workerid[1] = Stage2((void *)&stage_queues[1]);
  // workerid[2] = Stage3((void *)&stage_queues[2]);
  Pipe(workerid, (void *)&stage_queues[0], (void *)&stage_queues[1], (void *)&stage_queues[2]);
  
  // for (i = 0; i < NRSTAGES; i++)
  //   pthread_join(workerid[i], NULL);

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


