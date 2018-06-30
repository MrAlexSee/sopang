#!/bin/sh

inDir="data"
outFile="out"

for i in $(seq 1 27);
do
    ./sopang -d -o ${outFile}8.txt $inDir/chr${i}.eds $inDir/patterns8.txt;
    ./sopang -d -o ${outFile}16.txt $inDir/chr${i}.eds $inDir/patterns16.txt;
    ./sopang -d -o ${outFile}32.txt $inDir/chr${i}.eds $inDir/patterns32.txt;
    ./sopang -d -o ${outFile}64.txt $inDir/chr${i}.eds $inDir/patterns64.txt;
done
