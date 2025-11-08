#include <stdio.h>
#include <stdlib.h>

#include "primitives.h"
#include "tf.h"
#include "mem.h"
#include "stack.h"

/* ===================== Primitives Operations =================== */

void primitiveAdd(tfctx *ctx) {
    if (ctx->sp < 2) {
      fprintf(stderr, "Stack underflow: '+' requires two values!\n");
      exit(1);
    }
    tfobj *first = stackPop(ctx);
    tfobj *second = stackPop(ctx);
  
    if (first->type != TFOBJ_TYPE_INT || second->type != TFOBJ_TYPE_INT) {
      fprintf(stderr, "The addition requires two integers\n");
      exit(1);
    }
    int result = first->i + second->i;
    tfobj *objResult = createIntObject(result);
  
    stackPush(ctx, objResult);
  
    // We are done with our local references
    decRef(objResult); // Our var ref is gone, ref is on stack now
    decRef(first);
    decRef(second);
  }
  
void primitivePrint(tfctx *ctx) {
    if (ctx->sp < 1) {
        fprintf(stderr, "Stack underflow: '.' requires a value!\n");
        exit(1);
    }
    tfobj *val = stackPop(ctx);
    if (val->type != TFOBJ_TYPE_INT) {
        fprintf(stderr, "Can't print a symbol\n");
        exit(1);
    }
    printf("%d\n", val->i);
    decRef(val);
}

void primitiveDuplicate(tfctx *ctx) {
    if (ctx->sp < 1) {
        fprintf(stderr, "Stack underflow: 'dup' requires a value!\n");
        exit(1);
    }
    tfobj *val = ctx->stack[ctx->sp - 1];
    stackPush(ctx, val);
}