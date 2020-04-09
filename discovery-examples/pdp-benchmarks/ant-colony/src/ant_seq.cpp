#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>

#define const_beta 2
#define const_xi 0.1
#define const_rho 0.1
#define const_q0 0.9
#define const_max_int 2147483647
#define const_e 0.000001

unsigned int num_ants;
unsigned int num_jobs;
unsigned int *weight;
unsigned int *deadline;
unsigned int *process_time;
unsigned int **all_results;
double *tau;
unsigned int *cost;
unsigned int best_t = UINT_MAX;
unsigned int *best_result;
double t1;

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

using namespace std;
//using namespace tbb;

void init(char *fname, unsigned int num_ants) {         
  unsigned int num_tasks;
  ifstream infile(fname);
  if (infile.is_open()) {
    infile >> num_jobs >> num_tasks;

    weight = new unsigned int[num_jobs];
    deadline = new unsigned int[num_jobs];
    process_time = new unsigned int[num_jobs];
    
    for(int j=0; j<num_jobs; j++) 
      infile >> process_time[j];
    for(int j=0; j<num_jobs; j++)
      infile >> weight[j];
    for(int j=0; j<num_jobs; j++) 
      infile >> deadline[j];
    
    all_results = new unsigned int *[num_ants];
    for (int j=0; j<num_ants; j++) 
      all_results[j] = new unsigned int[num_jobs];
    tau = new double [num_jobs*num_jobs];
    for (int j=0; j<num_jobs; j++)
      for (int i=0; i<num_jobs; i++)
	tau[i*num_jobs+j] = 1.0;
    best_result = new unsigned int[num_jobs];
  }
}

static void mdd( double *res, unsigned int scheduled_time ) 
{
  unsigned int i;
  for (i=0; i<num_jobs; i++) {
    if (scheduled_time + process_time[i] > deadline[i]) 
      res[i] = 1.0 / (scheduled_time + process_time[i] );
    else 
      res[i] = 1.0 / (deadline[i] );
  }
}


static void findSolution( unsigned int *results, 
                          double *pij,
                          double *eta,
                          double *tau)
{
  unsigned int i,j,k;
  unsigned int scheduled_time = 0;
  unsigned int remain_time = 0;
  double sumpij = 0;
  unsigned int *tabus = new unsigned int[num_jobs];
  double q; 
  double *tauk;

  double maxp;
  double x;

  memset(tabus, 1, num_jobs * sizeof(unsigned int));
  
  for (i = 0; i < num_jobs; i++) 
    remain_time += process_time[i];
  
  for (k = 0; k < num_jobs-1; k++){
    tauk = &tau[k*num_jobs];
    mdd(eta, scheduled_time);
    for (i = 0; i < num_jobs; i++) {
      if (tabus[i] != 0) {
	pij[i] = pow(eta[i], const_beta) * tauk[i];
	sumpij += pij[i];
      } else pij[i] = 0;
    }
    
    q = ((double)rand())/RAND_MAX;
    if (q < const_q0){
      j = 0;
      maxp = pij[0];
      for (i = 1; i < num_jobs; i++)
	if (pij[i] > maxp){
	  j = i;
	  maxp= pij[i];
	}
    }
    else{
      q = ((double)rand())/RAND_MAX;
      q *= sumpij;
      double temp = 0.0;
      j = 0;
      while(temp - const_e < q && j < num_jobs ){
	temp += pij[j];
	j++;
      }
      j--;
      while ( tabus[j] == 0) j--;
    }
    if (j>num_jobs)
      fprintf (stderr, "kuckuricku %d\n", j);
    results[k] = j;
    tabus[j] = 0;
    scheduled_time += process_time[j];
    remain_time -= process_time[j];
    
  }
  //	find the last job
  j = 0;
  for (i = 0; i < num_jobs; i++){
    if (tabus[i] != 0) j = i;
  }
  k = num_jobs-1;
  results[k] = j;
  free( tabus);
}

unsigned int fitness( unsigned int *result ) {
  unsigned int cost = 0;
  unsigned int i,j;
  unsigned int time = 0;
  for (i = 0; i < num_jobs; i++){
    j = result[i];
    time += process_time[j];
    if (time > deadline[j]) {
      cost += (time - deadline[j]) * weight[j];
    }
  }
  return(cost);
}



unsigned int solve ( unsigned int ant_id ) {
  unsigned int cost;
  double *pij = new double[num_jobs];
  double *eta = new double[num_jobs];

  findSolution( all_results[ant_id], 
                pij,
                eta,
                tau);

  cost = fitness(all_results[ant_id]);

  
  free( pij);
  free( eta);
  
  return(cost);
  
}

void update( unsigned int best_t, unsigned int *best_result ) {
  unsigned int i,j;
  double ddd = const_rho / best_t * num_jobs;
  double dmin = 1.0 / 2.0 / num_jobs;
  
  for (i = 0; i < num_jobs; i++) {
    for (j = 0; j < num_jobs; j++) {
      tau[i*num_jobs+j] *= 1-const_rho;
      if (tau[i*num_jobs+j] < dmin) 
        tau[i*num_jobs+j] = dmin;
    }
  }
  
  for (i = 0; i < num_jobs; i++) {
    tau[i*num_jobs+best_result[i]] +=  ddd;
  }
  
}

unsigned int pick_best(unsigned int **best_result)
{
  unsigned int i,j;
  unsigned int best_t = UINT_MAX;

  for (i=0; i<num_ants; i++) {
      if(cost[i] < best_t) {
	best_t = cost[i];
	*best_result = all_results[i];
      }
  }
  
  return (best_t);
}

int main(int argc, char **argv) {
  unsigned int num_iter; 
  char *fname;
  unsigned int i, j;

  if (argc<4) { 
    cerr << "Usage: ant_seq <nr_iterations> <nr_ants> <input_filename>" << endl;
    exit(1);
  }
  
  num_iter = atoi(argv[1]);
  num_ants = atoi(argv[2]);
  fname = argv[3];
  
  init (fname, num_ants);
  cost = new unsigned int[num_ants];
  
  t1 = get_current_time();
  for (j=0; j<num_iter; j++) {
    for (i=0; i<num_ants; i++) { 
      cost[i] = solve (i);
    }
    best_t = pick_best(&best_result);
    update(best_t, best_result);
  }

  t1 = get_current_time() - t1;
  cout << "Total runtime is " << t1 << " -- best solution found has cost " << best_t << endl;
}


