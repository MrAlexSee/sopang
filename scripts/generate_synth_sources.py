"""
Generates a synthetic sources file for elastic-degenerate text.
"""

import random
import re

# Path to the input file with elastic degenerate text for which the sources will be generated.
pInEDTextFilePath = "../sample/chr1337.eds"

# Path to the output sources file.
pOutEDSourcesFilePath = "sources.edss"

# Total number of sources.
pNSources = 10

# Maximum number of sources.
# For each non-deterministic segment variant, there will be [1, pMaxNSources] sources.
pMaxNSources = 3

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

def generateSources(segments, nSources, maxNSources):
    sources = []
    
    for segment in segments:
        # Sources are generated only for non-deterministic segments.
        if len(segment) == 1:
            continue

        sourcesForSegment = []
        for _ in xrange(len(segment)):
            sourcesForVariant = []

            curNSources = random.randint(1, maxNSources)
            sourcesForVariant += [random.randint(0, nSources - 1) for _ in xrange(curNSources)]
                        
            sourcesForSegment += [sourcesForVariant]

        sources += [sourcesForSegment]

    return sources

def dumpSources(sources, outFilePath):
    if not sources:
        return

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
    segments = readEDSegments(pInEDTextFilePath)
    sources = generateSources(segments, pNSources, pMaxNSources)

    dumpSources(sources, pOutEDSourcesFilePath)

if __name__ == "__main__":
    main()
