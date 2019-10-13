"""
A prototype for verifying matches based on the sources file.
It is assumed that both text and sources have already been parsed and processed.
We only test the verification here.
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


def verifyTree(text, sources, pattern, indexToSourceIndex, indexToMatch):
    verifiedMatches = set()
    print(f'\nPattern: \"{pattern}"')

    for matchIndex, match in indexToMatch.items():
        print(f"Verifying: {matchIndex} -> {match}")

        charInVariantIndex = match[1]
        patternCharIndex = len(pattern) - 1

        # 1. We go back to the beginning of the segment where the match occurred.
        assert pattern[-1 - charInVariantIndex : ] == text[matchIndex][match[0]][ : charInVariantIndex + 1]
        patternCharIndex -= (charInVariantIndex + 1)

        print(f'Jumped back by {charInVariantIndex + 1} chars, index in pattern = {patternCharIndex}')

        if patternCharIndex < 0:
            print("Matched pattern doesn't span multiple segments")
            verifiedMatches.add(matchIndex)
            break

        # 2. We build a tree of all possible match paths. Branch out each time a match occurs.
        matchTree = nx.DiGraph()

        rootNode = (matchIndex, match[0])
        matchTree.add_node(rootNode, pattern_char_index=patternCharIndex)

        leafList = [rootNode]
        segmentIndex = matchIndex - 1

        while leafList and segmentIndex >= 0:
            assert nx.is_directed_acyclic_graph(matchTree)

            print(f"Segment: {segmentIndex} (len = {len(text[segmentIndex])}) -> #leaves = {len(leafList)}")
            newLeafList = []

            # Each time we traverse a segment, we need to update every leaf.
            for leaf in leafList:
                print(f" Checking leaf: {leaf}")

                # A deterministic segment -- we simply jump over the entire segment.
                if len(text[segmentIndex]) == 1:
                    print(f"  Deterministic: {text[segmentIndex][0]}")
                    matchTree.node[leaf]["pattern_char_index"] -= len(text[segmentIndex][0])

                    if matchTree.node[leaf]["pattern_char_index"] < 0:
                        print("  Match ends within the variant")
                        matchTree.node[leaf]["match_complete"] = True

                        if leaf == rootNode:
                            verifiedMatches.add(matchIndex)
                    else:
                        newLeafList += [leaf]
                else:
                    for variantIndex, variant in enumerate(text[segmentIndex]):
                        matchNode = (segmentIndex, variantIndex)

                        if len(variant) == 0:
                            print(f"  Checking variant: {variantIndex} -> empty")

                            matchTree.add_node(matchNode, pattern_char_index = matchTree.node[leaf]["pattern_char_index"])
                            matchTree.add_edge(leaf, matchNode)

                            newLeafList += [matchNode]
                            continue

                        print(f"  Checking variant: {variantIndex} -> {variant}")
                        curCharIndex = len(variant) - 1
                        curPatternIndex = matchTree.node[leaf]["pattern_char_index"]

                        while curCharIndex >= 0:
                            if curPatternIndex < 0:
                                # A match within this variant -> add a new match complete node.
                                print("  Match ends within the variant")

                                matchTree.add_node(matchNode, match_complete=True)
                                matchTree.add_edge(leaf, matchNode)
                                break

                            if pattern[curPatternIndex] != variant[curCharIndex]:
                                break

                            curCharIndex -= 1
                            curPatternIndex -= 1

                        if curPatternIndex < 0:
                            # A match containing the entire variant -> add a new match complete node.
                            print("  Match contains the entire variant")

                            matchTree.add_node(matchNode, match_complete=True)
                            matchTree.add_edge(leaf, matchNode)
                            break

                        # Managed to traverse (match) this whole variant -> add a new continuation node.
                        if curCharIndex < 0:
                            print("  Matched the variant")

                            matchTree.add_node(matchNode, pattern_char_index = curPatternIndex)
                            matchTree.add_edge(leaf, matchNode)

                            newLeafList += [matchNode]

                leafList = newLeafList
            segmentIndex -= 1

        print("Constructed a match tree")
        assert nx.is_directed_acyclic_graph(matchTree)

        # draw_graph(matchTree)

        leaves = [node for node, degree in matchTree.out_degree() if degree == 0]
        leaves = [leaf for leaf in leaves if "match_complete" in matchTree.node[leaf]]

        print(f"Leaf count = {len(leaves)}")

        for leaf in leaves:
            matchedSources = None
            paths = list(nx.all_simple_paths(matchTree, rootNode, leaf))

            if not paths:
                continue

            print(f"Leaf {leaf}, checking path: {paths[0]}")

            for node in paths[0]:
                if node[0] not in indexToSourceIndex:
                    continue

                newSources = set(sources[indexToSourceIndex[node[0]]][node[1]])
                matchedSources = newSources if matchedSources is None else matchedSources & newSources

            if matchedSources:
                print(f"Matched sources: {matchedSources}")
                verifiedMatches.add(matchIndex)

    print(f"Verified matches: {verifiedMatches}")
    return verifiedMatches


def verifyLeaves(text, sources, pattern, indexToSourceIndex, indexToMatch):
    print(pattern, indexToMatch)

    def verifyMatch(text, sources, pattern, indexToSourceIndex, matchIndex, match):
        patternCharIndex = len(pattern) - match[1] - 2

        if patternCharIndex < 0:
            return True

        rootSources = None
        if matchIndex in indexToSourceIndex:
            rootSources = set(sources[indexToSourceIndex[matchIndex]][match[0]])

        leaves = [[rootSources, patternCharIndex]]
        segmentIndex = matchIndex - 1

        while leaves and segmentIndex >= 0:
            if len(text[segmentIndex]) == 1:
                for leaf in leaves:
                    leaf[1] -= len(text[segmentIndex][0])
                    if leaf[1] < 0:
                        return True
            else:
                newLeaves = []

                for leaf in leaves:
                    for variantIndex, variant in enumerate(text[segmentIndex]):
                        print(leaf, variant)
                        curSources = set(sources[indexToSourceIndex[segmentIndex]][variantIndex])

                        if len(variant) == 0:
                            newSources = curSources & leaf[0] if leaf[0] else curSources

                            if newSources:
                                newLeaves += [[newSources, leaf[1]]]
                            continue

                        curCharIndex = len(variant) - 1
                        curPatternIndex = leaf[1]

                        while curCharIndex >= 0:
                            if curPatternIndex < 0:
                                newSources = curSources & leaf[0] if leaf[0] else curSources
                                if newSources:
                                    return True

                            if pattern[curPatternIndex] != variant[curCharIndex]:
                                break

                            curCharIndex -= 1
                            curPatternIndex -= 1

                        if curPatternIndex < 0:
                            newSources = curSources & leaf[0] if leaf[0] else curSources
                            if newSources:
                                return True

                        if curCharIndex < 0:
                            newSources = curSources & leaf[0] if leaf[0] else curSources

                            if newSources:
                                newLeaves += [[newSources, curPatternIndex]]

                leaves = newLeaves
            segmentIndex -= 1

        return False

    ret = set()

    for matchIndex, match in indexToMatch.items():
        if verifyMatch(text, sources, pattern, indexToSourceIndex, matchIndex, match):
            ret.add(matchIndex)

    print(ret)
    return ret


def runTesting(verifyFun):
    print("\n[Text 1]")
    # {ADT,AC,GGT}{CG,A}{AG,C}
    # 3 {{0}{1}}{{0,1}}{{0}}
    text1 = [["ADT", "AC", "GGT"], ["CG", "A"], ["AG", "C"]]
    sources1 = [[[0], [1], [2]], [[0, 1], [2]], [[0], [1, 2]]]
    indexToSourceIndex1 = {0: 0, 1: 1, 2: 2} # segment index -> sources vector index
    indexToMatch1 = {2: (0, 0)} # segment index -> (variant index, char in variant index)

    assert verifyFun(text1, sources1, "DTCGA", indexToSourceIndex1, indexToMatch1) == set([2])
    assert verifyFun(text1, sources1, "GTCGA", indexToSourceIndex1, indexToMatch1) == set()

    print("\n[Text 2]")
    text2 = [["ADT", "AC", "GGT"], ["CGGA"], ["CGAAA", "A"]]
    sources2 = [[[0], [1], [2]], [[0], [1, 2]]]
    indexToSourceIndex2 = {0: 0, 2: 1}

    assert verifyFun(text2, sources2, "CGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verifyFun(text2, sources2, "GGACGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verifyFun(text2, sources2, "CGGACGA", indexToSourceIndex2, {2: (0, 2)}) == set([2])
    assert verifyFun(text2, sources2, "DTCGGACG", indexToSourceIndex2, {2: (0, 1)}) == set([2])
    assert verifyFun(text2, sources2, "GTCGGACG", indexToSourceIndex2, {2: (0, 1)}) == set()
    assert verifyFun(text2, sources2, "GGTCGGAA", indexToSourceIndex2, {2: (1, 0)}) == set([2])
    assert verifyFun(text2, sources2, "ADTCGGACGAA", indexToSourceIndex2, {2: (0, 3)}) == set([2])
    assert verifyFun(text2, sources2, "ADTCGGACGAAA", indexToSourceIndex2, {2: (0, 4)}) == set([2])
    assert verifyFun(text2, sources2, "AD", indexToSourceIndex2, {0: (0, 1)}) == set([0])
    assert verifyFun(text2, sources2, "ADT", indexToSourceIndex2, {0: (0, 2)}) == set([0])

    print("\n[Text 3]")
    text3 = [["ADT", "AC", "GGT"], ["CGGA"], ["CGAAA", "TTT", "GGG"], ["AAC", "TC"]]
    sources3 = [[[0], [1], [2]], [[0], [1], [2]], [[0, 1], [2]]]
    indexToSourceIndex3 = {0: 0, 2: 1, 3: 2}

    assert verifyFun(text3, sources3, "GATTTAA", indexToSourceIndex3, {3: (0, 1)}) == set([3])
    assert verifyFun(text3, sources3, "GAGGGAA", indexToSourceIndex3, {3: (0, 1)}) == set()
    assert verifyFun(text3, sources3, "ADTCGGACGA", indexToSourceIndex3, {2: (0, 2)}) == set([2])
    assert verifyFun(text3, sources3, "GGTCGGACGA", indexToSourceIndex3, {2: (0, 2)}) == set()
    assert verifyFun(text3, sources3, "CGGA", indexToSourceIndex3, {1: (0, 3)}) == set([1])
    assert verifyFun(text3, sources3, "ADTC", indexToSourceIndex3, {1: (0, 0)}) == set([1])
    assert verifyFun(text3, sources3, "ADTCG", indexToSourceIndex3, {1: (0, 1)}) == set([1])
    assert verifyFun(text3, sources3, "ADTCGG", indexToSourceIndex3, {1: (0, 2)}) == set([1])
    assert verifyFun(text3, sources3, "ADTCGGA", indexToSourceIndex3, {1: (0, 3)}) == set([1])
    assert verifyFun(text3, sources3, "ADTCGGAC", indexToSourceIndex3, {2: (0, 0)}) == set([2])
    assert verifyFun(text3, sources3, "ADTCGGACG", indexToSourceIndex3, {2: (0, 1)}) == set([2])
    assert verifyFun(text3, sources3, "ADTCGGACGA", indexToSourceIndex3, {2: (0, 2)}) == set([2])
    assert verifyFun(text3, sources3, "ADTCGGACGAA", indexToSourceIndex3, {2: (0, 3)}) == set([2])
    assert verifyFun(text3, sources3, "ADTCGGACGAAA", indexToSourceIndex3, {2: (0, 4)}) == set([2])
    assert verifyFun(text3, sources3, "ADTCGGAT", indexToSourceIndex3, {2: (1, 0)}) == set([])
    assert verifyFun(text3, sources3, "ADTCGGAG", indexToSourceIndex3, {2: (2, 0)}) == set([])
    assert verifyFun(text3, sources3, "ADT", indexToSourceIndex3, {0: (0, 2)}) == set([0])
    assert verifyFun(text3, sources3, "AD", indexToSourceIndex3, {0: (0, 1)}) == set([0])
    assert verifyFun(text3, sources3, "A", indexToSourceIndex3, {0: (0, 0)}) == set([0])

    print("\n[Text 4]")
    text4 = [["AA"], ["ADT", "AC", "GGT", ""], ["CGGA"], ["CGAAA", ""], ["AAC", "TC"]]
    sources4 = [[[0], [1], [2], [3]], [[0], [1, 2, 3]], [[0, 1], [2, 3]]]
    indexToSourceIndex4 = {1: 0, 3: 1, 4: 2}

    assert verifyFun(text4, sources4, "CGGATC", indexToSourceIndex4, {4: (1, 1)}) == set([4])
    assert verifyFun(text4, sources4, "ACG", indexToSourceIndex4, {2: (0, 1)}) == set([2])
    assert verifyFun(text4, sources4, "AACG", indexToSourceIndex4, {2: (0, 1)}) == set([2])
    assert verifyFun(text4, sources4, "AADTCG", indexToSourceIndex4, {2: (0, 1)}) == set([2])
    assert verifyFun(text4, sources4, "AAADTCG", indexToSourceIndex4, {2: (0, 1)}) == set([2])
    assert verifyFun(text4, sources4, "AAADTCGGACGA", indexToSourceIndex4, {3: (0, 2)}) == set([3])
    assert verifyFun(text4, sources4, "AAACCGGACGA", indexToSourceIndex4, {3: (0, 2)}) == set([])
    assert verifyFun(text4, sources4, "AAGGTCGGACGA", indexToSourceIndex4, {3: (0, 2)}) == set([])
    assert verifyFun(text4, sources4, "AACGGACGA", indexToSourceIndex4, {3: (0, 2)}) == set([])
    
    assert verifyFun(text4, sources4, "ACCGGAAAC", indexToSourceIndex4, {4: (0, 2)}) == set([4]) # 1-123-01
    assert verifyFun(text4, sources4, "AAACCGGATC", indexToSourceIndex4, {4: (1, 1)}) == set([]) # 1-123-23
    assert verifyFun(text4, sources4, "AAADTCGGATC", indexToSourceIndex4, {4: (1, 1)}) == set([]) # 0-123-23
    assert verifyFun(text4, sources4, "AAGGTCGGAAAC", indexToSourceIndex4, {4: (0, 2)}) == set([]) # 2-123-01
    assert verifyFun(text4, sources4, "AAGGTCGGATC", indexToSourceIndex4, {4: (1, 1)}) == set([4]) # 2-123-23
    assert verifyFun(text4, sources4, "AACGGAAAC", indexToSourceIndex4, {4: (0, 2)}) == set([]) # 3-123-01
    assert verifyFun(text4, sources4, "AACGGATC", indexToSourceIndex4, {4: (1, 1)}) == set([4]) # 3-123-23

    print("\n[Text 5]")
    text5 = [["ADT", "AC", "GGT"], ["CGGA", "ADT"], ["CG", "AC"]]
    sources5 = [[[0], [1], [2]], [[0, 1], [2]], [[0, 2], [1]]]
    indexToSourceIndex5 = {0: 0, 1: 1, 2: 2}

    assert verifyFun(text5, sources5, "ADTC", indexToSourceIndex5, {1: (0, 0), 2: (0, 0)}) == set([1, 2])

    print("\n[Text 6]")
    text6 = [["ADT", "AC", ""],
             ["AC"],
             ["CGGA", "ADT", ""],
             ["CG", "AC", ""],
             ["AC", "AT"]]
    sources6 = [[[0], [1], [2, 3]],
                [[0, 1], [3], [2]],
                [[0, 2], [1], [3]],
                [[0, 3], [1, 2]]]
    indexToSourceIndex6 = {0: 0, 2: 1, 3: 2, 4: 3}

    assert verifyFun(text6, sources6, "ACADTAC", indexToSourceIndex6, {4: (0, 1)}) == set([4]) # 3-3-03
    assert verifyFun(text6, sources6, "ACADTAT", indexToSourceIndex6, {4: (1, 1)}) == set([]) # 3-3-12
    assert verifyFun(text6, sources6, "ADTACA", indexToSourceIndex6, {4: (0, 0)}) == set([]) # 0-2-3-03 | 3-1-03
    assert verifyFun(text6, sources6, "ADTACAC", indexToSourceIndex6, {4: (0, 1)}) == set([]) # 0-2-3-03 | 3-1-03
    assert verifyFun(text6, sources6, "ADTACA", indexToSourceIndex6, {4: (1, 0)}) == set([]) # 0-2-3-12 | 3-1-12
    assert verifyFun(text6, sources6, "ADTACAT", indexToSourceIndex6, {4: (1, 1)}) == set([]) # 0-2-3-12 | 3-1-12
    assert verifyFun(text6, sources6, "ACACAC", indexToSourceIndex6, {4: (0, 1)}) == set([]) # 1-2-3-03 | 2-1-03
    assert verifyFun(text6, sources6, "ACACAT", indexToSourceIndex6, {4: (1, 1)}) == set([]) # 1-2-3-12 | 2-1-12

    print("\n[Text 7]")
    text7 = [["ADT", "AC", ""],
             ["AC"],
             ["CGGA", "ADT", ""],
             ["CG", "AC", ""],
             ["AC", "AT"]]
    sources7 = [[[0], [1], [2, 3]],
                [[2], [3], [0, 1]],
                [[1], [3], [0, 2]],
                [[0, 3], [1, 2]]]
    indexToSourceIndex7 = {0: 0, 2: 1, 3: 2, 4: 3}

    assert verifyFun(text7, sources7, "ACADTAC", indexToSourceIndex7, {4: (0, 1)}) == set([]) # 3-02-03
    assert verifyFun(text7, sources7, "ACADTAT", indexToSourceIndex7, {4: (1, 1)}) == set([]) # 3-02-12
    assert verifyFun(text7, sources7, "ADTACA", indexToSourceIndex7, {4: (0, 0)}) == set([4]) # 0-01-02-03 | 3-3-03
    assert verifyFun(text7, sources7, "ADTACAC", indexToSourceIndex7, {4: (0, 1)}) == set([4]) # 0-01-02-03 | 3-3-03
    assert verifyFun(text7, sources7, "ADTACA", indexToSourceIndex7, {4: (1, 0)}) == set([]) # 0-01-02-12 | 3-3-12
    assert verifyFun(text7, sources7, "ADTACAT", indexToSourceIndex7, {4: (1, 1)}) == set([]) # 0-01-02-12 | 3-3-12
    assert verifyFun(text7, sources7, "ACACAC", indexToSourceIndex7, {4: (0, 1)}) == set([]) # 1-01-02-03 | 01-3-03
    assert verifyFun(text7, sources7, "ACACAT", indexToSourceIndex7, {4: (1, 1)}) == set([]) # 1-01-02-12 | 01-3-12

    print("\n[Text 8]")
    text8 = [["ACGT"]]

    assert verifyFun(text8, [], "ACGT", {}, {0: (0, 3)}) == set([0])
    assert verifyFun(text8, [], "ACG", {}, {0: (0, 2)}) == set([0])
    assert verifyFun(text8, [], "AC", {}, {0: (0, 1)}) == set([0])
    assert verifyFun(text8, [], "A", {}, {0: (0, 0)}) == set([0])

    print("\n!!All cases passed!!")


def main():
    runTesting(verifyTree)
    runTesting(verifyLeaves)

    print('\n!!All functions passed!!')


if __name__ == "__main__":
    main()
