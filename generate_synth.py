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

nSegments = 100 * 1000 # 100, 500, 1000, 1600 thousands segments

alphabet = "ACGTN"

nDegeneratePositions = int(0.1 * nSegments) # Number of degenerate positions, 10 % of the text as in Grossi et al.
nMaxSegmentVariants = 10
nMaxVariantLength = 10

outFile = "text.eds"

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
            assert len(curStrings) > 0

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

    with open(outFile, "w") as f:
        f.write(outStr)

    print "Dumped to file: {0}".format(outFile)

def main():
    textSizeMB = round(nSegments / 1000.0 / 1000.0, 3)
    print "Started, alph = \"{0}\", text size = {1}m".format(alphabet, textSizeMB)

    text = randomString(alphabet, nSegments)

    degenPosSet = set() # (randomly drawn) degenerate positions
    while len(degenPosSet) < nDegeneratePositions:
        cur = random.randint(0, nSegments - 1)
        degenPosSet.add(cur)

    degenPosList = sorted(list(degenPosSet))
    degenStrings = {} # Dictionary: position in text -> list of a few strings

    print "Generating degenerate strings..."

    for iD in xrange(nDegeneratePositions):
        curPos = degenPosList[iD]

        howMany = random.randint(1, nMaxSegmentVariants) # Degenerate letter is defined as a "non-empty set of strings".
        curSet = set()

        while len(curSet) < howMany:
            curLen = random.randint(0, nMaxVariantLength) # Includes empty strings.
            curStr = randomString(alphabet, curLen)
            curSet.add(curStr)

        degenStrings[curPos] = curSet

    assert len(degenPosList) == len(degenStrings) == nDegeneratePositions
    print "Generated degenerate strings at #positions = {0}k".format(nDegeneratePositions / 1000.0)

    dumpToFile(text, set(degenPosList), degenStrings)

if __name__ == "__main__":
    main()
