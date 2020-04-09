
/**

   We detect that this is an histogram.

 **/

#define MAX_HIST_BUCKET  20
#define NK 100

#include <cmath>
#include <cstdlib>
#include <iostream>
#define MAX(a,b) (a<b)?b:a; 

double q[MAX_HIST_BUCKET];

double ep_part( double * x)
{
    double t1, t2, t3, t4, x1, x2;
    double sx, sy, tm, an, tt, gc;
    int    i, l;
    for (i = 0; i < NK; i++) {
      x1 = 2.0 * x[2*i+0] - 1.0;
      x2 = 2.0 * x[2*i+1] - 1.0;
      t1 = x1 * x1 + x2 * x2;
      std::cout << t1 << std::endl;
      if (t1 <= 1.0) {
        t2   = sqrt(-2.0 * log(t1) / t1);
        t3   = (x1 * t2);
        t4   = (x2 * t2);
        l    = MAX(fabs(t3), fabs(t4));
        q[l] = q[l] + 1.0;
        sx   = sx + t3;
        sy   = sy + t4;
      }
    }
    return sx+sy;
}



int main ( int argc , char ** argv ) {

  double* x = (double*) malloc (NK * sizeof(double)) ;

  for (unsigned int i = 0 ; i < NK ; i++) x[i] = 1.2;
  
  double res = ep_part(x);

  std::cout << res << std::endl;

  
}
