# ToyForth Interpreter in C

[![CI](https://github.com/renvins/toyforth/workflows/CI/badge.svg)](https://github.com/renvins/toyforth/actions)

A minimal, clean interpreter for a Forth-like language, written in C from scratch. This project was built for fun and is a perfect starter kit for learning about or building your own programming language.

## Table of Contents

- [Project Philosophy](#project-philosophy)
- [How it Works (High-Level View)](#how-it-works-a-high-level-view)
- [Project Structure](#project-structure)
- [How to Build](#how-to-build)
- [How to Run](#how-to-run)
- [Testing](#testing)
- [Available Words](#available-words)
- [Example](#example)
- [Deep Dive: Understanding the Internals](#deep-dive-understanding-the-internals)
  - [1. The Core Object: `tfobj`](#1-the-core-object-tfobj)
  - [2. Reference Counting](#2-reference-counting-memory-without-malloc-chaos)
  - [3. The Parser](#3-the-parser-from-text-to-objects)
  - [4. The Stack-Based VM](#4-the-stack-based-vm)
  - [5. Primitives](#5-primitives-implementing-language-features-in-c)
  - [6. The Dictionary](#6-the-dictionary-symbol--function-mapping)
  - [7. Design Patterns](#7-design-patterns-youll-recognize)
  - [8. Documentation](#8-documentation)

## Project Philosophy

This is a "toy" interpreter built for fun and learning. Its main goal is to be a small, understandable example of how an interpreter works, from parsing text to managing memory.

It is designed to be a clean, solid foundation for anyone who wants to experiment with building their own language. Because it's small, it's easy to read, modify, and extend.

## How it Works (A High-Level View)

The interpreter follows a classic **read → parse → execute** pipeline:

1.  **File Reader (`readFile`)**: First, the `.tf` source file is read into a single string.
2.  **Parser (`compile`)**: A simple parser walks the string and turns it into a `list` of objects. It can create `integer` objects (like `10`) and `symbol` objects (like `+`).
3.  **VM (`exec`)**: A tiny stack-based virtual machine loops over the list of objects.
      * If it sees data (an integer), it pushes it onto the stack.
      * If it sees a symbol, it looks for a matching primitive (a C function) and executes it.
4.  **Memory (`incRef`/`decRef`)**: All objects (`tfobj`) are managed by a simple reference counting system. This prevents memory leaks and is a core concept in many high-level languages.

For example, the program `10 20 +` becomes:
```
┌─────────────┐
│ Source File │  10 20 +
└──────┬──────┘
       │ readFile()
       ▼
┌─────────────┐
│ Raw String  │  "10 20 +"
└──────┬──────┘
       │ compile()
       ▼
┌─────────────┐
│ Object List │  [IntObj(10), IntObj(20), SymbolObj("+")]
└──────┬──────┘
       │ exec()
       ▼
┌─────────────┐     Stack Operations:
│   VM Loop   │     1. push 10      → [10]
│             │     2. push 20      → [10, 20]
│  Primitives │     3. call '+'     → [30]
│   Execute   │        - lookup "+" in dict
│             │        - call primitiveAdd()
└─────────────┘        - pop, compute, push
```

## Project Structure

The project is organized into modular components:

| File | Purpose | Key Functions |
|------|---------|---------------|
| `tf.h` | Core type definitions | `tfobj`, `tfctx`, `tfparser` structs |
| `main.c` | Entry point, VM loop | `main()`, `exec()`, `readFile()` |
| `parser.c/h` | Tokenization & compilation | `compile()`, `parseObject()` |
| `mem.c/h` | Memory & object lifecycle | `incRef()`, `decRef()`, `createXxxObject()` |
| `stack.c/h` | Stack operations | `stackPush()`, `stackPop()` |
| `list.c/h` | Dynamic list manipulation | `listAppendObject()` |
| `dict.c/h` | Symbol → function lookup | `lookupPrimitive()` |
| `primitives.c/h` | Built-in word implementations | `primitiveAdd()`, `primitivePrint()`, etc. |

**Reading guide**: Start with `main.c` to see the big picture, then dive into `parser.c` (how text becomes objects), `mem.c` (how objects are managed), and finally `primitives.c` (how operations work). The other files are support utilities.

### Learning Paths

**🚀 I want to try it quickly:**
1. `make && make test`
2. Explore the test files in `tests/` directory
3. Edit test files and experiment
4. Read the [Available Words](#available-words) section

**📖 I want to understand interpreters:**
1. Read [How it Works](#how-it-works-a-high-level-view)
2. Read the [Deep Dive](#deep-dive-understanding-the-internals) sections in order
3. Read through `main.c`, `parser.c`, and `primitives.c` with inline documentation
4. Run `make test` to see examples of all features

**🔧 I want to add a feature:**
1. Look at how existing primitives are implemented in `primitives.c`
2. Follow the pattern: implement in `primitives.c`, declare in `primitives.h`, add to `dict.c`
3. Add tests for your new feature in `tests/`
4. See the inline documentation for guidance

## How to Build

Use the provided Makefile:

```bash
make            # builds the 'toyforth' binary
make -j         # parallel build (faster)
make test       # run the test suite
make clean      # removes objects and binary
```

You can override compiler flags if needed:

```bash
make CFLAGS="-std=c11 -Wall -Wextra -O2"
```

The Makefile uses incremental compilation, so it only rebuilds changed files. The project compiles with `-Wall -Wextra -Werror` by default, ensuring clean, warning-free code.

## How to Run

Execute the compiled program by passing it the path to a ToyForth source file (`.tf`):

```bash
./toyforth path/to/your/program.tf
```

Run the comprehensive test suite:

```bash
make test
```

## Testing

ToyForth includes a comprehensive test suite with 8 test files covering all functionality:

- **`arithmetic.tf`** - Basic arithmetic operations
- **`stack_ops.tf`** - Stack manipulation (dup, swap, drop)
- **`complex.tf`** - Complex multi-operation expressions
- **`edge_cases.tf`** - Boundary conditions and special cases
- **`negative_numbers.tf`** - Negative number handling
- **`comments.tf`** - Comment parsing
- **`whitespace.tf`** - Whitespace handling
- **`stress.tf`** - Stress tests (factorial, deep stacks)

Run all tests with:
```bash
make test
# or
./run_tests.sh
```

All tests pass with 100% success rate. Each test file demonstrates different features of the language and serves as documentation through examples.

## Available Words

ToyForth includes these built-in primitives:

**Arithmetic:**
- **`+`** - Add two integers (`a b -- sum`)
- **`-`** - Subtract two integers (`a b -- a-b`)
- **`*`** - Multiply two integers (`a b -- product`)

**Stack Manipulation:**
- **`dup`** - Duplicate the top value (`a -- a a`)
- **`drop`** - Discard the top value (`a -- `)
- **`swap`** - Swap the top two values (`a b -- b a`)

**I/O:**
- **`.`** - Pop and print the top integer

**Comments:**
- **`\`** - Line comment (from `\` to end of line)

All primitives include proper error handling with line/column information for debugging.

## Example

Here are some examples demonstrating ToyForth features:

**Basic Arithmetic:**
```forth
\ Addition and multiplication
5 10 + .        \ Prints: 15
3 7 * .         \ Prints: 21
```

**Stack Manipulation:**
```forth
\ Duplicate and add
7 dup + .       \ Prints: 14 (7 + 7)

\ Swap and subtract
10 20 swap - .  \ Prints: 10 (20 - 10 after swap)
```

**Complex Expressions:**
```forth
\ Calculate (5 + 3) * 2
5 3 + 2 * .     \ Prints: 16

\ Square a number using dup
5 dup * .       \ Prints: 25
```

**Comments:**
```forth
\ This is a comment
10 20 + .       \ Inline comment - Prints: 30
```

See the `tests/` directory for many more examples covering all features.

---

## Deep Dive: Understanding the Internals

This section explains the key concepts and design decisions behind ToyForth. If you want to understand how interpreters work or extend this project, read on!

### 1. The Core Object: `tfobj`

Every value in ToyForth is represented as a `tfobj` (ToyForth Object). This unified representation is inspired by languages like Python and Ruby.

**Key insight**: By wrapping everything in a single struct with a `type` field and a union, we can have one stack that holds any kind of value.

```c
typedef struct tfobj {
  int refcount;        // For memory management
  int type;            // TFOBJ_TYPE_INT, TFOBJ_TYPE_SYMBOL, etc.
  union {
    int i;             // For integers and booleans
    struct {           // For strings and symbols
      char *ptr;
      size_t len;
    } str;
    struct {           // For lists (the compiled program)
      struct tfobj **ele;
      size_t len;
      size_t capacity;
    } list;
  };
} tfobj;
```

**Why this matters**: This design means we can:
- Store different types on the same stack
- Add new types (like floats or strings) without changing the stack
- Use the same `incRef`/`decRef` functions for all types

### 2. Reference Counting: Memory Without `malloc` Chaos

Every `tfobj` has a `refcount` field. When you create an object, `refcount = 1`. When something takes ownership (like pushing to stack), we call `incRef(obj)` (increments count). When done, call `decRef(obj)` (decrements count). When `refcount` reaches 0, the object frees itself.

**Example flow for `10 20 +`**:

```c
// Parser creates: refcount = 1
tfobj *ten = createIntObject(10);

// Add to program list: refcount = 2 (parser + list)
listAppendObject(program_list, ten);

// Parser done: refcount = 1 (just the list)
decRef(ten);

// VM pushes to stack: refcount = 2 (list + stack)
stackPush(ctx, ten);

// VM finishes: refcount = 1 (just stack)
// Program list freed: refcount = 0 → freed!
```

**Why not garbage collection?**: Reference counting is simpler to understand and implement. The trade-off is that cycles (A → B → A) would leak, but our simple language doesn't create them.

**Pro tip**: Every `stackPush` increments, every `stackPop` returns without decrementing, the caller must `decRef` when done. This convention makes ownership explicit.

**Visual representation of refcounting**:

```
Time    Action                      Object(10)      Who holds it?
────────────────────────────────────────────────────────────────
T1      createIntObject(10)         refcount=1      parser's local var
T2      listAppendObject(...)       refcount=2      parser + program_list
T3      parser decRef(ten)          refcount=1      program_list
T4      exec: stackPush(...)        refcount=2      program_list + stack
T5      primitiveAdd: stackPop()    refcount=2      program_list + local var
T6      primitiveAdd: decRef()      refcount=1      program_list
T7      main: decRef(program)       refcount=0      ❌ FREED
```

The beauty: no manual `free()` calls in application code, yet no memory leaks!

### 3. The Parser: From Text to Objects

The parser (`compile` function in `parser.c`) does two jobs:
1. **Tokenize**: Split `"10 20 +"` into tokens: `["10", "20", "+"]`
2. **Objectify**: Convert tokens to typed objects

**Key parsing logic**:

```c
static tfobj *parseObject(tfparser *p) {
    char c = *p->p;
    
    // Is it a number? (digit or negative sign followed by digit)
    if (isdigit(c) || (c == '-' && isdigit(*(p->p + 1)))) {
        char *end_ptr;
        int val = strtol(p->p, &end_ptr, 10);  // Parse base-10 integer
        p->p = end_ptr;                         // Advance past number
        return createIntObject(val);
    } 
    
    // Otherwise it's a symbol (word)
    char *start = p->p;
    while (*p->p != '\0' && !isspace(*p->p)) {
        p->p++;  // Collect non-whitespace
    }
    size_t len = p->p - start;
    // ... copy and create symbol object
}
```

**Design choice**: We parse integers directly but keep symbols as strings. At execution time, we look up symbols in the primitive table. An alternative would be to resolve symbols during parsing (like compiling to bytecode), but keeping them as strings makes debugging easier and the code simpler.

### 4. The Stack-Based VM

Forth is a **stack-based language**: data lives on a stack, and operations consume/produce values on that stack.

**How `10 20 + .` executes**:

```
Step 1: Push 10
Stack: [10]

Step 2: Push 20
Stack: [10, 20]

Step 3: Execute '+'
  - Pop 20 (top)
  - Pop 10 (next)
  - Compute 10 + 20 = 30
  - Push 30
Stack: [30]

Step 4: Execute '.'
  - Pop 30
  - Print "30"
Stack: []
```

**The VM loop** (simplified from `main.c`):

```c
void exec(tfctx *ctx, tfobj *program) {
  for (size_t i = 0; i < program->list.len; i++) {
    tfobj *o = program->list.ele[i];
    
    if (o->type == TFOBJ_TYPE_INT) {
      stackPush(ctx, o);          // Data? Push it.
    }
    else if (o->type == TFOBJ_TYPE_SYMBOL) {
      WordFn fn = lookupPrimitive(o->str.ptr);  // Symbol? Look it up.
      fn(ctx);                                   // Call the C function.
    }
  }
}
```

**Key insight**: The VM doesn't know what `+` does it just calls the registered C function. This makes adding new words trivial: write a C function, add it to the table in `dict.c`.

### 5. Primitives: Implementing Language Features in C

Each word is a C function with signature `void word(tfctx *ctx)`. It can inspect and modify the stack.

**Anatomy of `primitiveAdd`**:

```c
void primitiveAdd(tfctx *ctx) {
    // 1. Check stack has enough values
    if (ctx->sp < 2) {
      fprintf(stderr, "Stack underflow: '+' requires two values!\n");
      exit(1);
    }
    
    // 2. Pop operands (note: top of stack first)
    tfobj *b = stackPop(ctx);  // second operand
    tfobj *a = stackPop(ctx);  // first operand
  
    // 3. Type check
    if (a->type != TFOBJ_TYPE_INT || b->type != TFOBJ_TYPE_INT) {
      fprintf(stderr, "The addition requires two integers\n");
      exit(1);
    }
    
    // 4. Compute result
    int result = a->i + b->i;
    tfobj *objResult = createIntObject(result);
  
    // 5. Push result
    stackPush(ctx, objResult);
  
    // 6. Clean up (stack has a reference, we don't need these)
    decRef(objResult);  // Stack still holds it
    decRef(a);          // We're done with these
    decRef(b);
}
```

**Why `decRef(objResult)` after pushing?** `stackPush` increments the refcount, so after pushing, `objResult` has refcount 2 (our variable + stack). We decrement our variable's reference, leaving it with refcount 1 (just the stack). When it's eventually popped, the stack will decrement, and at refcount 0 it'll free.

**Adding a new primitive** requires:
1. Write the C function in `primitives.c`
2. Declare it in `primitives.h`
3. Add an entry to `primitiveMappings[]` in `dict.c`

That's it! No VM changes needed.

### 6. The Dictionary: Symbol → Function Mapping

The dictionary (`dict.c`) is a simple linear lookup table:

```c
typedef struct PrimitiveEntry {
    char *name;
    WordFn fn;  // Function pointer
} PrimitiveEntry;

static const PrimitiveEntry primitiveMappings[] = {
    {"+", primitiveAdd},
    {"-", primitiveSub},
    {".", primitivePrint},
    // ...
    {NULL, NULL}  // Sentinel
};

WordFn lookupPrimitive(const char *name) {
    for (size_t i = 0; primitiveMappings[i].name != NULL; i++) {
        if (strcmp(name, primitiveMappings[i].name) == 0) 
            return primitiveMappings[i].fn;
    }
    return NULL;  // Not found
}
```

**Performance note**: This is O(n) lookup. For dozens of words, it's fine. If you add hundreds, consider a hash table. The beauty of this design is that it's trivial to understand and extend.

### 7. Design Patterns You'll Recognize

This codebase demonstrates several classic CS concepts:

- **Tagged unions** (`tfobj.type` + union): Represents sum types (a value is *one of* several types)
- **Reference counting**: Automatic memory management without a garbage collector
- **Function pointers**: Enables data-driven execution (the dictionary maps strings to functions)
- **Separation of concerns**: Parser doesn't know about execution, VM doesn't know about parsing
- **Stack machine**: Simple execution model, used in JVM, Lua, and many others

### 8. Documentation

The entire codebase is fully documented with **Doxygen-style comments**:

**All header files include:**
- File-level documentation explaining the module's purpose
- Function documentation with `@brief`, `@param`, and `@return` tags
- Detailed explanations of memory management and ownership semantics
- Usage examples where appropriate

**All implementation files include:**
- File-level documentation
- Internal function documentation
- Inline comments explaining complex logic

**To browse the documentation:**
- Read inline comments in the source files
- All functions explain their parameters, return values, and side effects
- Memory management rules are clearly documented
- Error conditions are explicitly stated

**Example documentation:**
```c
/**
 * @brief Push an object onto the execution stack
 * @param ctx Execution context containing the stack
 * @param o Object to push (its reference count is incremented)
 *
 * The stack takes ownership of a reference to the object...
 */
void stackPush(tfctx *ctx, tfobj *o);
```

The documentation makes the codebase easy to understand and extend, perfect for learning or building your own language features.

---

## Quick Reference Card

**Build & Run**
```bash
make              # Build
make test         # Run test suite
make clean        # Clean build artifacts
```

**Stack Notation Guide**
```
( before -- after )
Examples:
  +       ( a b -- sum )      Pop two, push sum
  *       ( a b -- product )  Pop two, push product
  dup     ( a -- a a )        Duplicate top
  drop    ( a -- )            Discard top
  swap    ( a b -- b a )      Swap top two
```

**Adding a New Primitive**
```c
// 1. primitives.c
void primitiveMyWord(tfctx *ctx) {
    // your implementation
}

// 2. primitives.h
void primitiveMyWord(tfctx *ctx);

// 3. dict.c (add to primitiveMappings[])
{"myword", primitiveMyWord},
```

**Memory Management Rules**
```c
// Creating objects → refcount = 1
tfobj *o = createIntObject(42);

// Sharing ownership → increment
incRef(o);

// Done with it → decrement
decRef(o);  // Frees at refcount=0

// stackPush increments, stackPop doesn't decrement
// (caller is responsible for decRef)
```

---

## Special Thanks

I want to thank [@antirez](https://github.com/antirez) for the idea of the project and for what he's done for the OSS community!
