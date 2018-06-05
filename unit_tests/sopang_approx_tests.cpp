#include <string>
#include <unordered_set>
#include <vector>

#include "catch.hpp"
#include "repeat.hpp"
#include "sopang_whitebox.hpp"

#include "../helpers.hpp"
#include "../sopang.hpp"

using namespace std;

namespace inverted_basilisk
{

namespace
{
const string alphabet = "ACGTN";

constexpr int maxPatSize = 64;
constexpr int nRandIter = 10;
}

TEST_CASE("is matching for a single segment exact correct, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (unsigned k : { 1, 2, 3, 4 })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGT", alphabet, k);
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
    }
}

TEST_CASE("is matching for a single segment approx correct for 1 error, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NCGT", "ANGT", "ACNT", "ACGN" })
    {
        for (unsigned k : { 1, 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
        }
    }
}

TEST_CASE("is matching for a single segment approx correct for 2 errors, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NNGT", "ANNT", "ACNN" })
    {
        unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res1.size() == 0);
        
        for (unsigned k : { 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
        }
    }
}

TEST_CASE("is matching approx multiple determinate segments correct for 1 error, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NCGT", "ANGT", "ACNT", "ACGN" })
    {
        for (unsigned k : { 1, 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
            
            REQUIRE(res.size() == 4);

            for (unsigned i : { 0, 2, 4, 6 })
            {
                REQUIRE(res.count(i) == 1);
            }            
        }
    }
}

TEST_CASE("is matching approx multiple determinate segments correct for 2 errors, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NNGT", "ANNT", "ACNN" })
    {
        unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res1.size() == 0);
        
        for (unsigned k : { 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
            
            for (unsigned i : { 0, 2, 4, 6 })
            {
                REQUIRE(res.count(i) == 1);
            }
        }
    }
}

TEST_CASE("is filling approx mask buffer correct for a predefined pattern", "[approx]")
{
    // TODO
}

TEST_CASE("is filling approx mask buffer correct for repeated same character in pattern", "[approx]")
{
    // TODO
}

}
