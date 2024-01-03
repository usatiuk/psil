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
