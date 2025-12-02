# Advent of Code 2025

This repository contains my solutions for the [Advent of Code 2025](https://adventofcode.com/2025) programming puzzles.

## Tech Stack

The solutions are implemented in **C23** and compiled using a **Makefile**. The goal is to solve the puzzles without relying on any external dependencies, using only the standard C library.

## Project Structure

The project is organized by day. Each day's solution is contained within its own directory (e.g., `day01/`, `day02/`, etc.).

Each directory will contain:

- `main.c`: The C source code for the solution.
- `example.txt`: An example puzzle input for that day.

## Building and Running

A central `Makefile` is provided to build and run the solutions.

### Build All Solutions

To compile all solutions:

```bash
make all
```

### Build a Specific Day

To compile the solution for a specific day (e.g., day 1):

```bash

make bin/day01

```

This will create an executable file named `day01` in the `bin/` directory.

### Run a Specific Day

After building, you can run the solution by passing the path to an input file as an argument:

```bash

./bin/day01 day01/example.txt

```

### Clean Up

To remove all compiled executables:

```bash
make clean
```

## Dependencies

This project strives to be dependency-free, relying only on a C23 compatible compiler (like GCC or Clang) and `make`.
