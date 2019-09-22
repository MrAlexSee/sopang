"""
A prototype for verifying matches based on the sources file.
It is assumed that both text and sources have already been parsed and processed. We only check the verification.
"""

import matplotlib.pyplot as plt
import networkx as nx


def draw_graph(graph):
    pos = nx.drawing.nx_agraph.graphviz_layout(graph)

    nx.draw_networkx_edges(graph, pos, alpha=0.6)
    nx.draw_networkx_nodes(graph, pos, alpha=0.6)

    labels = {}
    for node in graph.nodes(data=True):
        label = str(node[0])
        if "pattern_char_index" in node[1]:
            label += f' -> {node[1]["pattern_char_index"]}'
        else:
            label += ' <>'

        labels[node[0]] = label

    nx.draw_networkx_labels(graph, pos, labels=labels)
    plt.show()

def verify(text, sources, pattern, indexToSourceIndex, indexToMatch):
    verifiedMatches = set()
    print(f'\nPattern: \"{pattern}"')

    for matchIndex, match in indexToMatch.items():
        print(f"Verifying: {matchIndex} -> {match}")

        charInVariantIndex = match[1]
        patternCharIndex = len(pattern) - 1

        # 1. We go back to the beginning of the segment where the match occurred.
        patternCharIndex -= (charInVariantIndex + 1)

        if patternCharIndex < 0:
            print("Matched pattern doesn't span multiple segments")
            verifiedMatches.add(matchIndex)
            break

        # 2. Build a tree of all possible match paths. Branch out each time a match occurs.
        matchTree = nx.DiGraph()

        rootNode = (matchIndex, match[0])
        matchTree.add_node(rootNode, pattern_char_index=patternCharIndex)

        leafList = [rootNode]
        segmentIndex = matchIndex - 1

        while leafList and segmentIndex >= 0:
            assert nx.is_directed_acyclic_graph(matchTree)

            print(f"Segment: {segmentIndex} -> #leaves = {len(leafList)}")
            newLeafList = []

            # Each time we traverse a segment, we need to update every leaf.
            for leaf in leafList:
                print(f" Checking leaf: {leaf}")

                # A deterministic segment -- we simply jump over the entire segment.
                if len(text[segmentIndex]) == 1:
                    print(f"  Deterministic: {text[segmentIndex][0]}")
                    matchTree.node[leaf]["pattern_char_index"] -= len(text[segmentIndex][0])

                    if matchTree.node[leaf]["pattern_char_index"] < 0:
                        print("  Match ends in the middle of the variant")
                        matchTree.node[leaf]["match_complete"] = True

                        if leaf == rootNode:
                            verifiedMatches.add(matchIndex)
                    else:
                        newLeafList += [leaf]
                else:
                    for variantIndex, variant in enumerate(text[segmentIndex]):
                        print(f"  Checking variant: {variantIndex} -> {variant}")
                        matchNode = (segmentIndex, variantIndex)

                        curCharIndex = len(variant) - 1
                        curPatternIndex = matchTree.node[leaf]["pattern_char_index"]

                        while curCharIndex >= 0:
                            # A match in the middle of this variant -> add a new match complete node.
                            if curPatternIndex < 0:
                                print("  Match ends in the middle of the variant")

                                matchTree.add_node(matchNode, match_complete=True)
                                matchTree.add_edge(leaf, matchNode)
                                break

                            if pattern[curPatternIndex] != variant[curCharIndex]:
                                break

                            curCharIndex -= 1
                            curPatternIndex -= 1

                        # Managed to traverse (match) this whole variant -> add a new continuation node.
                        if curCharIndex < 0:
                            print("  Matched the variant")

                            matchTree.add_node(matchNode, pattern_char_index = curPatternIndex)
                            matchTree.add_edge(leaf, matchNode)

                            newLeafList += [matchNode]

                leafList = newLeafList
                segmentIndex -= 1

        print("Constructed a match tree")
        assert not leafList and nx.is_directed_acyclic_graph(matchTree)

        # draw_graph(matchTree)

        leaves = [node for node, degree in matchTree.out_degree() if degree == 0]
        print(f"Leaf count = {len(leaves)}")

        for leaf in leaves:
            matchedSources = None
            paths = list(nx.all_simple_paths(matchTree, rootNode, leaf))

            if not paths:
                continue

            for node in paths[0]:
                newSources = set(sources[indexToSourceIndex[node[0]]][node[1]])
                matchedSources = matchedSources & newSources if matchedSources else newSources

        if matchedSources:
            print(f"Matched sources: {matchedSources}")
            verifiedMatches.add(matchIndex)

    print(f"Verified matches: {verifiedMatches}")
    return verifiedMatches

def main():
    print("\n[Text 1]")
    # {ADT,AC,GGT}{CG,A}{AG,C}
    # 3 {{0}{1}}{{0,1}}{{0}}
    text1 = [["ADT", "AC", "GGT"], ["CG", "A"], ["AG", "C"]]
    sources1 = [[[0], [1], [2]], [[0, 1], [2]], [[0], [1, 2]]]
    indexToSourceIndex1 = {0: 0, 1: 1, 2: 2} # segment index -> sources vector index
    indexToMatch1 = {2: (0, 0)} # segment index -> (variant index, char in variant index)

    assert verify(text1, sources1, "DTCGA", indexToSourceIndex1, indexToMatch1) == set([2])
    assert verify(text1, sources1, "GTCGA", indexToSourceIndex1, indexToMatch1) == set()

    print("\n[Text 2]")
    text2 = [["ADT", "AC", "GGT"], ["CGGA"], ["CGAAA", "A"]]
    sources2 = [[[0], [1], [2]], [[0], [1, 2]]]
    indexToSourceIndex2 = {0: 0, 2: 1}

    assert verify(text2, sources2, "CGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verify(text2, sources2, "GGACGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verify(text2, sources2, "CGGACGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verify(text2, sources2, "DTCGGACG", indexToSourceIndex2, {2: (0, 1)}) == set([2])
    assert verify(text2, sources2, "GTCGGACG", indexToSourceIndex2, {2: (0, 1)}) == set()

    print("\n[Text 3]")
    text3 = [["ADT", "AC", "GGT"], ["CGGA"], ["CGAAA", "TTT", "GGG"], ["AAC", "TC"]]
    sources3 = [[[0], [1], [2]], [[0], [1], [2]], [[0, 1], [2]]]
    indexToSourceIndex3 = {0: 0, 2: 1, 3: 2}

    assert verify(text3, sources3, "GATTTAA", indexToSourceIndex3, {3: (0, 1)}) == set([3])
    assert verify(text3, sources3, "GAGGGAA", indexToSourceIndex3, {3: (0, 1)}) == set()

    print("!!All passed!!")

if __name__ == "__main__":
    main()
