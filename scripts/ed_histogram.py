"""
Prints the histogram of character counts in deterministic/non-deterministic segments 
for all .eds files from a given folder.
"""

import os
import re

# Input directory path containing .eds ED text files.
pInDir = "../sample"

def main():
    res = []
    edsFiles = [os.path.join(pInDir, f) for f in os.listdir(pInDir) if f.endswith(".eds")]

    for edsFile in edsFiles:
        print "Reading file: {0}".format(edsFile)

        with open(edsFile, "r") as f:
            data = f.read()

        # The question mark allows for non-greedy matching.
        detFiltered = re.sub(r"\{.+?\}", "", data)

        sizeMB = len(data) / 1024.0 / 1024.0
        sizeFiltMB = len(detFiltered) / 1024.0 / 1024.0
        ratio = sizeFiltMB / sizeMB

        res += [(edsFile, round(sizeFiltMB, 2), round(sizeMB, 2), round(ratio, 2))]
        res.sort(key = lambda t: t[0])

    for t in res:
        print t

if __name__ == "__main__":
    main()
