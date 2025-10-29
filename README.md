# ToyForth Interpreter

This is a simple, stack-based interpreter for a mini Forth-like language, written in C. It reads a program from a file, parses it into a list of objects (integers and symbols), and then executes them.

## Features

  * **Stack-Based VM**: Operations work by manipulating a central data stack.
  * **Object System**: Supports basic object types including integers, symbols, booleans, and lists.
  * **Parser**: Includes a simple parser that reads space-separated tokens from a file. It can parse integers and symbols.
  * **File Execution**: The interpreter reads and executes code from a specified source file.
  * **Primitives**: Implements a small set of core operations:
      * `+` : Pops two integers from the stack, adds them, and pushes the result.
      * `.` : Pops one integer from the stack and prints it to the console, followed by a newline.

## How to Build

Compile the `toyforth.c` file using a C compiler like `gcc`:

```bash
gcc toyforth.c -o toyforth
```

## How to Run

Execute the compiled program by passing it the path to a ToyForth source file (`.tf`) as a command-line argument.

```bash
./toyforth path/to/your/program.tf
```

### Example

The provided `test.tf` file contains the following code:

```forth
10 20 + .
```

Running this file will:

1.  Push `10` onto the stack.
2.  Push `20` onto the stack.
3.  Execute `+`, popping `20` and `10`, adding them (10 + 20 = 30), and pushing the result `30` back onto the stack.
4.  Execute `.`, popping `30` and printing it to the console.

**Output:**

```bash
$ ./toyforth test.tf
This is the file i've read: 10 20 + .
30
```
