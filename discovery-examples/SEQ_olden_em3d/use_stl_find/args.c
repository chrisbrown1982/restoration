/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"

EM3DArguments dealwithargs(int argc, char *argv[])
{
	EM3DArguments results ;
	results.NumNodes = 0;
	results.local_p = 0; /* percentage of local edges */
	results.d_nodes = 0; /* degree of nodes */
	results.n_nodes = 0; /* number of nodes (E and H) */
	results.DebugFlag = 1;
	results.nonlocals = 0;
  if (argc > 4)
	  results.NumNodes = atoi(argv[4]);
  else
	  results.NumNodes = 1;

  if (argc > 1)
	  results.n_nodes = atoi(argv[1]);
  else
	  results.n_nodes = 64;

  if (argc > 2)
	  results.d_nodes = atoi(argv[2]);
  else
	  results.d_nodes = 3;

  if (argc > 3)
	  results.local_p = atoi(argv[3]);
  else
	  results.local_p = 75;

  return results;
}


