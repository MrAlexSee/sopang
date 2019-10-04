"""
Parses a fasta and VCF file pair in order to obtain elastic-degenerate text with sources.

Usage: parse_fasta_vcf.py <input-chr.fa> <input-variants.vcf> <output-chr.eds> <output-sources.edss> [options]

Arguments:
  <input-chr.fa>             path to the input fasta (reference) file
  <input-variants.vcf>       path to the input VCF (variants) file
  <output-chr.eds>           path to the output elastic-degenerate text file
  <output-sources.edss>      path to the output sources file

Options:
  --compress                 dump compressed files (recommended extensions: .edz .edsz)
  --include-ref-sources      dump sources for a reference sequence (results in a longer but explicit sources file), ignored for compress
  -h --help                  show this screen
  -v --version               show version
"""

from docopt import docopt

import itertools
import sys
import vcf
import zstd


def shouldProcessRecord(record):
    alphabet = set("ACGTN")

    for refChar in record.REF:
        if refChar.upper() not in alphabet:
            return False

    for altSequence in record.ALT:
        if type(altSequence) != vcf.model._Substitution:
            return False

        for altChar in altSequence.sequence:
            if altChar.upper() not in alphabet:
                return False

    return True


def getSourcesMapFromVcfReader(vcfReader, lineCount):
    processedCount, ignoredCount = 0, 0

    # The returned structure is a map: [position] -> [source map].
    ret = {}

    sampleNameToIndex = {}
    nextSampleIndex = 0

    chromosomeId = None

    for recordIdx, record in enumerate(vcfReader, 1):
        processedPercentage = 100.0 * recordIdx / lineCount

        sys.stdout.write("\rRough progress: {0:.2f}%".format(processedPercentage))
        sys.stdout.flush()

        if not shouldProcessRecord(record):
            ignoredCount += 1
            continue

        if not chromosomeId:
            chromosomeId = record.CHROM
        else:
            if chromosomeId != record.CHROM:
                print("Mismatching chromosome IDs: {0} <> {1}".format(chromosomeId, record.CHROM))
                return None

        # We shall store only the sources where the variation (alt) occurs.
        # This is a map [alternative sequence] -> [source indexes].
        curSources = {}

        for sample in record.samples:
            indexes = [int(i) for i in sample.data.GT.split("|")]
            assert len(indexes) == 2

            # We take the 1st index of the diploid as one source and the 2nd index as another source.
            for diploidIndex, altIndex in enumerate(indexes):
                if altIndex == 0: # 0 indicates the reference sequence.
                    continue

                assert 1 <= altIndex <= len(record.ALT)
                altSequence = record.ALT[altIndex - 1].sequence

                if altSequence not in curSources:
                    curSources[altSequence] = set()

                sampleName = sample.sample + "_" + str(diploidIndex)

                if sampleName not in sampleNameToIndex:
                    sampleNameToIndex[sampleName] = nextSampleIndex
                    nextSampleIndex += 1

                curSources[altSequence].add(sampleNameToIndex[sampleName])

        if curSources:
            if record.POS in ret:
                ret[record.POS].update(curSources)
            else:
                ret[record.POS] = curSources

        processedCount += 1

    print("\nProcessed VCF #records = {0}, ignored #records = {1}".format(processedCount, ignoredCount))
    return ret, nextSampleIndex, chromosomeId


def packNumber(x):
    assert isinstance(x, int)
    if x < 128:
        return chr(128 + x)
    else:
        return chr(int(x / 128)) + chr(128 + (x % 128))


def processLineCompressed(line, charIdx, sourcesMap):
    text, sourcesText = "", ""
    processedVcfPositionsCount = 0

    segmentStartMark = chr(127)

    for curChar in line[ : -1].upper():
        if charIdx not in sourcesMap:
            text += curChar
            charIdx += 1
            continue

        assert len(sourcesMap[charIdx]) > 0

        text += "{"
        sourcesText += segmentStartMark

        for altSequence, sourceIndexes in sourcesMap[charIdx].items():
            text += altSequence + ","

            sourceIndexList = sorted(list(sourceIndexes))

            sourcesText += packNumber(len(sourceIndexList))
            sourcesText += packNumber(sourceIndexList[0])

            for i in range(1, len(sourceIndexList), 1):
                sourcesText += packNumber(sourceIndexList[i] - sourceIndexList[i - 1])

        text += curChar + "}"

        charIdx += 1
        processedVcfPositionsCount += 1

    return text, sourcesText, processedVcfPositionsCount


def dumpCompressedFiles(args, text, sourcesText):
    zstdCompressionLevel = 22
    print("Using zstd compression level = {0}".format(zstdCompressionLevel))

    text = zstd.compress(text.encode(), zstdCompressionLevel)

    with open(args["<output-chr.eds>"], "wb") as f:
        f.write(text)

    sourcesText = zstd.compress(sourcesText.encode(), zstdCompressionLevel)

    with open(args["<output-sources.edss>"], "wb") as f:
        f.write(sourcesText)

    print("Dumped compressed ED text to: {0} and ED sources to: {1}".format(args["<output-chr.eds>"], args["<output-sources.edss>"]))


