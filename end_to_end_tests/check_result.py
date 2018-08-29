"""
Checks whether the result (number of matches) from sopang is the same as the desired values.
"""

import sys

pInFile = "res.txt"

def readMatchesFromFile(inFile):
    with open(pInFile, "r") as f:
        data = f.read()

    lines = data.split("\n")
    lines = [l for l in lines if l]

    matches = []

    for l in lines:
        if l.startswith("#results"):
            matches += [int(l.split()[-1])]

    print "Got #matches = {0} from file".format(len(matches))
    return matches

def readMatchesFromCmdLine():
    matches = sys.argv[1].split()
    matches = [int(n) for n in matches]

    print "Got #matches = {0} from cmd-line".format(len(matches))
    return matches

def main():
    fileMatches = readMatchesFromFile(pInFile)
    cmdMatches = readMatchesFromCmdLine()

    if fileMatches == cmdMatches:
        print "OK (matches = {0})".format(fileMatches)
    else:
        print "ERROR: mismatches"

if __name__ == "__main__":
    main()
