/**
 * @file tf.h
 * @brief Core type definitions and data structures for ToyForth interpreter
 *
 * This header defines the fundamental types used throughout the ToyForth
 * interpreter, including the unified object representation (tfobj), parser
 * state, and execution context.
 */

#ifndef TF_H
#define TF_H

#include <stddef.h>

/* ===================== Data types =================== */

/** @brief Type tag for integer objects */
#define TFOBJ_TYPE_INT 0

/** @brief Type tag for string objects */
#define TFOBJ_TYPE_STR 1

/** @brief Type tag for boolean objects */
#define TFOBJ_TYPE_BOOL 2

/** @brief Type tag for list objects (arrays of objects) */
#define TFOBJ_TYPE_LIST 3

/** @brief Type tag for symbol objects (words/identifiers) */
#define TFOBJ_TYPE_SYMBOL 4

/** @brief Initial capacity for the execution stack */
#define INITIAL_STACK_CAPACITY 256

/* ===================== Data structures =================== */

/**
 * @brief ToyForth object - unified representation for all values
 *
 * This is the core type of the interpreter. All values (integers, symbols,
 * lists, etc.) are represented as tfobj instances. The type field determines
 * which union member is valid.
 *
 * Memory management uses reference counting: when refcount reaches 0, the
 * object is automatically freed.
 *
 * Source location (src_line, src_column) is tracked for error reporting.
 */
typedef struct tfobj {
  int refcount;        /**< Reference count for memory management */
  int type;            /**< Object type (TFOBJ_TYPE_*) */
  int src_line;        /**< Source line number where this object originated */
  int src_column;      /**< Source column number where this object originated */
  union {
    int i;             /**< Integer value (for INT and BOOL types) */
    struct {
      char *ptr;       /**< Pointer to string data (for STR and SYMBOL) */
      size_t len;      /**< Length of string in bytes */
    } str;
    struct {
      struct tfobj **ele;  /**< Array of object pointers (for LIST type) */
      size_t len;          /**< Number of elements currently in list */
      size_t capacity;     /**< Allocated capacity of list */
    } list;
  };
} tfobj;

/**
 * @brief Parser state for reading and tokenizing source code
 *
 * Tracks the current position in the source text and maintains line/column
 * information for error reporting.
 */
typedef struct tfparser {
  char *prg;         /**< Pointer to start of the program text */
  char *p;           /**< Current position in the program text */
  int line;          /**< Current line number (1-indexed) */
  int column;        /**< Current column number (1-indexed) */
} tfparser;

/**
 * @brief Execution context for the ToyForth virtual machine
 *
 * Contains the runtime stack and tracks the currently executing object
 * for error reporting. The stack grows dynamically as needed.
 */
typedef struct tfctx {
  tfobj **stack;           /**< Array of object pointers forming the stack */
  size_t sp;               /**< Stack pointer (index of next free slot) */
  size_t capacity;         /**< Allocated capacity of stack array */
  tfobj *current_object;   /**< Currently executing object (for error context) */
} tfctx;

#endif