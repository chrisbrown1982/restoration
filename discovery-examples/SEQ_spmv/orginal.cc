/**
   We detect SPMV
 **/

__attribute__((noinline))
void sparse_mv(int lastrow, int firstrow, int* rowstr, int* colidx, double* a, double* z, double* r)
{
  double sum = 0.0;
  for (int j = 0; j < lastrow - firstrow + 1; j++) {
    double d = 0.0;
    for (int k = rowstr[j]; k < rowstr[j+1]; k++) {
      d = d + a[k]*z[colidx[k]];
    }
    r[j] = d;
  }
}
