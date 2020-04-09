#include <stdio.h>

extern unsigned foo;

void main() {
  my_increment();
  my_increment();
  printf("foo=%u\n",foo);
}
