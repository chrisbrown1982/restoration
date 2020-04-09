#include <stdio.h>
#include <stdlib.h>

int global = 0;

/* Both functions f and g are not side-effect free, i.e. they
   change the value of a global variable */

/* Not side-effect free */
int f(int x)
{
  global += 1;
  return 2 * x;
}

/* Not side-effect free */
int g(int x)
{
  global += 2;
}


int main(int argc, char *argv[])
{
  int a = 3;
  int b = 4;

  /* The two calls to f and g are commutative, i.e.
     when called in reverse order (g before f) the values of 
     c, d, and global are indistinguishable from the original
     order of execution. f and g can, in fact, be executed in
     parallel if accesses to the shared variable global are
     transactional.
  */
  /* Suggested annotation: f, g are commutative in this calling context
     Note: f, g may not be commutative in different calling context, e.g.
     f(g(a)) */
  int c = f(a);
  int d = g(b);

  printf ("%d, %d, %d\n", c, d, global);

  return EXIT_SUCCESS;
}
