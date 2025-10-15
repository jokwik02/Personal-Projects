#ifndef __DLIST_H
#define __DLIST_H

#include <stdbool.h>
#include "util.h"

/*
 * Declaration of a generic, directed list for the "Datastructures
 * and algorithms" courses at the Department of Computing Science,
 * Umea University. The list stores void pointers, so it can be used
 * to store all types of values. After use, the function list_kill
 * must be called to de-allocate the dynamic memory used by the list
 * itself. The de-allocation of any dynamic memory allocated for the
 * element values is the responsibility of the user of the list,
 * unless a free_function is registered in list_empty.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *	    Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *	    Lars Karlsson (larsk@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0 2018-01-28: First public version.
 *   v1.1 2023-01-19: Added dlist_pos_equal and dlist_pos_is_valid functions.
 *   v1.2 2023-01-20: Renamed dlist_pos_equal to dlist_pos_are_equal.
 *   v1.3 2023-03-23: Renamed dlist_pos_are_equal to dlist_pos_is_equal.
 */

// ==========PUBLIC DATA TYPES============

// List type.
typedef struct dlist dlist;

// List position type.
typedef struct cell *dlist_pos;


// ==========DATA STRUCTURE INTERFACE==========

/**
 * dlist_empty() - Create an empty dlist.
 * @free_func: A pointer to a function (or NULL) to be called to
 *	       de-allocate memory on remove/kill.
 *
 * Returns: A pointer to the new list.
 */
dlist *dlist_empty(free_function free_func);

/**
 * dlist_is_empty() - Check if a dlist is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool dlist_is_empty(const dlist *l);

/**
 * dlist_first() - Return the first position of a dlist, i.e. the
 *		   position of the first element in the list.
 * @l: List to inspect.
 *
 * Returns: The first position in the given list.
 */
dlist_pos dlist_first(const dlist *l);

/**
 * dlist_next() - Return the next position in a dlist.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *	    NOTE: The return value is undefined for the last position.
 */
dlist_pos dlist_next(const dlist *l, const dlist_pos p);

/**
 * dlist_is_end() - Check if a given position is at the end of a dlist.
 * @l: List to inspect.
 * @p: Any valid position in the list.
 *
 * Returns: True if p is at the end of the list.
 */
bool dlist_is_end(const dlist *l, const dlist_pos p);

/**
 * dlist_inspect() - Return the value of the element at a given
 *		     position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: Returns the value at the given position as a void pointer.
 *	    NOTE: The return value is undefined for the last position.
 */
void *dlist_inspect(const dlist *l, const dlist_pos p);

/**
 * dlist_insert() - Insert a new element with a given value into a dlist.
 * @l: List to manipulate.
 * @v: Value (pointer) to be inserted into the list.
 * @p: Position in the list before which the value should be inserted.
 *
 * Creates a new element and inserts it into the list before p.
 * Stores data in the new element.
 *
 * Returns: The position of the newly created element.
 */
dlist_pos dlist_insert(dlist *l, void *v, const dlist_pos p);

/**
 * dlist_remove() - Remove an element from a dlist.
 * @l: List to manipulate.
 * @p: Position in the list of the element to remove.
 *
 * Removes the element at position p from the list. If a free_func
 * was registered at list creation, calls it to deallocate the memory
 * held by the element value.
 *
 * Returns: The position after the removed element.
 */
dlist_pos dlist_remove(dlist *l, const dlist_pos p);

/**
 * dlist_kill() - Destroy a given dlist.
 * @l: List to destroy.
 *
 * Return all dynamic memory used by the list and its elements. If a
 * free_func was registered at list creation, also calls it for each
 * element to free any user-allocated memory occupied by the element values.
 *
 * Returns: Nothing.
 */
void dlist_kill(dlist *l);

/**
 * dlist_print() - Iterate over the list element and print their values.
 * @l: List to inspect.
 * @print_func: Function called for each element.
 *
 * Iterates over the list and calls print_func with the value stored
 * in each element.
 *
 * Returns: Nothing.
 */
void dlist_print(const dlist *l, inspect_callback print_func);

/**
 * dlist_pos_is_valid() - Return true for a valid position in a dlist
 * @l: List to inspect.
 * @p: Any position.
 *
 * Returns: True if p is a valid position in the list, otherwise false.
 */
bool dlist_pos_is_valid(const dlist *l, const dlist_pos p);

/**
 * dlist_pos_is_equal() - Return true if two positions in a dlist are equal.
 * @l: List to inspect.
 * @p1: First position to compare.
 * @p2: Second position to compare.
 *
 * Returns: True if p1 and p2 refer to the same position in l, otherwise False.
 *	    NOTE: The result is defined only if p1 and p2 are valid positions in l.
 *          p1 and p2 are assumed to be valid positions in l, no check is performed.
 */
bool dlist_pos_is_equal(const dlist *l, const dlist_pos p1, const dlist_pos p2);

#endif
