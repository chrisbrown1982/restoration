#include <iostream>
#include <vector>
#include <ctime>
#include <sys/time.h>
#include <pthread.h>
// #include <hwloc.h>

#include <tbb/tbb.h>

using namespace tbb;

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>

/*
 * Input Parameters
 */

double **a;
double **b;
double **res;

int dim;
int thread_count;

typedef struct {
  int row_start;
  int row_end;
} input_data;

input_data *in;


double get_current_time()
{
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;


  if (gettimeofday(&tval, NULL) == -1)
    result = -1.0;
  else if(!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  }
  else
    result = (double) (tval.tv_sec - start) + 1.0e-6*(tval.tv_usec - startu);
  
  return result;
}

// #define OVER_FMT "handler(%d ) Overflow at %p! bit=0x%llx \n"
// #define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__); exit(retval); }

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

double multiply_row_by_column (double **mat1, int row, double **mat2, int col)
{
  int k;
  double sum=0;
  for (k=0; k<dim; k++)
    sum += mat1[row][k] * mat2[k][col];

  return sum;
}



void multiply_row_by_matrix (double **mat1, int row, double **mat2, double **res)
{
  for (int col=0; col<dim; col++)
    res[row][col] = multiply_row_by_column (mat1, row, mat2, col);

}

class MultiplyRowsMatrixTBB {
  double **mat1;
  double **mat2;
  double **res;
public:
  void operator() (const blocked_range<size_t>& r) const {
    for (size_t i=r.begin(); i!=r.end(); i++) {
      multiply_row_by_matrix(a, i, b, res);
    }
  }
  MultiplyRowsMatrixTBB(double **mat1, double **mat2, double **res) :
    mat1(mat1), mat2(mat2), res(res)
  {}
};
  
void threads_create()
{
  int status;

  parallel_for(blocked_range<size_t>(0,dim), MultiplyRowsMatrixTBB(a, b, res));

}


int main (int argc, char *argv[])
{
  if (argc<3) {
    std::cerr << "Usage: matmult_TBB2 <num_workers> <matrix_dim>" <<"\n";
    exit(1);
  }
  dim = atoi(argv[2]);
  int num_workers = atoi(argv[1]);

  task_scheduler_init init(num_workers);

  a = (double **) malloc (sizeof(double *) * dim);
  b = (double **) malloc (sizeof(double *) * dim);
  res = (double **) malloc (sizeof(double *) * dim);
  for (int i=0; i<dim; i++) {
    a[i] = (double *) malloc (sizeof(double) * dim);
    b[i] = (double *) malloc (sizeof(double) * dim);
    res[i] = (double *) malloc (sizeof(double) * dim);
    for (int j=0; j<dim; j++) {
      a[i][j] = 42.0;
      b[i][j] = 42.0;
    }
  }

  double t1 = get_current_time();

  thread_count = num_workers;

  threads_create();

  double t2 = get_current_time();
  std::cout << "Total execution time: " << t2-t1 << "\n";

  return 0;
}

