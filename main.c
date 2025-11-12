/**
 * @file main.c
 * @brief Main entry point and virtual machine execution loop
 *
 * This module contains:
 * - File reading utilities
 * - The VM execution loop (exec)
 * - Program entry point (main)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tf.h"
#include "mem.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"

/* ===================== File I/O =================== */

/**
 * @brief Read an entire file into a string
 * @param filename Path to the file to read
 * @return Null-terminated string containing file contents
 *
 * Reads the entire file into a dynamically allocated buffer. The caller
 * must free() the returned string. Exits with an error if the file cannot
 * be opened.
 */
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

/* ===================== Virtual Machine =================== */

/**
 * @brief Execute a compiled program
 * @param ctx Execution context (contains the stack)
 * @param program List object containing the compiled program
 *
 * This is the main VM loop. It iterates through the program list:
 * - Data objects (integers, booleans) are pushed onto the stack
 * - Symbol objects are looked up in the primitive dictionary and executed
 *
 * The current object is tracked in ctx->current_object for error reporting.
 * Exits with an error if an unknown symbol is encountered.
 */
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
        runtimeError(ctx, "Found an unknown keyword while executing the program");
        break;
    }
  }
}

/* ===================== Main Entry Point =================== */

/**
 * @brief Program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 on error
 *
 * Usage: toyforth <filename>
 *
 * Reads the specified ToyForth source file, compiles it, and executes it.
 * Properly cleans up all allocated resources before exiting.
 */
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
