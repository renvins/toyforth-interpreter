/**
 * @file stack.h
 * @brief Stack manipulation functions for the VM
 *
 * Provides push and pop operations for the execution stack. The stack
 * automatically grows as needed.
 */

#ifndef STACK_H
#define STACK_H
#include "tf.h"

/**
 * @brief Push an object onto the execution stack
 * @param ctx Execution context containing the stack
 * @param o Object to push (its reference count is incremented)
 *
 * The stack takes ownership of a reference to the object. If the stack
 * is full, it automatically doubles its capacity. This function never
 * fails (it exits on out-of-memory).
 */
void stackPush(tfctx *ctx, tfobj *o);

/**
 * @brief Pop an object from the execution stack
 * @param ctx Execution context containing the stack
 * @return The popped object (caller is responsible for decRef)
 *
 * Returns the top object from the stack without decrementing its reference
 * count. The caller must call decRef() when done with the object. Exits
 * with an error if the stack is empty.
 */
tfobj *stackPop(tfctx *ctx);

#endif