#ifndef STACK_H
#define STACK_H
#include "tf.h"

void stackPush(tfctx *ctx, tfobj *o);
tfobj *stackPop(tfctx *ctx);

#endif