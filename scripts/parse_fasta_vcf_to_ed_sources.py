"""
Parses a fasta and VCF file pair in order to obtain elastic-degenerate text with sources.
"""

import itertools
import vcf

# Input fasta file path.
pInFastaFilePath = "/home/alex/Documents/Chromosomes/danio_rerio_chr1.fa"

# Input VCF file path.
pInVCFFilePath = "/home/alex/Documents/Chromosomes/danio_rerio.vcf"

# Input chromosome ID.
pChromosomeID = 1

# Output elastic-degenerate text file path.
pOutEDTextFilePath = "text.eds"

# Output sources file path.
pOutEDSourcesFilePath = "sources.edss"

def parseFastaAndVCF(inFastaFilePath, inVCFFilePath, chromosomeID):
    with open(inFastaFilePath, "r") as f:
        lines = [l for l in f.read().split("\n")[1 : ] if l]

    text = "".join(lines)
    textMB = float(len(text)) / 1000.0 / 1000.0

    print "Read text, MB = {0:.2f}".format(textMB)

    vcfReader = vcf.Reader(open(inVCFFilePath, "r"))
    nProcessedEntries = 0

    for record in vcfReader:
        chromID = int(record.CHROM)

        if chromID == chromosomeID:
            nProcessedEntries += 1
            pass
        elif chromID > chromosomeID:
            break

    print "Processed vcf #entries = {0} for chromosome = {1}".format(nProcessedEntries, chromosomeID)

    return [], []

def sanityCheck(segments, sources):
    assert segments and sources

    iSource = 0
    nNonDeterministicSegments = 0
    
    for segment in segments:
        assert len(segment) > 0

        if len(segment) == 1:
            continue

        assert len(segment) == len(sources[iSource])

        # Make sure that the are no duplicates in all sublists.
        allSources = list(itertools.chain(*sources[iSource]))
        assert len(allSources) == len(set(allSources))

        iSource += 1
        nNonDeterministicSegments += 1

    assert nNonDeterministicSegments == len(sources)

def dumpEDText(segments, outFilePath):
    text = ""

    for segment in segments:
        if len(segment) == 1:
            text += "{" + segment[0] + "}"
        else:
            text += "{" + ",".join(segment) + "}"

    with open(outFilePath, "w") as f:
        f.write(text)

    print("Dumped ed text to: {0} for #segments = {1}".format(outFilePath, len(segments)))

def dumpSources(sources, outFilePath):
    text = ""

    for sourcesForSegment in sources:
        textsForSegment = []

        for sourcesForVariant in sourcesForSegment:
            textsForSegment += ["{" + ",".join([str(v) for v in sourcesForVariant]) + "}"]

        text += "{" + "".join(textsForSegment) + "}"

    with open(outFilePath, "w") as f:
        f.write(text)

    print("Dumped sources to: {0} for #segments = {1}".format(outFilePath, len(sources)))

def main():
    segments, sources = parseFastaAndVCF(pInFastaFilePath, pInVCFFilePath, pChromosomeID)
    sanityCheck(segments, sources)

    dumpEDText(segments, pOutEDTextFilePath)
    dumpSources(sources, pOutEDSourcesFilePath)

if __name__ == "__main__":
    main()
