#include <string>
#include <vector>

#include "catch.hpp"

#include "../sopang.hpp"

using namespace std;

namespace sopang
{

namespace
{

const string alphabet = "ACGTN";

}

TEST_CASE("is matching a single segment with empty sources correct for whole segment match", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, { }, "ACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching a single segment with empty sources correct for partial segment match", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, { }, pattern, alphabet);
        REQUIRE(res == unordered_set<unsigned>{ 0 });
    }
}

TEST_CASE("is matching sources for 3 non-deterministic segments correct", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{ANT,AC,GGT}{CG,A}{AG,C}", &nSegments, &segmentSizes);

    const vector<vector<set<int>>> sources { { { 0 }, { 1 }, { 2 } }, { { 0, 1 }, { 2 } }, { { 0 }, { 1, 2 } } };
    const auto sourceMap = Sopang::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<unsigned> &expected) {
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(res == expected);
    };

    testMatch("NTCGA", { 2 });
    testMatch("GTCGA", { });
}

TEST_CASE("is matching sources for 3 segments with deterministic correct", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{ANT,AC,GGT}CGGA{CGAAA,A}", &nSegments, &segmentSizes);

    const vector<vector<set<int>>> sources { { { 0 }, { 1 }, { 2 } }, { { 0 }, { 1, 2 } } };
    const auto sourceMap = Sopang::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<unsigned> &expected) {
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(res == expected);
    };

    testMatch("AN", { 0 });
    testMatch("ANT", { 0 });
    testMatch("CGA", { 2 });
    testMatch("GGACGA", { 2 });
    testMatch("CGGACGA", { 2 });
    testMatch("NTCGGACG", { 2 });
    testMatch("GTCGGACG", { });
    testMatch("GGTCGGAA", { 2 });
    testMatch("ANTCGGACGAA", { 2 });
    testMatch("ANTCGGACGAAA", { 2 });
}

TEST_CASE("is matching sources for 4 segments with deterministic correct", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{ANT,AC,GGT}CGGA{CGAAA,TTT,GGG}{AAC,TC}", &nSegments, &segmentSizes);

    const vector<vector<set<int>>> sources { { { 0 }, { 1 }, { 2 } }, { { 0 }, { 1 }, { 2 } }, { { 0, 1 }, { 2 } } };
    const auto sourceMap = Sopang::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<unsigned> &expected) {
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(res == expected);
    };

    testMatch("A", { 0 });
    testMatch("AN", { 0 });
    testMatch("ANT", { 0 });
    testMatch("GATTTAA", { 3 });
    testMatch("GAGGGAA", { });
    testMatch("ANTCGGACGA", { 2 });
    testMatch("GGTCGGACGA", { });
    testMatch("CGGA", { 1 });
    testMatch("ANTC", { 1 });
    testMatch("ANTCG", { 1 });
    testMatch("ANTCGG", { 1 });
    testMatch("ANTCGGA", { 1 });
    testMatch("ANTCGGAC", { 2 });
    testMatch("ANTCGGACG", { 2 });
    testMatch("ANTCGGACGA", { 2 });
    testMatch("ANTCGGACGAA", { 2 });
    testMatch("ANTCGGACGAAA", { 2 });
    testMatch("ANTCGGAT", { });
    testMatch("ANTCGGATT", { });
    testMatch("ANTCGGATTT", { });
    testMatch("ANTCGGAG", { });
    testMatch("ANTCGGAGG", { });
    testMatch("ANTCGGAGGG", { });
}

TEST_CASE("is matching sources for 5 segments with deterministic and empty variants correct", "[sources]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AA{ANT,AC,GGT,}CGGA{CGAAA,}{AAC,TC}", &nSegments, &segmentSizes);

    const vector<vector<set<int>>> sources { { { 0 }, { 1 }, { 2 }, { 3 } }, { { 0 }, { 1, 2, 3 } }, { { 0, 1 }, { 2, 3 } } };
    const auto sourceMap = Sopang::sourcesToSourceMap(nSegments, segmentSizes, sources);

    Sopang sopang;

    const auto testMatch = [&](const string &pattern, const unordered_set<unsigned> &expected) {
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, sourceMap, pattern, alphabet);
        REQUIRE(res == expected);
    };

    testMatch("ACG", { 2, 3 });
    testMatch("AACG", { 2 });

    testMatch("AANTC", { 2 });
    testMatch("AANTCG", { 2 });
    testMatch("AANTCGG", { 2 });
    testMatch("AANTCGGA", { 2 });

    testMatch("AANTCGGAC", { 3 });
    testMatch("AANTCGGACG", { 3 });
    testMatch("AANTCGGACGA", { 3 });
    testMatch("AANTCGGACGAA", { 3 });
    testMatch("AANTCGGACGAAA", { 3 });

    testMatch("AANTCGGACGAAAA", { 4 });
    testMatch("AANTCGGACGAAAAA", { 4 });
    testMatch("AANTCGGACGAAAAAC", { 4 });

    testMatch("AACGGAC", { });
    testMatch("AACGGACG", { });
    testMatch("AACGGACGA", { });
    testMatch("AACGGACGAA", { });
    testMatch("AACGGACGAAA", { });

    testMatch("CGGAAAC", { 4 });
    testMatch("CGGATC", { 4 });

    testMatch("ACCGGAA", { 4 }); // 1-123-01
    testMatch("ACCGGAAA", { 4 }); // 1-123-01
    testMatch("ACCGGAAAC", { 4 }); // 1-123-01
    testMatch("AAACCGGAT", { }); // 1-123-23
    testMatch("AAACCGGATC", { }); // 1-123-23
    testMatch("AAANTCGGAT", { }); // 0-123-23
    testMatch("AAANTCGGATC", { }); // 0-123-23
    testMatch("AAGGTCGGAA", { }); // 2-123-01
    testMatch("AAGGTCGGAAA", { }); // 2-123-01
    testMatch("AAGGTCGGAAAC", { }); // 2-123-01
    testMatch("AAGGTCGGAT", { 4 }); // 2-123-23
    testMatch("AAGGTCGGATC", { 4 }); // 2-123-23
    testMatch("AACGGAA", { }); // 3-123-01
    testMatch("AACGGAAA", { }); // 3-123-01
    testMatch("AACGGAAAC", { }); // 3-123-01
    testMatch("AACGGAT", { 4 }); // 3-123-23
    testMatch("AACGGATC", { 4 }); // 3-123-23
}

} // namespace sopang
