#include <string.h>

#include "dict.h"
#include "primitives.h"

/* ===================== Mappings =================== */
/* Here there will be the structs and table of mappings
 * for the primitives, to handle in an efficient way
 * the execute function, without have to switch through
 * every possible primitive manually.
 
 * A table will be used and the mapping search will run
 * in O(n) because we will only have dozens of primitives
 * therefore this time complexity will be perfectly fine. */

/* Struct that will work as a dictionary */
typedef struct PrimitiveEntry {
    char *name;
    WordFn fn;
} PrimitiveEntry;

static const PrimitiveEntry primitiveMappings[] = {
{"+", primitiveAdd},
{"-", primitiveSub},
{".", primitivePrint},
{"dup", primitiveDuplicate},
{"drop", primitiveDrop},
{"swap", primitiveSwap},
{NULL, NULL} // Just for sentinel
};

/* Function used to lookup for the inserted primitive
* and execute it directly. Returns NULL if not found. */
WordFn lookupPrimitive(const char *name) {
    for (size_t i = 0; primitiveMappings[i].name != NULL; i++) {
        if (strcmp(name, primitiveMappings[i].name) == 0) return primitiveMappings[i].fn;
    }
    return NULL;
}