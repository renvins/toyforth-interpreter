/**
 * @file stack.c
 * @brief Implementation of stack operations for the VM
 *
 * Provides push and pop operations with automatic stack growth and
 * proper reference count management.
 */

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "tf.h"
#include "mem.h"

/* ===================== Stack Manipulation =================== */

void stackPush(tfctx *ctx, tfobj *o) {
    if (ctx->sp >= ctx->capacity) {
      ctx->capacity = ctx->capacity * 2;
      ctx->stack = xrealloc(ctx->stack, sizeof(tfobj *) * ctx->capacity);
    }
    incRef(o);
    ctx->stack[ctx->sp] = o;
    ctx->sp++;
}

tfobj *stackPop(tfctx *ctx) {
  if (ctx->sp == 0) {
    runtimeError(ctx, "Stack underflow error!");
  }
  ctx->sp--;
  tfobj *popped_item = ctx->stack[ctx->sp];

  return popped_item;
}