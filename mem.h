#ifndef MEM_H
#define MEM_H
#include <stddef.h>
#include "tf.h"

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

void incRef(tfobj *o);
void decRef(tfobj *o);

tfobj *createStringObject(char *s, size_t len);
tfobj *createIntObject(int i);
tfobj *createBoolObject(int i);
tfobj *createSymbolObject(char *s, size_t len);
tfobj *createListObject(size_t capacity);

tfctx *createContext(void);
void freeContext(tfctx *ctx);

void runtimeError(tfctx *ctx, const char *msg);
#endif