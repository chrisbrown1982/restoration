#include <stdio.h>
#include <stdlib.h>

int global = 0;

/* f is not side-effect free, i.e. they
   change the value of a global variable */

/* Not side-effect free */
int f(int x)
{
  global += 1;
  return 2 * x;
}


int main(int argc, char *argv[])
{
  int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int b[10];
  int i;

  /* Suggested annotation: This loop is commutative, i.e. iterations of the
     loop can be executed in any order (even different from the original
     sequential order. Furthermore, this is sort of a map, but the function
     f has a bit of a flaw, it is not side-effect free. However, it's side-
     effect doesn't hurt us here since it's commutative. More annotations
     include that each element of "a" is touched exactly once. Also each
     element of "b" is written exactly once. This could also be a task farm
     following the same argument as for map (bar the commutative side-effect
     on global).
  */
  for(i = 0; i < 10; i++) {
    b[i] = f(a[i]);
  }

  /* Just print out the values */
  for(i = 0; i < 10; i++)
    printf("%d\n",b[i]);

  printf ("%d\n", global);

  return EXIT_SUCCESS;
}
