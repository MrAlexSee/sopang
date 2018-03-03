#!/bin/sh

for i in $(seq 1 27);
do
    ./sopang $i 8;
    ./sopang $i 16;
    ./sopang $i 32;
    ./sopang $i 64;
done
