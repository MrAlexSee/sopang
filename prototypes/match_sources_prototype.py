"""
A prototype for verifying matches based on the sources file.
It is assumed that both text and sources have already been parsed and processed. We only check the verification.
"""

import networkx as nx

def verify(text, sources, pattern, indexToSourceIndex, indexToMatch):
    verifiedMatches = set()

    for matchIndex, match in indexToMatch.items():
        print(f"Verifying: {matchIndex}")

        patternCharIndex = len(pattern) - 1
        charInSegmentIndex = match[0]

        # 1. Go back to the beginning of the segment where the match occurred.
        while charInSegmentIndex >= 0:
           patternCharIndex -= 1
           charInSegmentIndex -= 1

           if patternCharIndex < 0:
               verifiedMatches.add(matchIndex)
               break

        if charInSegmentIndex >= 0:
            print("Matched pattern doesn't span multiple segments")
            break

        # 2. Build a tree of all possible match paths. Branch out each time a match occurs.
        matchTree = nx.DiGraph()

        matchTree.add_node((matchIndex, match[0]), pattern_char_index=patternCharIndex, match_active=True, match_complete=False)
        leafList = [(matchIndex, match[0])]

        segmentIndex = matchIndex - 1

        while segmentIndex >= 0:
            assert nx.is_directed_acyclic_graph(matchTree) and leafList

            print(f"{segmentIndex} -> #leaves = {len(leafList)}")
            newLeafList = []

            # Each time we traverse a segment, we need to update every leaf.
            for leaf in leafList:
                # A deterministic segment -- we simply jump over the entire segment.
                if len(text[segmentIndex]) == 1:
                    matchTree[leaf]["pattern_char_index"] -= len(text[segmentIndex][0])

                    if matchTree[leaf]["pattern_char_index"] < 0:
                        matchTree[leaf]["match_complete"] = True
                else:
                    for variantIndex, variant in enumerate(text[segmentIndex]):
                        matchNode = (segmentIndex, variantIndex)

                        curCharIndex = len(variant) - 1
                        curPatternIndex = leaf["pattern_char_index"]

                        while curCharIndex >= 0:
                            # A match in the middle of this variant -> add a new match complete node.
                            if curPatternIndex < 0:
                                matchTree.add_edge(leaf, matchNode)
                                matchTree[matchNode]["match_complete"] = True
                                break

                            if pattern[curPatternIndex] != variant[curCharIndex]:
                                break

                            curCharIndex -= 1
                            curPatternIndex -= 1

                        # Managed to traverse this whole variant -> add a new continuation node.
                        if curCharIndex < 0:
                            matchTree.add_edge(leaf, matchNode)
                            newLeafList += [matchNode]

                    leafList = newLeafList

                segmentIndex -= 1

        print("Constructed a match tree")
        print(matchTree)

        paths = nx.dag_to_branching(matchTree)

        for path in paths:
            print(f"Checking a path: {path}")
            # TODO: check if there's a common part.

def main():
    # {ADT,AC,GGT}{CG,A}{AG,C}
    # 3 {{0}{1}}{{0,1}}{{0}}
    text1 = [["ADT", "AC", "GGT"], ["CG", "A"], ["AG", "C"]]
    sources1 = [[(0), (1), (2)], [(0, 1), (2)], [(0), (1, 2)]]

    # Pattern = DTCGA
    indexToSourceIndex1 = {2: 0} # segment index -> sources vector index
    indexToMatch1 = {2: (0, 0)} # segment index -> (variant index, letter in variant index)

    assert verify(text1, sources1, "DTCGA", indexToSourceIndex1, indexToMatch1) == [2]

    print("All passed")

if __name__ == "__main__":
    main()
