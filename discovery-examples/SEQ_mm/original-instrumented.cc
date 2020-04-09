/***

    we detect a GEMM,

**/


#include <stdio.h>
#include <stdlib.h>
#include <sanitizer/dfsan_interface.h>

#define N 3

#define LOOP1_BODY 1
#define LOOP2_BODY 2
#define LOOP3_BODY 3

bool loop1_marked = false;
bool loop2_marked = false;
bool loop3_marked = false;

float matrix1[N][N];
float matrix2[N][N];
float matrix3[N][N];


int main() {


  dfsan_begin_marking(LOOP1_BODY);
  for(int i = 0; i < N; i++) {
    if (!loop2_marked) {
      dfsan_begin_marking(LOOP2_BODY);
      loop2_marked = true;
    }
    for(int j = 0; j < N; j++) {
      float c = 0.0f;
      if (!loop3_marked) {
        dfsan_begin_marking(LOOP3_BODY);
        loop3_marked = true;
      }
      for(int k = 0; k < N; k++) {
        c+=matrix1[i][k]*matrix2[k][j];
      }
      if (loop3_marked) {
        dfsan_end_marking(LOOP3_BODY);
      }
      matrix3[i][j]=c;
    }
    if (loop2_marked) {
      dfsan_end_marking(LOOP2_BODY);
    }
  }
  dfsan_end_marking(LOOP1_BODY);

  // Preserve the trace by using the output matrix.
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      printf("%f ", matrix3[i][j]);
    }
    printf("\n");
  }

  return EXIT_SUCCESS;


}


