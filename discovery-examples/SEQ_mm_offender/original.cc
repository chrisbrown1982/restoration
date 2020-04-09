/***

     we detect a GEMM

      if   c+=matrix1[i][k]*matrix2[k][j] + 3;
      was  c+=matrix1[i][k]*matrix2[k][j];
          

 **/
float matrix1[1000][1000];
float matrix2[1000][1000];
float matrix3[1000][1000];



void function()
{

    for(int i = 0; i < 1000; i++)
        for(int j = 0; j < 1000; j++)
        {
            float c = 0.0f;
            for(int k = 0; k < 1000; k++)
               #pragma discovery if_line_was "c+=matrix1[i][k]*matrix2[k][j];"
               c+=matrix1[i][k]*matrix2[k][j] + 3;
            matrix3[i][j]=c;
        }

}


