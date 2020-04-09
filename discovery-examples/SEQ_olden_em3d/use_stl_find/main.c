/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
#include "make_graph.h"
#include <chrono>
#define EXTRA_DEBUG 0
void print_graph(graph_t *graph)
{
  node_t *cur_node;
  cur_node=graph->e__nodes;

  for(; cur_node; cur_node=cur_node->next)
    {
      printf("E: value %f, from_count %d\n", *cur_node->value, cur_node->from_count);

      printf("   from_values [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  printf(" %f ", *cur_node->from_values[i]);
      }
      printf("]\n");

      printf("   from_coeffs [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  printf(" %f ", cur_node->coeffs[i]);
      }
      printf("]\n");


    }
  cur_node=graph->h__nodes;
  for(; cur_node; cur_node=cur_node->next)
    {
	  printf("H: value %f, from_count %d\n", *cur_node->value, cur_node->from_count);

	  printf("   from_values [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  printf(" %f ", *cur_node->from_values[i]);
      }
      printf("]\n");

      printf("   from_coeffs [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  printf(" %f ", cur_node->coeffs[i]);
      }
      printf("]\n");


    }
}


int main(int argc, char *argv[])
{
  int i;
  graph_t *graph;

  EM3DArguments arguments = dealwithargs(argc,argv);

  printf("Hello world--Doing em3d with args %d %d %d %d\n",
		  arguments.n_nodes,arguments.d_nodes,arguments.local_p,arguments.NumNodes);
  auto t0 = std::chrono::system_clock::now();
  graph=initialize_graph(arguments.n_nodes, arguments.d_nodes, arguments.local_p);
  auto t1 = std::chrono::system_clock::now();
arguments.DebugFlag = EXTRA_DEBUG;  
if (arguments.DebugFlag)
     print_graph(graph);

  auto t2 = std::chrono::system_clock::now();
  compute_nodes(graph->e__nodes);
  auto t3 = std::chrono::system_clock::now();
  compute_nodes(graph->h__nodes);
  auto t4 = std::chrono::system_clock::now();
  
  if (arguments.DebugFlag) 
      print_graph(graph);

  printf("nonlocals = %d\n",arguments.nonlocals);
  printf("generation    %f sec\n", std::chrono::duration<double>(t1-t0).count());
  printf("computation1  %f sec\n", std::chrono::duration<double>(t3-t2).count());
  printf("computation2  %f sec\n", std::chrono::duration<double>(t4-t3).count());
  printf("total         %f sec\n", std::chrono::duration<double>(t4-t0).count());

  printstats();
  return 0;
}
