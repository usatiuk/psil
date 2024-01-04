# PSIL

a simple lisp interpreter

## How to compile

Should be a regular cmake build, something like

```shell
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=YES
cmake --build build  --parallel $(nproc)
```

should be enough (you can also change the build type for Release and disable sanitize in case it's too slow, also in
case of problems you can look at the `.gitlab-ci.yml`)

Pro-tip: all these snippets can be conveniently run from an IDE if it supports it (at least clion does)

## How to run tests

Unit tests (assuming build in `build`):

```shell
ctest --test-dir build --verbose --parallel $(nproc)
```

CLI tests (also assuming build in `build` and being in repo root):

```shell
PSIL="../build/src/psil" clitests/testall.sh
```

# How to use

You can just run the executable and you'll be in REPL mode (ctrl-c to exit should work, or `(quit)`),
or specify an input file like `-f <file>`, the effect is the same as if you
had put it there yourself.

When reading from a file, REPL is still enabled by default and can be disabled with `--repl-`

## Options

You can also change some of the behaviours of the interpreter:

`--cell_limit:limit` (for example `--cell_limit:10000`) - limit the amount of cells that can be used

`--command_strs[+/-]` (for example `--command_strs+`) - use string representation of commands, useful for debugging the
compiler with verbose logging

`--default_log_level:level` (for example `--default_log_level:3`) - change the default logging level

`--repl[+/-]` (for example `--repl-`) - enable/disable repl

`--gc_threshold:threshold` (for example `--gc_threshold:70`) - percentage of cell_limit at which concurrent gc is
triggered

### Log options

Log can be configured in format of `--log:TAG:LEVEL` (for example `--log:VM:3`)

Where level is a number from 0 to 4 (0 - no logs, 4 - most logs)

Possible values for tags are:

`MemoryContext` - at level 2 it will print GC stats for each run, level 3 a little more stats, level 4 is debugging mode

`Compiler` - level 3 will print the compilation result of everything evaluated, you also probably want to
have `--command_strs+` enabled with it

`VM` - level 3 will print function application and return info, level 4 is debugging mode which will print every
instruction and the machine state before/after

# Some cooler examples

See the GC in action:

```shell
build/src/psil -f clitests/fib.test.psil --repl- --log:Compiler:3 --command_strs+ --cell_limit:10000 --gc_threshold:10 --log:MemoryContext:2
```

See a tree of function applications:

```shell
build/src/psil -f clitests/coffee.test.psil --repl- --log:Compiler:3 --command_strs+ --log:VM:3
```

Super debug mode:

```shell
build/src/psil -f clitests/decorate.test.psil --repl- --command_strs+ --default_log_level:4 --log:MemoryContext:3
```

# Some notes on the implementation

The implementation is rather straightforward, based mostly on the compiler from "The Architecture of Symbolic
Computers", with little modification and some additions. Notably, the let/letrec is more lisp-like, using racket-like
name-value pairs instead of them being in separate lists like in the book. Also, there's support for top-level functions
using `define`, quoting using `(quote value)`, and a simple concurrent garbage collector.

There are three basic value types which is a string atom, number atom, and a cons cell.

String atoms are basically used only internally, and you can't do much with them other than printing and comparing them.
With number
atoms you can do all the usual arithmetic, and they also serve as bools - any value greater than 0 is considered true
for the purposes of `if`. And of course, all the usual stuff with cons cells - `car`, `cdr`, `cons`...

Other language features were implemented to the extent that was required for writing some simple programs copied from
what I have done for homework - it's in the `clitests` folder (and for simpler examples there's `examples.psil`, and it
probably serves as the best reference for the
language that this interpreter interprets :)

