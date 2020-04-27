#include <vector>
#include <iostream>
#include <cmath>
#include <fcntl.h>
#include <pthread.h>
//#include <sys/mman.h>
//#include <ff/farm.hpp>
//#include <cholconst.h>
#include <complex.h>
//#include <common.h>
#include <tbb/tbb.h>

#define MAXTASKS 100000

int MATSIZE = 400;

using namespace tbb;

typedef std::complex<float> comp_t;

/* Structure for the task of Cholesky computation */
typedef struct {
    int last;
    comp_t *a;
    comp_t *l;
} task_t;

task_t seq_worker(task_t task) {
	comp_t *a = task.a;
    comp_t *l = task.l;
	int i, k, j;	// indexes used in loops
	float sumSQR;	// support variable
	comp_t sum;		// support variable
	int idx;		// index of the current element
		
	for (j = 0; j < MATSIZE; j++) {
		sumSQR = 0.;
			
		for (k = 0; k < j; k++) {
			idx = j * MATSIZE + k;	// l[j][k] index
			//sum += l[j][k] * l[j][k];
			sumSQR += ((l[idx].real()) * (l[idx].real()) +
				    (l[idx].imag()) * (l[idx].imag()));
		}
			
		idx = j * MATSIZE + j;	// a[j][j] and l[j][j] index
		//sum = a[j][j] - sum;	// L[j][j]^2
		sumSQR = a[idx].real() - sumSQR;
		//l[j][j] = sqrt(sum);
		l[idx] = sqrt(sumSQR);
			
		for (i = j + 1; i < MATSIZE; i++) {
			sum = 0;
				
			for (k = 0; k < j; k++) {
				//sum += l[i][k] * l[j][k];
				comp_t tmp;
				comp_t trasp;
				idx = j * MATSIZE + k;	// l[j][k] index
				trasp.real(l[idx].real());
                trasp.imag(- l[idx].imag());
				tmp = l[i * MATSIZE + k] * trasp;
				sum = sum + tmp;
			}
				
			//l[i][j] = (a[i][j] - sum) / l[j][j];
			comp_t app1;
			idx = i * MATSIZE + j;	// a[i][j] and l[i][j] index
			app1 = a[idx] - sum;
			l[idx] = app1 / l[j * MATSIZE + j];
		}
	}
		
	return task;
}

class WorkerTBB {
    task_t *tasks;
    public:
        void operator() (const blocked_range<size_t> &r) const
        {
            for (size_t i=r.begin(); i!=r.end(); i++) {
                task_t task = tasks[i];
                seq_worker(tasks[i]);
            }
        }

        WorkerTBB(task_t *tasks):
            tasks(tasks)
        {}
};

int main(int argc, 
    char * argv[]) 
{

    if (argc<3) {
        std::cerr << "usage: " 
                  << argv[0] 
                  << " streamlen inputfile\n";
        return -1;
    }
    
    int streamlen = atoi(argv[1]);
	//char *infile = argv[2];
    FILE *fp;
	
	//FILE *fp = fopen(infile, "r");
	//if (!fp)
	//{
	//	std::cerr << "Error opening input file\n";
	//	return -1;
	//}
	
	// Array of tasks (each task is made up of an A matrix and a L matrix)
	task_t *tasks = new task_t[streamlen];
	
    //task_t main_task;
	
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
	//int n;
	//fscanf(fp, "%d", &n);
	//MATSIZE = n;
    int n = MATSIZE;
    float real_in, imag_in;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			//fscanf(fp, "%f", &real_in);
            //tasks[0].a[(i * MATSIZE) + j].real(real_in);
			//fscanf(fp, "%f", &imag_in);
            //tasks[0].a[(i * MATSIZE) + j].imag(imag_in);
            tasks[0].a[(i * MATSIZE) + j].real(2.0);
            tasks[0].a[(i * MATSIZE) + j].imag(2.0);
            // printf("Fuko (%d,%d)\n", i, j);
		}
	
	//fclose(fp);
	
	// Initialization of matrix L
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			tasks[0].l[(i * MATSIZE) + j].real(0.);
			tasks[0].l[(i * MATSIZE) + j].imag(0.);
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


   int i;
   //for (i=0;i<streamlen;i++) {
   //     seq_worker(tasks[i]);
    //}
   parallel_for(blocked_range<size_t>(0,streamlen),WorkerTBB(tasks)); 

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
			fprintf(fp, "% 6.3f ", tasks[0].l[i * MATSIZE + j].real());
			fprintf(fp, "% 6.3fi ", tasks[0].l[i * MATSIZE + j].imag());
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

    
    
