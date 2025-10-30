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
