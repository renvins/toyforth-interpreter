#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tf.h"
#include "mem.h"
#include "list.h"

/* ===================== Parsing & compile =================== */

static void skipWhitespace(tfparser *p) {
    while (*(p->p) && isspace(*(p->p))) {
      p->p++;
    }
}
  
/* This function parses an object from the current
 * pointed element of the parser. It checks between
 * a number and a symbol */
static tfobj *parseObject(tfparser *p) {
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