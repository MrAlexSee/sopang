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
constexpr int nRandIter = 100;

constexpr int nTextRepeats = 100;

}

TEST_CASE("is matching a single segment correct for whole segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching a single segment correct for partial segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACG", alphabet);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
    }
}

TEST_CASE("is matching single letter pattern correct for single match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,CA}ACN{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "N", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching single letter pattern correct for multiple matches", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,CA}ACN{,A}CGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "A", alphabet);
    REQUIRE(res.size() == 6);
    
    for (unsigned i : { 0, 1, 2, 3, 5, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for whole segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGT", alphabet);
    REQUIRE(res.size() == 4);

    for (unsigned i : { 0, 2, 4, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for partial segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, pattern, alphabet);
        REQUIRE(res.size() == 4);

        for (unsigned i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is matching multiple determinate segments correct for whole segment match for long generated text where start and end are determinate", "[exact]")
{
    const string det = "GAACTA";
    string text = det;

    for (int i = 0; i < nTextRepeats; ++i)
    {
        text += "{ANA,ATA,TATA,GATA}";
    }

    text += det;

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, det, alphabet);
    REQUIRE(res.size() == 2);

    REQUIRE(res.count(0) == 1);
    REQUIRE(res.count(nTextRepeats + 1) == 1);
}

TEST_CASE("is matching multiple determinate segments correct for partial segment match for long generated text where start and end are determinate", "[exact]")
{
    const string det = "GAACTA";
    string text = det;

    for (int i = 0; i < nTextRepeats; ++i)
    {
        text += "{ANA,ATA,TATA,GATA}";
    }

    text += det;

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "GAACTA", "AACTA", "ACTA", "CTA", "GAACT", "GAAC" })
    {
        unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, pattern, alphabet);
        REQUIRE(res.size() == 2);

        REQUIRE(res.count(0) == 1);
        REQUIRE(res.count(nTextRepeats + 1) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for whole segment match for long generated text where segments are once determinate once indeterminate", "[exact]")
{
    const string det = "GAACTA";
    const string ndet = "{ANA,ATA,TATA,GATA}";

    string text;

    for (int i = 0; i < nTextRepeats; ++i)
    {
        text += ndet + det;
    }

    text += ndet;

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, det, alphabet);
    REQUIRE(res.size() == nTextRepeats);

    for (unsigned i = 1; i < 2 * nTextRepeats; i += 2)
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for partial segment match for long generated text where segments are once determinate once indeterminate", "[exact]")
{
    const string det = "GAACTA";
    const string ndet = "{ANA,ATA,TATA,GATA}";

    string text;

    for (int i = 0; i < nTextRepeats; ++i)
    {
        text += ndet + det;
    }

    text += ndet;

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "GAACTA", "AACTA", "ACTA", "CTA", "GAACT", "GAAC" })
    {
        unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, pattern, alphabet);
        REQUIRE(res.size() == nTextRepeats);

        for (unsigned i = 1; i < 2 * nTextRepeats; i += 2)
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is matching single indeterminate and determinate segments for spanning correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "CAC", "AAC", "TAA", "TCA" })
    {
        unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, pattern, alphabet);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments for spanning correct for determinate start and end", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGTA", alphabet);
    REQUIRE(res.size() == 4);
    
    for (unsigned i : { 1, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments for spanning correct for indeterminate start and end", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{A,C}ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT,C}ACGT{A,C}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "CAC", alphabet);
    REQUIRE(res.size() == 3);
    
    for (unsigned i : { 1, 3, 7 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern starting and ending with text correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{A,C}ACGT{G,C}ACGT{,T}ACGT{GGGG,TTTT,C}AAC{A,G}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "AACG", alphabet);
    REQUIRE(res.size() == 2);

    REQUIRE(res.count(1) == 1);
    REQUIRE(res.count(8) == 1);
}

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple empty words for spanning correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "TAC", alphabet);
    REQUIRE(res.size() == 4);
    
    for (unsigned i : { 2, 4, 6, 8 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern for contiguous indeterminate segments correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}{,AA}{,AAAAA,TTTT}{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "AAA", alphabet);
    REQUIRE(res.size() == 3);
    
    for (unsigned i : { 2, 3, 4 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple words for spanning correct for non-dna alphabet", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AACABBCBBC{A,AAB,ACCA}BB{C,ACABBCBB,CBA}BACABBC{B,CABB,BBC,AACABB,CBC}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "CABBCB", "ABC");
    REQUIRE(res.size() == 4);
    
    for (unsigned i : { 0, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern length 8 correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGT", alphabet);
    REQUIRE(res.size() == 3);

    for (unsigned i : { 2, 4, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern length 16 correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(6) == 1);
}

TEST_CASE("is matching pattern length 32 correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}CGTACGT{A,}CGTACGT{A,CGT}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGTACGTACGTACGTACGT",alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(10) == 1);
}

TEST_CASE("is matching pattern length 64 correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}CGTACGT{A,}CGTACGT{A,}CGTACGTACGTACGTACGTACGTACGTACGT{A,CGT}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(12) == 1);
}

TEST_CASE("is matching pattern equal to text correct", "[exact]")
{
    repeat(nRandIter, []() {
        for (int size = 1; size <= maxPatSize; ++size)
        {    
            string text = Helpers::genRandomString(size, alphabet);
            Sopang sopang;

            unsigned nSegments;
            unsigned *segmentSizes;
            const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);
        
            REQUIRE(nSegments == 1);
            REQUIRE(segmentSizes[0] == 1);

            unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, text, alphabet);
        
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    });
}

TEST_CASE("is filling mask buffer correct for a predefined pattern", "[exact]")
{
    const string pattern = "ACAACGT";

    Sopang sopang;
    SopangWhitebox::fillPatternMaskBuffer(sopang, pattern, alphabet);

    const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

    uint64_t maskA = maskBuffer[static_cast<size_t>('A')];
    const vector<unsigned> expectedA { 0x0, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1 };

    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskA & (0x1ULL << i)) >> i) == expectedA[i]);
    }

    uint64_t maskC = maskBuffer[static_cast<size_t>('C')];
    const vector<unsigned> expectedC { 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskC & (0x1ULL << i)) >> i) == expectedC[i]);
    }

    uint64_t maskG = maskBuffer[static_cast<size_t>('G')];
    const vector<unsigned> expectedG { 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskG & (0x1ULL << i)) >> i) == expectedG[i]);
    }

    uint64_t maskT = maskBuffer[static_cast<size_t>('T')];
    const vector<unsigned> expectedT { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskT & (0x1ULL << i)) >> i) == expectedT[i]);
    }

    uint64_t maskN = maskBuffer[static_cast<size_t>('N')];
    const vector<unsigned> expectedN { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskN & (0x1ULL << i)) >> i) == expectedN[i]);
    }
}

TEST_CASE("is filling mask buffer correct for repeated same character in pattern", "[exact]")
{
    for (const char c : alphabet)
    {
        for (int size = 1; size <= maxPatSize; ++size)
        {
            string pattern = "";
            repeat(size, [c, &pattern]() { pattern += c; });

            Sopang sopang;
            SopangWhitebox::fillPatternMaskBuffer(sopang, pattern, alphabet);

            const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);
            uint64_t m = 0x1;

            for (int shift = 0; shift < size; ++shift)
            {
                for (const char curC : alphabet)
                {
                    if (curC == c) // Corresponding occurrences should be set to 0.
                    {
                        REQUIRE((maskBuffer[static_cast<size_t>(curC)] & m) == 0x0);
                    }
                    else
                    {
                        REQUIRE((maskBuffer[static_cast<size_t>(curC)] & m) != 0x0);
                    }
                }
                m <<= 1;
            }
        }
    }
}

}
