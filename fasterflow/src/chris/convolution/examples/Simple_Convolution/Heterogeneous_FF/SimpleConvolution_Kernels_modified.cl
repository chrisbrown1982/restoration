__kernel void simpleConvolution(__global  ushort  * output,
                                __global  ushort  * input,
                                __global  float  * mask,
                                const     uint  inputDimensions,
                                const     uint  maskDimensions,
                                __local   ushort * aTile,
                                __local   float * bTile)

{

    uint maskWidth  = maskDimensions;
    uint maskHeight = maskDimensions;
    int tid   = get_global_id(0);
    uint width  = inputDimensions;
    uint height = inputDimensions;
    //    uint vstep = (maskWidth  -1)/2;
    // uint hstep = (maskHeight -1)/2;
    int x      = tid/width; //input row number
    int y      = tid%width; // input column number
  
  // define the coordinates of this workitem thread 
  // in the 2D tile 
    int lx = get_local_id(0)/maskWidth;
    int ly = get_local_id(0)%maskWidth;
// loading into shared memory
    bTile[lx*maskWidth + ly] = mask[lx* maskWidth + ly];
    //barrier(CLK_LOCAL_MEM_FENCE);
    aTile[lx*2*maskWidth + ly] = (x - maskWidth/2 >= 0) && (x - maskWidth/2 < width)  && 
       (y - maskWidth/2 >= 0) && (y - maskWidth/2 < height) ? input[(x-maskWidth/2)*width + y - maskWidth/2] : 0;
    aTile[lx*2*maskWidth + (maskWidth +ly)] = (x - maskWidth/2 >= 0) && (x - maskWidth/2 < width)  && 
       (y + maskWidth/2 >= 0) && (y + maskWidth/2 < height) ? input[(x-maskWidth/2)*width + y + maskWidth/2] : 0;
    aTile[(maskWidth + lx)*2*maskWidth + ly] = (x + maskWidth/2 >= 0) && (x + maskWidth/2 < width)  && 
       (y - maskWidth/2 >= 0) && (y - maskWidth/2 < height) ? input[(x+maskWidth/2)*width + y - maskWidth/2] : 0;
    aTile[(maskWidth + lx)*2*maskWidth + (maskWidth + ly)] = (x + maskWidth/2 >= 0) && (x + maskWidth/2 < width)  && 
       (y + maskWidth/2 >= 0) && (y + maskWidth/2 < height) ? input[(x+maskWidth/2)*width + y + maskWidth/2] : 0;
     barrier(CLK_LOCAL_MEM_FENCE);  
   float sumFX =0;
   //computing the convolution
    for (int i = 1; i < maskWidth; i++ )
      for (int j = 1; j < maskHeight; j++ )
        sumFX += ((float)aTile[(lx + i)*2*maskWidth + (ly + j)] * bTile[(maskWidth - i)*maskWidth + (maskHeight - j)]);
    /*
     *To round to the nearest integer
     */
    sumFX += 0.5f;
    output[tid] = //aTile[(lx+1+((maskWidth-1)/2))*2*maskWidth+(ly+1+ (maskWidth-1)/2)];
    (ushort)sumFX; 
}
