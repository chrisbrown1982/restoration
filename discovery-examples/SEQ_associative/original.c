/* A simple example of an associative loop, which is not commutative, though.
   This means, we can parallelise the loop using a parallel reduction tree
   mechanism using intermediate (private) variables for intermediate results,
   but this works only from "left-to-right". What we cannot do (as the loop is
   not commutative) is to work from right-to-left or, in fact, any other
   evaluation order. The loop is not commutative and, but it is associative,
   which is sufficient for reduction tree evaluation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strings[10] = { "abc", "def", "ghi", "jkl", "mno", "pqr", "stu", "vwx", "yz0", "123" };

int main(int argc, int *argv[])
{
   int i;
   char result[255] = "";

/*
Suggested annotation: associative, but not commutative loop
*/
   for(i = 0; i < 10; i++) {
      strcat(result, strings[i]);
   }

   printf("Result: %s\n",result);

   return EXIT_SUCCESS;
}

