#ifndef DICT_H
#define DICT_H
#include "tf.h"

/* We define a function pointer because primitive
 * functions will all have the same structure.
 * The context is the running environment context */
typedef void (*WordFn)(tfctx *ctx);
WordFn lookupPrimitive(const char *name);

#endif 