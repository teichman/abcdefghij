#!/bin/bash

mkdir -p output
for BASE in `seq -w 2 20`; do
    ./treesearch -b $BASE | tee output/$BASE.txt
done
