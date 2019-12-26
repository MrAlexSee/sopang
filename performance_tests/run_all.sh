#!/bin/sh

# These paths should be relative to the root folder of this repository.
dataDirPath="data"
outputFileName="results"
sopangExe="sopang"

maxChromosomeId=28

cd ..

if [ ! -f ${sopangExe} ];
then
    echo "Executable does not exist: ${sopangExe}"
    exit 1
fi

# Running regular ED-text matching.
for i in $(seq 1 ${maxChromosomeId});
do
    echo $i
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns8.txt -d -o ${outputFileName}_8.txt
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns16.txt -d -o ${outputFileName}_16.txt
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns32.txt -d -o ${outputFileName}_32.txt
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns64.txt -d -o ${outputFileName}_64.txt
done

echo ""

# Running ED-text matching with sources.
for i in $(seq 1 ${maxChromosomeId});
do
    echo $i
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns8.txt -d -o ${outputFileName}_8_sources.txt --in-sources-file ${dataDirPath}/chr${i}.edss
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns16.txt -d -o ${outputFileName}_16_sources.txt --in-sources-file ${dataDirPath}/chr${i}.edss
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns32.txt -d -o ${outputFileName}_32_sources.txt --in-sources-file ${dataDirPath}/chr${i}.edss
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns64.txt -d -o ${outputFileName}_64_sources.txt --in-sources-file ${dataDirPath}/chr${i}.edss
done
