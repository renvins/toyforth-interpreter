/**
 * @file list.c
 * @brief Implementation of dynamic list operations
 *
 * Provides functions for manipulating list objects, including appending
 * elements and automatic capacity management.
 */

#include "list.h"
#include "tf.h"
#include "mem.h"

/* ===================== List Manipulation =================== */

void listAppendObject(tfobj *list, tfobj *o) {
    if (list->list.len >= list->list.capacity) {
        list->list.capacity = list->list.capacity * 2;
        list->list.ele = xrealloc(list->list.ele, sizeof(tfobj *) * list->list.capacity);
    }
    incRef(o);
    list->list.ele[list->list.len] = o;
    list->list.len++;
}