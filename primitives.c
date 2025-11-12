#include <stdio.h>
#include <stdlib.h>

#include "primitives.h"
#include "tf.h"
#include "mem.h"
#include "stack.h"

/* ===================== Primitives Operations =================== */

void primitiveAdd(tfctx *ctx) {
    if (ctx->sp < 2) {
      runtimeError(ctx, "Stack underflow: '+' requires two values!");
    }
    tfobj *first = stackPop(ctx);
    tfobj *second = stackPop(ctx);
  
    if (first->type != TFOBJ_TYPE_INT || second->type != TFOBJ_TYPE_INT) {
      runtimeError(ctx,"The addition requires two integers");
    }
    int result = first->i + second->i;
    tfobj *objResult = createIntObject(result);
  
    stackPush(ctx, objResult);
  
    // We are done with our local references
    decRef(objResult); // Our var ref is gone, ref is on stack now
    decRef(first);
    decRef(second);
}

void primitiveSub(tfctx *ctx) {
  if (ctx->sp < 2) {
    runtimeError(ctx, "Stack underflow: '-' requires two values!");
  }
  tfobj *a = stackPop(ctx);
  tfobj *b = stackPop(ctx);
  if (a->type != TFOBJ_TYPE_INT || b->type != TFOBJ_TYPE_INT) {
    runtimeError(ctx,"The subtraction requires two integers");
  }
  int result = (b->i) - (a->i);
  tfobj *resObject = createIntObject(result);
  
  stackPush(ctx, resObject);
  decRef(resObject);
  decRef(a);
  decRef(b);
}

void primitiveDrop(tfctx *ctx) {
  if (ctx->sp < 1) {
    runtimeError(ctx, "Stack underflow: 'drop' requires one value!");
  }
  tfobj *popped = stackPop(ctx);
  decRef(popped);
}

void primitiveSwap(tfctx *ctx) {
  if (ctx->sp < 2) {
    runtimeError(ctx,"Stack underflow: 'swap' requires two values!");
  }
  tfobj *a = ctx->stack[ctx->sp - 1];
  tfobj *b = ctx->stack[ctx->sp - 2];

  ctx->stack[ctx->sp - 1] = b;
  ctx->stack[ctx->sp - 2] = a;
}

void primitivePrint(tfctx *ctx) {
  if (ctx->sp < 1) {
      runtimeError(ctx,"Stack underflow: '.' requires a value!");
  }
  tfobj *val = stackPop(ctx);
  if (val->type != TFOBJ_TYPE_INT) {
      runtimeError(ctx, "Can't print a symbol!");
  }
  printf("%d\n", val->i);
  decRef(val);
}

void primitiveDuplicate(tfctx *ctx) {
    if (ctx->sp < 1) {
        runtimeError(ctx,"Stack underflow: 'dup' requires a value!");
    }
    tfobj *val = ctx->stack[ctx->sp - 1];
    stackPush(ctx, val);
}