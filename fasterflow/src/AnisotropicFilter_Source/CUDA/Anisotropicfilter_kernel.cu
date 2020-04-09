

__global__ void anisotropy_kernel(float1* imInD, int M,int N, float k, float lambda, short type) {
         
	     int i = blockIdx.x * blockDim.x + threadIdx.x;
         int j = blockIdx.y * blockDim.y + threadIdx.y;
         int index = j+i*M;
         int len = N*M;

        float deltaN; 
		float deltaS;
		float deltaW;
		float deltaE;
		float cN; 
		float cS;
		float cW;
		float cE;
		int indexN; 
		int indexS;
		int indexW;
		int indexE;
		float val;
    
        
        indexN = (j)+(i-1)*(M);
        indexS = (j)+(i+1)*(M);
        indexW = (j-1)+(i)*(M);
        indexE = (j+1)+(i)*(M);        

        if (i>1)
            deltaN = imInD[indexN].x-imInD[index].x;
            
        if (indexS < len)
            deltaS = imInD[indexS].x-imInD[index].x;  
              
        if (j>1)
            deltaW = imInD[indexW].x-imInD[index].x;   
             
        if (indexE < len)
            deltaE = imInD[indexE].x-imInD[index].x;   
      
        if (type==1) {
            cN = exp(-(pow((deltaN / k),2)));
            cS = exp(-(pow((deltaS / k),2)));
            cW = exp(-(pow((deltaW / k),2)));
            cE = exp(-(pow((deltaE / k),2)));  
        } else {
            cN = 1/(1+pow((deltaN / k),2));
            cS = 1/(1+pow((deltaS / k),2));
            cW = 1/(1+pow((deltaW / k),2));
            cE = 1/(1+pow((deltaE / k),2));        
        }

		
		val = (lambda*(cN*deltaN + cS*deltaS + cW*deltaW + cE*deltaE));  
		imInD[index].x += val;

        
		
        __syncthreads();    
}




