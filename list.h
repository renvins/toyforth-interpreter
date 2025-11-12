/**
 * @file list.h
 * @brief Dynamic list manipulation functions
 *
 * Provides operations for working with list objects (TFOBJ_TYPE_LIST).
 */

#ifndef LIST_H
#define LIST_H
#include "tf.h"

/**
 * @brief Append an object to a list
 * @param list The list object to append to (must be TFOBJ_TYPE_LIST)
 * @param o Object to append (its reference count is incremented)
 *
 * Adds the object to the end of the list, incrementing its reference count.
 * If the list is at capacity, it automatically doubles in size. This
 * function never fails (it exits on out-of-memory).
 */
void listAppendObject(tfobj *list, tfobj *o);

#endif