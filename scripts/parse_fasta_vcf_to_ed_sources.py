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

def getSourcesMapFromVcfReader(vcfReader):
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

                altSequence = record.ALT[altIndex].sequence

                if altSequence not in curSources:
                    curSources[altSequence] = []

                sampleName = sample.sample

                if sampleName not in sampleNameToIndex:
                    sampleNameToIndex[sampleName] = nextSampleIndex
                    nextSampleIndex += 1

                curSources[altSequence] += [sampleNameToIndex[sampleName]]
            
        if curSources:
            for kv in curSources.iteritems():
                # We ensure that there are no duplicates.
                assert len(kv[1]) == len(set(kv[1]))

            if record.POS in ret:
                ret[record.POS].update(curSources)
            else:
                ret[record.POS] = curSources

        nProcessedEntries += 1

    print("Processed vcf #entries = {0}, ignored #entries = {1}".format(nProcessedEntries, nIgnoredEntries))
    return ret, nextSampleIndex

def parseFastaFile(inFastaFilePath, outTextFilePath, outSourcesFilePath, sourcesMap, nSources):
    inFileHandle = open(inFastaFilePath, "r")
    outTextFileHandle, outSourcesFileHandle = open(outTextFilePath, "w"), open(outSourcesFilePath, "w")

    charIdx = 0
    
    text, sourcesText = "", ""
    step = 500

    for line in inFileHandle:
        if line[0] == ">":
            continue
    
        for curChar in line[ : -1].upper():
            if charIdx in sourcesMap:
                usedSources = set()

                text += "{"
                sourcesText += "{"

                for kv in sourcesMap[charIdx].iteritems():
                    text += kv[0] + ","
                    sourcesText += "{" + ",".join([str(i) for i in sorted(kv[1])]) + "}"

                    usedSources.update(set(kv[1]))

                text += curChar + "}"

                # We associate the reference sequence with the remaining sources.
                curSources = [s for s in xrange(nSources) if s not in usedSources]
                sourcesText += "{" + ",".join([str(i) for i in sorted(curSources)]) + "}}"
            else:
                text += curChar

            charIdx += 1

        if len(text) > step:
            outTextFileHandle.write(text)
            outSourcesFileHandle.write(sourcesText)
            text, sourcesText = "", ""

def main():
    args = docopt(__doc__, version="0.1.0")

    print("Starting buffered processing of vcf file from: {0}".format(args["<input_variants.vcf>"]))
    print("This might take a very long time...")

    vcfReader = vcf.Reader(open(args["<input_variants.vcf>"], "r"))
    sourcesMap, nSources = getSourcesMapFromVcfReader(vcfReader)

    print("Sources map: variant #positions = {0}, #sources = {1}".format(len(sourcesMap), nSources))

    print("\nParsing fasta file from: {0}".format(args["<input_chr.fa>"]))
    parseFastaFile(args["<input_chr.fa>"], args["<output_chr.eds>"], args["<output_sources.edss>"], sourcesMap, nSources)

if __name__ == "__main__":
    main()
