#!/bin/sh

# These paths should be relative to the root folder of this repository.
dataDirPath="data"
outputFileName="results"
sopangExe="sopang"

minChromosomeId=25
maxChromosomeId=28

cd ..

if [ ! -f ${sopangExe} ];
then
    echo "Executable does not exist: ${sopangExe}"
    exit 1
fi

# Running regular ED-text matching.
for i in $(seq ${minChromosomeId} ${maxChromosomeId});
do
    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns8.txt -d -o ${outputFileName}_8.txt

    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns16.txt -d -o ${outputFileName}_16.txt

    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns32.txt -d -o ${outputFileName}_32.txt

    ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns64.txt -d -o ${outputFileName}_64.txt
done

# Running ED-text matching with sources.
for i in $(seq ${minChromosomeId} ${maxChromosomeId});
do
    for sourceCount in 16 128 1024 8192 16000
    do
        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns8.txt -d -o ${outputFileName}_8_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss
        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns8.txt -d -o ${outputFileName}_8_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss --full-sources-output

        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns16.txt -d -o ${outputFileName}_16_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss
        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns16.txt -d -o ${outputFileName}_16_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss --full-sources-output

        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns32.txt -d -o ${outputFileName}_32_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss
        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns32.txt -d -o ${outputFileName}_32_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss --full-sources-output

        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns64.txt -d -o ${outputFileName}_64_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss
        ./${sopangExe} ${dataDirPath}/chr${i}.eds ${dataDirPath}/patterns64.txt -d -o ${outputFileName}_64_sources.txt --in-sources-file ${dataDirPath}/chr${i}_${sourceCount}.edss --full-sources-output
    done
done
