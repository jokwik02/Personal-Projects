#include <stdio.h>
#include <stdlib.h>

#include <array_2d.h>

/*
 * Implementation of a generic 2D array for the "Datastructures and
 * algorithms" courses at the Department of Computing Science, Umea
 * University.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0  2018-01-28: First public version.
 *   v1.1  2018-04-03: Moved freehandler to last in create parameter list.
 */

// ===========INTERNAL DATA TYPES============

struct array_2d {
	int low[2]; // Low index limits.
	int high[2]; // High index limits.
	int array_size; // Number of array elements.
	void **values; // Pointer to where the actual values are stored.
	free_function free_func;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * array_2d_create() - Create an array without values.
 * @lo1: low index limit for first dimension.
 * @hi1: high index limit for first dimension.
 * @lo2: low index limit for second dimension.
 * @hi2: high index limit for second dimension.
 * @free_func: A pointer to a function (or NULL) to be called to
 *	       de-allocate memory on remove/kill.
 *
 * The index limits are inclusive, i.e. all indices i such that low <=
 * i <= high are defined.
 *
 * Returns: A pointer to the new array, or NULL if not enough memory
 * was available.
 */
array_2d *array_2d_create(int lo1, int hi1, int lo2, int hi2,
			  free_function free_func)
{
	// Allocate array structure.
	array_2d *a=calloc(1, sizeof(*a));
	// Store index limit.
	a->low[0]=lo1;
	a->low[1]=lo2;
	a->high[0]=hi1;
	a->high[1]=hi2;

	// Number of elements.
	a->array_size = (hi1-lo1+1)*(hi2-lo2+1);

	// Store free function.
	a->free_func=free_func;

	a->values=calloc(a->array_size, sizeof(void *));

	// Check whether the allocation succeeded.
	if (a->values == NULL) {
		free(a);
		a=NULL;
	}
	return a;
}

/**
 * array_2d_low() - Return the low index limit for the array.
 * @a: array to inspect.
 * @d: dimension number, 1 or 2.
 *
 * Returns: The low index limit for dimension number d.
 */
int array_2d_low(const array_2d *a, int d)
{
	return a->low[d-1];
}

/**
 * array_2d_high() - Return the high index limit for the array.
 * @a: array to inspect.
 * @d: dimension number, 1 or 2.
 *
 * Returns: The high index limit for dimension number d.
 */
int array_2d_high(const array_2d *a, int d)
{
	return a->high[d-1];
}

/**
 * array_2d_linear_index() - Internal function to compute linear index from list
 *			  of indices.
 * @a: array to inspect.
 * @i: First index.
 * @j: Second index.
 *
 * Returns: The linear index corresponding to the list of indices.
 *	    NOTE: The result is undefined if the number of arguments
 *	    or index value are out of bounds.
 */
static int array_2d_linear_index(const array_2d *a,int i, int j)
{
	int rows=a->high[0]-a->low[0]+1;
	int ix=(i-a->low[0])+(j-a->low[1])*rows;
	//fprintf(stderr,"(%d,%d) -> %d\n",i,j,ix);
	return ix;
}

/**
 * array_2d_inspect_value() - Inspect a value at a given array position.
 * @a: array to inspect.
 * @i: First index of position to inspect.
 * @j: Second index of position to inspect.
 *
 * Returns: The element value at the specified position. The result is
 *	    undefined if no value are stored at that position.
 */
void *array_2d_inspect_value(const array_2d *a, int i, int j)
{
	int ix=array_2d_linear_index(a, i, j);
	// Return the value.
	return a->values[ix];
}

/**
 * array_2d_has_value() - Check if a value is set at a given array position.
 * @a: array to inspect.
 * @i: First index of position to inspect.
 * @j: Second index of position to inspect.
 *
 * Returns: True if a value is set at the specified position, otherwise false.
 */
bool array_2d_has_value(const array_2d *a, int i, int j)
{
	int ix=array_2d_linear_index(a, i, j);
	// Return true if the value is not NULL.
	return a->values[ix] != NULL;
}

/**
 * array_2d_set_value() - Set a value at a given array position.
 * @a: array to modify.
 * @v: value to set element to, or NULL to clear value.
 * @i: First index of position to modify.
 * @j: Second index of position to modify.
 *
 * If the old element value is non-NULL, calls free_func if it was
 * specified at array creation.
 *
 * Returns: Nothing.
 */
void array_2d_set_value(array_2d *a, void *v, int i, int j)
{
	int ix=array_2d_linear_index(a, i, j);
	// Call free_func if specified and old element value was non-NULL.
	if (a->free_func != NULL && a->values[ix] != NULL) {
		a->free_func( a->values[ix] );
	}
	// Set value.
	a->values[ix]=v;
}

/**
 * array_2d_kill() - Return memory allocated by array.
 * @a: array to kill.
 *
 * Iterates over all elements. If free_func was specified at array
 * creation, calls it for every non-NULL element value.
 *
 * Returns: Nothing.
 */
void array_2d_kill(array_2d *a)
{
	if (a->free_func) {
		// Return user-allocated memory for each non-NULL element.
		for (int i=0; i<a->array_size; i++) {
			if (a->values[i] != NULL) {
				a->free_func(a->values[i]);
			}
		}
	}
	// Free actual storage.
	free(a->values);
	// Free array structure.
	free(a);
}

/**
 * array_2d_print() - Iterate over the array element and print their values.
 * @a: Array to inspect.
 * @print_func: Function called for each non-NULL element.
 *
 * Iterates over each position in the array. Calls print_func for each
 * non-NULL value.
 *
 * Returns: Nothing.
 */
void array_2d_print(const array_2d *a, inspect_callback print_func)
{
	printf("[\n");
	for (int i=array_2d_low(a,1); i<=array_2d_high(a,1); i++) {
		printf(" [ ");
		for (int j=array_2d_low(a,2); j<=array_2d_high(a,2); j++) {
			if (array_2d_has_value(a,i,j)) {
				printf("[");
				print_func(array_2d_inspect_value(a,i,j));
				printf("]");
			} else {
				printf("[  ]");
			}
			if (j<array_2d_high(a,2)) {
				printf(", ");
			}
		}
		printf(" ]\n");
	}
	printf(" ]\n");
}
