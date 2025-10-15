#include <stdio.h>
#include <stdlib.h>

#include <list.h>

/*
 * Implementation of a generic, undirected list for the
 * "Datastructures and algorithms" courses at the Department of
 * Computing Science, Umea University.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *	    Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *	    Lars Karlsson (larsk@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0 2018-01-28: First public version.
 *   v1.1 2023-01-19: Added list_pos_equal and list_pos_is_valid functions.
 *   v1.2 2023-01-20: Renamed list_pos_equal to list_pos_are_equal.
 *   v1.3 2023-02-25: Renamed list header fields head and tail.
 *   v1.4 2023-03-23: Renamed list_pos_are_equal to list_pos_is_equal.
 */

// ===========INTERNAL DATA TYPES============

/*
 * The list elements are implemented as two-cells with forward and
 * backward links and a void * for the value. The list uses two border
 * cells at the start and end of the list.
 */
struct cell {
	struct cell *next;
	struct cell *previous;
	void *value;
};

struct list {
	struct cell *head;
	struct cell *tail;
	free_function free_func;
};

/*
 * Data structure interface
 */

/**
 * list_empty() - Create an empty list.
 * @free_func: A pointer to a function (or NULL) to be called to
 *	       de-allocate memory on remove/kill.
 *
 * Returns: A pointer to the new list.
 */
list *list_empty(free_function free_func)
{
	// Allocate memory for the list head.
	list *l = calloc(1, sizeof(list));

	// Allocate memory for the border cells.
	l->head = calloc(1, sizeof(struct cell));
	l->tail = calloc(1, sizeof(struct cell));

	// Set consistent links between border elements.
	l->head->next = l->tail;
	l->tail->previous = l->head;

	// Store the free function.
	l->free_func = free_func;

	return l;
}

/**
 * list_is_empty() - Check if a list is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool list_is_empty(const list * l)
{
	// List is empty if there are no cells between head and tail.
	return (l->head->next == l->tail);
}

/**
 * list_first() - Return the first position of a list, i.e. the
 *		  position of the first element in the list.
 * @l: List to inspect.
 *
 * Returns: The first position in the given list.
 */
list_pos list_first(const list * l)
{
	// First position is position of first element.
	return l->head->next;
}

/**
 * list_end() - Return the last position of a list, i.e. the position
 *		after the last element in the list.
 * @l: List to inspect.
 *
 * Returns: The last position in the given list.
 */
list_pos list_end(const list * l)
{
	// Last position is position *after* last element.
	return l->tail;
}

/**
 * list_next() - Return the next position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *	    NOTE: The return value is undefined for the last position.
 */
list_pos list_next(const list * l, const list_pos p)
{
	if (list_pos_is_equal(l, p, list_end(l))) {
		// This should really throw an error.
		fprintf(stderr,"list_next: Warning: Trying to navigate "
			"past end of list!");
	}
	return p->next;
}

/**
 * list_previous() - Return the previous position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the first in the list.
 *
 * Returns: The position in the list before the given position.
 *	    NOTE: The return value is undefined for the first position.
 */
list_pos list_previous(const list * l, const list_pos p)
{
	if (list_pos_is_equal(l, p, list_first(l))) {
		// This should really throw an error.
		fprintf(stderr,"list_previous: Warning: Trying to navigate "
			"past beginning of list!\n");
	}
	return p->previous;
}

/**
 * list_inspect() - Return the value of the element at a given
 *		    position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: Returns the value at the given position as a void pointer.
 *	    NOTE: The return value is undefined for the last position.
 */
void *list_inspect(const list * l, const list_pos p)
{
	if (list_pos_is_equal(l, p, list_end(l))) {
		// This should really throw an error.
		fprintf(stderr,"list_inspect: Warning: Trying to inspect "
			"position at end of list!\n");
	}
	return p->value;
}

/**
 * list_insert() - Insert a new element with a given value into a list.
 * @l: List to manipulate.
 * @v: Value (pointer) to be inserted into the list.
 * @p: Position in the list before which the value should be inserted.
 *
 * Creates a new element and inserts it into the list before p.
 * Stores data in the new element.
 *
 * Returns: The position of the newly created element.
 */
