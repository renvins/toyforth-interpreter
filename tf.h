#ifndef TF_H
#define TF_H

#include <stddef.h>

/* ===================== Data types =================== */

#define TFOBJ_TYPE_INT 0
#define TFOBJ_TYPE_STR 1
#define TFOBJ_TYPE_BOOL 2
#define TFOBJ_TYPE_LIST 3
#define TFOBJ_TYPE_SYMBOL 4

#define INITIAL_STACK_CAPACITY 256

/* ===================== Data structures =================== */

/* ToyForth object: it's the base object used for 
*  all the operations in our interpreter. */
typedef struct tfobj {
  int refcount;
  int type; // TFOBJ_TYPE_*
  union {
    int i;
    struct {
      char *ptr;
      size_t len;
    } str;
    struct {
      struct tfobj **ele;
      size_t len;
      size_t capacity;
    } list;
  };
} tfobj;

/* Parser used to read the program from a file */
typedef struct tfparser {
  char *prg; // pointer to start of the program
  char *p; // pointer to program
} tfparser;

/* Context of the running environment. We used a stack
 * based structure implemented by a list of pointers */
typedef struct tfctx {
  tfobj **stack;
  size_t sp;       // index of next free slot
  size_t capacity; // the maximum capacity of the stack
} tfctx;

#endif