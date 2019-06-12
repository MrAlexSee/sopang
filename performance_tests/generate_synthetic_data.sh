#!/bin/sh

dataDirPath="data"
scriptsDirPath="scripts"

additionalArgs="--alphabet ACGTN --degenerate-positions-factor 0.1 --max-segment-variants 10 --max-variant-size 10"

cd ..

mkdir -p ${dataDirPath}

python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr25.eds --segment-count 100000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr26.eds --segment-count 500000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr27.eds --segment-count 1000000 ${additionalArgs}
python3 ./${scriptsDirPath}/generate_synth_ed_text.py ${dataDirPath}/chr28.eds --segment-count 1600000 ${additionalArgs}

echo ""

python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr25.eds ${dataDirPath}/chr25.edss --source-count 10
python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr26.eds ${dataDirPath}/chr26.edss --source-count 10
python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr27.eds ${dataDirPath}/chr27.edss --source-count 10
python3 ./${scriptsDirPath}/generate_synth_sources.py ${dataDirPath}/chr28.eds ${dataDirPath}/chr28.edss --source-count 10
