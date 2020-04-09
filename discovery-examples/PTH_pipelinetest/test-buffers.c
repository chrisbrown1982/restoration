#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

/*
  Simulates a pipeline with four stages (guesser, regex filter, prefix-suffix
  adder, and tester) where the data is stored and passed around in a buffer
  (modeled as a queue). This is a refinement of the pgpry model from
  test-orig.cpp, where the flow of data through the pipeline is captured
  explicitly. Explicit data-flow is essential in our trace-based pattern
  discovery approach. The rate of each stage can be controlled via the
  SLEEP_FACTOR_x macros.
*/

/* Number of threads allocated to each stage. */
#define NUM_THREADS_GUESSER 1
#define NUM_THREADS_REGEXP_FILTER 2
#define NUM_THREADS_PREFIX_SUFFIX_ADDER 2
#define NUM_THREADS_TESTER 5
#define NUM_TOTAL_THREADS() \
  (NUM_THREADS_GUESSER + \
   NUM_THREADS_REGEXP_FILTER + \
   NUM_THREADS_PREFIX_SUFFIX_ADDER + \
   NUM_THREADS_TESTER)
/* Maximum size of the intermediate buffers. */
#define BUFFER_MAX_SIZE 10
/* Number of items sent through the pipeline. */
#define DATA_SIZE 10
/* Time (ms) a stage waits before checking the buffers again. */
#define WAIT_MILISLEEP 100
/* Time (ms) taken by a firing of the guesser. */
#define GUESSER_MILISLEEP 100
/* Number of times each stage is slower than the guesser. */
#define SLEEP_FACTOR_REGEXP_FILTER 2
#define SLEEP_FACTOR_PREFIX_SUFFIX_ADDER 2
#define SLEEP_FACTOR_TESTER 4
/* Whether to print debug information. */
#define DEBUG 0
/* Whether the guesser stage is stateful. */
#define STATE 1
/* Whether the regex filter stage filters out elements. */
#define FILTER 1
/* Whether the prefix-suffix adder stage duplicates elements. */
#define DUPLICATE 1
/* Whether the tester stage terminates when a certain element is found. */
#define TERMINATE 1
/* Value on which the tester stage terminates. */
#define TERMINATE_IN_VALUE 5

/*
  Whether each stage is done. A stage is done if its input queue is empty and
  all stages with smaller ids are done.
*/
bool done[NUM_TOTAL_THREADS()];

/*
  State of guesser stage. Only used if STATE is set to 1.
 */
int current_guess = 0;

/*
  Simple queue implementation, adapted from
  https://www.quora.com/How-do-you-make-a-simple-queue-program-in-C.
*/

typedef struct Node {
  int data;
  struct Node *next;
} Node;

typedef struct Queue {
  Node *front;
  Node *last;
  unsigned int size;
} Queue;

void init(Queue *q) {
  q->front = NULL;
  q->last = NULL;
  q->size = 0;
}

int pop(Queue *q) {
  q->size--;
  Node *tmp = q->front;
  int data = tmp->data;
  q->front = q->front->next;
  free(tmp);
  return data;
}

void push(Queue *q, int data) {
  q->size++;
  Node *tmp = (Node *) malloc(sizeof(Node));
  tmp->data = data;
  tmp->next = NULL;
  if (q->front == NULL) {
    q->front = tmp;
    q->last = tmp;
  } else {
    q->last->next = tmp;
    q->last = tmp;
  }
}

typedef struct StageArgs {
  int id;
  Queue * input;
  Queue * output;
} StageArgs;

pthread_mutex_t monk;

void milisleep(long t) {
  nanosleep((const struct timespec[]){{0, t * 1000000L}}, NULL);
}

bool AllDone(int l, int u) {
  pthread_mutex_lock(&monk);
  bool all_done = true;
  for (int i = l; i <= u; i++) {
    if (!done[i]) {
      all_done = false;
      break;
    }
  }
  pthread_mutex_unlock(&monk);
  return all_done;
}

bool BufferIsEmpty(Queue *q) {
  pthread_mutex_lock(&monk);
  bool state = q->size == 0;
  pthread_mutex_unlock(&monk);
  return state;
}

