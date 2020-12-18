#!/bin/bash

mkdir -p output
for BASE in `seq -w 2 2 54`; do
    ./treesearch --heuristic -s 10 -b $BASE | tee output/$BASE.txt &
done
