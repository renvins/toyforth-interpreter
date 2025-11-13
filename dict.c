/**
 * @file dict.c
 * @brief Implementation of the primitive dictionary
 *
 * Maps symbol names to their C implementation functions. Uses a simple
 * linear search table, which is efficient enough for the small number
 * of primitives typically defined.
 */

#include <string.h>

#include "dict.h"
#include "primitives.h"

/* ===================== Primitive Dictionary =================== */

/**
 * @brief Internal structure mapping a name to a function
 *
 * Each entry in the primitive table maps a symbol name (like "+")
 * to its implementation function.
 */
typedef struct PrimitiveEntry {
    const char *name;
    WordFn fn;
} PrimitiveEntry;

/**
 * @brief Table of all built-in primitives
 *
 * This table maps symbol names to their implementation functions.
 * To add a new primitive: add an entry here, implement the function
 * in primitives.c, and declare it in primitives.h.
 *
 * The table is terminated with a {NULL, NULL} sentinel.
 */
static const PrimitiveEntry primitiveMappings[] = {
{"+", primitiveAdd},
{"-", primitiveSub},
{"*", primitiveMul},
{".", primitivePrint},
{"dup", primitiveDuplicate},
{"drop", primitiveDrop},
{"swap", primitiveSwap},
{NULL, NULL} // Sentinel marking end of table
};

WordFn lookupPrimitive(const char *name) {
    for (size_t i = 0; primitiveMappings[i].name != NULL; i++) {
        if (strcmp(name, primitiveMappings[i].name) == 0) return primitiveMappings[i].fn;
    }
    return NULL;
}