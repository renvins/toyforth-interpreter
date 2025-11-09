#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "tf.h"

void primitiveAdd(tfctx *ctx);
void primitiveSub(tfctx *ctx);
void primitiveDrop(tfctx *ctx);
void primitiveSwap(tfctx *ctx);
void primitivePrint(tfctx *ctx);
void primitiveDuplicate(tfctx *ctx);

#endif