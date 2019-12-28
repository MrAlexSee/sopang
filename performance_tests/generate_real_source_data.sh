#!/bin/sh

# These paths should be relative to the root folder of this repository.
dataDirPath="data"
scriptsDirPath="scripts/parse_fasta_vcf_cpp"

inputFastaFileName="hs37d5.fa"
maxChromosomeId=23

cd ..
mkdir -p ${dataDirPath}
cd ${scriptsDirPath}
make rebuild
cd ../..

for i in $(seq 1 ${maxChromosomeId});
do
    echo $i

    ./${scriptsDirPath}/parse_fasta_vcf ${dataDirPath}/${inputFastaFileName} ${dataDirPath}/chr${i}.vcf ${dataDirPath}/chr${i}.edz ${dataDirPath}/chr${i}.edsz
done
