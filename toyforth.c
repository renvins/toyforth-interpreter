#include <stdio.h>
#include <stdlib.h>

#define TFOBJ_TYPE_INT 0
#define TFOBJ_TYPE_STR 1
#define TFOBJ_TYPE_BOOL 2
#define TFOBJ_TYPE_LIST 3
#define TFOBJ_TYPE_SYMBOL 4

#define INITIAL_STACK_CAPACITY 256

/* ===================== Data structures =================== */

typedef struct tfobj {
  int refcount;
  int type; // TFOBJ_TYPE_*
  union {
    int i;
    struct {
      char *ptr;
      size_t len;
    } str;
    struct {
      struct tfobj **ele;
      size_t len;
      size_t capacity;
    } list;
  };
} tfobj;

typedef struct tfparser {
  char *prg; // the program to compile into a list
  char *p;   // next token to parse
} tfparser;

typedef struct tfctx {
  tfobj **stack;
  size_t sp;       // index of next free slot
  size_t capacity; // the maximum capacity of the stack
} tfctx;

/* ===================== Allocation wrappers =================== */

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Out of memory allocationg %zu bytes\n", size);
    exit(1);
  }
  return ptr;
}

/* ===================== Object related functions =================== */

/* Allocate and initi a new Toy Forth object. */
tfobj *createObject(int type) {
  tfobj *o = xmalloc(sizeof(tfobj));
  o->type = type;
  o->refcount = 1;
  return o;
}

tfobj *createStringObject(char *s, size_t len) {
  tfobj *o = createObject(TFOBJ_TYPE_STR);
  o->str.ptr = s;
  o->str.len = len;
  return o;
}

tfobj *createIntObject(int i) {
  tfobj *o = createObject(TFOBJ_TYPE_INT);
  o->i = i;
  return o;
}

tfobj *createBoolObject(int i) {
  tfobj *o = createObject(TFOBJ_TYPE_BOOL);
  o->i = i;
  return o;
}

tfobj *createSymbolObject(char *s, size_t len) {
  tfobj *o = createStringObject(s, len);
  o->type = TFOBJ_TYPE_SYMBOL;
  return o;
}

tfobj *createListObject(size_t capacity) {
  tfobj *o = createObject(TFOBJ_TYPE_LIST);
  o->list.capacity = capacity;
  o->list.len = 0;
  o->list.ele = xmalloc(sizeof(tfobj *) * o->list.capacity);

  return o;
}

tfctx *createContext() {
  tfctx *ctx = xmalloc(sizeof(tfctx));

  ctx->sp = 0;
  ctx->capacity = INITIAL_STACK_CAPACITY;
  ctx->stack = xmalloc(sizeof(tfobj *) * ctx->capacity);

  return ctx;
}

/* ===================== Stack Manipulation =================== */

void stackPush(tfctx *ctx, tfobj *o) {
  if (ctx->sp >= ctx->capacity) {
    ctx->capacity = ctx->capacity * 2;
    ctx->stack = realloc(ctx->stack, sizeof(tfobj *) * ctx->capacity);
  }
  ctx->stack[ctx->sp] = o;
  ctx->sp++;
}

tfobj *stackPop(tfctx *ctx) {
  if (ctx->sp == 0) {
    fprintf(stderr, "Stack underflow error!");
    exit(1);
  }
  ctx->sp--;
  tfobj *popped_item = ctx->stack[ctx->sp];
  return popped_item;
}

/* ===================== List Manipulation =================== */
void listAppendObject(tfobj *list, tfobj *o) {
  if (list->list.len >= list->list.capacity) {
    list->list.capacity = list->list.capacity * 2;
    list->list.ele =
        realloc(list->list.ele, sizeof(tfobj *) * list->list.capacity);
  }
  list->list.ele[list->list.len] = o;
  list->list.len++;
}

/* ===================== Read file =================== */

char *readFile(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "File not found\n");
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);

  rewind(file);
  char *buffer = xmalloc(size + 1);
  fread(buffer, sizeof(char), size, file);

  buffer[size] = '\0';
  return buffer;
}

/* ===================== Compile & Execute =================== */

tfobj *compile(char *progtxt) {
  tfparser *pstorage = xmalloc(sizeof(tfparser));
  pstorage->prg = progtxt;
  pstorage->p = progtxt; // p is the moving pointer

  tfobj *program_list = createListObject(16);
  return program_list;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  tfctx *ctx = createContext();
  tfobj *list = createListObject(5);

  for (int i = 0; i < 5; i++) {
    tfobj *o = createIntObject(i);
    stackPush(ctx, o);
    listAppendObject(list, o);
  }

  int list_index = 0;
  while (ctx->sp != 0) {
    tfobj *o = stackPop(ctx);
    printf("Current popped object: %d\n", o->i);
    printf("Current list item: %d\n", list->list.ele[list_index]->i);
    list_index++;
  }

  char *progtxt = readFile(argv[1]);
  printf("This is the file i've read: %s", progtxt);
  return 0;
}
