#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tf.h"
#include "mem.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"

/* ===================== Read file =================== */

/* This function totally reads data (text) from
 * a file. It will be used to compile our data */
static char *readFile(const char *filename) {
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
  fclose(file);

  return buffer;
}

void exec(tfctx *ctx, tfobj *program) {
  for (size_t i = 0; i < program->list.len; i++) {
    tfobj *o = program->list.ele[i];
    ctx->current_object = o;
    switch (o->type) {
      case TFOBJ_TYPE_INT:
      case TFOBJ_TYPE_BOOL:
        // It's just data so we can
        // push it to the stack
        stackPush(ctx, o);
        break;
      case TFOBJ_TYPE_SYMBOL: {
        /* We lookup the primitives' table to execute
        * the correct symbol's function */
        WordFn fn = lookupPrimitive(o->str.ptr);
        if (!fn) {
          char error_msg[256];
          snprintf(error_msg, sizeof(error_msg), "Unknown word '%s'", o->str.ptr);
          runtimeError(ctx, error_msg);
        }
        fn(ctx);
        break;
      }
      default:
        runtimeError(ctx,"Found an unknown keyword while executing the program!");
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

  tfobj *program = compile(progtxt);
  exec(ctx, program);

  decRef(program);
  freeContext(ctx);
  free(progtxt);

  return 0;
}
