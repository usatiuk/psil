#! /bin/bash
cd "$(dirname "$0")"

FAILED=()

PSIL="../cmake-build-debug/src/psil"

for FILE in *.psil; do
      echo "TESTING $FILE"
      $PSIL -f $FILE --repl- > $FILE.res
      if [ $? -ne 0 ]; then
          FAILED+=("test-"$FILE)
          continue
      fi

      diff -w $FILE.res $FILE.ex

      if [ $? -ne 0 ]; then
          FAILED+=("test-"$FILE)
          continue
      fi

      rm $FILE.res
done

if [ ${#FAILED[@]} -eq 0 ]; then
    echo "ALL TESTS PASSED"
else
    echo "FAILED: ${FAILED[@]}"
    exit -1
fi