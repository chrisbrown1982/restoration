#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define MAX_BOARDSIZE 21
#define MAX_TASKS 10000
#define BUFSIZE 10000
#define NRSTAGES 2
typedef uint64_t SOLUTIONTYPE;


#define MIN_BOARDSIZE 2

SOLUTIONTYPE g_numsolutions = 0; 

int boardsize;
int dep;
int ntasks=0;

/* Structure to keep info about one element (task) in pipeline queues */
typedef struct {
    int last;
    int numrow;
    int aQueenBitCol;
    int aQueenBitNegDiag;
    int aQueenBitPosDiag;
    SOLUTIONTYPE num_solutions;
} task_t;

/* Structure to keep info about queues between stages */
typedef struct {
  task_t *elements;
  int nr_elements; /* how many elements are currently in the queue */
  int capacity;
  int readFrom; /* position in the array from which to read from */
  int addTo; /* position in the array of the first free slot for adding data */
  int termination; /* this is set to 1 when threads waiting to read from this queue need to terminate */
  pthread_mutex_t queue_lock;
  pthread_cond_t queue_cond_write; /* signalled when we write something into the queue, wakes up threads waiting to read from an empty queue */
  pthread_cond_t queue_cond_read; /* signalled when we read something from the queue, wakes up threads waiting to write to a full queue */
} queue_t;

/* Structure to keep information about input and output queue of a pipeline stage */
typedef struct {
    queue_t *inputQueue;
    queue_t *outputQueue;
} pipeline_stage_queues_t;

static unsigned long streamlen=0;

void InitialiseQueue(queue_t *queue, int capacity) {
  queue->elements = (task_t*) malloc (sizeof(task_t) * capacity);
  queue->readFrom = 0;
  queue->addTo = 0;
  queue->nr_elements = 0;
  queue->capacity = capacity;
  queue->termination = 0;
  pthread_mutex_init(&queue->queue_lock, NULL);
  pthread_cond_init(&queue->queue_cond_write, NULL);
  pthread_cond_init(&queue->queue_cond_read, NULL);
}

void add_to_queue(queue_t *queue, task_t elem)
{
  pthread_mutex_lock(&queue->queue_lock);
  /* If the queue is full, wait until something reads from it before adding a new element */
  if (queue->nr_elements == queue->capacity) {
    pthread_cond_wait(&queue->queue_cond_read,&queue->queue_lock);
  }
  queue->elements[queue->addTo] = elem;
  queue->addTo = (queue->addTo + 1) % queue->capacity;
  queue->nr_elements++;
  pthread_cond_signal(&queue->queue_cond_write);
  pthread_mutex_unlock(&queue->queue_lock);
}

task_t read_from_queue(queue_t *queue)
{
    task_t elem;
    pthread_mutex_lock(&queue->queue_lock);
    if (queue->nr_elements == 0 && !queue->termination) {
        pthread_cond_wait(&queue->queue_cond_write,&queue->queue_lock);
    }
    if (queue->termination) {
        elem.last = 1;
    } else {
        elem = queue->elements[queue->readFrom];
        queue->nr_elements--;
        queue->readFrom = (queue->readFrom + 1) % queue->capacity;
    }   
    pthread_cond_signal(&queue->queue_cond_read);
    pthread_mutex_unlock(&queue->queue_lock);
    return elem;
}

void signal_termination(queue_t *queue) 
{
    pthread_mutex_lock(&queue->queue_lock);
    queue->termination = 1;
    pthread_cond_broadcast(&queue->queue_cond_write);
    pthread_mutex_unlock(&queue->queue_lock);
}

