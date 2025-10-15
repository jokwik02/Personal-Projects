#include <stdio.h>
#include "include/graph.h"
#include <string.h>
#include "include/dlist.h"
#include <stdlib.h>
#include <ctype.h>
#include "include/queue.h"

#define SIZE 40
#define SIZE_OF_LINE 81
#define MAX_NODES 50

/*
 * Program that outputs if there is a path between two nodes in a graph.
 * The nodes are read from a file specified by the user as an argument.
 * The program goes through the graph with the width-first-method.
 *
 * Authors: Hinok Zakir Saleh (c22hsh@cs.umu.se)
 * 	    Joakim Wiksten    (c22jwn@cs.umu.se)
 *
 * Based on earlier code by: Niclas Borlin (niclas@cs.umu.se)
 *
 * Version information:
 *   v1.0  2023-03-17: First public version.
 */

// Declaration of functions.
int check_prog_params(int argc, const char *argv[], FILE **in_file_p);
int first_non_white_space(const char *s);
bool line_is_blank(const char *s);
bool line_is_comment(const char *s);
bool find_path(graph *g, node *startnode, node *endnode);
void *create_graph_from_textfile(int argc, const char *argv[]);

int main(int argc, const char *argv[])
{
	graph *g = create_graph_from_textfile(argc, argv);
	
	char *input = malloc(81);
	strcpy(input, "Placeholder");
	char *src_name = malloc(40);
	char *dest_name = malloc(40);

	// runs as long as user does not input "quit".
	while (strcmp(input, "quit\n") != 0)
	{
		printf("Enter origin and destination (quit to exit): ");
		fgets(input, SIZE_OF_LINE, stdin);
		if (strcmp(input, "quit\n") == 0)
		{
			break;
		}

		sscanf(input, "%40s %40s", src_name, dest_name);
		node *src = graph_find_node(g, src_name);
		// Restarts if node1 does not exist in graph.
		if (src == NULL)
		{
			fprintf(stderr, "Could not find node1\n");
			continue;
		}
		// Restarts if node2 does not exist in graph.
		node *dest = graph_find_node(g, dest_name);
		if (dest == NULL)
		{
			fprintf(stderr, "Could not find node2\n");
			continue;
		}
		if (find_path(g, src, dest))
		{
			printf("There is a path from %s to %s.\n", src_name, dest_name);
		}
		else
		{
			printf("There is no path from %s to %s.\n", src_name, dest_name);
		}
		printf("\n");
	}

	printf("Normal exit.");
	free(input);
	free(src_name);
	free(dest_name);
	graph_kill(g);
	return 0;
}

// ===========DEFINITIONS OF FUNCTIONS============

/**
 * create_graph_from_textfile() - Creates a graph from a text file.
 * @argc: The amount of parameters put into the program
 * @argv: The parameters that are put into the program
 *
 * Returns: The created graph, or NULL if something fails.
 *
 */
void *create_graph_from_textfile(int argc, const char *argv[])
{
	graph *g = graph_empty(MAX_NODES);
	FILE *in_file_p = NULL;
	int max_edges;
	int check = check_prog_params(argc, argv, &in_file_p);

	// Only runs if input file can be opened and only two start arguments are found
	if (check == 0)
	{
		char line[SIZE_OF_LINE];
		// Searches for the max edge value.
		while (fgets(line, SIZE_OF_LINE, in_file_p) != NULL)
		{
			// Ignore blank lines and comment lines.
			if (line_is_blank(line) || line_is_comment(line))
			{
				continue;
			}

			sscanf(line, "%d", &max_edges);
			break;
		}
		int num_edges = 0;
		while (fgets(line, SIZE_OF_LINE, in_file_p) != NULL)
		{
			// Ignore blank lines and comment lines.
			if (line_is_blank(line) || line_is_comment(line))
			{
				continue;
			}
			char *label1, *label2;
			label1 = malloc(40);
			label2 = malloc(40);
			// Reads the nodes
			sscanf(line, "%40s %40s", label1, label2);
			if (label1 != NULL && label2 != NULL)
			{
				// Checks if the nodes already are in the graph before it adds.
				if (!graph_is_empty(g))
				{
					if (graph_find_node(g, label1) == NULL)
					{
						g = graph_insert_node(g, label1);
					}
					if (graph_find_node(g, label2) == NULL)
					{
						g = graph_insert_node(g, label2);
					}
				}
				else
				{
					g = graph_insert_node(g, label1);
					g = graph_insert_node(g, label2);
				}

				// Adds edge from node 1 to node 2
				struct node *n1 = graph_find_node(g, label1);
				struct node *n2 = graph_find_node(g, label2);
				g = graph_insert_edge(g, n1, n2);
				num_edges++;
			}
			free(label1);
			free(label2);
		}

		fclose(in_file_p);
		if(num_edges != max_edges)
		{
			fprintf(stderr, "Amount of edges does not match the given amount in text file");
			graph_kill(g);
			exit(EXIT_FAILURE);
		}

		return g;
	}
	graph_kill(g);
	exit(EXIT_FAILURE);
}
/**
 * first_non_white_space() - Checks what the position is of the first non-white-space character.
 * @s: char pointer to be checked.
 *
 * Returns: Position of first non-white-space char, or -1 if it fails.
 *
 */
