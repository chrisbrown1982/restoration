/**
 We detect histogram
 **/

#include <cstdlib>
#include <cstdio>
#include <iostream>



#define ITER  10


void komplett_unsinnig(float* array, int* array2, int SIZE)
{
    for(int i = 0; i < ITER; i++)
    {
        for(int j = 0; j < SIZE; j++)
           array[array2[j]] += 2.0;
    }
}



int main(int argc, char *argv[])
{

  
  int RANGE = atoi (argv[1]) ;
  int SIZE  = atoi (argv[2]) ;

  float* OUT = (float*) calloc (RANGE , sizeof(float)) ; 
  int*   IN  = (int*)   calloc (SIZE  , sizeof(float)) ;

  srand(0);
  for(int i = 0; i < SIZE; i++) {
    IN[i] = rand() % RANGE ;
    std::cout << " " << IN[i] ;
  }
  std::cout << std::endl;
  
  
  komplett_unsinnig(OUT, IN, SIZE);

  
  for(int i = 0; i < RANGE; i++) {
    std::cout << " " << OUT[i] ;
  }
  std::cout << std::endl;
  
  
  return EXIT_SUCCESS;
}
