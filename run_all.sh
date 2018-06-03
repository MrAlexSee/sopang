#!/bin/sh

inputDir="data"
outFile="out"

for i in $(seq 1 27);
do
    ./sopang -d -o ${outFile}8.txt $inputDir/chr${i}.eds $inputDir/patterns8.txt;
    ./sopang -d -o ${outFile}16.txt $inputDir/chr${i}.eds $inputDir/patterns16.txt;
    ./sopang -d -o ${outFile}32.txt $inputDir/chr${i}.eds $inputDir/patterns32.txt;
    ./sopang -d -o ${outFile}64.txt $inputDir/chr${i}.eds $inputDir/patterns64.txt;
done
