
/**

   We detect a stencil 

 **/

float matrix1[1000][1000];
float matrix2[1000][1000];
float matrix3[1000][1000];

__attribute__((noinline))
void psinv_simple(void* or_, void* ou, int n1, int n2, int n3, double c[4], int k)
{
  auto r = (double (*)[n2][n1])or_;
  auto u = (double (*)[n2][n1])ou;

  double c_0 = c[0];
  double c_1 = c[1];
  double c_2 = c[2];

  int i3, i2, i1;

  for (i3 = 1; i3 < n3-1; i3++) {
    for (i2 = 1; i2 < n2-1; i2++) { 
      for (i1 = 1; i1 < n1-1; i1++) {
        u[i3][i2][i1] = u[i3][i2][i1]
                      + c_0 * ( r[i3][i2-1][i1] + r[i3][i2+1][i1]
                              + r[i3-1][i2][i1] + r[i3+1][i2][i1])
                      + c_1 * ( r[i3][i2][i1-1] + r[i3][i2][i1+1]
                              + r[i3][i2-1][i1] + r[i3][i2+1][i1]
                              + r[i3-1][i2][i1] + r[i3+1][i2][i1])
                      + c_2 * ( r[i3-1][i2-1][i1] + r[i3-1][i2+1][i1]
                              + r[i3+1][i2-1][i1] + r[i3+1][i2+1][i1]
                              + r[i3][i2-1][i1-1] + r[i3][i2+1][i1-1]
                              + r[i3-1][i2][i1-1] + r[i3+1][i2][i1-1]
                              + r[i3][i2-1][i1+1] + r[i3][i2+1][i1+1]
                              + r[i3-1][i2][i1+1] + r[i3+1][i2][i1+1] );
        //--------------------------------------------------------------------
        // Assume c[3] = 0    (Enable line below if c[3] not= 0)
        //--------------------------------------------------------------------
        //            + c[3] * ( r2[i1-1] + r2[i1+1] )
        //--------------------------------------------------------------------
      }
    }
  }
}
