/**
 * @file primitives.h
 * @brief Built-in primitive word implementations
 *
 * This module contains the C implementations of all built-in ToyForth
 * words (primitives). Each primitive manipulates the execution stack
 * and performs type checking.
 */

#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "tf.h"

/**
 * @brief Add two integers ( a b -- sum )
 * @param ctx Execution context
 *
 * Pops two integers from the stack, adds them, and pushes the result.
 * Exits with an error if the stack has fewer than 2 values or if either
 * value is not an integer.
 */
void primitiveAdd(tfctx *ctx);

/**
 * @brief Subtract two integers ( a b -- a-b )
 * @param ctx Execution context
 *
 * Pops two integers from the stack (b then a), computes a-b, and pushes
 * the result. Exits with an error if the stack has fewer than 2 values
 * or if either value is not an integer.
 */
void primitiveSub(tfctx *ctx);

/**
 * @brief Discard the top stack value ( a -- )
 * @param ctx Execution context
 *
 * Pops and discards the top value from the stack. Exits with an error
 * if the stack is empty.
 */
void primitiveDrop(tfctx *ctx);

/**
 * @brief Swap the top two stack values ( a b -- b a )
 * @param ctx Execution context
 *
 * Exchanges the positions of the top two values on the stack. Exits with
 * an error if the stack has fewer than 2 values.
 */
void primitiveSwap(tfctx *ctx);

/**
 * @brief Pop and print an integer ( n -- )
 * @param ctx Execution context
 *
 * Pops an integer from the stack and prints it to stdout followed by a
 * newline. Exits with an error if the stack is empty or if the top value
 * is not an integer.
 */
void primitivePrint(tfctx *ctx);

/**
 * @brief Duplicate the top stack value ( a -- a a )
 * @param ctx Execution context
 *
 * Pushes a copy of the top stack value onto the stack. Exits with an
 * error if the stack is empty.
 */
void primitiveDuplicate(tfctx *ctx);

#endif