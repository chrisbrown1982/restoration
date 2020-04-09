/**
   We detect AXPY
 **/

__attribute__((noinline))
void vector_addition(float* vector_left, float* vector_right, unsigned n)
{
    for(unsigned i = 0; i < n; i++)
    {
        vector_left[i] += vector_right[i];
    }
}
