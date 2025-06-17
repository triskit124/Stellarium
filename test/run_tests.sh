#! /bin/bash

script_dir="$(dirname $(realpath $0))"

# run each executible in build/test/
for file in $script_dir/../build/test/test_*; do
    if [ -x $file ]; then
        $file
    fi
done
