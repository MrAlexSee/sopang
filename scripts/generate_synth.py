"""
Grossi et al., On-line pattern matching on similar texts, CPM'17

page 11:
Synthetic data. Synthetic ED texts were created randomly (uniform distribution over
the DNA alphabet) with n ranging from 100,000 to 1,600,000; and the percentage of
degenerate positions was set to 10%. For each degenerate position within the synthetic ED
texts, the number of strings was chosen randomly, with an upper bound set to 10. The
length of each string of a degenerate position was chosen randomly, with an upper bound
again set to 10. Every non-degenerate position within the synthetic ED texts contained a
single letter. Four different patterns of length m = 8, 16, 32, or 64 were given as input to all
three programs, along with the aforementioned synthetic ED texts, resulting in four sets of
output.
"""

import random

pNSegments = 100 * 1000 # Total number of segments: 100, 500, 1000, 1600 thousands segments.
pAlphabet = "ACGTN" # Alphabet for character sampling.

# Number of segments (must be smaller than or equal to nSegments) which are non-deterministic,
# i.e. contain multiple variants.
pNDegeneratePositions = int(0.1 * pNSegments) # 10% of the text as in Grossi et al.

# Maximum number of variants (a), the number of variants for each non-deterministic segment
# will be sampled from the interval [2, a].
pNMaxSegmentVariants = 10

# Maximum length of each segment variant (b), the length for each variant 
# will be sampled from the interval [0, b] (segments might contain empty words).
pNMaxVariantLength = 10

pOutFile = "text.eds" # Output file path.

def main():
    textSizeMB = round(pNSegments / 1000.0 / 1000.0, 3)
    print "Started, alph = \"{0}\", text size = {1}m".format(pAlphabet, textSizeMB)

    text = randomString(pAlphabet, pNSegments)

    # Randomly drawn degenerate positions.
    degenPosList = random.sample(xrange(pNSegments), pNDegeneratePositions)
    # Dictionary: position in text -> list of a few strings.
    degenStrings = {}

    print "Generating degenerate strings for #positions = {0}k".format(pNDegeneratePositions / 1000.0)

    for curPos in degenPosList:
        # Degenerate letter is defined as a "non-empty set of strings".
        howMany = random.randint(2, pNMaxSegmentVariants)
        curSet = set()

        while len(curSet) < howMany:
            curLen = random.randint(0, pNMaxVariantLength) # Includes empty strings.
            curStr = randomString(pAlphabet, curLen)
            curSet.add(curStr)

        degenStrings[curPos] = curSet

    assert len(degenPosList) == len(degenStrings) == pNDegeneratePositions
    dumpToFile(text, set(degenPosList), degenStrings)

def randomString(alph, size):
    sigma = len(alph)
    return "".join(alph[random.randint(0, sigma - 1)] for _ in xrange(size))

def dumpToFile(text, degenPosSet, degenStrings):
    print "Generating output elastic-degenerate text..."
    outStr = ""

    for i in xrange(len(text)):
        if i not in degenPosSet:
            outStr += text[i]
        else:
            curStrings = list(degenStrings[i])
            assert curStrings

            if len(curStrings) == 1:
                outStr += curStrings[0]
                continue
            else:
                curRun = "{"

                for iD in xrange(len(curStrings)):
                    curRun += curStrings[iD]

                    if iD != len(curStrings) - 1:
                        curRun += ","

                outStr += curRun + "}"

    with open(pOutFile, "w") as f:
        f.write(outStr)

    print "Dumped to file: {0}".format(pOutFile)

if __name__ == "__main__":
    main()
