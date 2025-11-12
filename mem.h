/**
 * @file mem.h
 * @brief Memory management and object lifecycle functions
 *
 * This module handles memory allocation, reference counting, object creation,
 * and error reporting for the ToyForth interpreter.
 */

#ifndef MEM_H
#define MEM_H
#include <stddef.h>
#include "tf.h"

/* ===================== Memory allocation wrappers =================== */

/**
 * @brief Allocate memory with automatic error handling
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory (never NULL)
 *
 * This function wraps malloc() and automatically exits the program with
 * an error message if allocation fails. This ensures NULL checks are not
 * needed throughout the codebase.
 */
void *xmalloc(size_t size);

/**
 * @brief Reallocate memory with automatic error handling
 * @param ptr Pointer to previously allocated memory (or NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory (never NULL)
 *
 * This function wraps realloc() and automatically exits the program with
 * an error message if reallocation fails.
 */
void *xrealloc(void *ptr, size_t size);

/* ===================== Reference counting =================== */

/**
 * @brief Increment an object's reference count
 * @param o Object to increment (NULL-safe)
 *
 * Call this when a new reference to an object is created (e.g., when
 * storing it in a data structure). Does nothing if o is NULL.
 */
void incRef(tfobj *o);

/**
 * @brief Decrement an object's reference count
 * @param o Object to decrement (NULL-safe)
 *
 * Call this when a reference to an object is no longer needed. When the
 * reference count reaches 0, the object is automatically freed. Does
 * nothing if o is NULL.
 */
void decRef(tfobj *o);

/* ===================== Object creation =================== */

/**
 * @brief Create a new string object
 * @param s Pointer to string data (takes ownership)
 * @param len Length of string in bytes
 * @return New string object with refcount=1
 *
 * The string pointer 's' must be heap-allocated, as it will be freed
 * when the object is destroyed.
 */
tfobj *createStringObject(char *s, size_t len);

/**
 * @brief Create a new integer object
 * @param i Integer value
 * @return New integer object with refcount=1
 */
tfobj *createIntObject(int i);

/**
 * @brief Create a new boolean object
 * @param i Boolean value (0=false, non-zero=true)
 * @return New boolean object with refcount=1
 */
tfobj *createBoolObject(int i);

/**
 * @brief Create a new symbol object (identifier/word)
 * @param s Pointer to symbol string data (takes ownership)
 * @param len Length of symbol string in bytes
 * @return New symbol object with refcount=1
 *
 * Similar to createStringObject, but creates a SYMBOL type instead.
 * The string pointer 's' must be heap-allocated.
 */
tfobj *createSymbolObject(char *s, size_t len);

/**
 * @brief Create a new list object
 * @param capacity Initial capacity (number of elements)
 * @return New empty list object with refcount=1
 *
 * The list starts empty (len=0) but has pre-allocated space for
 * 'capacity' elements. The list grows automatically as needed.
 */
tfobj *createListObject(size_t capacity);

/* ===================== Context management =================== */

/**
 * @brief Create a new execution context with an empty stack
 * @return New context with initialized stack
 *
 * The context must be freed with freeContext() when done.
 */
tfctx *createContext(void);

/**
 * @brief Free an execution context and all objects on its stack
 * @param ctx Context to free
 *
 * This decrements the reference count of all objects still on the stack,
 * then frees the context structure itself.
 */
void freeContext(tfctx *ctx);

/* ===================== Error handling =================== */

/**
 * @brief Report a runtime error and exit the program
 * @param ctx Execution context (for stack depth and source location)
 * @param msg Error message to display
 *
 * This function prints an error message including line/column information
 * (if available from ctx->current_object) and stack depth, then exits
 * the program with status 1.
 */
void runtimeError(tfctx *ctx, const char *msg);

#endif