#!/bin/sh

# These paths should be relative to the root folder of this repository.
dataDirPath="data"
scriptsDirPath="scripts"

additionalArgs="--alphabet ACGTN --degenerate-positions-factor 0.1 --max-segment-variants 10 --max-variant-size 10"

cd ..

mkdir -p ${dataDirPath}

python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr25.eds --segment-count 100000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr26.eds --segment-count 500000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr27.eds --segment-count 1000000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr28.eds --segment-count 1600000 ${additionalArgs}

for sourceCount in 16 128 1024 8192 16000
do
    python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr25.eds ${dataDirPath}/chr25_${sourceCount}.edss --source-count ${sourceCount}

    python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr26.eds ${dataDirPath}/chr26_${sourceCount}.edss --source-count ${sourceCount}

    python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr27.eds ${dataDirPath}/chr27_${sourceCount}.edss --source-count ${sourceCount}

    python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr28.eds ${dataDirPath}/chr28_${sourceCount}.edss --source-count ${sourceCount}
done
