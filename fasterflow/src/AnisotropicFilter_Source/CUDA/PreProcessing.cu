/*
*
 */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// includes, project
//#include <cutil_inline.h>

// includes, kernels
//#include "Anisotropicfilter_kernel.cu"
__global__ void anisotropy_kernel(float1* imInD, int M,int N, float k, float lambda, short type);

#define BLOCK_SIZE 16

int N,M;
int iter;
double k,lambda;
float1* imInD;
int size;
short otype;





extern "C" void
initPreProcessing(int imWidth, int imHeight, int niter, float kappa, float lampda, int type)
{

	N = imWidth;
    M = imHeight;
	
	iter = niter;  
    k = kappa;  
    lambda = lampda;   
    otype = type;  

	
	cudaMalloc((void **)&imInD,M*N*sizeof(float));


}


extern "C" void
executePreProcessing(float* imIn, float* output, int len)
{

	/* execute anisotropie filter */
	cudaMemcpy(imInD, imIn,len*sizeof(float), cudaMemcpyHostToDevice);
    
    dim3 dimBlock(BLOCK_SIZE,BLOCK_SIZE);
    dim3 dimGrid(ceil(N / (float)dimBlock.x), ceil(M / (float)dimBlock.y));    

    for (int i=1;i<=iter;++i) {
        anisotropy_kernel<<<dimGrid,dimBlock>>>(imInD, M, N,(float)k,(float)lambda, otype);
    }

 cudaDeviceSynchronize();
//cudaError_t cerr = cudaGetLastError();
//cudaGetErrorString(cerr);
	//CUT_CHECK_ERROR("Kernel execution failed");
	
   
    cudaMemcpy(output,imInD,len*sizeof(float),cudaMemcpyDeviceToHost);  
	
	cudaThreadExit();
	
	//cutilSafeCall( cudaFree( d_Temp ) );    
	
}



  
