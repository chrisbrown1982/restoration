#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define MAX_BOARDSIZE 21
#define MAX_TASKS 10000
typedef uint64_t SOLUTIONTYPE;


#define MIN_BOARDSIZE 2

SOLUTIONTYPE g_numsolutions = 0; 

int boardsize;
int dep;
int ntasks=0;

typedef struct {
    int numrow;
    int aQueenBitCol;
    int aQueenBitNegDiag;
    int aQueenBitPosDiag;
} task_t;

static unsigned long streamlen=0;

struct tq {
  volatile char lock;
  pthread_cond_t lockcond;
  pthread_mutex_t lockmutex;

  // used for notification locks
  //  pthread_condattr_t notifycondattr;
  pthread_cond_t notifycond;
  pthread_mutex_t notifymutex;
  sem_t sem;
  sem_t locksem;
  int addat;
  int removeat;
  task_t* ts[MAX_TASKS+1];
};

struct tq* tq;
struct tq* tq2;

struct tq *newtq() {
  struct tq *tq = (struct tq *)malloc(sizeof(struct tq));
  tq->lock = 0; tq->addat = 0; tq->removeat = 0;
  //  pthread_condattr_init(&tq->notifycondattr);
  //  pthread_cond_init(&tq->notifycond,&tq->notifycondattr);
  pthread_cond_init(&tq->notifycond,NULL); // NULL?
  sem_init (&tq->sem, 0, -1);
  sem_init (&tq->locksem, 0, 0);
  pthread_mutex_init(&tq->notifymutex,NULL);
  return(tq);
}


