#! /bin/bash
cd "$(dirname "$0")"

FAILED=()

for FILE in *.test.psil; do
      echo "TESTING $FILE"
      $PSIL -f $FILE --repl- --default_log_level:0 > $FILE.res
      if [ $? -ne 0 ]; then
          FAILED+=("test-"$FILE)
          continue
      fi

      diff -w $FILE.res $FILE.ex

      if [ $? -ne 0 ]; then
          FAILED+=("test-"$FILE)
          continue
      fi

       echo "$FILE OK"
      rm $FILE.res
done

if [ ${#FAILED[@]} -eq 0 ]; then
    echo "ALL TESTS PASSED"
else
    echo "FAILED: ${FAILED[@]}"
    exit -1
fi