bool BufferIsFull(Queue *q) {
  pthread_mutex_lock(&monk);
  bool state = q->size == BUFFER_MAX_SIZE;
  pthread_mutex_unlock(&monk);
  return state;
}

void *guesser(void *threadarg) {
  StageArgs * args = (StageArgs*)threadarg;
  int in;
  int out;
  while (true) {
    if (BufferIsEmpty(args->input)) {
      pthread_mutex_lock(&monk);
      done[args->id] = true;
      if (DEBUG) printf("guesser[%d]: E\n", args->id);
      pthread_mutex_unlock(&monk);
      pthread_exit(NULL);
    }
    if (!BufferIsFull(args->output)) {
      pthread_mutex_lock(&monk);
      in = pop(args->input);
      if (STATE) {
        out = current_guess;
        current_guess++;
      } else {
        out = in + 1;
      }
      pthread_mutex_unlock(&monk);
      if (DEBUG) printf("guesser[%d]: %d\n", args->id, out);
      milisleep(GUESSER_MILISLEEP);
      pthread_mutex_lock(&monk);
      push(args->output, out);
      pthread_mutex_unlock(&monk);
    }
    milisleep(WAIT_MILISLEEP);
  }
  pthread_exit(NULL);
}

void *regexp_filter(void *threadarg) {
  StageArgs * args = (StageArgs*)threadarg;
  int in;
  int out;
  while (true) {
    if (BufferIsEmpty(args->input) && AllDone(0, args->id - 1)) {
      pthread_mutex_lock(&monk);
      done[args->id] = true;
      if (DEBUG) printf("               regexp_filter[%d]: E\n", args->id);
      pthread_mutex_unlock(&monk);
      pthread_exit(NULL);
    }
    if (!BufferIsEmpty(args->input) && !BufferIsFull(args->output)) {
      pthread_mutex_lock(&monk);
      in = pop(args->input);
      pthread_mutex_unlock(&monk);
      out = FILTER ? in : in + 1;
      milisleep(GUESSER_MILISLEEP * SLEEP_FACTOR_REGEXP_FILTER);
      // Simulate filtering by dropping multiples of 3.
      if (!FILTER || (out % 3 != 0)) {
        pthread_mutex_lock(&monk);
        if (DEBUG) printf("               regexp_filter[%d]: %d\n", args->id, out);
        push(args->output, out);
        pthread_mutex_unlock(&monk);
      }
    }
    milisleep(WAIT_MILISLEEP);
  }
  pthread_exit(NULL);
}

void *prefix_suffix_adder(void *threadarg) {
  StageArgs * args = (StageArgs*)threadarg;
  int in;
  int out1, out2;
  while (true) {
    if (BufferIsEmpty(args->input) && AllDone(0, args->id - 1)) {
      pthread_mutex_lock(&monk);
      done[args->id] = true;
      if (DEBUG) printf("                                    prefix_suffix_adder[%d]: E\n", args->id);
      pthread_mutex_unlock(&monk);
      pthread_exit(NULL);
    }
    if (!BufferIsEmpty(args->input) && !BufferIsFull(args->output)) {
      pthread_mutex_lock(&monk);
      in = pop(args->input);
      pthread_mutex_unlock(&monk);
      // Simulate addition of prefixes and suffixes by pushing two variants.
      out1 = in + 1;
      if (DUPLICATE) out2 = in - 1;
      if (DEBUG) {
        if (DUPLICATE) printf("                                    prefix_suffix_adder[%d]: %d %d\n", args->id, out1, out2);
        else           printf("                                    prefix_suffix_adder[%d]: %d\n",    args->id, out1);
      }
      milisleep(GUESSER_MILISLEEP * SLEEP_FACTOR_PREFIX_SUFFIX_ADDER);
      pthread_mutex_lock(&monk);
      push(args->output, out1);
      if (DUPLICATE) push(args->output, out2);
      pthread_mutex_unlock(&monk);
    }
    milisleep(WAIT_MILISLEEP);
  }
  pthread_exit(NULL);
}

