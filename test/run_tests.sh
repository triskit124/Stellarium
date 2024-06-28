#! /bin/bash

# run each executible in build/test/
for file in ../build/test/test_*; do
    if [ -x $file ]; then
        $file
    fi
done
