/*
  This is one of several examples making use of a user-defined single-linked list.
  In this example, we create a list, populate it with a few numbers and then increment
  each element by one using a generic traversal function, which applies the "increment"
  function to each list element. Effectively, this is a map skeleton, but with the added
  complication that we're dealing with a linked list.

  Depending on its use an advanced analysis could possibly determine that the linked list
  is not really required at all, but could be replaced with another STL data structure/
  container and the map could be parallelised, which in this current version is prevented
  by the linked list data structure.

  At least, we could parallelise the traversal loop in the traverse function using decoupled
  software pipelining, but it would be better to recognise it as a map and replace the list
  data structure with something more "parallel". This means we would need to know how the
  data structure is used and modified and where these uses and modifications happen.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAXDATA 1000 // Length of list.
                     // NB length is now fixed, arbitrarily chosen.

typedef int (*callback)(const int);

/* Plus 1 */
int increment(const int n)
{
  return n + 1;
}

/* For the type translation case. */
/* Very simple (sequential) map implementation over c arrays. Proper version is
   polymorphic, &c. */
void Map(const callback f, int* xs)
{
  for (int i = 0; i < 10; i++) /* Blegh. C++ vectors would avoid this. */
                               /* Can't use while xs[i] != 0 bc 0 is an int. */
    {
      xs[i] = f(xs[i]);
    }
}

void display(int* xs)
{
  for (int i = 0; i < 10; i++) /* Blegh. C++ vectors would avoid this. */
                               /* Can't use while xs[i] != 0 bc 0 is an int. */
    {
      printf("%d ", xs[i]);
    }
}

int main(int argc, char *argv[])
{
  /* Linked list is now a standard c array (or desired alternative). */
  /* We assume that we have discovered that the linked list can be replaced
     with an equivalent (un)ordered data structure, and have thus translated
     it. */
  /* Size of the array (or equivalent) comes from profiling? If we know we're
     not going to change the spine of the list, we can specify exact lengths,
     perhaps? */
  int xs[MAXDATA];

  callback inc = increment;

  /* Append a few numbers */
  /* Flipped to decrement in order to replicate ordering. */
  for(int i = 9; i >= 0; i--) {
    xs[9-i] = i;
  }

  /* Increment each element by 1 */
  Map(inc, xs);

  /* Now print the list */
  display(xs);

  return EXIT_SUCCESS;
}


/*
 * Process:
 *
 * 1. Determine properties of custom data structure. (How?)
 *   - Type of list
 *   - Is ordering important for operations? (Yes, for display. No, for traverse)
 * 2. Translate data strcuture
 *   - Inspect each function, translate if it uses that data type
 *   - Would need to somehow work out what the elements in the data type are;
 *     e.g. next = tail of list, data = member,
 *     Would also need to work out how to translate operations that interact with these things
 *
 * 3. Inspect main:
 *   - Creation and initialisation of data structure
 *     - Assignment statements
 *     - For loop calling prepend (inspect prepend, inspect create)
 *       - Assignment only
 *
 *  - Inspect traverse, again discover it a map operation
 *    Have some idea of how to do this, the trick is how to work out what the primitive operations are
 *    Maybe compare data structure instances before and after application to infer how things are going?
 *
 */