list_pos list_insert(list * l, void *v, const list_pos p)
{
	// Allocate memory for a new cell.
	list_pos elem = malloc(sizeof(struct cell));

	// Store the value.
	elem->value = v;
	// Add links to/from the new cell.
	elem->next = p;
	elem->previous = p->previous;
	p->previous = elem;
	elem->previous->next = elem;

	// Return the position of the new cell.
	return elem;
}

/**
 * list_remove() - Remove an element from a list.
 * @l: List to manipulate.
 * @p: Position in the list of the element to remove.
 *
 * Removes the element at position p from the list. If a free_func
 * was registered at list creation, calls it to deallocate the memory
 * held by the element value.
 *
 * Returns: The position after the removed element.
 */
list_pos list_remove(list *l, const list_pos p)
{
	// Remember return position.
	list_pos next_pos = p->next;
	// Link past this element.
	p->previous->next = p->next;
	p->next->previous = p->previous;

	// Call free_func if registered.
	if (l->free_func != NULL) {
		// Free any user-allocated memory for the value.
		l->free_func(p->value);
	}
	// Free the memory allocated to the cell itself.
	free(p);
	// Return the position of the next element.
	return next_pos;
}

/**
 * list_kill() - Destroy a given list.
 * @l: List to destroy.
 *
 * Returns all dynamic memory used by the list and its elements. If a
 * free_func was registered at list creation, also calls it for each
 * element to free any user-allocated memory occupied by the element values.
 *
 * Returns: Nothing.
 */
void list_kill(list * l)
{
	// Use public functions to traverse the list.

	// Start with the first element (will be defined even for an
	// empty list).
	list_pos p = list_first(l);

	// Remove first element until list is empty.
	while (!list_is_empty(l)) {
		p = list_remove(l, p);
	}

	// Free border elements and the list head.
	free(l->head);
	free(l->tail);
	free(l);
}

/**
 * list_print() - Iterate over the list element and print their values.
 * @l: List to inspect.
 * @print_func: Function called for each element.
 *
 * Iterates over the list and calls print_func with the value stored
 * in each element.
 *
 * Returns: Nothing.
 */
void list_print(const list * l, inspect_callback print_func)
{
	// Start at the beginning of the list.
	list_pos p = list_first(l);

	printf("( ");
	while (!list_pos_is_equal(l, p, list_end(l))) {
		// Call print_func with the element value at the
		// current position.
		print_func(list_inspect(l, p));
		// Advance to next position.
		p = list_next(l, p);
		// Print separator unless at element.
		if (!list_pos_is_equal(l, p, list_end(l))) {
			printf(", ");
		}
	}
	printf(" )\n");
}

/**
 * list_pos_is_equal() - Return true if two positions in a list are equal.
 * @l: List to inspect.
 * @p1: First position to compare.
 * @p2: Second position to compare.
 *
 * Returns: True if p1 and p2 refer to the same position in l, otherwise False.
 *	    NOTE: The result is defined only if p1 and p2 are valid positions in l.
 */
bool list_pos_is_equal(const list *l, const list_pos p1, const list_pos p2)
{
	// Since we don't need to check whether p1 or p2 are valid, we
	// only need to compare them directly.
	return p1 == p2;
}

/**
 * list_pos_is_valid() - Return true for a valid position in a list
 * @l: List to inspect.
 * @p: Any position.
 *
 * Returns: True if p is a valid position in the list, otherwise false.
 */
bool list_pos_is_valid(const list *l, const list_pos p)
{
	// Iterate over all positions in l.
	list_pos q = list_first(l);
	while (!list_pos_is_equal(l, q, list_end(l))) {
		if (list_pos_is_equal(l, p, q)) {
			// We found the position in the list.
			return true;
		}
		// Advance to the next valid position,
		q = list_next(l, q);
	}
	// p was not among valid positions in l.
	return false;
}
