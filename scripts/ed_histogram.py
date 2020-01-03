"""
Prints the histogram of character counts in deterministic/non-deterministic segments.

Usage: ed_histogram.py <input-dir> [options]

Arguments:
  <input-dir>       path to the input directory containing .eds files

Options:
  -h --help         show this screen
  -v --version      show version
"""

from docopt import docopt

import os
import re


def processFile(filePath, sizes):
    print("Reading file: {0}".format(filePath))

    with open(filePath, "r") as f:
        text = f.read()

    # The question mark allows for non-greedy matching.
    detSegmentsText = re.sub(r"\{.+?\}", "", text)

    detSizeMB = len(detSegmentsText) / (2 ** 20)
    totalSizeMB = len(text) / (2 ** 20)
    ratio = detSizeMB / totalSizeMB

    sizes += [[filePath, detSizeMB, totalSizeMB, ratio]]


def main():
    args = docopt(__doc__, version="0.1.0")
    inputDirPath = args["<input-dir>"]

    edsFileNames = [fileName for fileName in os.listdir(inputDirPath) if fileName.endswith(".eds")]
    edsFilePaths = [os.path.join(inputDirPath, fileName) for fileName in edsFileNames]

    sizes = []

    for edsFilePath in edsFilePaths:
        processFile(edsFilePath, sizes)

    for item in sorted(sizes):
        print("File: {0} : deterministic size = {1:.3f} MB, total size = {2:.3f} MB, ratio = {3:.3f}".format(*item))


if __name__ == "__main__":
    main()
