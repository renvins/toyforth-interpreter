#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "tf.h"

/* ===================== De/Allocation wrappers =================== */

static void freeObject(tfobj *o);

/* Returns a pointer to the start of allocated memory
and it automatically handles the out of memory error */
void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
      fprintf(stderr, "Out of memory allocating %zu bytes\n", size);
      exit(1);
    }
    return ptr;
}
  
/* Returns a pointer to the start of reallocated memory
 * and it automatically handles the out of memory error */
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
  
  /* ===================== Object related functions =================== */
  
/* Allocate and init a new ToyForth object. */
static tfobj *createObject(int type) {
    tfobj *o = xmalloc(sizeof(tfobj));
    o->type = type;
    o->refcount = 1;
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
  
  /* Creates the needed context (stack) for the environment */
tfctx *createContext() {
    tfctx *ctx = xmalloc(sizeof(tfctx));

    ctx->sp = 0;
    ctx->capacity = INITIAL_STACK_CAPACITY;
    ctx->stack = xmalloc(sizeof(tfobj *) * ctx->capacity);

    return ctx;
}
  
/* Completely frees the context of running environment */
void freeContext(tfctx *ctx) {
    // decRef all the objects still on the stack
    for (size_t i = 0; i < ctx->sp; i++) {
        decRef(ctx->stack[i]);
    }
    free(ctx->stack);
    free(ctx);
}