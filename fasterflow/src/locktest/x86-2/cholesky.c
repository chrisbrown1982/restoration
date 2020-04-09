#include <vector>
#include <iostream>
#include <cmath>
#include <fcntl.h>
//#include <sys/mman.h>
//#include <ff/farm.hpp>
#include <cholconst.h>
#include <complex.h>
#include <common.h>

#define MAX_TASKS 100000


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
      if ( 0 == old_value)
        break;
      sem_wait(&tq->locksem);

   } while ( 1 ) ;
   // lock++;
}

void my_unlock_tq( struct tq *tq )
{
   // tq->lock = 0 ;
   __sync_lock_release(&tq->lock);

   sem_post(&tq->locksem);
   // unlock++;
}

task_t *gettask(struct tq *tq) {
  void *t;


  sem_wait(&tq->sem);
  my_lock_tq(tq);
  // fprintf(stderr, "gettask2 progressed\n"); 
  t = tq->ts[tq->removeat];
  //
  if (++tq->removeat == MAX_TASKS)
     tq->removeat = 0;
  //
  my_unlock_tq(tq);

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

  tq->ts[tq->addat++] = v;
  if (tq->addat == MAX_TASKS)
    tq->addat = 0;

    sem_post(&tq->sem);

  my_unlock_tq(tq);
}

int worker(task_t *task) {
		comp_t *a = task->a;
		comp_t *l = task->l;
		int i, k, j;	// indexes used in loops
		float sumSQR;	// support variable
		comp_t sum;		// support variable
		int idx;		// index of the current element
		
		for (j = 0; j < MATSIZE; j++) {
			sumSQR = 0.;
			
			for (k = 0; k < j; k++) {
				idx = j * MATSIZE + k;	// l[j][k] index
				//sum += l[j][k] * l[j][k];
				sumSQR += ((l[idx].real) * (l[idx].real) +
				           (l[idx].imag) * (l[idx].imag));
			}
			
			idx = j * MATSIZE + j;	// a[j][j] and l[j][j] index
			//sum = a[j][j] - sum;	// L[j][j]^2
			sumSQR = a[idx].real - sumSQR;
			//l[j][j] = sqrt(sum);
			l[idx].real = sqrt(sumSQR);
			
			for (i = j + 1; i < MATSIZE; i++) {
				sum.real = 0.;
				sum.imag = 0.;
				
				for (k = 0; k < j; k++) {
					//sum += l[i][k] * l[j][k];
					comp_t tmp;
					comp_t trasp;
					idx = j * MATSIZE + k;	// l[j][k] index
					trasp.real = l[idx].real;
					trasp.imag = -(l[idx].imag);
					COMP_MUL(l[i * MATSIZE + k], trasp, tmp);
					COMP_SUM(sum, tmp, sum);
				}
				
				//l[i][j] = (a[i][j] - sum) / l[j][j];
				comp_t app1;
				idx = i * MATSIZE + j;	// a[i][j] and l[i][j] index
				COMP_SUB(a[idx], sum, app1);
				COMP_DIV(app1, l[j * MATSIZE + j], l[idx]);
			}
		}
		
		return 42;
}


void stage1()
{
    task_t* t;
    int r;
    while(1)
    {
         t = gettask(tq);
        // printf("getting a task, computing\n");
         r = worker(t);
        // printf("putting a task...\n");
         puttask(tq2, r);
    }

}