int first_non_white_space(const char *s)
{
	// Start at first char.
	int i = 0;
	// Advance until we hit EOL as long as we're loooking at white-space.
	while (s[i] && isspace(s[i]))
	{
		i++;
	}
	if (s[i])
	{
		// Return position of found a non-white-space char.
		return i;
	}
	else
	{
		// Return fail.
		return -1;
	}
}

/**
 * line_is_blank() - Checks if a line is blank.
 * @s: line to be checked.
 *
 * Returns: True if it the line is blank, otherwise false.
 *
 */
bool line_is_blank(const char *s)
{
	// Line is blank if it only contained white-space chars.
	return first_non_white_space(s) < 0;
}

/**
 * line_is_comment() - Checks if a line is a comment.
 * @s: line to be checked.
 *
 * Returns: True if it the line is a comment, otherwise false.
 *
 */
bool line_is_comment(const char *s)
{
	int i = first_non_white_space(s);
	return (i >= 0 && s[i] == '#');
}

/**
 * find_path() - Checks there is a path from one node to another.
 * @g: Graph containing the nodes.
 * @src: The source node
 * @dest: The destination node
 *
 * Returns: True if there is a path from the source node to the destination node, otherwise false.
 *
 */
bool find_path(graph *g, node *src, node *dest)
{

	if (nodes_are_equal(src, dest))
	{
		return true;
	}

	// Create an empty queue and enqueue the source node
	queue *q = queue_empty(NULL);

	q = queue_enqueue(q, src);

	while (!queue_is_empty(q))
	{
		// Take the node from the front of the queue, set it at seen and get its neighbours
		node *n = queue_front(q);
		q = queue_dequeue(q);

		graph_node_set_seen(g, n, true);

		dlist *neighbours = graph_neighbours(g, n);
		dlist_pos pos = dlist_first(neighbours);

		// Iterate over the list of neighbours
		while (!dlist_is_end(neighbours, pos))
		{
			node *n2 = dlist_inspect(neighbours, pos);

			// Check if the node has already been seen
			if (!graph_node_is_seen(g, n2))
			{
				// If the neighbour node is the same as the destination node, return true
				if (nodes_are_equal(n2, dest))
				{
					dlist_kill(neighbours);
					queue_kill(q);
					g = graph_reset_seen(g);
					return true;
				}
				// If not, set it as seen and put it in the queue
				g = graph_node_set_seen(g, n2, true);
				q = queue_enqueue(q, n2);
			}

			// Check the next neighbour
			pos = dlist_next(neighbours, pos);
		}

		// Check the next node in the queue
		dlist_kill(neighbours);
	}

	// No path was found, return false
	queue_kill(q);
	g = graph_reset_seen(g);
	return false;
}

/**
 * check_prog_params() - Checks whether the parameters for the program are correct.
 * @argc: The amount of parameters put into the program
 * @argv: The parameters that are put into the program
 * @in_file_p: The varabel to hold the input file
 *
 * Returns: Zero if all the paramaters are correct, One if they are not.
 *
 */
int check_prog_params(int argc, const char *argv[], FILE **in_file_p)
{

	int res;
	if (argc > 2 || argc < 2)
	{
		fprintf(stderr, "Usage: %s <input configuration file>", argv[0]);
		res = 1;
		return res;
	}
	*in_file_p = fopen(argv[1], "r");

	if (*in_file_p == NULL)
	{
		fprintf(stderr, "Could not open the file: %s\n", argv[1]);
		res = 1;
		return res;
	}

	return 0;
}