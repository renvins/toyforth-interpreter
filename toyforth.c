#include <ctype.h>
#include <libc.h>
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

/* Creates the needed context (stack) for the operations */
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

/* This function append an object to a tfobjects list.
 * It is resized when the len reaches maximum capacity */
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

/* This function totally reads data (text) from
 * a file. It will be used to compile our data */
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

void skipWhitespace(tfparser *p) {
  while (*(p->p) && isspace(*(p->p))) {
    p->p++;
  }
}

/* This function parses an object from the current
 * pointed element of the parser. It checks between
 * a number and a symbol */
tfobj *parseObject(tfparser *p) {
  char c = *p->p;
  if (isdigit(c) || (c == '-') && isdigit(*(p->p + 1))) {
    char *end_ptr;
    // strtol converts string to long, last arg is base
    // end_ptr moves to the next of last digit
    int val = strtol(p->p, &end_ptr, 10);
    p->p = end_ptr;

    return createIntObject(val);
  } else {
    char *start = p->p;
    while (*p->p != '\0' && !isspace(*p->p)) {
      p->p++;
    }
    size_t len = p->p - start;
    char *sym_str = xmalloc(len + 1);
    memcpy(sym_str, start, len);
    sym_str[len] = '\0';

    return createSymbolObject(sym_str, len);
  }
}

/* This function creates a parsers and a list of tfobject
 * to make them readable and executable by execute function */
tfobj *compile(char *progtxt) {
  tfparser *pstorage = xmalloc(sizeof(tfparser));
  pstorage->prg = progtxt;
  pstorage->p = progtxt; // p is the moving pointer

  tfobj *program_list = createListObject(16);

  while (*(pstorage->p) != '\0') {
    skipWhitespace(pstorage);
    if (*pstorage->p == '\0')
      break;

    tfobj *o = parseObject(pstorage);
    if (o) {
      listAppendObject(program_list, o);
    }
  }
  return program_list;
}

void exec(tfctx *ctx, tfobj *program) {
  for (int i = 0; i < program->list.len; i++) {
    tfobj *o = program->list.ele[i];
    switch (o->type) {
    case TFOBJ_TYPE_INT:
    case TFOBJ_TYPE_BOOL:
      // It's just data so we can
      // push it to the stack
      stackPush(ctx, o);
      break;
    }
  }
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

  tfobj *program = compile(progtxt);
  printf("Compiled program has %zu objects:\n", program->list.len);
  for (size_t i = 0; i < program->list.len; i++) {
    tfobj *o = program->list.ele[i];
    if (o->type == TFOBJ_TYPE_INT) {
      printf("  Object %zu: INT (%d)\n", i, o->i);
    } else if (o->type == TFOBJ_TYPE_SYMBOL) {
      printf("  Object %zu: SYMBOL (%s)\n", i, o->str.ptr);
    } else {
      printf("  Object %zu: OTHER (%d)\n", i, o->type);
    }
  }
  return 0;
}
