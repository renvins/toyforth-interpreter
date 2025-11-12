/**
 * @file dict.h
 * @brief Primitive word dictionary and lookup
 *
 * Maps symbol names to their corresponding C implementation functions.
 * All primitives have a uniform signature for easy dispatch.
 */

#ifndef DICT_H
#define DICT_H
#include "tf.h"

/**
 * @brief Function pointer type for primitive word implementations
 *
 * All primitive words are C functions with this signature. They receive
 * the execution context and can manipulate the stack, create objects, etc.
 * Primitives are responsible for type checking and error handling.
 */
typedef void (*WordFn)(tfctx *ctx);

/**
 * @brief Look up a primitive word by name
 * @param name Symbol name to look up (null-terminated string)
 * @return Function pointer to the primitive, or NULL if not found
 *
 * Performs a linear search through the primitive table. This is O(n) but
 * acceptable for small numbers of primitives (dozens). Returns NULL if
 * the word is not defined.
 */
WordFn lookupPrimitive(const char *name);

#endif 