def parseFastaFileCompressed(args, sourcesMap, sourceCount, searchedChromosomeId):
    text = ""
    sourcesText = "{0}\n".format(sourceCount)

    inGenome = False
    charIdx = 0

    processedVcfPositionsCount, processedLinesCount = 0, 0

    for lineIdx, line in enumerate(open(args["<input-chr.fa>"], "r"), 1):
        if line[0] == ">":
            if inGenome: # Encountered the next genome -> finish processing.
                print("Exited genome: {0} at line: {1}".format(searchedChromosomeId, lineIdx))
                break

            curChromosomeId = line[1 : ].split()[0]

            if curChromosomeId == searchedChromosomeId:
                inGenome = True
                print("Entered genome: {0} at line: {1}".format(searchedChromosomeId, lineIdx))
                continue

        if not inGenome:
            continue

        curText, curSourcesText, curProcessedVcfPositionsCount = processLineCompressed(line, charIdx, sourcesMap)

        text += curText
        sourcesText += curSourcesText
        processedVcfPositionsCount += curProcessedVcfPositionsCount

        charIdx += len(line) - 1
        processedLinesCount += 1

    print("\nFinished parsing the fasta file")
    print("Processed VCF #positions = {0}, processed genome #lines = {1}".format(processedVcfPositionsCount, processedLinesCount))

    dumpCompressedFiles(args, text, sourcesText)


def processLine(line, charIdx, sourcesMap, sourceCount):
    text, sourcesText = "", ""
    processedVcfPositionsCount = 0

    for curChar in line[ : -1].upper():
        if charIdx not in sourcesMap:
            text += curChar
            charIdx += 1
            continue

        assert len(sourcesMap[charIdx]) > 0
        text += "{"

        # If there is more than one sources sequence for the current position,
        # we need to enclose them with additional brackets.
        if sourceCount or len(sourcesMap[charIdx]) > 1:
            sourcesText += "{"

        usedSources = set()

        for altSequence, sourceIndexes in sourcesMap[charIdx].items():
            text += altSequence + ","
            sourcesText += "{" + ",".join([str(i) for i in sorted(sourceIndexes)]) + "}"

            usedSources.update(sourceIndexes)

        text += curChar + "}"

        # We associate the reference sequence with the remaining sources only if the source count is provided,
        # i.e. when the explicit sources file has been requested.
        if sourceCount:
            assert len(usedSources) < sourceCount

            curSources = [s for s in range(sourceCount) if s not in usedSources]
            sourcesText += "{" + ",".join([str(i) for i in sorted(curSources)]) + "}"

        if sourceCount or len(sourcesMap[charIdx]) > 1:
            sourcesText += "}"

        charIdx += 1
        processedVcfPositionsCount += 1

    return text, sourcesText, processedVcfPositionsCount


def parseFastaFileBuffered(args, sourcesMap, sourceCount, searchedChromosomeId):
    outTextFileHandle = open(args["<output-chr.eds>"], "w")
    outSourcesFileHandle = open(args["<output-sources.edss>"], "w")

    sourceCountForLine = sourceCount if args["--include-ref-sources"] else 0

    text = ""
    sourcesText = "{0}\n".format(sourceCount)

    charIdx = 0
    inGenome = False

    processedVcfPositionsCount, processedLinesCount = 0, 0

    outBufferSize = 1000
    bufferedWritesCount = 0

    for lineIdx, line in enumerate(open(args["<input-chr.fa>"], "r"), 1):
        if line[0] == ">":
            if inGenome: # Encountered the next genome -> finish processing.
                print("Exited genome: {0} at line: {1}".format(searchedChromosomeId, lineIdx))
                break

            curChromosomeId = line[1 : ].split()[0]

            if curChromosomeId == searchedChromosomeId:
                inGenome = True
                print("Entered genome: {0} at line: {1}".format(searchedChromosomeId, lineIdx))
                continue

        if not inGenome:
            continue

        curText, curSourcesText, curProcessedVcfPositionsCount = processLine(line, charIdx, sourcesMap, sourceCountForLine)

        text += curText
        sourcesText += curSourcesText
        processedVcfPositionsCount += curProcessedVcfPositionsCount

        charIdx += len(line) - 1
        processedLinesCount += 1

        if len(text) > outBufferSize:
            outTextFileHandle.write(text)
            outSourcesFileHandle.write(sourcesText)

            text, sourcesText = "", ""
            bufferedWritesCount += 1

    print("\nFinished parsing the fasta file")
    print("Processed VCF #positions = {0}, processed genome #lines = {1}".format(processedVcfPositionsCount, processedLinesCount))

    print("Dumped ED text to: {0} and ED sources to: {1}, performed #writes = {2}".format(args["<output-chr.eds>"], args["<output-sources.edss>"], bufferedWritesCount))


def main():
    args = docopt(__doc__, version="0.1.0")

    print("Starting buffered processing of VCF file from: {0}".format(args["<input-variants.vcf>"]))
    print("This might take a very long time...")

    inputFileHandle = open(args["<input-variants.vcf>"], "r")
    lineCount = sum(1 for line in inputFileHandle if not line.startswith("#"))

    inputFileHandle.seek(0)
    vcfReader = vcf.Reader(inputFileHandle)

    sourcesMap, sourceCount, chromosomeId = getSourcesMapFromVcfReader(vcfReader, lineCount)

    print("Sources map: variant #positions = {0}, #sources = {1}".format(len(sourcesMap), sourceCount))
    print("Current chromosome: {0}".format(chromosomeId))

    print("\nParsing fasta file from: {0}".format(args["<input-chr.fa>"]))

    if args["--compress"]:
        parseFastaFileCompressed(args, sourcesMap, sourceCount, chromosomeId)
    else:
        parseFastaFileBuffered(args, sourcesMap, sourceCount, chromosomeId)


if __name__ == "__main__":
    main()
