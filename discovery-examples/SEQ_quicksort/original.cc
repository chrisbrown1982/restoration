/**

   quickSort( int a[], int l, int r) return the list "a" with elements from "l" to "r" sorted (algorithmic level)
   This also is a divide and conquer pattern

   Example copy past from the internet
   http://www.comp.dit.ie/rlawlor/Alg_DS/sorting/quickSort.c

 **/

#include <stdio.h>
#include <stdlib.h>


int partition( int a[], int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;
		
   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}



void quickSort( int a[], int l, int r)
{
   int j;

   if( l < r ) 
   {
   	// divide and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
	
}






int main() 
{
	int a[] = { 7, 12, 1, -2, 0, 15, 4, 11, 9};

	int i;
	printf("\nUnsorted array is:  ");
	for(i = 0; i < 9; ++i)
		printf(" %d ", a[i]);

	quickSort( a, 0, 8);

	printf("\nSorted array is:  ");
	for(i = 0; i < 9; ++i)
		printf(" %d ", a[i]);
	printf("\n\n");
	
   return EXIT_SUCCESS;
}

