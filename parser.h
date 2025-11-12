/**
 * @file parser.h
 * @brief Parser and compiler for ToyForth source code
 *
 * Converts raw source text into a list of executable objects (integers
 * and symbols). Handles tokenization, number parsing, and whitespace.
 */

#ifndef PARSER_H
#define PARSER_H
#include "tf.h"

/**
 * @brief Compile source text into an executable object list
 * @param progtxt Null-terminated source code string
 * @return A list object containing the compiled program
 *
 * This function tokenizes and parses the input text, creating objects
 * for each token. Numbers become integer objects, and words become symbol
 * objects. Returns a list with refcount=1 that the caller must eventually
 * decRef().
 *
 * The parser handles:
 * - Integers (including negative numbers)
 * - Symbols (words/identifiers)
 * - Whitespace (spaces, tabs, newlines)
 * - Backslash comments (from \ to end of line)
 *
 * Line and column information is tracked for error reporting.
 */
tfobj *compile(char *progtxt);

#endif