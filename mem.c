/**
 * @file mem.c
 * @brief Implementation of memory management and object lifecycle
 *
 * This module provides the core memory management functionality for ToyForth,
 * including safe allocation wrappers, reference counting, and object creation.
 */

#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "tf.h"

/* ===================== De/Allocation wrappers =================== */

/**
 * @brief Internal helper to free an object and its resources
 * @param o Object to free
 *
 * This static function is called when an object's refcount reaches 0.
 * It frees any owned resources (strings, list elements) and then frees
 * the object itself.
 */
static void freeObject(tfobj *o);

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
      fprintf(stderr, "Out of memory allocating %zu bytes\n", size);
      exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
      fprintf(stderr, "Out of memory reallocating %zu bytes\n", size);
      exit(1);
    }
    return ptr;
}

void incRef(tfobj *o) {
    if (o == NULL)
        return;
    o->refcount++;
}
  
void decRef(tfobj *o) {
    if (o == NULL)
        return;

    o->refcount--;
    if (o->refcount == 0) {
        freeObject(o);
    }
}
  
static void freeObject(tfobj *o) {
    if (o == NULL) {
        return;
    }

    if (o->type == TFOBJ_TYPE_STR || o->type == TFOBJ_TYPE_SYMBOL) {
        free(o->str.ptr);
    } else if (o->type == TFOBJ_TYPE_LIST) {
        for (size_t i = 0; i < o->list.len; i++) {
        decRef(o->list.ele[i]);
        }
        free(o->list.ele);
    }
    free(o);
}
  
/* ===================== Object creation =================== */

/**
 * @brief Internal helper to allocate and initialize a base object
 * @param type Object type (TFOBJ_TYPE_*)
 * @return Newly allocated object with refcount=1
 *
 * This static function is used by all the createXxxObject functions to
 * allocate and initialize the common fields of a tfobj.
 */
static tfobj *createObject(int type) {
    tfobj *o = xmalloc(sizeof(tfobj));
    o->type = type;
    o->refcount = 1;
    o->src_line = 0;
    o->src_column = 0;
    return o;
}

tfobj *createStringObject(char *s, size_t len) {
    tfobj *o = createObject(TFOBJ_TYPE_STR);
    o->str.ptr = s;
    o->str.len = len;
    return o;
}

tfobj *createIntObject(int i) {
    tfobj *o = createObject(TFOBJ_TYPE_INT);
    o->i = i;
    return o;
}

tfobj *createBoolObject(int i) {
    tfobj *o = createObject(TFOBJ_TYPE_BOOL);
    o->i = i;
    return o;
}

tfobj *createSymbolObject(char *s, size_t len) {
    tfobj *o = createStringObject(s, len);
    o->type = TFOBJ_TYPE_SYMBOL;
    return o;
}

tfobj *createListObject(size_t capacity) {
    tfobj *o = createObject(TFOBJ_TYPE_LIST);
    o->list.capacity = capacity;
    o->list.len = 0;
    o->list.ele = xmalloc(sizeof(tfobj *) * o->list.capacity);

    return o;
}

/* ===================== Context management =================== */

tfctx *createContext() {
    tfctx *ctx = xmalloc(sizeof(tfctx));

    ctx->sp = 0;
    ctx->capacity = INITIAL_STACK_CAPACITY;
    ctx->stack = xmalloc(sizeof(tfobj *) * ctx->capacity);
    ctx->current_object = NULL;

    return ctx;
}

void freeContext(tfctx *ctx) {
    // decRef all the objects still on the stack
    for (size_t i = 0; i < ctx->sp; i++) {
        decRef(ctx->stack[i]);
    }
    free(ctx->stack);
    free(ctx);
}

void runtimeError(tfctx *ctx, const char *msg) {
    fprintf(stderr, "Runtime error");
    if (ctx->current_object && ctx->current_object->src_line > 0) {
        fprintf(stderr, " at line %d, column %d",
            ctx->current_object->src_line,
            ctx->current_object->src_column);
    }
    fprintf(stderr, ": %s\n", msg);
    fprintf(stderr, "Stack depth: %zu\n", ctx->sp);
    exit(1);
}