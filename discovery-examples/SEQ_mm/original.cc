/***

    we detect a GEMM,

**/


#include <stdio.h>
#include <stdlib.h>

float matrix1[1000][1000];
float matrix2[1000][1000];
float matrix3[1000][1000];


int main() {


  for(int i = 0; i < 1000; i++) {
    for(int j = 0; j < 1000; j++) {
      float c = 0.0f;
      for(int k = 0; k < 1000; k++) {
        c+=matrix1[i][k]*matrix2[k][j];
      }
      matrix3[i][j]=c;
    }
  }

  return EXIT_SUCCESS;


}


