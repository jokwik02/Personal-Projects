#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/graph.h"
#include "include/dlist.h"
#include "include/array_2d.h"


/*
 * Implementation of a directed graph, specifically to be used 
 * in a program that builds a graph and checks if there is a 
 * path between two nodes
 *
 * Authors: Hinok Zakir Saleh (c22hsh@cs.umu.se)
 * 	    Joakim Wiksten    (c22jwn@cs.umu.se)
 *
 * Version information:
 *   v1.0 2023-03-17: First public version.
 */


// ===========INTERNAL DATA TYPES============

// Anonymous declarations of node and graph.
typedef struct node
{
	int id;
	char name[40];
	bool seen;
} node;
typedef struct graph
{
	int max_nodes;
	int numb_of_nodes;
	dlist *nodes;
	array_2d *matrix;
} graph;

// =========== NODE COMPARISON FUNCTION ===========

/**
 * nodes_are_equal() - Check whether two nodes are equal.
 * @n1: Pointer to node 1.
 * @n2: Pointer to node 2.
 *
 * Returns: true if the nodes are equal, and false if they are not.
 *
 */
bool nodes_are_equal(const node *n1, const node *n2)
{
	return (n1->id == n2->id);
}

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * graph_empty() - Create an empty graph.
 * @max_nodes: The maximum number of nodes the graph can hold.
 *
 * Returns: A pointer to the new graph.
 */

graph *graph_empty(int max_nodes)
{
	//Allocate memory for a new graph
	graph *g = malloc(sizeof(struct graph));

	//Create a 2d array to represent the edges 
	g->matrix = array_2d_create(0, max_nodes, 0, max_nodes, free);
	g->max_nodes = max_nodes;
	g->numb_of_nodes = 0;

	//Creates a direct list for the graphs nodes
	g->nodes = dlist_empty(free);

	//Iterate over the whole matrix and set every value to zero
	for (int i = 0; i < max_nodes; i++)
	{
		for (int j = 0; j < max_nodes; j++)
		{
			int *v = malloc(sizeof(int));
			*v = 0;
			array_2d_set_value(g->matrix, v, i, j);
		}
	}
	return g;
}

/**
 * graph_is_empty() - Check if a graph is empty, i.e. has no nodes.
 * @g: Graph to check.
 *
 * Returns: True if graph is empty, otherwise false.
 */
bool graph_is_empty(const graph *g)
{
	return (dlist_is_empty(g->nodes));
}

/**
 * graph_insert_node() - Inserts a node with the given name into the graph.
 * @g: Graph to manipulate.
 * @s: Node name.
 *
 * Creates a new node with a copy of the given name and puts it into
 * the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_node(graph *g, const char *s)
{
	if (g->numb_of_nodes >= g->max_nodes)
	{
		fprintf(stderr, "There is no more space for nodes in graph.\n");
		return g;
	}

	//Allocate memory for a new node
	node *n = malloc(sizeof(node));
	dlist_pos pos = dlist_first(g->nodes);

	//Set the nodes name and id
	strcpy(n->name, s);
	n->id = g->numb_of_nodes;
	n->seen = false;

	//Insert the node at the end of the list
	while(!dlist_is_end(g->nodes, pos))
	{
		pos = dlist_next(g->nodes, pos);
	}
	dlist_insert(g->nodes, n, pos);
	g->numb_of_nodes++;

	return g;
}

/**
 * graph_find_node() - Find a node stored in the graph.
 * @g: Graph to manipulate.
 * @s: Node identifier, e.g. a char *.
 *
 * Returns: A pointer to the found node, or NULL.
 */
node *graph_find_node(const graph *g, const char *s)
{

	if (!dlist_is_empty(g->nodes))
	{
		//Iterate over the list of nodes until the node with the desired name is found
		dlist_pos pos = dlist_first(g->nodes);
		while(!dlist_is_end(g->nodes, pos))
		{
			node *temp_node = dlist_inspect(g->nodes, pos);
			if (strcmp(temp_node->name, s) == 0)
			{
				return temp_node;
			}
			pos = dlist_next(g->nodes, pos);
		}
	}

	//A node is not found
	return NULL;
}

/**
 * graph_node_is_seen() - Return the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to return seen status for.
 *
 * Returns: The seen status for the node.
 */
bool graph_node_is_seen(const graph *g, const node *n)
{
	return n->seen;
}

/**
 * graph_node_set_seen() - Set the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to set seen status for.
 * @s: Status to set.
 *
 * Returns: The modified graph.
 */
graph *graph_node_set_seen(graph *g, node *n, bool seen)
{
	n->seen = seen;
	return g;
}

/**
 * graph_reset_seen() - Reset the seen status on all nodes in the graph.
 * @g: Graph to modify.
 *
 * Returns: The modified graph.
 */
graph *graph_reset_seen(graph *g)
{
	//Iterates over the list of nodes and sets all nodes to not seen
	dlist_pos pos = dlist_first(g->nodes);
	while(!dlist_is_end(g->nodes, pos))
	{
		node *n = dlist_inspect(g->nodes, pos);
		n->seen = false;
		pos = dlist_next(g->nodes, pos);
	}
	return g;
}

/**
 * graph_insert_edge() - Insert an edge into the graph.
 * @g: Graph to manipulate.
 * @n1: Source node (pointer) for the edge.
 * @n2: Destination node (pointer) for the edge.
 *
 * NOTE: Undefined unless both nodes are already in the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_edge(graph *g, node *n1, node *n2)
{
	if (n1 == NULL || n2 == NULL)
	{
		fprintf(stderr, "Both nodes must be in graph");
		return g;
	}

	//Sets the value of the correct position in the graphs matrix to one,
	//to represent an edge from node 1 to node 2, and free the memory of the 
	//previous value
	int *v = malloc(sizeof(int));
	*v = 1;
	array_2d_set_value(g->matrix, v, n1->id, n2->id);
	return g;
}

/**
 * graph_neighbours() - Return a list of neighbour nodes.
 * @g: Graph to inspect.
 * @n: Node to get neighbours for.
 *
 * Returns: A pointer to a list of nodes. Note: The list must be
 * dlist_kill()-ed after use.
 */
dlist *graph_neighbours(const graph *g, const node *n)
{
	dlist *neighbours = dlist_empty(NULL);
	dlist_pos n_pos = dlist_first(g->nodes);
	dlist_pos neighbour_pos = dlist_first(neighbours);

	//Iterate over the correct column in the matrix and check if there is an edge 
	//from the given node to the node in that position in the matrix
	for (int i = 0; i < g->numb_of_nodes; i++)
	{
		int *vertice = array_2d_inspect_value(g->matrix, n->id, i);
		int v = *vertice;
		if (v == 1)
		{
			//insert the correct node into the list of neigbours
			dlist_insert(neighbours, dlist_inspect(g->nodes, n_pos), neighbour_pos);
			neighbour_pos = dlist_next(neighbours, neighbour_pos);
		}
		n_pos = dlist_next(g->nodes, n_pos);
	}
	return neighbours;
}

/**
 * graph_kill() - Destroy a given graph.
 * @g: Graph to destroy.
 *
 * Return all dynamic memory used by the graph.
 *
 * Returns: Nothing.
 */
void graph_kill(graph *g)
{
	dlist_kill(g->nodes);	
	array_2d_kill(g->matrix);
	free(g);
}
