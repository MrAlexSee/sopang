"""
Generates synthetic elastic-degenerate text.

Usage: generate_synth_ed_text.py <output-file> [options]

Arguments:
  <output-file>                        output file path

Options:
  --alphabet arg                       alphabet used for character sampling [default: ACGTN]
  --degenerate-positions-factor arg    number of segments which are non-deterministic (contain multiple variants), e.g., 0.5 = half of all segments [default: 0.1]
  --max-segment-variants arg           maximum number of variants (a), the number of variants for each non-deterministic segment will be sampled from the interval [2, a] [default: 10]
  --max-variant-size arg               maximum size of each segment variant (b), the size for each variant will be sampled from the interval [0, b] (segments might contain empty words) [default: 10]
  --segment-count arg                  total number of segments [default: 10000]
  -h --help                            show this screen
  -v --version                         show version
"""

from docopt import docopt

import random

def randomString(alphabet, size):
    sigma = len(alphabet)
    return "".join(alphabet[random.randint(0, sigma - 1)] for _ in range(size))

def transformTextToED(text, degenerateStrings):
    ret = ""

    for charIdx in range(len(text)):
        if charIdx in degenerateStrings:
            variants = degenerateStrings[charIdx]
            assert len(variants) > 1

            ret += "{" + ",".join(variants) + "}"
        else:
            ret += text[charIdx]

    return ret

def generateDegenerateStrings(degenerateSegmentCount, totalSegmentCount, args):
    degeneratePositions = random.sample(range(totalSegmentCount), degenerateSegmentCount)
    ret = {}

    for position in degeneratePositions:
        # Degenerate letter is defined as a "non-empty set of strings".
        variantCount = random.randint(2, int(args["--max-segment-variants"]))
        curSet = set()

        while len(curSet) < variantCount:
            curLen = random.randint(0, int(args["--max-variant-size"])) # Includes empty strings.
            curStr = randomString(args["--alphabet"], curLen)

            curSet.add(curStr)

        ret[position] = curSet

    return ret    

def main():
    args = docopt(__doc__, version="0.1.0")
    totalSegmentCount = int(args["--segment-count"])

    textSizeMB = totalSegmentCount / (2 ** 20)
    print("Started, alphabet = \"{0}\", text size = {1:.3f} MB".format(args["--alphabet"], textSizeMB))

    text = randomString(args["--alphabet"], totalSegmentCount)

    degenerateSegmentCount = int(float(args["--degenerate-positions-factor"]) * totalSegmentCount)

    ratio = degenerateSegmentCount / totalSegmentCount
    print("Generating: degenerate/total counts = {0}/{1}, ratio = {2:.2f}".format(degenerateSegmentCount, totalSegmentCount, ratio))

    # This is a map [position] -> [set of variant strings].
    degenerateStrings = generateDegenerateStrings(degenerateSegmentCount, totalSegmentCount, args)
    edText = transformTextToED(text, degenerateStrings)

    outputFilePath = args["<output-file>"]

    with open(outputFilePath, "w") as f:
        f.write(edText)

    print("Dumped to file: {0}".format(outputFilePath))

if __name__ == "__main__":
    main()
