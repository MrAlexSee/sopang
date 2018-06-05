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

TEST_CASE("is approx matching for a single segment exact correct, whole segment match", "[approx]")
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

TEST_CASE("is approx matching for a single segment exact correct, partial segment match", "[approx]")
{    
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACG", "CGT" })
    {
        for (unsigned k : { 1, 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
            
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
        }
    }
}

TEST_CASE("is approx matching for a single segment correct for 1 error, whole segment match", "[approx]")
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
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching for a single segment correct for 1 error, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACN", "ANG", "NCG", "NGT", "CNT", "CGN" })
    {
        for (unsigned k : { 1, 2, 3 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
          
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching for a single longer segment correct for 1 error, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGTAAGGCTTTAAGCTTA", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ANGCT", "AGNCT", "AGGNT", "AGGCN" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
    }
}

TEST_CASE("is approx matching for a single longer segment at 2nd position correct for 1 error, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{ACC,AAAC}ACGTAAGGCTTTAAGCTTA{CC,AA}", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ANGCT", "AGNCT", "AGGNT", "AGGCN" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(1) == 1);
    }
}

TEST_CASE("is approx matching for a single segment correct for 2 errors, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NNG", "ANN", "NCN", "NNT", "NGN", "CNN" })
    {
        unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res1.size() == 0);
        
        for (unsigned k : { 2, 3, 4 })
        {
            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, k);
          
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments exact correct, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 1 error, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NCGT", "ANGT", "ACNT", "ACGN" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 1 error, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACN", "ANG", "NCG", "NGT", "CNT", "CGN" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching for a single segment correct for 2 errors, whole segment match", "[approx]")
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
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors, whole segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NNGT", "ANNT", "ACNN" })
    {
        unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res1.size() == 0);

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 2);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors, partial segment match", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGTT{A,C}ACGTT{,A}ACGTT{AAAAA,GGGG}ACGTT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "NNGT", "ANNT", "NCNT", "NNTT", "NGNT", "CNNT" })
    {
        unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res1.size() == 0);

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 2);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments spanning exact correct", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "TAAC", "TCAC" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments spanning correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "TAAN", "TANC", "TNAC", "NAAC", "TCAN", "TCNC", "NCAC" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is approx matching for contiguous indeterminate segments correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}{,AA}{,AAAAA,TTTT}{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, "CGTCAANA", alphabet, 1);
    
    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(3) == 1);

    unordered_set<unsigned> res2 = sopang.matchApprox(segments, nSegments, segmentSizes, "AAAANAAAC", alphabet, 1);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(5) == 1);

    unordered_set<unsigned> res3 = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTANT", alphabet, 1);

    REQUIRE(res3.size() == 1);
    REQUIRE(res3.count(3) == 1);
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words spanning correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{AA,CCAA}ACGT{AAA,CCC,TTT}{,AA}{A,C,}{AAA,CCC,T}TTCC{AA,CC}AAA", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTAAN", alphabet, 1);
    
    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(2) == 1);

    unordered_set<unsigned> res2 = sopang.matchApprox(segments, nSegments, segmentSizes, "AAAANAAA", alphabet, 1);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(5) == 1);

    string basePattern = "TTTAACCCCTTC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res3 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res3.size() == 1);
        REQUIRE(res3.count(6) == 1);
    }
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words spanning correct for 1 error, non-dna alphabet", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AACABBCBBC{A,AAB,ACCA}BB{C,ACABBCBB,CBA}BACABBC{B,CABB,BBC,AACABB,CBC}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, "CABNCB", "ABCN", 1);
    REQUIRE(res.size() == 4);
    
    for (unsigned i : { 0, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is approx matching pattern length 8 correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    string basePattern = "ACGTACGT";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == 3);

        for (unsigned i : { 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching pattern length 8 correct for 2 errors", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    string patternBase = "ACGTACGT";

    for (size_t i = 0; i < patternBase.size(); ++i)
    {
        string pattern = patternBase;
        
        pattern[i] = 'N';
        pattern[Helpers::randIntRangeExcluded(0, patternBase.size() - 1, i)] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 2);
        REQUIRE(res.size() == 3);

        for (unsigned i : { 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
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
