#!/bin/bash

mkdir -p output
for BASE in `seq -w 2 2 56`; do
    ./treesearch --heuristic -b $BASE | tee output/$BASE.txt &
done
