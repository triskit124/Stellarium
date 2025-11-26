#! /usr/bin/env bash

script_dir="$(dirname $(realpath $0))"

CODE=0
LOGFILE=${script_dir}/test_latest.log

if [[ -f "$LOGFILE" ]]; then
    rm $LOGFILE
fi
touch $LOGFILE

# run each executible in build/test/
for test in $script_dir/../build/test/test_*; do
    if [ -x $test ]; then
        if ! $test >> $LOGFILE; then
            CODE=1
        fi
    fi
done

exit $CODE
