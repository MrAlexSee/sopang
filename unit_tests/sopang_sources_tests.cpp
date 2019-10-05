#include <string>
#include <vector>

#include "catch.hpp"

#include "../helpers.hpp"
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

    unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, {}, "ACGT", alphabet);

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
        unordered_set<unsigned> res = sopang.matchWithSources(segments, nSegments, segmentSizes, {}, pattern, alphabet);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
    }
}

} // namespace sopang
