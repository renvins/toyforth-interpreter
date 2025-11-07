#include "tf.h"
#include "mem.h"

/* ===================== List Manipulation =================== */

/* Append an object to a tfobjects list.
 * List is resized when the len reaches maximum capacity */
void listAppendObject(tfobj *list, tfobj *o) {
    if (list->list.len >= list->list.capacity) {
        list->list.capacity = list->list.capacity * 2;
        list->list.ele = xrealloc(list->list.ele, sizeof(tfobj *) * list->list.capacity);
    }
    incRef(o);
    list->list.ele[list->list.len] = o;
    list->list.len++;
}