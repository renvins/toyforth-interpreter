# ToyForth Interpreter in C

A minimal, clean interpreter for a Forth-like language, written in C from scratch. This project was built for fun and is a perfect starter kit for learning about or building your own programming language.

## Project Philosophy

This is a "toy" interpreter built for fun and learning. Its main goal is to be a small, understandable example of how an interpreter works, from parsing text to managing memory.

It is designed to be a clean, solid foundation for anyone who wants to experiment with building their own language. Because it's small, it's easy to read, modify, and extend.

## How it Works (A High-Level View)

The interpreter is broken down into a few simple parts:

1.  **File Reader (`readFile`)**: First, the `.tf` source file is read into a single string.
2.  **Parser (`compile`)**: A simple parser walks the string and turns it into a `list` of objects. It can create `integer` objects (like `10`) and `symbol` objects (like `+`).
3.  **VM (`exec`)**: A tiny stack-based virtual machine loops over the list of objects.
      * If it sees data (an integer), it pushes it onto the stack.
      * If it sees a symbol, it looks for a matching primitive (a C function) and executes it.
4.  **Memory (`incRef`/`decRef`)**: All objects (`tfobj`) are managed by a simple reference counting system. This prevents memory leaks and is a core concept in many high-level languages.

## Project Structure

The project is organized into modular components:

- `main.c` - Entry point, file reader, and VM execution loop
- `parser.c/h` - Tokenizes and compiles source text into objects
- `mem.c/h` - Memory management (reference counting) and object creation
- `stack.c/h` - Stack operations (push/pop)
- `list.c/h` - Dynamic list manipulation
- `dict.c/h` - Primitive word lookup table
- `primitives.c/h` - Implementation of built-in words
- `tf.h` - Core type definitions

## How to Build

Use the provided Makefile:

```bash
make            # builds the 'toyforth' binary
make -j         # parallel build (faster)
make clean      # removes objects and binary
```

You can override compiler flags if needed:

```bash
make CFLAGS="-std=c11 -Wall -Wextra -O2"
```

The Makefile uses incremental compilation, so it only rebuilds changed files.

## How to Run

Execute the compiled program by passing it the path to a ToyForth source file (`.tf`):

```bash
./toyforth path/to/your/program.tf
```

Or use the convenience target to run the test file:

```bash
make run
```

## Available Words

ToyForth currently includes these built-in primitives:

- **`+`** - Add two integers (`a b -- sum`)
- **`-`** - Subtract two integers (`a b -- a-b`)
- **`.`** - Pop and print the top integer
- **`dup`** - Duplicate the top value (`a -- a a`)
- **`drop`** - Discard the top value (`a -- `)
- **`swap`** - Swap the top two values (`a b -- b a`)

## Example

The provided `tests/test.tf` file demonstrates several primitives:

```forth
10 20 dup + . 30 40 + . 20 30 - . 40 50 swap - .
```

**Step-by-step execution:**

1. `10 20 dup +` → Stack: `[30]` (10 + 20 after dup)
2. `.` → Prints `40` (actually 20 + 20 from dup)
3. `30 40 +` → Stack: `[70]`
4. `.` → Prints `70`
5. `20 30 -` → Stack: `[-10]` (20 - 30)
6. `.` → Prints `-10`
7. `40 50 swap -` → Stack: `[10]` (50 - 40 after swap)
8. `.` → Prints `10`

**Output:**

```bash
$ make run
40
70
-10
10
```

## Special Thanks
I want to thank [@antirez](https://github.com/antirez) for the idea of the project and for what he's done for the OSS community!
