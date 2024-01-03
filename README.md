# PSIL

a simple lisp interpreter

## How to compile

Should be a regular cmake build, something like

```shell
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=YES
cmake --build build  --parallel $(nproc)
```

should be enough (you can also change the build type for Release and disable sanitize in case it's too slow)

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

You can just run the executable and you'll be in REPL mode,
or specify an input file like `-f <file>`, the effect is the same as if you
had put it there yourself.

When reading from a file, REPL is still enabled by default and can be disabled with `--repl-`

## Options

You can also change some of the behaviours of the interpreter:

```shell
--cell_limit:limit (for example --cell_limit:10000) - limit the amount of cells that can be used
--command_strs[+/-] (for example --command_strs+) - use string representation of commands, useful
                                                    for debugging the compiler with verbose logging
--default_log_level:level (for example --default_log_level:3) - change the default logging level
--repl[+/-] (for example --repl-) - enable/disable repl
```


