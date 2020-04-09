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

static void * thread_func (void* arg)
{
  input_data *input = (input_data *)arg;
  for (unsigned int i = input->row_start; i < input->row_end; i++) {
	  multiply_row_by_matrix(a, i, b, res);
  }

  return (void *)0;
}

class MultiplyRowsMatrixTBB {
  double **mat1;
  double **mat2;
  double **res;
  int chunk_size;
  int extra_pool;
public:
  void operator() (const blocked_range<size_t>& r) const {
    for (size_t i=r.begin(); i!=r.end(); i++) {
      int extra = i > extra_pool ? 0 : 1;
      int chunk = chunk_size + extra;
      int count = chunk_size * i + (i > extra_pool ? extra_pool : i);
      in[i].row_start = count;
      in[i].row_end = count + chunk;
      thread_func((void *)(&in[i]));
    }
  }
  MultiplyRowsMatrixTBB(double **mat1, double **mat2, double **res, int chunk_size, int extra_pool) :
    mat1(mat1), mat2(mat2), res(res), chunk_size(chunk_size), extra_pool(extra_pool)
  {}
};
  
void threads_create()
{
  int status;

  in = (input_data *) malloc (sizeof(input_data) * thread_count);
  
  int chunk_size = dim / thread_count;
  int extra_pool = dim - (thread_count * chunk_size);

  parallel_for(blocked_range<size_t>(0,thread_count), MultiplyRowsMatrixTBB(a, b, res, chunk_size, extra_pool));

}


int main (int argc, char *argv[])
{
  if (argc<3) {
    std::cerr << "Usage: SchedulingExample <num_workers> <matrix_dim>" <<"\n";
    exit(1);
  }
  dim = atoi(argv[2]);
  int num_workers = atoi(argv[1]);

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