int main(int argc, 
         char * argv[]) {

    if (argc < 4) {
        std::cerr << "usage: " 
                  << argv[0] 
                  << " nworkers streamlen inputfile\n";
        return -1;
    }
    
    int nworkers = atoi(argv[1]);
    int streamlen = atoi(argv[2]);
	char *infile = argv[3];

    if (nworkers <= 0 || streamlen <= 0) {
        std::cerr << "Wrong parameters values\n";
        return -1;
    }
	
	FILE *fp = fopen(infile, "r");
	if (!fp)
	{
		std::cerr << "Error opening input file\n";
		return -1;
	}
	
	// Array of tasks (each task is made up of an A matrix and a L matrix)
	task_t *tasks = new task_t[streamlen];
	
	// Variables needed to use HUGE_PAGES
	int huge_size;
	//char mem_file_name[32];
	//int fmem;
	char *mem_block;
	
	// HUGE_PAGES initialization
	huge_size = streamlen * MATSIZE * MATSIZE * sizeof(comp_t) * 2;
	/*huge_size = (huge_size + HUGE_PAGE_SIZE-1) & ~(HUGE_PAGE_SIZE-1);
	
	sprintf(mem_file_name, "/huge/huge_page_cbe.bin");
	assert((fmem = open(mem_file_name, O_CREAT | O_RDWR, 0755)) != -1);
	remove(mem_file_name);

	assert((mem_block = (char*) mmap(0, huge_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fmem, 0)) != MAP_FAILED);
	*/
	
	// Memory allocation without HUGE_PAGES
	mem_block = (char *) malloc(huge_size);
	
	tasks[0].a = (comp_t *) mem_block;
	tasks[0].l = (comp_t *) (mem_block + (MATSIZE * MATSIZE * sizeof(comp_t)));
	
	// Reads matrix A from input file
	int n;
	fscanf(fp, "%d", &n);
	assert(n == MATSIZE);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			fscanf(fp, "%f", &tasks[0].a[(i * MATSIZE) + j].real);
			fscanf(fp, "%f", &tasks[0].a[(i * MATSIZE) + j].imag);
		}
	
	fclose(fp);
	
	// Initialization of matrix L
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			tasks[0].l[(i * MATSIZE) + j].real = 0.;
			tasks[0].l[(i * MATSIZE) + j].imag = 0.;
		}
	}
	
	// Replication of matrices A and L to generate a stream of tasks
	for (int i = 1; i < streamlen; i++) {
		tasks[i].a = (comp_t *)
			(mem_block + ((i * 2) * MATSIZE * MATSIZE * sizeof(comp_t)));
		tasks[i].l = (comp_t *)
			(mem_block + ((i * 2 + 1) * MATSIZE * MATSIZE * sizeof(comp_t)));
		memcpy((comp_t *) tasks[i].a, (const comp_t *) tasks[0].a,
		       (size_t) MATSIZE * MATSIZE * sizeof(comp_t));
		memcpy((comp_t *) tasks[i].l, (const comp_t *) tasks[0].l,
			   (size_t) MATSIZE * MATSIZE * sizeof(comp_t));
	}


   // farm bit
   tq = newtq();
   tq2 = newtq();
   
   createfarm(stage1, nworkers);
   
   int i;
   
   //  int i;
   for (i=0;i<streamlen;i++) {
     //      fprintf(stderr, "Getting Task in main %d\n", i);
           puttask(tq, task[i]);
       //    fprintf(stderr, "Got a task in main %d\n", i);
    }
   
   for (i=0;i<streamlen;i++) {
     //      fprintf(stderr, "Getting Task in main %d\n", i);
           gettask(tq2);
       //    fprintf(stderr, "Got a task in main %d\n", i);
    }
   
    std::cout << "DONE, completion time = " << cpu_end_time-cpu_start_time << " (ms),"
	          << " service time = " << ((cpu_end_time-cpu_start_time) / streamlen) << " (ms)\n";
	
    
	// Prints the result matrix l[0] in a file
	const char *factorizationFileName = "./choleskyIntelStandard.txt";
	
	fp = fopen(factorizationFileName, "w+");
	if (fp == NULL) {
		perror("Error opening output file");
		return -1;
	}
	
	fprintf(fp, "Standard algorithm farm version on Intel: L result matrix \n");
	
	for (int i = 0; i < MATSIZE; i++) {
		int j;
		fprintf(fp, "[ ");
		for (j = 0; j <= i ; j++) {
			fprintf(fp, "% 6.3f ", tasks[0].l[i * MATSIZE + j].real);
			fprintf(fp, "% 6.3fi ", tasks[0].l[i * MATSIZE + j].imag);
		}
		for ( ; j < MATSIZE; j++) {
			fprintf(fp, "% 6.3f ", 0.);
			fprintf(fp, "% 6.3fi ", 0.);
		}
		
		fprintf(fp, " ]\n");
	}
	
	fprintf(fp, "\n");
	
	fclose(fp);
	
	delete [] tasks;
	free(mem_block);	// without HUGE_PAGES
	
    return 0;
}

    
    
