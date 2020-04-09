/* For copyright information, see olden_v1.0/COPYRIGHT */

/* make_graph.c - Create a graph to be solved for the electromagnetic
 *                problem in 3 dimensions.
 *
 * By:  Martin C. Carlisle
 * Date: Feb 23, 1994
 *
 */

#define SEED1 0
#define SEED2 0
#define SEED3 0



#include "em3d.h"
#include "util.h"
#include <chrono>

/* We expect node_table to be a local table of e or h nodes */
void fill_table(node_t *node_table, double *values, int size)
{

	// Get uniform is statefull need to be call in order
	for (int i = 0 ; i < size ; i ++ ) {
		values[i] = gen_uniform_double();
	}

	node_table[size - 1].value = &(values[size - 1]);
	node_table[size - 1].from_count = 0;
	node_table[size - 1].next = NULL;


	for (int i = size - 2 ; i >= 0 ; i -- ) {
		node_table[i].value = &(values[i]);
		node_table[i].from_count = 0;
		node_table[i].next = &(node_table[i+1]);
	}



}
#include<vector>
#include <algorithm>

void make_neighbors(node_t *nodelist, node_t *table, int tablesz,
		int degree, int percent_local)
{

	for (int i = 0 ; i < tablesz ; i ++) {
		node_t *cur_node = &(nodelist[i]);
		cur_node->to_nodes = (node_t **)malloc(degree*(sizeof(node_t *)));
	}


	for (int i = 0 ; i < tablesz ; i ++) {
		node_t *cur_node = &(nodelist[i]);
		node_t *other_node;

		int k = 0;

		std::vector<int> numbers;
		while (numbers.size() < degree) {
			int number = gen_number(tablesz);
	        int res = check_percent(percent_local);
	        if (!res) gen_signed_number(1);
			if (  std::find(numbers.begin(), numbers.end(), number) == numbers.end() ) {
				numbers.push_back(number);
			}
		}

		for (int j=0; j<degree; j++) {
			other_node = &(table[numbers[j]]);   /* <------ 4% load miss penalty */
			cur_node->to_nodes[j]=other_node;       /* <------ 6.5% store penalty */
			++cur_node->to_nodes[j]->from_count;            /* <----- 12% load miss penalty */
		}
	}
}

void update_from_coeffs(node_t *nodelist, int n_nodes) {


	/* Setup coefficient and from_nodes vectors for h nodes */
	for (int i = 0 ; i < n_nodes ; i ++) {
		node_t *cur_node = &(nodelist[i]);
		int from_count = cur_node->from_count;

		if (from_count < 1) {
			printf("Help! no from count (from_count=%d) \n", from_count);
			cur_node->from_values = (double **)malloc(20 * sizeof(double *));
			cur_node->coeffs = (double *)malloc(20 * sizeof(double));
			cur_node->from_length = 0;
		} else {
			cur_node->from_values = (double **)malloc(from_count * sizeof(double *));
			cur_node->coeffs = (double *)malloc(from_count * sizeof(double));
			cur_node->from_length = 0;
		}
	}
}

void fill_from_fields(node_t *nodelist, int size, int degree) {

	// random is stateful
	// #pragma omp parallel for schedule(static) num_threads(THREADS)
	for (int i = 0 ; i < size ; i++) {

		node_t* cur_node = &nodelist[i];


		for (int j = 0; j < degree; j++) {
			int count;
			node_t *other_node = cur_node->to_nodes[j]; /* <-- 6% load miss penalty */
			double **otherlist;
			double *value = cur_node->value;

			if (!other_node) printf("Help!!\n");
			count=(other_node->from_length)++;  /* <----- 30% load miss penalty */
			otherlist=other_node->from_values;  /* <----- 10% load miss penalty */

			if (!otherlist) {
				/*printf("node 0x%p list 0x%p count %d\n",
                 other_node,otherlist,thecount);*/
				otherlist = other_node->from_values;
				/*printf("No from list!! 0x%p\n",otherlist);*/
			}

			otherlist[count] = value;                 /* <------ 42% store penalty */

			/* <----- 42+6.5% store penalty */
			other_node->coeffs[count]=gen_uniform_double();
		}
	}
}



void make_tables(table_t *table,int n_nodes, int d_nodes, int local_p) {
	node_t *h_table,*e_table;
	double *h_values, *e_values;

	init_random(SEED1);

	h_values = (double *)malloc(n_nodes*sizeof(double));
	h_table = (node_t  *)malloc(n_nodes*sizeof(node_t));

	fill_table(h_table,h_values,n_nodes);

	e_values = (double *)malloc(n_nodes*sizeof(double));
	e_table = (node_t  *)malloc(n_nodes*sizeof(node_t));

	fill_table(e_table,e_values,n_nodes);

	table->e__table = e_table;
	table->h__table = h_table;
}

void make_all_neighbors(table_t *table, int n_nodes, int d_nodes, int local_p) {

	init_random(SEED2);
	/* We expect table to be remote */

	make_neighbors(table->h__table,
			table->e__table,n_nodes,
			d_nodes,local_p);

	make_neighbors(table->e__table,
			table->h__table,n_nodes,
			d_nodes,local_p);


}

void update_all_from_coeffs(table_t *table, int n_nodes)
{
	update_from_coeffs(table->h__table, n_nodes);
	update_from_coeffs(table->e__table, n_nodes);
}

void fill_all_from_fields(table_t *table, int n_nodes, int d_nodes)
{
	init_random(SEED3);
	fill_from_fields(table->h__table,n_nodes, d_nodes);
	fill_from_fields(table->e__table,n_nodes, d_nodes);
}





graph_t *initialize_graph(int n_nodes, int d_nodes, int local_p) {

	table_t *table = (table_t *)malloc(sizeof(table_t));
	graph_t *retval = (graph_t *)malloc(sizeof(graph_t));


	// **********************************************************
	// Here we have a table of nodes, generation can be parallel
	// **********************************************************

	printf("making tables \n");
	make_tables(table,n_nodes, d_nodes, local_p);



	/* At this point, for each h node, we give it the appropriate number
     of neighbors as defined by the degree */
	printf("making neighbors\n");
	  auto t0 = std::chrono::system_clock::now();
	make_all_neighbors(table, n_nodes, d_nodes, local_p);
	  auto t1 = std::chrono::system_clock::now();
	  printf("neighbors    %f sec\n", std::chrono::duration<double>(t1-t0).count());

	/* We now create from count and initialize coefficients */
	printf("updating from and coeffs\n");
	update_all_from_coeffs(table, n_nodes);

	/* Fill the from fields in the nodes */
	printf("filling from fields\n");
	fill_all_from_fields(table, n_nodes, d_nodes);

	printf("localizing coeffs, from_nodes\n");
	printf("cleanup for return now\n");
	retval->e__nodes = table->e__table;
	retval->h__nodes = table->h__table;

	printf("Clearing NumMisses\n");
	printf("Returning\n");

	return retval;
}
