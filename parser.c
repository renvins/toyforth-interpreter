#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tf.h"
#include "mem.h"
#include "list.h"

/* ===================== Parsing & compile =================== */

static void advance(tfparser *p) {
  if (*p->p == '\n') {
    p->line++;
    p->column = 1;
  } else {
    p->column++;
  }
  p->p++;
}

static void skipWhitespace(tfparser *p) {
    while (*(p->p) && isspace(*(p->p))) {
      advance(p);
    }
}

static void skipComments(tfparser *p) {
  if(*(p->p) == '\\') {
    while (*(p->p) && *(p->p) != '\n') {
      advance(p);
    }
    advance(p);
  }
}

static void setObjectLocation(tfobj *o, int line, int column) {
  if (o == NULL) {
      return;
  }
  o->src_line = line;
  o->src_column = column;
}
  
/* This function parses an object from the current pointed element
 * of the parser. It checks between a number and a symbol */
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
  
/* This function creates a parsers and a list of tfobject
 * to make them readable and executable by execute function */
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