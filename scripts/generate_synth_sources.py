"""
Generates a synthetic sources file for elastic-degenerate text.
Each source (input individual) is associated with exactly one variant from each non-deterministic segment.
Each variant is associated with some sources.

Usage: generate_synth_sources.py <input-ed-file> <output-src-file> [options]

Options:
  --source-count arg           total number of sources (input individuals) [default: 10]
  --include-last-sequence      dump sources for the last sequence (results in a longer but explicit sources file)
"""

from docopt import docopt

import random

def readEDSegments(inFilePath):
    with open(inFilePath, "r") as f:
        text = f.read()

    ret = []
    detCharacterCount = 0

    for part in text.split("{"):
        if "}" in part:
            parts = part.split("}")
            assert len(parts) == 2

            # This is a non-deterministic segment.
            assert "," in parts[0]

            ret += [parts[0].split(",")]

            # This is either a deterministic segment or an empty string
            # when non-deterministic segments are contiguous.
            assert "," not in parts[1]

            if parts[1]:
                ret += [[parts[1]]]
                detCharacterCount += len(parts[1])
        else:
            ret += [[part]]

    print("Parsed #segments = {0}, deterministic #characters = {1}".format(len(ret), detCharacterCount))
    return ret

def generateSources(segments, sourceCount, includeLast):
    sources = []
    
    for segment in segments:
        # Sources are generated only for non-deterministic segments.
        if len(segment) == 1:
            continue

        if sourceCount < len(segment):
            raise Exception("not enough sources for segment: {0} < {1}".format(sourceCount, len(segment)))

        sourcesForSegment = []
        
        sourcesToSelect = [i for i in range(sourceCount)]
        random.shuffle(sourcesToSelect)

        startIdx = 0
        stepIdx = int(sourceCount / len(segment))

        for variantIdx in range(len(segment)):
            if variantIdx != len(segment) - 1:
                sourcesForSegment += [sourcesToSelect[startIdx : startIdx + stepIdx]]
            elif includeLast:
                sourcesForSegment += [sourcesToSelect[startIdx : ]]

            startIdx += stepIdx

        sources += [sourcesForSegment]

        if includeLast:
            assert len(sourcesForSegment) == len(segment)
            assert sum([len(s) for s in sourcesForSegment]) == sourceCount
        else:
            assert len(sourcesForSegment) == len(segment) - 1
            assert sum([len(s) for s in sourcesForSegment]) < sourceCount

    return sources

def dumpSources(sources, sourceCount, outFilePath):
    text = "{0}\n".format(sourceCount)

    for sourcesForSegment in sources:
        textsForSegment = []

        for sourcesForVariant in sourcesForSegment:
            textsForSegment += ["{" + ",".join([str(v) for v in sourcesForVariant]) + "}"]

        text += "{" + "".join(textsForSegment) + "}"

    with open(outFilePath, "w") as f:
        f.write(text)

    print("Dumped sources to: {0}, #sources = {1}".format(outFilePath, sourceCount))

def main():
    args = docopt(__doc__, version="0.1.0")
    sourceCount = int(args["--source-count"])

    segments = readEDSegments(args["<input-ed-file>"])
    sources = generateSources(segments, sourceCount, args["--include-last-sequence"])

    ratio = len(sources) / len(segments)
    print("Non-det/det segment ratio = {0:.2f}".format(ratio))

    dumpSources(sources, sourceCount, args["<output-src-file>"])

if __name__ == "__main__":
    main()
