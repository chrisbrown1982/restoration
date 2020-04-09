
/**

 In this function we detect hoist select, that is 
 jump select to one of two places with the same base addr. 

 BEGIN HOISTSELECT
 {"base":"i64* %5",
  "input1":"%68 = getelementptr inbounds i64, i64* %5, i64 %67",
  "input2":"%65 = getelementptr inbounds i64, i64* %5, i64 %61",
  "select":"%69 = select i1 %64, i64* %68, i64* %65"}
 END HOISTSELECT

 **/

struct cartesian {float x, y, z;};

int doCompute(struct cartesian *data1, int n1, struct cartesian *data2, int n2,
              int doSelf, long long *data_bins, int nbins, float *binb)
{
    int i, j, k;
    if (doSelf)
    {
        n2 = n1;
        data2 = data1;
    }
  
    for (i = 0; i < n1; i++)
    {
        const float xi = data1[i].x;
        const float yi = data1[i].y;
        const float zi = data1[i].z;
      
        for (j = 0; j < n2; j++)
        {
            float dot = xi * data2[j].x + yi * data2[j].y + zi * data2[j].z;

            // run binary search
            int min = 0;
            int max = nbins;
            int k, indx;

            while (max > min+1)
            {
                k = (min + max) / 2;
                if(dot >= binb[k])
                    max = k;
                else 
                    min = k;
            }

            if (dot >= binb[min]) 
            {
                data_bins[min] += 1;
            }
            else if (dot < binb[max]) 
            { 
                data_bins[max+1] += 1;
            }
            else 
            { 
                data_bins[max] += 1;
            }
        }
    }

    return 0;
}