void *tester(void *threadarg) {
  StageArgs * args = (StageArgs*)threadarg;
  int buff;
  int in;
  int out;
  while (true) {
    if (BufferIsEmpty(args->input) && AllDone(0, args->id - 1)) {
      pthread_mutex_lock(&monk);
      done[args->id] = true;
      if (DEBUG) printf("                                                                 tester[%d]: E\n", args->id);
      pthread_mutex_unlock(&monk);
      pthread_exit(NULL);
    }
    /* The output buffer is unconstrained. */
    if (!BufferIsEmpty(args->input)) {
      pthread_mutex_lock(&monk);
      in = pop(args->input);
      pthread_mutex_unlock(&monk);
      if (TERMINATE && in == TERMINATE_IN_VALUE) {
        if (DEBUG) printf("                                                                 tester[%d]: T\n", args->id);
        printf("found value: %d\n", in);
        exit(EXIT_SUCCESS);
      }
      out = in + 1;
      milisleep(GUESSER_MILISLEEP * SLEEP_FACTOR_TESTER);
      pthread_mutex_lock(&monk);
      if (DEBUG) printf("                                                                 tester[%d]: %d\n", args->id, out);
      push(args->output, out);
      pthread_mutex_unlock(&monk);
    }
    milisleep(WAIT_MILISLEEP);
  }
  pthread_exit(NULL);
}

int main () {
  pthread_t threads[NUM_TOTAL_THREADS()];
  int rc;
  int i, n;
  int output;

  Queue in, mid1, mid2, mid3, out;
  init(&in);
  init(&mid1);
  init(&mid2);
  init(&mid3);
  init(&out);

  for (i = 0; i < DATA_SIZE; i++) {
    push(&in, i);
    printf("input: %d\n", i);
  }

  assert(in.size == DATA_SIZE);
  assert(mid1.size == 0);
  assert(mid2.size == 0);
  assert(mid3.size == 0);
  assert(out.size == 0);

  /* Create guessers. */
  for (i = 0; i < NUM_THREADS_GUESSER; i++) {
    StageArgs *args = (StageArgs*) malloc(sizeof(*args));
    args->id = i;
    args->input = &in;
    args->output = &mid1;
    if (DEBUG) printf("guesser[%d]: I\n", args->id);
    rc = pthread_create(&threads[i], NULL, guesser, args);
    assert(rc == 0);
  }

  /* Create regexp filters. */
  for (n = i + NUM_THREADS_REGEXP_FILTER; i < n; i++) {
    StageArgs *args = (StageArgs*) malloc(sizeof(*args));
    args->id = i;
    args->input = &mid1;
    args->output = &mid2;
    if (DEBUG) printf("               regexp_filter[%d]: I\n", args->id);
    rc = pthread_create(&threads[i], NULL, regexp_filter, args);
    assert(rc == 0);
  }

  /* Create prefix-suffix filters. */
  for (n = i + NUM_THREADS_PREFIX_SUFFIX_ADDER; i < n; i++) {
    StageArgs *args = (StageArgs*) malloc(sizeof(*args));
    args->id = i;
    args->input = &mid2;
    args->output = &mid3;
    if (DEBUG) printf("                                    prefix_suffix_adder[%d]: I\n", args->id);
    rc = pthread_create(&threads[i], NULL, prefix_suffix_adder, args);
    assert(rc == 0);
  }

  /* Create tester filters. */
  for (n = i + NUM_THREADS_TESTER; i < n; i++) {
    StageArgs *args = (StageArgs*) malloc(sizeof(*args));
    args->id = i;
    args->input = &mid3;
    args->output = &out;
    if (DEBUG) printf("                                                                 tester[%d]: I\n", args->id);
    rc = pthread_create(&threads[i], NULL, tester, args);
    assert(rc == 0);
  }

  for (i = 0; i < NUM_TOTAL_THREADS(); i++) {
    pthread_join(threads[i], NULL);
  }

  assert(in.size == 0);
  assert(mid1.size == 0);
  assert(mid2.size == 0);
  assert(mid3.size == 0);

  /*
    TODO: in the real application, how will the flow of wrong candidate
    passwords be reflected in the DDG? They might not yield any observable
    effect.
  */
  while(out.size > 0) {
    output = pop(&out);
    printf("output: %d\n", output);
  }
}
