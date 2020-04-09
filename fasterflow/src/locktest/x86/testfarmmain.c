#include <stdio.h>

#define MAX_TASKS 100

int tq1[MAX_TASKS+2];
int tq2[MAX_TASKS+2];

extern void worker(int tqi[],int tqo[]);

int main() {
  tq1[0] = 2; tq1[1] = 0;
  tq2[0] = 2; tq2[1] = 0;
  puttask(tq1,40);
  worker(tq1,tq2);
  fprintf(stderr,"res=%d\n",gettask(tq2));
}
