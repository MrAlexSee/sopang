#include <set>
#include <string>
#include <vector>

#include "catch.hpp"

#include "../parsing.hpp"
#include "../sopang.hpp"

using namespace std;

namespace sopang
{

namespace
{

const string alphabet = "ACGTN";

}

TEST_CASE("is matching a single segment with empty sources correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACGT", "ACG", "CGT", "AC", "CG", "GT", "A", "C", "G", "T" })
    {
        const unordered_set<int> resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, { }, pattern, alphabet);
        REQUIRE(resSet == unordered_set<int>{ 0 });

        const unordered_map<int, set<int>> resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, { }, pattern, alphabet);
        REQUIRE(resMap == unordered_map<int, set<int>>{ {0, {}} });
    }
}

TEST_CASE("is matching sources for 3 non-deterministic segments correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,GGT}{CG,A}{AG,C}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2 } }, { { 0, 1 }, { 2 } }, { { 0 }, { 1, 2 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("TCGA", { 2 }, { {2, {0}} });
    testMatch("TCGAG", { 2 }, { {2, {0}} });
    testMatch("NTCGA", { 2 }, { {2, {0}} });
    testMatch("NTCGAG", { 2 }, { {2, {0}} });
    testMatch("ANTCGA", { 2 }, { {2, {0}} });
    testMatch("ANTCGAG", { 2 }, { {2, {0}} });
    testMatch("GTCGA", { }, { });
    testMatch("GTCGAG", { }, { });
    testMatch("GGTCGA", { }, { });
    testMatch("GGTCGAG", { }, { });
}

TEST_CASE("is matching sources for 3 segments with deterministic correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,GGT}CGGA{CGAAA,A}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2 } }, { { 0 }, { 1, 2 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("AN", { 0 }, { {0, {0}} });
    testMatch("ANT", { 0 }, { {0, {0}} });
    testMatch("CGA", { 2 }, { {2, {0}} });
    testMatch("GGACGA", { 2 }, { {2, {0}} });
    testMatch("CGGACGA", { 2 }, { {2, {0}} });
    testMatch("NTCGGACG", { 2 }, { {2, {0}} });
    testMatch("GTCGGACG", { }, { });
    testMatch("GGTCGGAA", { 2 }, { {2, {2}} });
    testMatch("ANTCGGACGAA", { 2 }, { {2, {0}} });
    testMatch("ANTCGGACGAAA", { 2 }, { {2, {0}} });
}

TEST_CASE("is matching sources for 4 segments with deterministic correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,GGT}CGGA{CGAAA,TTT,GGG}{AAC,TC}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2 } }, { { 0 }, { 1 }, { 2 } }, { { 0, 1 }, { 2 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("A", { 0, 1, 2, 3 }, { {0, {0, 1}}, {1, {}}, {2, {0}}, {3, {0, 1}} });
    testMatch("AN", { 0 }, { {0, {0}} });
    testMatch("ANT", { 0 }, { {0, {0}} });
    testMatch("GATTTAA", { 3 }, { {3, {1}} });
    testMatch("GAGGGAA", { }, { });
    testMatch("ANTCGGACGA", { 2 }, { {2, {0}} });
    testMatch("GGTCGGACGA", { }, { });
    testMatch("CGGA", { 1 }, { {1, {}} });
    testMatch("ANTC", { 1 }, { {1, {0}} });
    testMatch("ANTCG", { 1 }, { {1, {0}} });
    testMatch("ANTCGG", { 1 }, { {1, {0}} });
    testMatch("ANTCGGA", { 1 }, { {1, {0}} });
    testMatch("ANTCGGAC", { 2 }, { {2, {0}} });
    testMatch("ANTCGGACG", { 2 }, { {2, {0}} });
    testMatch("ANTCGGACGA", { 2 }, { {2, {0}} });
    testMatch("ANTCGGACGAA", { 2 }, { {2, {0}} });
    testMatch("ANTCGGACGAAA", { 2 }, { {2, {0}} });
    testMatch("ANTCGGAT", { }, { });
    testMatch("ANTCGGATT", { }, { });
    testMatch("ANTCGGATTT", { }, { });
    testMatch("ANTCGGAG", { }, { });
    testMatch("ANTCGGAGG", { }, { });
    testMatch("ANTCGGAGGG", { }, { });
}

TEST_CASE("is matching sources for 3 non-deterministic segments multiple matches correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,GGT}{CGGA,ANT}{CG,AC}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2 } }, { { 0, 1 }, { 2 } }, { { 0, 2 }, { 1 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("ANTC", { 1, 2 }, { {1, {0}}, {2, {2}} });
    testMatch("TC", { 1, 2 }, { {1, {0}}, {2, {2}} });
    testMatch("AC", { 0, 2 }, { {0, {1}}, {2, {1}} });
    testMatch("TA", { 1 }, { {1, {2}} });
    testMatch("CC", { 1 }, { {1, {1}} });
}

