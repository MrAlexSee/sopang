#!/bin/sh

# These paths should be relative to the root folder of this repository.
dataDirPath="data"
scriptsDirPath="scripts"

inputFastaFileName="hs37d5.fa"
maxChromosomeId=24

cd ..
mkdir -p ${dataDirPath}

for i in $(seq 1 ${maxChromosomeId});
do
    echo $i

    python3 ./${scriptsDirPath}/parse_fasta_vcf.py ${dataDirPath}/${inputFastaFileName} ${dataDirPath}/chr${i}.vcf ${dataDirPath}/chr${i}.edz ${dataDirPath}/chr${i}.edsz --compress
done
