"""
Generates a synthetic sources file for elastic-degenerate text.
Each source (input individual) is associated with exactly one variant from each non-deterministic segment.
"""

import random

# Path to the input file with elastic-degenerate text for which the sources will be generated.
pInEDTextFilePath = "../sample/chr1337.eds"

# Path to the output sources file.
pOutEDSourcesFilePath = "sources.edss"

# Total number of sources (input individuals). Must be at least as large as the maximum number of variants in a non-deterministic segment in the input elastic-degenerate text.
pNSources = 10

def readEDSegments(inFilePath):
    with open(inFilePath, "r") as f:
        data = f.read()

    segments = []

    for part in data.split("{"):
        # This is a deterministic segment.
        if "}" not in part:
            segments += [[part]]
        else:
            curParts = part.split("}")
            assert len(curParts) == 2

            # This is a non-deterministic segment.
            assert "," in curParts[0]
            segments += [curParts[0].split(",")]

            # This is either a deterministic segment or an empty string
            # when non-deterministic segments are contiguous.
            assert "," not in curParts[1]
            if curParts[1]:
                segments += [[curParts[1]]]

    print "Parsed #segments = {0}".format(len(segments))
    return segments

def generateSources(segments, nSources):
    sources = []
    
    for segment in segments:
        # Sources are generated only for non-deterministic segments.
        if len(segment) == 1:
            continue

        if nSources < len(segment):
            print "Not enough sources for segment: {0} < {1}".format(nSources, len(segment))
            return []

        sourcesForSegment = []
        
        sourcesToSelect = [i for i in xrange(nSources)]
        random.shuffle(sourcesToSelect)

        iStart = 0
        iStep = int(nSources / len(segment))

        for iVariant in xrange(len(segment)):
            if iVariant != len(segment) - 1:
                sourcesForVariant = sourcesToSelect[iStart : iStart + iStep]
            else:
                sourcesForVariant = sourcesToSelect[iStart : ]

            sourcesForSegment += [sourcesForVariant]
            iStart += iStep

        assert len(sourcesForSegment) == len(segment)
        assert sum([len(s) for s in sourcesForSegment]) == nSources

        sources += [sourcesForSegment]

    return sources

def dumpSources(sources, outFilePath):
    text = ""

    for sourcesForSegment in sources:
        textsForSegment = []

        for sourcesForVariant in sourcesForSegment:
            textsForSegment += ["{" + ",".join([str(v) for v in sourcesForVariant]) + "}"]

        text += "{" + "".join(textsForSegment) + "}"

    with open(outFilePath, "w") as f:
        f.write(text)

    print "Dumped sources to: {0} for non-det #segments = {1}".format(outFilePath, len(sources))

def main():
    segments = readEDSegments(pInEDTextFilePath)
    sources = generateSources(segments, pNSources)

    if not sources:
        return

    print "Non-det/det segment ratio = {0:.2f}".format(float(len(sources)) / len(segments))
    dumpSources(sources, pOutEDSourcesFilePath)

if __name__ == "__main__":
    main()
