#!/bin/sh

outputDirPath="data"
vcfRootAddress="ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502"

cd ..
mkdir -p ${outputDirPath}

wget "ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/technical/reference/phase2_reference_assembly_sequence/hs37d5.fa.gz" -O ${outputDirPath}/hs37d5.fa.gz
tar -xzf ${outputDirPath}/hs37d5.fa.gz

maxChromosomeId=22

for i in $(seq 1 ${maxChromosomeId});
do
    wget "${vcfRootAddress}/ALL.chr${i}.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz" -O ${outputDirPath}/chr${i}.vcf.gz
    tar -xzf ${outputDirPath}/chr${i}.vcf.gz
done

wget "${vcfRootAddress}/ALL.chrX.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz" -O ${outputDirPath}/chrX.vcf.gz
tar -xzf ${outputDirPath}/chrX.vcf.gz

wget "${vcfRootAddress}/ALL.chrY.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz" -O ${outputDirPath}/chrY.vcf.gz
tar -xzf ${outputDirPath}/chrY.vcf.gz
