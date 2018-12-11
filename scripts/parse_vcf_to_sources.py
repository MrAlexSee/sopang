"""
Parses a VCF file in order to obtain sources for elastic-degenerate text.
"""

# Path to the input VCF file.
pInVCFFilePath = "test.vcf"

# Path to the output sources file.
pOutEDSourcesFilePath = "sources.edss"

def extractSourcesFromVCF(inFilePath):
    # TODO
    return []

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
    sources = extractSourcesFromVCF(pInVCFFilePath)
    dumpSources(sources, pOutEDSourcesFilePath)

if __name__ == "__main__":
    main()
