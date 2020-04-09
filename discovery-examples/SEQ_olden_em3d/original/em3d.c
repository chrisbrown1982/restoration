/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
int nonlocals=0;

#define EXTRA_DEBUG 0

void compute_nodes(register node_t *nodelist)
{
  register int i;
  register node_t *localnode;

  int counter = -1;
  for (; nodelist; ) {
	  counter++;

    register double cur_value;
    register int from_count ;
    register double *other_value;
    register double coeff;
    register double value;
    /*register double *coeffs;*/
    /*register node_t **from_nodes;*/
    
    localnode = nodelist;
    cur_value = *localnode->value;
    from_count = localnode->from_count-1;

    if (EXTRA_DEBUG) printf("1) Current node is %d, value %f fromt count %d\n", counter , cur_value , from_count);

    for (i=0; i < from_count; i+=2) {
    	 if (EXTRA_DEBUG)  printf(" - iteration %d \n", i);
      other_value = localnode->from_values[i];
      coeff = localnode->coeffs[i];
      if (EXTRA_DEBUG)  printf("   - other_value %f coef %f\n", *other_value, coeff);

      if (other_value)
        value = *other_value;
      else
        value = 0;

      cur_value -= coeff*value;
      other_value = localnode->from_values[i+1];
      coeff = localnode->coeffs[i];
      
      if (other_value)
        value = *other_value;
      else
        value = 0;
      
      cur_value -= coeff*value;
      if (EXTRA_DEBUG)  printf("   - iteration %d other_value %f coef %f cur_value %f\n", i, *other_value, coeff, cur_value);

      /*chatting("from %d, coeff %f, value %f\n",count,coeff,value);*/
    }

    if (EXTRA_DEBUG)  printf("2) Current node is %d, value %f value %f fromt count %d\n", counter , value,  cur_value , from_count);

    if (i==from_count)  {
      other_value = localnode->from_values[i];
      coeff = localnode->coeffs[i];
      
      if (other_value)
        value = *other_value;
      else
        value = 0;
      
      cur_value -= coeff*value;
    }

    if (EXTRA_DEBUG)  printf("3) Current node is %d, value %f value %f fromt count %d\n", counter , value,  cur_value , from_count);



    *localnode->value = cur_value;

     if (EXTRA_DEBUG)  printf("4) check update %f\n", *localnode->value);

    nodelist = localnode->next;
  } /* for */
}
