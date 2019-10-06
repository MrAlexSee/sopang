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

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
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

} // namespace sopang