TEST_CASE("is matching sources for 5 segments with deterministic and empty variants correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AA{ANT,AC,GGT,}CGGA{CGAAA,}{AAC,TC}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2 }, { 3 } }, { { 0 }, { 1, 2, 3 } }, { { 0, 1 }, { 2, 3 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("ACG", { 2, 3 }, { {2, {3}}, {3, {0}} });
    testMatch("AACG", { 2 }, { {2, {3}} });

    testMatch("AANTC", { 2 }, { {2, {0}} });
    testMatch("AANTCG", { 2 }, { {2, {0}} });
    testMatch("AANTCGG", { 2 }, { {2, {0}} });
    testMatch("AANTCGGA", { 2 }, { {2, {0}} });

    testMatch("AANTCGGAC", { 3 }, { {3, {0}} });
    testMatch("AANTCGGACG", { 3 }, { {3, {0}} });
    testMatch("AANTCGGACGA", { 3 }, { {3, {0}} });
    testMatch("AANTCGGACGAA", { 3 }, { {3, {0}} });
    testMatch("AANTCGGACGAAA", { 3 }, { {3, {0}} });

    testMatch("AANTCGGACGAAAA", { 4 }, { {4, {0}} });
    testMatch("AANTCGGACGAAAAA", { 4 }, { {4, {0}} });
    testMatch("AANTCGGACGAAAAAC", { 4 }, { {4, {0}} });

    testMatch("AACGGAC", { }, { });
    testMatch("AACGGACG", { }, { });
    testMatch("AACGGACGA", { }, { });
    testMatch("AACGGACGAA", { }, { });
    testMatch("AACGGACGAAA", { }, { });

    testMatch("CGGAAAC", { 4 }, { {4, {1}} });
    testMatch("CGGATC", { 4 }, { {4, {2, 3}} });

    testMatch("ACCGGAA", { 4 }, { {4, {1}} }); // 1-123-01
    testMatch("ACCGGAAA", { 4 }, { {4, {1}} }); // 1-123-01
    testMatch("ACCGGAAAC", { 4 }, { {4, {1}} }); // 1-123-01
    testMatch("AAACCGGAT", { }, { }); // 1-123-23
    testMatch("AAACCGGATC", { }, { }); // 1-123-23
    testMatch("AAANTCGGAT", { }, { }); // 0-123-23
    testMatch("AAANTCGGATC", { }, { }); // 0-123-23
    testMatch("AAGGTCGGAA", { }, { }); // 2-123-01
    testMatch("AAGGTCGGAAA", { }, { }); // 2-123-01
    testMatch("AAGGTCGGAAAC", { }, { }); // 2-123-01
    testMatch("AAGGTCGGAT", { 4 }, { {4, {2}} }); // 2-123-23
    testMatch("AAGGTCGGATC", { 4 }, { {4, {2}} }); // 2-123-23
    testMatch("AACGGAA", { }, { }); // 3-123-01
    testMatch("AACGGAAA", { }, { }); // 3-123-01
    testMatch("AACGGAAAC", { }, { }); // 3-123-01
    testMatch("AACGGAT", { 4 }, { {4, {3}} }); // 3-123-23
    testMatch("AACGGATC", { 4 }, { {4, {3}} }); // 3-123-23
}

TEST_CASE("is matching sources for 5 segments with deterministic and empty variants v2 correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,}AC{CGGA,ANT,}{CG,AC,}{AC,AT}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2, 3 } }, { { 0, 1 }, { 3 }, { 2 } },
                                                    { { 0, 2 }, { 1 }, { 3 } }, { { 0 , 3 }, { 1, 2 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("ACANTAC", { 4 }, { {4, {3}} }); // 3-1 | 3-3-03
    testMatch("ACANTAT", { }, { }); // 3-3-12
    testMatch("ANTACA", { }, { }); // 0-3 | 3-1-03 | 3-1-12 | 0-2-1 | 0-2-3-03 | 0-2-3-12
    testMatch("ANTACAC", { }, { }); // 0-2-1 | 0-2-3-03 | 3-1-03
    testMatch("ANTACAT", { }, { }); // 0-2-3-12 | 3-1-12
    testMatch("ACACAC", { }, { }); // 1-2-1 | 1-2-3-03 | 2-1-03
    testMatch("ACACAT", { }, { }); // 1-2-3-12 | 2-1-12
}

TEST_CASE("is matching sources for 5 segments with deterministic and empty variants v3 correct", "[sources]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ANT,AC,}AC{CGGA,ANT,}{CG,AC,}{AC,AT}", &nSegments, &segmentSizes);

    const vector<vector<Sopang::SourceSet>> sources { { { 0 }, { 1 }, { 2, 3 } }, { { 2 }, { 3 }, { 0, 1 } },
                                                    { { 1 }, { 3 }, { 0, 2 } }, { { 0, 3 }, { 1, 2 } } };
    const auto sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<int> &expectedSet, const unordered_map<int, set<int>> &expectedMap) {
        const auto resSet = sopang.matchWithSourcesVerify(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resSet == expectedSet);

        const auto resMap = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(resMap == expectedMap);
    };

    testMatch("ACANTAC", { 3 }, { {3, {3}} }); // 3-3 | 3-02-03
    testMatch("ACANTAT", { }, { }); // 3-02-12
    testMatch("ANTACA", { 4 }, { {4, {0, 3}} }); // 0-3 | 0-01-3 | 0-01-02-03 | 0-01-02-12 | 3-3-03 | 3-3-12
    testMatch("ANTACAC", { 4 }, { {4, {0, 3}} }); // 0-01-3 | 0-01-02-03 | 3-3-03
    testMatch("ANTACAT", { }, { }); // 0-01-02-12 | 3-3-12
    testMatch("ACACAC", { }, { }); // 1-01-01-3 | 1-01-02-03 | 01-3-03
    testMatch("ACACAT", { }, { }); // 1-01-02-12 | 01-3-12
}

} // namespace sopang