task_t workerStage2(task_t task)
{
    SOLUTIONTYPE workersolutions=0;

    // int board_size;    
    int aQueenBitCol[MAX_BOARDSIZE]; /* marks colummns which already have queens */
    int aQueenBitPosDiag[MAX_BOARDSIZE]; /* marks "positive diagonals" which already have queens */
    int aQueenBitNegDiag[MAX_BOARDSIZE]; /* marks "negative diagonals" which already have queens */
    int aStack[MAX_BOARDSIZE + 2]; /* we use a stack instead of recursion */
    int board_minus = boardsize-1;
    int mask= (1 << boardsize) -1;
    
    
    SOLUTIONTYPE numsolutions=0;

    register int* pnStack= aStack + 1; /* stack pointer */
    register int numrows; /* numrows redundant - could use stack */
    register unsigned int lsb; /* least significant bit */
    register unsigned int bitfield; /* bits which are set mark possible positions for a queen */

                        
    /* Initialize stack */
    aStack[0] = -1; /* set sentinel -- signifies end of stack */
            
    numrows                   = task.numrow;
    aQueenBitCol[numrows]     = task.aQueenBitCol;
    aQueenBitNegDiag[numrows] = task.aQueenBitNegDiag;
    aQueenBitPosDiag[numrows] = task.aQueenBitPosDiag;
        
    bitfield = mask & ~(aQueenBitCol[numrows] | aQueenBitNegDiag[numrows] | aQueenBitPosDiag[numrows]);
        
    /* this is the critical loop */
    for (;;) {
            /* could use 
               lsb = bitfield ^ (bitfield & (bitfield -1)); 
               to get first (least sig) "1" bit, but that's slower. */
            lsb = -((signed)bitfield) & bitfield; /* this assumes a 2's complement architecture */
            if (0 == bitfield)  {
                bitfield = *--pnStack; /* get prev. bitfield from stack */
                if (pnStack == aStack) { /* if sentinel hit.... */
                    break ;
                }
                --numrows;
                continue;
            }
            bitfield &= ~lsb; /* toggle off this bit so we don't try it again */
            
            if (numrows < board_minus) { /* we still have more rows to process? */
                int n = numrows++;
                aQueenBitCol[numrows] = aQueenBitCol[n] | lsb;
                aQueenBitNegDiag[numrows] = (aQueenBitNegDiag[n] | lsb) >> 1;
                aQueenBitPosDiag[numrows] = (aQueenBitPosDiag[n] | lsb) << 1;
                
                *pnStack++ = bitfield;
                
                /* We can't consider positions for the queen which are in the same
                   column, same positive diagonal, or same negative diagonal as another
                   queen already on the board. */
                bitfield = mask & ~(aQueenBitCol[numrows] | aQueenBitNegDiag[numrows] | aQueenBitPosDiag[numrows]);
                continue;
            } else  {
                /* We have no more rows to process; we found a solution. */
                /* Comment out the call to printtable in order to print the solutions as board position*/
                ++numsolutions;
                bitfield = *--pnStack;
                --numrows;
                continue;
            }
    }
        
    /* multiply solutions by two, to count mirror images */
    workersolutions += (numsolutions << 1);
    task.num_solutions = workersolutions;
    return task;
}

void* stage2(void *arg)
{
    pipeline_stage_queues_t *myQueues = (pipeline_stage_queues_t *)arg;
    queue_t *myOutputQueue = myQueues->outputQueue;
    queue_t *myInputQueue = myQueues->inputQueue;
    task_t t, r;
    do
    {
        t = read_from_queue(myInputQueue);
        if (!t.last) {
            r = workerStage2(t);
            add_to_queue(myOutputQueue, r);
        } else {
            signal_termination(myInputQueue);
        }
    } while (!t.last);
    return NULL;

}

void workerStage1(int board_size, int depth, queue_t *myOutputQueue)
{
    int aQueenBitCol[MAX_BOARDSIZE]; /* marks colummns which already have queens */
    int aQueenBitPosDiag[MAX_BOARDSIZE]; /* marks "positive diagonals" which already have queens */
    int aQueenBitNegDiag[MAX_BOARDSIZE]; /* marks "negative diagonals" which already have queens */
    int aStack[MAX_BOARDSIZE + 2]; /* we use a stack instead of recursion */
    register int* pnStack;
        
    register int numrows = 0; /* numrows redundant - could use stack */
    register unsigned int lsb; /* least significant bit */
    register unsigned int bitfield; /* bits which are set mark possible positions for a queen */
    int i;
    int odd = board_size & 1; /* 0 if board_size even, 1 if odd */
    int board_minus = board_size - 1; /* board size - 1 */
    int mask = (1 << board_size) - 1; /* if board size is N, mask consists of N 1's */
        
        
    /* Initialize stack */
    aStack[0] = -1; /* set sentinel -- signifies end of stack */
        
    /* NOTE: (board_size & 1) is true iff board_size is odd */
    /* We need to loop through 2x if board_size is odd */
    for (i = 0; i < (1 + odd); ++i) {
        /* We don't have to optimize this part; it ain't the 
            critical loop */
        bitfield = 0;
        if (0 == i) {
            /* Handle half of the board, except the middle
                column. So if the board is 5 x 5, the first
                row will be: 00011, since we're not worrying
                about placing a queen in the center column (yet).
            */
            int half = board_size>>1; /* divide by two */
            /* fill in rightmost 1's in bitfield for half of board_size
                If board_size is 7, half of that is 3 (we're discarding the remainder)                  
                and bitfield will be set to 111 in binary. */
            bitfield = (1 << half) - 1;
            pnStack = aStack + 1; /* stack pointer */
                
            aQueenBitCol[0] = aQueenBitPosDiag[0] = aQueenBitNegDiag[0] = 0;
        } else {
            /* Handle the middle column (of a odd-sized board).
                Set middle column bit to 1, then set
                half of next row.
                So we're processing first row (one element) & half of next.
                So if the board is 5 x 5, the first row will be: 00100, and
                the next row will be 00011.
            */
            bitfield = 1 << (board_size >> 1);
            numrows = 1; /* prob. already 0 */
                
            /* The first row just has one queen (in the middle column).*/
            //aQueenBitRes[0] = bitfield;
            aQueenBitCol[0] = aQueenBitPosDiag[0] = aQueenBitNegDiag[0] = 0;
            aQueenBitCol[1] = bitfield;
                
            /* Now do the next row.  Only set bits in half of it, because we'll
                flip the results over the "Y-axis".  */
            aQueenBitNegDiag[1] = (bitfield >> 1);
            aQueenBitPosDiag[1] = (bitfield << 1);
            pnStack = aStack + 1; /* stack pointer */
            *pnStack++ = 0; /* we're done w/ this row -- only 1 element & we've done it */
            bitfield = (bitfield - 1) >> 1; /* bitfield -1 is all 1's to the left of the single 1 */
            depth++; // for a odd-sized board we have to go a step forward
        }
            
        /* this is the critical loop */
        for (;;) {
            /* could use 
                lsb = bitfield ^ (bitfield & (bitfield -1)); 
                to get first (least sig) "1" bit, but that's slower. */
            lsb = -((signed)bitfield) & bitfield; /* this assumes a 2's complement architecture */
            if (0 == bitfield) {
                bitfield = *--pnStack; /* get prev. bitfield from stack */
                if (pnStack == aStack) { /* if sentinel hit.... */
                    task_t task;
                    task.last = 1;
                    add_to_queue(myOutputQueue, task);
                    break ;
                }
                --numrows;
                continue;
            }
            bitfield &= ~lsb; /* toggle off this bit so we don't try it again */
                   
            assert(numrows<board_minus);
                
            if (numrows == (depth-1)) {
                task_t task;
                task.last             = 0;
                task.numrow           = numrows;
                task.aQueenBitCol     = aQueenBitCol[numrows];
                task.aQueenBitNegDiag = aQueenBitNegDiag[numrows];
                task.aQueenBitPosDiag = aQueenBitPosDiag[numrows];
                task.num_solutions    = 0;
                    
                ++streamlen;
                // ff_send_out(task); // FIX retry fallback?????
                add_to_queue(myOutputQueue, task);
                ntasks++;
                    
                bitfield = *--pnStack;
                if (pnStack == aStack) break;  /* if sentinel hit.... */
                --numrows;
                continue;
            }

            int n = numrows++;
            aQueenBitCol[numrows] = aQueenBitCol[n] | lsb;
            aQueenBitNegDiag[numrows] = (aQueenBitNegDiag[n] | lsb) >> 1;
            aQueenBitPosDiag[numrows] = (aQueenBitPosDiag[n] | lsb) << 1;
                
                
            *pnStack++ = bitfield;
                
            /* We can't consider positions for the queen which are in the same
                column, same positive diagonal, or same negative diagonal as another
                queen already on the board. */
            bitfield = mask & ~(aQueenBitCol[numrows] | aQueenBitNegDiag[numrows] | aQueenBitPosDiag[numrows]);
        }
    }
    

}

