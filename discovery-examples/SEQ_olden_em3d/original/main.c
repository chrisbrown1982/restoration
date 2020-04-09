/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
#include "make_graph.h"
#include <chrono>
#define EXTRA_DEBUG 0
extern int NumNodes;

int DebugFlag;

void print_graph(graph_t *graph, int id) 
{
  node_t *cur_node;
  cur_node=graph->e_nodes[id];

  for(; cur_node; cur_node=cur_node->next)
    {
      chatting("E: value %f, from_count %d\n", *cur_node->value, cur_node->from_count);

      chatting("   from_values [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  chatting(" %f ", *cur_node->from_values[i]);
      }
      chatting("]\n");

      chatting("   from_coeffs [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  chatting(" %f ", cur_node->coeffs[i]);
      }
      chatting("]\n");


    }
  cur_node=graph->h_nodes[id];
  for(; cur_node; cur_node=cur_node->next)
    {
      chatting("H: value %f, from_count %d\n", *cur_node->value, cur_node->from_count);

      chatting("   from_values [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  chatting(" %f ", *cur_node->from_values[i]);
      }
      chatting("]\n");

      chatting("   from_coeffs [");
      for (int i = 0; i < cur_node->from_count ; i++) {
    	  chatting(" %f ", cur_node->coeffs[i]);
      }
      chatting("]\n");


    }
}

extern int nonlocals;

int main(int argc, char *argv[])
{
  int i;
  graph_t *graph;

  dealwithargs(argc,argv);
  DebugFlag = EXTRA_DEBUG;
  chatting("Hello world--Doing em3d with args %d %d %d %d\n",
           n_nodes,d_nodes,local_p,NumNodes);

  auto t0 = std::chrono::system_clock::now();
  graph=initialize_graph();
  auto t1 = std::chrono::system_clock::now();
  if (DebugFlag) 
    for(i=0; i<NumNodes;i++)
      print_graph(graph,i);


  auto t2 = std::chrono::system_clock::now();
  compute_nodes(graph->e_nodes[0]);
  auto t3 = std::chrono::system_clock::now();
  compute_nodes(graph->h_nodes[0]);
  auto t4 = std::chrono::system_clock::now();

    if (DebugFlag) 
    for(i=0; i<NumNodes;i++)
      print_graph(graph,i);

    chatting("nonlocals = %d\n",nonlocals);
    chatting("generation    %f sec\n", std::chrono::duration<double>(t1-t0).count());
    chatting("computation1  %f sec\n", std::chrono::duration<double>(t3-t2).count());
    chatting("computation2  %f sec\n", std::chrono::duration<double>(t4-t3).count());chatting("total         %f sec\n", std::chrono::duration<double>(t4-t0).count());

  printstats();
  return 0;
}
