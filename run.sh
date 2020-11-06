#!/bin/bash

mkdir -p output
for BASE in `seq -w 2 16`; do
    ./basenum -b $BASE | tee output/$BASE.txt &
done
