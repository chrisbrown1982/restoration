#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define MAX_BOARDSIZE 21
typedef uint64_t SOLUTIONTYPE;


#define MIN_BOARDSIZE 2

SOLUTIONTYPE g_numsolutions = 0; 

int boardsize;
int dep;

/* Structure to keep info about one element (task) in pipeline queues */
typedef struct {
    int last;
    int numrow;
    int aQueenBitCol;
    int aQueenBitNegDiag;
    int aQueenBitPosDiag;
    SOLUTIONTYPE num_solutions;
} task_t;

static unsigned long streamlen=0;

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

struct {
    int *aQueenBitCol; /* marks colummns which already have queens */
    int *aQueenBitPosDiag; /* marks "positive diagonals" which already have queens */
    int *aQueenBitNegDiag; /* marks "negative diagonals" which already have queens */
    int *aStack; /* we use a stack instead of recursion */
    int *pnStack;
    int i;

    int numrows; /* numrows redundant - could use stack */
    unsigned int lsb; /* least significant bit */
    unsigned int bitfield; /* bits which are set mark possible positions for a queen */
}  stage1_state;

void initialise_stage1_state()
{
    stage1_state.aQueenBitCol = new int[MAX_BOARDSIZE];
    stage1_state.aQueenBitPosDiag = new int[MAX_BOARDSIZE];
    stage1_state.aQueenBitNegDiag = new int[MAX_BOARDSIZE]; 
    stage1_state.aStack = new int[MAX_BOARDSIZE+2];
    stage1_state.pnStack = stage1_state.aStack;
    stage1_state.aStack[0] = -1;
    stage1_state.i = -1;
    stage1_state.numrows = 0;
    stage1_state.bitfield = 0;
}

task_t workerStage1(int board_size, int depth)
{
    /* restoring state of the worker */
    int *aQueenBitCol = stage1_state.aQueenBitCol; /* marks colummns which already have queens */
    int *aQueenBitPosDiag = stage1_state.aQueenBitPosDiag; /* marks "positive diagonals" which already have queens */
    int *aQueenBitNegDiag = stage1_state.aQueenBitNegDiag; /* marks "negative diagonals" which already have queens */
    int *aStack = stage1_state.aStack; /* we use a stack instead of recursion */
    register int* pnStack = stage1_state.pnStack;
        
    register int numrows = stage1_state.numrows; /* numrows redundant - could use stack */
    register unsigned int lsb = stage1_state.lsb; /* least significant bit */
    register unsigned int bitfield = stage1_state.bitfield; /* bits which are set mark possible positions for a queen */
    int i = stage1_state.i;
    int odd = board_size & 1; /* 0 if board_size even, 1 if odd */
    int board_minus = board_size - 1; /* board size - 1 */
    int mask = (1 << board_size) - 1; /* if board size is N, mask consists of N 1's */

    /* This part goes into initialisation */
    //aStack[0] = -1;
    //i=-1;


    while (i<(1+odd)) {
        if (pnStack==aStack) {
            i++;
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
        }    
        
        /* could use 
            lsb = bitfield ^ (bitfield & (bitfield -1)); 
            to get first (least sig) "1" bit, but that's slower. */
        lsb = -((signed)bitfield) & bitfield; /* this assumes a 2's complement architecture */
        if (0 == bitfield) {
            bitfield = *--pnStack; /* get prev. bitfield from stack */
            if (pnStack != aStack)  /* if sentinel hit.... */
                --numrows;
            else
                i++;
            continue;
        }
        bitfield &= ~lsb; /* toggle off this bit so we don't try it again */
                   
        assert(numrows<board_minus);
                
        if (numrows == (depth-1)) {
            task_t task;
            task.numrow           = numrows;
            task.last             = 0;
            task.aQueenBitCol     = aQueenBitCol[numrows];
            task.aQueenBitNegDiag = aQueenBitNegDiag[numrows];
            task.aQueenBitPosDiag = aQueenBitPosDiag[numrows];
            task.num_solutions    = 0;
                    
            //++streamlen;
            // ff_send_out(task); // FIX retry fallback?????
            //ntasks++;
                    
            bitfield = *--pnStack;

            /* save state */
            if (pnStack != aStack)
                --numrows;
            else 
                i++;
            
            /* save state */
            stage1_state.pnStack = pnStack;
            stage1_state.numrows = numrows;
            stage1_state.lsb = lsb;
            stage1_state.bitfield = bitfield;
            stage1_state.i = i;

            return task; 
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

    task_t final_task;
    final_task.last = 1;

    return final_task;

}

int main(int argc, char** argv) {
    int check=0;

    if (argc == 1 || argc > 3) {
        /* user must pass in size of board */
        printf("Usage: nq_ff <width of board> [<depth>]\n"); 
        return 0;
    }

    boardsize = atoi(argv[1]);
    if (argv[2]) {
        dep=atoi(argv[2]);
        if (dep>=boardsize) dep=boardsize>>2;
        if (dep<2) dep=2;
    }
    
    /* check size of board is within correct range */
    if (MIN_BOARDSIZE > boardsize || MAX_BOARDSIZE < boardsize) {
        printf("Width of board must be between %d and %d, inclusive.\n", 
               MIN_BOARDSIZE, MAX_BOARDSIZE );
        return 0;
    }
    
    task_t in, out;

    initialise_stage1_state();

 
    do {
        in = workerStage1(boardsize, dep);
        if (!in.last) { 
            out = workerStage2(in);
            g_numsolutions += out.num_solutions;
        }
    } while (in.last != 1);


    printf ("Number of solutions is %llu\n", g_numsolutions);
    return 0;
    
}
    
    
    
    

