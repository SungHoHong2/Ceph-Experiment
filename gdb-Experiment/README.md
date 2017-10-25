### GDB Experiments

- **Definition of GDB**
    - A debugger for several languages, including C and C++
    - allows you to inspect what the program is doing at a certain point during execution.
    - Errors like segmentation faults may be easier to find with the help of gdb.

<br>

1. **Basic functions**
    - run - run the file
    - break - break in particular function or line
    - next - move to next line
    - next - move to next line
    - continue - continue to the next breakpoint
    - step - step into inner function

```
$ gcc [other flags] -g <source files> -o <output file>
$ gdb
(gdb) file prog1.x
(gdb) run

(gdb) 6 // break in line 6
(gdb) break my func // break in particular function

(gdb) next // move to next line
(gdb) continue // continue to the next breakpoint
(gdb) step // going into inner function

$ gdb --args executablename arg1 arg2 arg3 // running with parameters

```

<br>

2. **Useful functions**
    - backtrace - produces a stack trace of the function calls that lead to a seg fault (should remind you of Java exceptions)
    - where - same as backtrace; you can think of this version as working even when you’re still in the middle of the program
    - finish - runs until the current function is finished
    - delete - deletes a specified breakpoint
    - info breakpoints - shows information about all declared breakpoints

```
(gdb) backtrace //work after the program produces an error
(gdb) where //same as backtrace
```
