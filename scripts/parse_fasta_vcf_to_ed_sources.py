"""
Parses a fasta and VCF file pair in order to obtain elastic-degenerate text with sources.
Usage: parse_fasta_vcf_to_ed_sources.py <input_chr.fa> <input_variants.vcf> <output_chr.eds> <output_sources.edss>

Arguments:
  <input_chr.fa>        path to the input fasta (reference) file
  <input_variants.vcf>  path to the input vcf (variants) file
  <output_chr.eds>      path to the output elastic-degenerate text file
  <output_sources.edss> path to the output sources file

Options:
  -h --help       show this screen
  -v --version    show version
"""

from docopt import docopt

import itertools
import vcf # Requires the package "pyvcf".

def readFastaFile(inFastaFilePath):
    print "Reading the entire fasta file from: {0}".format(inFastaFilePath)
    text = ""

    with open(inFastaFilePath, "r") as f:
        for line in f:
            if line[0] == ">":
                continue

            text += line[ : -1].upper()

    textMB = float(len(text)) / 1000.0 / 1000.0
    print "Read input fasta text, MB = {0:.2f}".format(textMB)

    return text

def getSourcesMapFromVCF(vcfReader):
    nProcessedEntries, nIgnoredEntries = 0, 0
    ret = {}

    sampleNameToIndex = {}
    nextSampleIndex = 0

    alphabet = set("ACGTN")

    for record in vcfReader:
        ignoreEntry = False

        for refChar in record.REF:
            if refChar.upper() not in alphabet:
                ignoreEntry = True
                break

        if ignoreEntry:
            nIgnoredEntries += 1
            continue

        for altSequence in record.ALT:
            if type(altSequence) != vcf.model._Substitution:
                ignoreEntry = True
                break

            for altChar in altSequence.sequence:
                if altChar.upper() not in alphabet:
                    ignoreEntry
                    break

            if ignoreEntry:
                break

        if ignoreEntry:
            nIgnoredEntries += 1
            continue

        # We store only the sources where the variation (alt) occurs.
        curSources = {}

        for sample in record.samples:
            # We always take only the 1st index of the diploid.
            altIndex = int(sample.data.GT.split("|")[0])

            if altIndex != 0:
                assert 1 <= altIndex <= len(record.ALT)
                altIndex -= 1

                if record.ALT[altIndex] not in curSources:
                    curSources[record.ALT[altIndex]] = []

                sampleName = sample.sample

                if sampleName not in sampleNameToIndex:
                    sampleNameToIndex[sampleName] = nextSampleIndex
                    nextSampleIndex += 1

                curSources[record.ALT[altIndex]] += [sampleNameToIndex[sampleName]]

        if curSources:
            if record.POS in ret:
                ret[record.POS].update(curSources)
            else:
                ret[record.POS] = curSources

        nProcessedEntries += 1

    print "Processed vcf #entries = {0}, ignored #entries = {1}".format(nProcessedEntries, nIgnoredEntries)
    return ret, nextSampleIndex

def fastaAndSourcesMapToSegments(fastaText, sourcesMap, nSources):
    segments, sources = [], []

    for charIdx in xrange(len(fastaText)):
        if charIdx in sourcesMap:
            curSegment, curSources = [], []
            usedSources = set()

            for kv in sourcesMap[charIdx].iteritems():
                curSegment += [kv[0]]
                curSources += [kv[1]]

                usedSources.update(set(kv[1]))

            # We associate the reference sequence with the remaining sources.
            curSources += [[s for s in xrange(nSources) if s not in usedSources]]

            curSegment = [[fastaText[charIdx]]]

            segments += [curSegment]
            sources += [curSources]
        else:
            segments += [[fastaText[charIdx]]]

    return segments, sources

def parseFastaAndVCF(inFastaFilePath, inVCFFilePath):
    fastaText = readFastaFile(inFastaFilePath)
    vcfReader = vcf.Reader(open(inVCFFilePath, "r"))

    print "Starting buffered processing of vcf file from: {0}".format(inVCFFilePath)
    sourcesMap, nSources = getSourcesMapFromVCF(vcfReader)

    return fastaAndSourcesMapToSegments(fastaText, sourcesMap, nSources)

def sanityCheck(segments, sources):
    assert segments and sources

    sourceIdx = 0
    nNonDeterministicSegments = 0
    
    for segment in segments:
        assert len(segment) > 0

        if len(segment) == 1:
            continue

        assert len(segment) == len(sources[sourceIdx])

        # Make sure that the are no duplicates in all sublists.
        allSources = list(itertools.chain(*sources[sourceIdx]))
        assert len(allSources) == len(set(allSources))

        sourceIdx += 1
        nNonDeterministicSegments += 1

    assert nNonDeterministicSegments == len(sources)
    print "Sanity check passed"

def dumpEDText(segments, outFilePath):
    text = ""
    step = 100

    print "Buffered dumping of ED text to: {0} for #segments = {1} with step = {2}".format(outFilePath, len(segments), step)
    fileHandle = open(outFilePath, "a")

    fileHandle.truncate()

    for segmentIdx, segment in enumerate(segments):
        if segmentIdx != 0 and segmentIdx % step == 0:
            fileHandle.write(text)
            text = ""

        if len(segment) == 1:
            text += "{" + segment[0] + "}"
        else:
            text += "{" + ",".join(segment) + "}"

    fileHandle.close()
    print "Finished"

def dumpSources(sources, outFilePath):
    text = ""
    step = 100

    print "Buffered dumping of sources to: {0} for non-det #segments = {1} with step = {2}".format(outFilePath, len(sources), step)
    fileHandle = open(outFilePath, "a")

    fileHandle.truncate()

    for sourcesIdx, sourcesForSegment in enumerate(sources):
        if segmentIdx != 0 and segmentIdx % step == 0:
            fileHandle.write(text)
            text = ""

        textForSegment = []

        for sourcesForVariant in sourcesForSegment:
            textForSegment += ["{" + ",".join([str(v) for v in sourcesForVariant]) + "}"]

        text += "{" + "".join(textForSegment) + "}"

    fileHandle.close()
    print "Finished"

def main():
    args = docopt(__doc__, version="0.1.0")

    segments, sources = parseFastaAndVCF(args["<input_chr.fa>"], args["<input_variants.vcf>"])
    sanityCheck(segments, sources)

    dumpEDText(segments, args["<output_chr.eds>"])
    dumpSources(sources, args["<output_sources.edss>"])

if __name__ == "__main__":
    main()
