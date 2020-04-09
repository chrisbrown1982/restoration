#include <stdio.h>
#include <stdlib.h>

#define N 1024

unsigned long A[N][N];
unsigned long B[N][N];
unsigned long C[N][N];

int worker1(int i)
{
        unsigned long _C=0;
        int j=0;
        int k=0;    

        for(j=0;j<N;++j){

            for(k=0;k<N;++k)
               _C += A[i][k]*B[k][j];

            C[i][j] = _C;
            _C = 0;
        }

        return _C;
}

int main(int argc, char * argv[]) {
// 	int nworkers = atoi(argv[1]);

        int i, j;

	for(i=0;i<N;++i)
           for(j=0;j<N;++j) {
             A[i][j] = i+j;
             B[i][j] = i*j;
             C[i][j] = 0;
        }

        //printf("offloading tasks...\n");
	for (i=0; i<N; i++) {
          // offload the tasks to the farm...
          //printf("offloading tasks\n");
          worker1(i);
        }
}
