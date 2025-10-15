#ifndef __UTIL_H
#define __UTIL_H

/*
 * Utility function types for deallocating, printing and comparing
 * values stored by various data types.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *	    Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0 2018-01-28: First public version.
 *   v1.1 2018-02-06: Updated explanation for the compare_function.
 *   v1.2 2023-01-14: Added version DEFINE constants and strings.
 */

// Macros to create a version string out of version constants
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Version constants
#define CODE_BASE_MAJOR_VERSION 1
#define CODE_BASE_MINOR_VERSION 0
#define CODE_BASE_REVISION 14
#define CODE_BASE_PATCH 2

// Create a short version string
#define CODE_BASE_VERSION "v" \
    STR(CODE_BASE_MAJOR_VERSION) \
    "." \
    STR(CODE_BASE_MINOR_VERSION) \
    "." \
    STR(CODE_BASE_REVISION) \
    "." \
    STR(CODE_BASE_PATCH)

// Create a version string
#define CODE_BASE_LONG_VERSION "Version: " \
    STR(CODE_BASE_MAJOR_VERSION) \
    "." \
    STR(CODE_BASE_MINOR_VERSION) \
    "." \
    STR(CODE_BASE_REVISION) \
    "." \
    STR(CODE_BASE_PATCH)

// Type definition for de-allocator function, e.g. free().
typedef void (*free_function)(void *);

// Type definition for read-only callback for single-value containers,
// used by e.g. print functions.
typedef void (*inspect_callback)(const void *);

// Ditto for dual-value containers.
typedef void (*inspect_callback_pair)(const void *, const void *);

// Type definition for comparison function, used by e.g. table.
//
// Comparison functions should return values that indicate the order
// of the arguments. If the first argument is considered less/lower
// than the second, a negative value should be returned. If the first
// argument is considered more/higher than the second, a positive value
// should be returned. If the arguments are considered equal, a zero
// value should be returned.
typedef int compare_function(const void *,const void *);

#endif
