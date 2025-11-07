#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== Data types =================== */

#define TFOBJ_TYPE_INT 0
#define TFOBJ_TYPE_STR 1
#define TFOBJ_TYPE_BOOL 2
#define TFOBJ_TYPE_LIST 3
#define TFOBJ_TYPE_SYMBOL 4

#define INITIAL_STACK_CAPACITY 256

/* ===================== Data structures =================== */

/* ToyForth object: it's the base object used for 
*  all the operations in our interpreter. */
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
  char *prg; // pointer to start of the program
  char *p; // pointer to program
} tfparser;

typedef struct tfctx {
  tfobj **stack;
  size_t sp;       // index of next free slot
  size_t capacity; // the maximum capacity of the stack
} tfctx;

/* ===================== Forward Declarations =================== */

/* Tell the compiler these functions exist, to resolve circular dependency */
void freeObject(tfobj *o);
void decRef(tfobj *o);

/* Declare primitives functions */
void primitiveAdd(tfctx *ctx);
void primitivePrint(tfctx *ctx);
void primitiveDuplicate(tfctx *ctx);

/* ===================== Mappings =================== */
/* Here there will be the structs and table of mappings
 * for the primitives, to handle in an efficient way
 * the execute function, without have to switch through
 * every possible primitive manually.
 
 * A table will be used and the mapping search will run
 * in O(n) because we will only have dozens of primitives
 * therefore this time complexity will be perfectly fine. */

/* We define a function pointer because primitive
 * functions will all have the same structure.
 * The context is the running environment context */
typedef void (*WordFn)(tfctx *ctx);

/* Struct that will work as a dictionary */
typedef struct PrimitiveEntry {
  char *name;
  WordFn fn;
} PrimitiveEntry;

const PrimitiveEntry primitiveMappings[] = {
  {"+", primitiveAdd},
  {".", primitivePrint},
  {"dup", primitiveDuplicate},
  {NULL, NULL} // Just for sentinel
};

/* Function used to lookup for the inserted primitive
 * and execute it directly. Returns NULL if not found. */
WordFn lookupPrimitive(const char *name) {
  for (int i = 0; primitiveMappings[i].name != NULL; i++) {
    if (strcmp(name, primitiveMappings[i].name) == 0) return primitiveMappings[i].fn;
  }
  return NULL;
}

/* ===================== De/Allocation wrappers =================== */

/* Returns a pointer to the start of allocated memory
and it automatically handles the out of memory error */
void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Out of memory allocationg %zu bytes\n", size);
    exit(1);
  }
  return ptr;
}

void freeObject(tfobj *o) {
  if (o == NULL) {
    return;
  }

  if (o->type == TFOBJ_TYPE_STR || o->type == TFOBJ_TYPE_SYMBOL) {
    free(o->str.ptr);
  } else if (o->type == TFOBJ_TYPE_LIST) {
    for (int i = 0; i < o->list.len; i++) {
      decRef(o->list.ele[i]);
    }
    free(o->list.ele);
  }
  free(o);
}

void incRef(tfobj *o) {
  if (o == NULL)
    return;
  o->refcount++;
}

void decRef(tfobj *o) {
  if (o == NULL)
    return;

  o->refcount--;
  if (o->refcount == 0) {
    freeObject(o);
  }
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

void freeContext(tfctx *ctx) {
  // decRef all the objects still on the stack
  for (size_t i = 0; i < ctx->sp; i++) {
    decRef(ctx->stack[i]);
  }
  free(ctx->stack);
  free(ctx);
}

/* ===================== Stack Manipulation =================== */

void stackPush(tfctx *ctx, tfobj *o) {
  if (ctx->sp >= ctx->capacity) {
    ctx->capacity = ctx->capacity * 2;
    ctx->stack = realloc(ctx->stack, sizeof(tfobj *) * ctx->capacity);
  }
  incRef(o);
  ctx->stack[ctx->sp] = o;
  ctx->sp++;
}

/* Returns the top pointer without touching its refcount; 
*  caller retains responsibility for the returned object's refcount. */
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
  incRef(o);
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
    fprintf(stderr, "Stack underflow: '.' requires a value!\n");
    exit(1);
  }
  tfobj *val = ctx->stack[ctx->sp - 1];
  if (val->type != TFOBJ_TYPE_INT) {
    fprintf(stderr, "Can't print a symbol\n");
    exit(1);
  }
  stackPush(ctx, val);
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
  tfparser pstorage;
  pstorage.p = progtxt;

  tfobj *program_list = createListObject(16);

  while (*(pstorage.p) != '\0') {
    skipWhitespace(&pstorage);
    if (*pstorage.p == '\0')
      break;

    tfobj *o = parseObject(&pstorage);
    if (o) {
      listAppendObject(program_list, o);
      decRef(o);
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
    case TFOBJ_TYPE_SYMBOL:
      /* We lookup the primitives' table to execute
       * the correct symbol's function */
      WordFn fn = lookupPrimitive(o->str.ptr);
      fn(ctx);
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

  char *progtxt = readFile(argv[1]);
  printf("This is the file i've read: %s", progtxt);

  tfobj *program = compile(progtxt);
  exec(ctx, program);

  decRef(program);
  freeContext(ctx);
  free(progtxt);

  return 0;
}