void makeThread(void (*Worker)(),long t) {
  int rc;

  pthread_t *pt = (pthread_t *) malloc(sizeof(pthread_t));

  rc = pthread_create(pt, NULL, (void *(*)(void *))Worker, (void *)t);
  //  rc = pthread_create(pt, NULL, Worker, (void *)t);

  if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void createfarm(void (*Worker1)(), void (*Worker2)(), int n1, int n2) {
  int rc;
  long t;

  for(t=0;t<n1;t++)
    makeThread(Worker1,t);
    
  for(t=n1;t<n2;t++)
    makeThread(Worker2,t);  
}

void my_lock_tq( struct tq *tq )
{
   char old_value ;
   char new_value = 1 ;
   
   do {
      // __sync_val_compare_and_swap(&tq->lock,old_value,new_value);
      old_value = __sync_lock_test_and_set(&tq->lock,1);
      // use an xchg to obtain a lock
      /* __asm__ __volatile__( "lock; xchgb  %0,%1\n\t" :
                            "=r" (old_value), "+m" (tq->lock) :
                            "0" (new_value) : 
                            "cc" ) ;
      */
      // do we have the lock?  If so, exit the lock routine
      if ( 0 == old_value)
        break;

      // otherwise wait until signalled
   //    fprintf (stderr, "(%p,%d) waiting for condition\n", tq, pthread_self());
      //pthread_cond_wait(&tq->lockcond,&tq->lockmutex);
      sem_wait(&tq->locksem);

    //  fprintf (stderr, "(%p,%d) finished waiting for condition\n", tq, pthread_self());

      // don't assume we have the lock yet...

   } while ( 1 ) ;
   // lock++;
}

void my_unlock_tq( struct tq *tq )
{
   // tq->lock = 0 ;
   __sync_lock_release(&tq->lock);
   
   // this may be unnecessary in most cases and will only unlock one blocked thread
   // should not be called if there are no blocked threads -- hmmm, this gets complex!!
   //   pthread_cond_signal(&tq->lockcond);
   
   // this is safer, but may still be unnecessary
   // pthread_cond_broadcast(&tq->lockcond);
   // pthread_mutex_unlock(&tq->lockmutex);
   sem_post(&tq->locksem);
   // unlock++;
}


task_t *gettask(struct tq *tq) {
  void *t;
/*  while(1) {
 *      my_lock_tq(tq);
 *          if (tq->removeat == tq->addat) {
 *                my_unlock_tq(tq);
 *                      sem_wait(&tq->sem);
 *                          } else
 *                                break;
 *                                  }
 *                                    
 *                                      t = tq->ts[tq->removeat];
 *
 *                                        if (++tq->removeat == MAX_TASKS)
 *                                            tq->removeat = 0;
 *
 *                                              my_unlock_tq(tq);
 *                                              */

  sem_wait(&tq->sem);
  my_lock_tq(tq);
  // fprintf(stderr, "gettask2 progressed\n"); 
  t = tq->ts[tq->removeat];
  //
  if (++tq->removeat == MAX_TASKS)
     tq->removeat = 0;
  //
  my_unlock_tq(tq);
  //             // fprintf (stderr, "gettask2 lock released\n");
  //
  //               //pthread_mutex_unlock(&tq->notifymutex);
  //
  //                  // fprintf(stderr,"gettask2 got %d\n",t);
  //
  //                    //double cpu_end_time = get_current_time();
  //                      //gt1+=cpu_end_time - cpu_start_time;
  return(t);
}



void puttask(struct tq *tq, task_t* v) {
  int a, r;

  // fprintf(stderr,"putting2 %d\n",v);

  while(1) {
    my_lock_tq(tq);
    if (tq->addat == tq->removeat-1) 
      my_unlock_tq(tq);
    else
      break;
  }

  //do {
  //my_lock_tq(tq);
  //if (tq->addat == tq->removeat-1) {
  //  my_unlock_tq(tq); // exit(0);
  //}
  //} while (tq->addat == tq->removeat-1);

  // fprintf (stderr, "about to put task2\n");
  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

  // unlock BEFORE signalling...

  // signal if there are blocked tasks
  // fprintf (stderr, "(%d,%d)\n", tq->removeat, tq->addat);
  // if ((tq->removeat+1) % MAX_TASKS == tq->addat) {
    // fprintf(stderr,"unblocking...\n");
    //pthread_cond_signal(&tq->notifycond);
    sem_post(&tq->sem);
    // fprintf(stderr,"finished waiting\n");
  // }
 // fprintf(stderr,"put %d %d %d\n",v, tq->removeat+1, tq->addat);
  my_unlock_tq(tq);
}



/* Print the results at the end of the run */
void printResults(time_t* pt1, time_t* pt2)
{
    double secs;
    int hours , mins, intsecs;

    printf("End: \t%s", ctime(pt2));
    secs = difftime(*pt2, *pt1);
    intsecs = (int)secs;
    printf("Calculations took %d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    /* Print hours, minutes, seconds */
    hours = intsecs/3600;
    intsecs -= hours * 3600;
    mins = intsecs/60;
    intsecs -= mins * 60;
    if (hours > 0 || mins > 0) 
    {
        printf("Equals ");
        if (hours > 0) 
        {
            printf("%d hour%s, ", hours, (hours == 1) ? "" : "s");
        }
        if (mins > 0)
        {           
            printf("%d minute%s and ", mins, (mins == 1) ? "" : "s");
        }
        printf("%d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    }
}

int workerStage2(task_t * task)
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
        
    numrows                   = task->numrow;
    aQueenBitCol[numrows]     = task->aQueenBitCol;
    aQueenBitNegDiag[numrows] = task->aQueenBitNegDiag;
    aQueenBitPosDiag[numrows] = task->aQueenBitPosDiag;
        
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
    
    return task;
}

void stage2()
{
    task_t* t;
    task_t* r;
    while(1)
    {
         t = gettask(tq);
        // printf("getting a task, computing\n");
         r = workerStage2(t);
        // printf("putting a task...\n");
         puttask(tq2, t);
    }

}

void workerStage1(int board_size, int depth)
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
                        break ;
                    }
                    --numrows;
                    continue;
                }
                bitfield &= ~lsb; /* toggle off this bit so we don't try it again */
                
                assert(numrows<board_minus);
                
                if (numrows == (depth-1)) {
                    task_t * task = (task_t*)malloc(sizeof(task_t));
                    task->numrow           = numrows;
                    task->aQueenBitCol     = aQueenBitCol[numrows];
                    task->aQueenBitNegDiag = aQueenBitNegDiag[numrows];
                    task->aQueenBitPosDiag = aQueenBitPosDiag[numrows];
                    
                    ++streamlen;
                    // ff_send_out(task); // FIX retry fallback?????
                    fprintf(stderr, "sending out\n");
                    puttask(tq, task);
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

void stage1()
{
    int board_size=boardsize;
    int depth = dep;
         // t = gettask1(tq);
    //     printf("workerStage1\n");
         workerStage1(board_size, depth);
    //     printf("workerStage1 finished..\n");
         // puttask2(tq, r);
 
}


int main(int argc, char** argv) {
    int check=0;
    time_t t1, t2;
    int nworkers1=2;  // FIX
    int nworkers2=2;

    if (argc == 1 || argc > 6) {
        /* user must pass in size of board */
        printf("Usage: nq_ff <width of board> [<num-workers1> <num_workers2> <depth>]\n"); 
        return 0;
    }

    if (argc==5) check=1;
    
        boardsize = atoi(argv[1]);
    if (argv[2]) 
        nworkers1=atoi(argv[2]);
    if(argv[3])
        nworkers2=atoi(argv[3]);
    if (argv[4]) {
        dep=atoi(argv[4]);
        if (dep>=boardsize) dep=boardsize>>2;
        if (dep<2) dep=2;
    }
    
    /* check size of board is within correct range */
    if (MIN_BOARDSIZE > boardsize || MAX_BOARDSIZE < boardsize) {
        printf("Width of board must be between %d and %d, inclusive.\n", 
               MIN_BOARDSIZE, MAX_BOARDSIZE );
        return 0;
    }
    
    
    // pipeline bit here
    tq = newtq();
    tq2 = newtq();
    
     createfarm(stage1, stage2, nworkers1, nworkers2);
    
    int i; 
     for (i=0;i<ntasks;i++) {
     //      fprintf(stderr, "Getting Task in main %d\n", i);
           gettask(tq2);
       //    fprintf(stderr, "Got a task in main %d\n", i);
    }
    fprintf(stderr, "got %d tasks\n", ntasks);

    
    return 0;
    
}
    
    
    
    

