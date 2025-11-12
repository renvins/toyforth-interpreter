/**
 * @file parser.c
 * @brief Implementation of the ToyForth parser and compiler
 *
 * Converts source text into executable objects. Handles tokenization,
 * number parsing, symbol extraction, whitespace, and comments. Tracks
 * source locations for error reporting.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tf.h"
#include "mem.h"
#include "list.h"

/* ===================== Parsing & compile =================== */

/**
 * @brief Advance the parser by one character
 * @param p Parser state
 *
 * Moves the parser forward by one character, updating line and column
 * numbers appropriately. Handles newlines by incrementing the line
 * counter and resetting the column to 1.
 */
static void advance(tfparser *p) {
  if (*p->p == '\n') {
    p->line++;
    p->column = 1;
  } else {
    p->column++;
  }
  p->p++;
}

/**
 * @brief Skip whitespace characters
 * @param p Parser state
 *
 * Advances the parser past all consecutive whitespace characters
 * (spaces, tabs, newlines, etc.), updating line and column tracking.
 */
static void skipWhitespace(tfparser *p) {
    while (*(p->p) && isspace(*(p->p))) {
      advance(p);
    }
}

/**
 * @brief Skip backslash-style comments
 * @param p Parser state
 *
 * If the current character is '\', skips all characters until the end
 * of the line. This implements line comments: \ comment text here
 */
static void skipComments(tfparser *p) {
  if(*(p->p) == '\\') {
    while (*(p->p) && *(p->p) != '\n') {
      advance(p);
    }
    advance(p);
  }
}

/**
 * @brief Set source location information on an object
 * @param o Object to update (NULL-safe)
 * @param line Line number
 * @param column Column number
 *
 * Records the source location where this object was parsed. Used for
 * error reporting during execution.
 */
static void setObjectLocation(tfobj *o, int line, int column) {
  if (o == NULL) {
      return;
  }
  o->src_line = line;
  o->src_column = column;
}

/**
 * @brief Parse a single token into an object
 * @param p Parser state
 * @return Newly created integer or symbol object
 *
 * Determines whether the current token is a number or symbol and creates
 * the appropriate object. Numbers (including negative integers) become
 * TFOBJ_TYPE_INT, everything else becomes TFOBJ_TYPE_SYMBOL.
 *
 * The parser position is advanced past the parsed token.
 */
static tfobj *parseObject(tfparser *p) {
  int start_line = p->line;
  int start_column = p->column;

  char c = *p->p;
  if ((isdigit(c) || (c == '-')) && isdigit(*(p->p + 1))) {
    char *end_ptr;
    // strtol converts string to long, last arg is base
    // end_ptr moves to the next of last digit
    int val = strtol(p->p, &end_ptr, 10);
    p->p = end_ptr;

    tfobj *obj = createIntObject(val);
    setObjectLocation(obj, start_line, start_column);
    return obj;
  } else {
    char *start = p->p;
    while (*p->p != '\0' && !isspace(*p->p)) {
      advance(p);
    }
    size_t len = p->p - start;
    char *sym_str = xmalloc(len + 1);
    memcpy(sym_str, start, len);
    sym_str[len] = '\0';

    tfobj *obj = createSymbolObject(sym_str, len);
    setObjectLocation(obj, start_line, start_column);
    return obj;
  }
}

tfobj *compile(char *progtxt) {
    tfparser pstorage;
    pstorage.p = progtxt;
    pstorage.line = 1;
    pstorage.column = 1;
  
    tfobj *program_list = createListObject(16);
  
    while (*(pstorage.p) != '\0') {
      skipWhitespace(&pstorage);
      skipComments(&pstorage);
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