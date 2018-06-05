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

TEST_CASE("is matching for a single segment correct, whole segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching for a single segment correct, partial segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res1 = sopang.match(segments, nSegments, segmentSizes, "ACG", alphabet);

    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(0) == 1);

    unordered_set<unsigned> res2 = sopang.match(segments, nSegments, segmentSizes, "CGT", alphabet);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(0) == 1);
}

TEST_CASE("is matching single letter pattern correct, single match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,CA}ACN{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "N", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching single letter pattern correct, multiple matches", "[exact]")
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

TEST_CASE("is matching multiple determinate segments correct, whole segment match", "[exact]")
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

TEST_CASE("is matching multiple determinate segments correct, partial segment match", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res1 = sopang.match(segments, nSegments, segmentSizes, "ACG", alphabet);
    REQUIRE(res1.size() == 4);

    for (unsigned i : { 0, 2, 4, 6 })
    {
        REQUIRE(res1.count(i) == 1);
    }

    unordered_set<unsigned> res2 = sopang.match(segments, nSegments, segmentSizes, "CGT", alphabet);
    REQUIRE(res2.size() == 4);
    
    for (unsigned i : { 0, 2, 4, 6 })
    {
        REQUIRE(res2.count(i) == 1);
    }
}

TEST_CASE("is matching single indeterminate and determinate segments spanning correct", "[exact]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "CAC", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching multiple indeterminate and determinate segments spanning correct, determinate start and end", "[exact]")
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

TEST_CASE("is matching multiple indeterminate and determinate segments spanning correct, indeterminate start and end", "[exact]")
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

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple empty words spanning correct", "[exact]")
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

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple words spanning correct, non-dna alphabet", "[exact]")
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
    Sopang sopang;
    SopangWhitebox::fillPatternMaskBuffer(sopang, "ACAACGT", alphabet);

    uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

    uint64_t maskA = maskBuffer[static_cast<size_t>('A')];

    REQUIRE((maskA & static_cast<uint64_t>(0x1)) == 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 1)) != 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 2)) == 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 3)) == 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 4)) != 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 5)) != 0x0);
    REQUIRE((maskA & static_cast<uint64_t>(0x1 << 6)) != 0x0);

    uint64_t maskC = maskBuffer[static_cast<size_t>('C')];

    REQUIRE((maskC & static_cast<uint64_t>(0x1)) != 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 1)) == 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 2)) != 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 3)) != 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 4)) == 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 5)) != 0x0);
    REQUIRE((maskC & static_cast<uint64_t>(0x1 << 6)) != 0x0);

    uint64_t maskG = maskBuffer[static_cast<size_t>('G')];

    REQUIRE((maskG & static_cast<uint64_t>(0x1)) != 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 1)) != 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 2)) != 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 3)) != 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 4)) != 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 5)) == 0x0);
    REQUIRE((maskG & static_cast<uint64_t>(0x1 << 6)) != 0x0);

    uint64_t maskT = maskBuffer[static_cast<size_t>('T')];

    REQUIRE((maskT & static_cast<uint64_t>(0x1)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 1)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 2)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 3)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 4)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 5)) != 0x0);
    REQUIRE((maskT & static_cast<uint64_t>(0x1 << 6)) == 0x0);

    uint64_t maskN = maskBuffer[static_cast<size_t>('N')];

    REQUIRE((maskN & static_cast<uint64_t>(0x1)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 1)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 2)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 3)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 4)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 5)) != 0x0);
    REQUIRE((maskN & static_cast<uint64_t>(0x1 << 6)) != 0x0);
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

            uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);
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