void* stage1(void *arg)
{
    int board_size=boardsize;
    int depth = dep;
    pipeline_stage_queues_t *myQueues = (pipeline_stage_queues_t *)arg;
    queue_t *myOutputQueue = myQueues->outputQueue;
    workerStage1(board_size, depth, myOutputQueue);
    return NULL;
}


int main(int argc, char** argv) {
    int check=0;
    time_t t1, t2;
    int nworkers2=2;

    if (argc == 1 || argc > 4) {
        /* user must pass in size of board */
        printf("Usage: nq_ff <width of board> [<num_workers2> <depth>]\n"); 
        return 0;
    }

    if (argc==5) check=1;
    
    boardsize = atoi(argv[1]);
    if (argv[2]) 
        nworkers2=atoi(argv[2]);
    if (argv[3]) {
        dep=atoi(argv[3]);
        if (dep>=boardsize) dep=boardsize>>2;
        if (dep<2) dep=2;
    }
    
    /* check size of board is within correct range */
    if (MIN_BOARDSIZE > boardsize || MAX_BOARDSIZE < boardsize) {
        printf("Width of board must be between %d and %d, inclusive.\n", 
               MIN_BOARDSIZE, MAX_BOARDSIZE );
        return 0;
    }
    
  pthread_t worker_stage1;
  pthread_t workers_stage2[nworkers2];
  pthread_attr_t attr;
  long i;
  FILE *results;

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize queues */
  queue_t queue[NRSTAGES];
  for (i=0; i<NRSTAGES; i++) {
    int capacity;
    capacity = BUFSIZE;
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

    pthread_create(&worker_stage1, &attr, stage1, (void *)&stage_queues[0]);
    for (i=0; i<nworkers2; i++)
        pthread_create(&workers_stage2[i], &attr, stage2, (void *)&stage_queues[1]);

    pthread_join(worker_stage1,NULL); 
    for (i=0; i<nworkers2; i++) {
        pthread_join(workers_stage2[i], NULL);
    }

    queue_t finalOutputQueue = queue[NRSTAGES-1];

    assert (finalOutputQueue.nr_elements = ntasks);
    for (i=finalOutputQueue.readFrom; i<finalOutputQueue.addTo; i++) {
        g_numsolutions += finalOutputQueue.elements[i].num_solutions;
    }

    printf ("Number of solutions is %llu\n", g_numsolutions);
    return 0;
    
}
    
    
    
    

