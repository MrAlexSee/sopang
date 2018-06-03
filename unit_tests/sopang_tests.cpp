#define SOPANG_WHITEBOX \
    friend class SopangWhitebox;

#include <string>
#include <vector>

#include "catch.hpp"
#include "repeat.hpp"

#include "../helpers.hpp"
#include "../sopang.hpp"

using namespace std;

namespace inverted_basilisk
{

class SopangWhitebox
{
public:
    inline static void fillPatternMaskBuffer(Sopang &sopang, const string &arg1, const string &arg2)
    {
        sopang.fillPatternMaskBuffer(arg1, arg2);
    }
    inline static uint64_t *getMaskBuffer(Sopang &sopang)
    {
        return sopang.maskBuffer;
    }
};

namespace
{
const string alphabet = "ACGTN";
constexpr int maxPatSize = 64;
}

TEST_CASE("is parsing text for an empty string correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    Sopang::parseTextArray("", &nSegments, &segmentSizes);
    REQUIRE(nSegments == 0);
}

TEST_CASE("is parsing text for a single determinate segment correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 1);
    REQUIRE(segmentSizes[0] == 1);

    REQUIRE(segments[0][0] == "ACGT");
}

TEST_CASE("is parsing text for a single indeterminate segment correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("{A,C,G,T}", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 1);
    REQUIRE(segmentSizes[0] == 4);

    REQUIRE(segments[0][0] == "A");
    REQUIRE(segments[0][1] == "C");
    REQUIRE(segments[0][2] == "G");
    REQUIRE(segments[0][3] == "T");
}

TEST_CASE("is parsing text for determinate and indeterminate segments correct, start/end is indeterminate", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("{A,C}AAA{A,C,G,GGT}AAA{A,C}", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 5);

    REQUIRE(segmentSizes[0] == 2);
    REQUIRE(segmentSizes[1] == 1);
    REQUIRE(segmentSizes[2] == 4);
    REQUIRE(segmentSizes[3] == 1);
    REQUIRE(segmentSizes[4] == 2);

    REQUIRE(segments[0][0] == "A");
    REQUIRE(segments[0][1] == "C");
    REQUIRE(segments[1][0] == "AAA");
    REQUIRE(segments[2][0] == "A");
    REQUIRE(segments[2][1] == "C");
    REQUIRE(segments[2][2] == "G");
    REQUIRE(segments[2][3] == "GGT");
    REQUIRE(segments[3][0] == "AAA");
    REQUIRE(segments[4][0] == "A");
    REQUIRE(segments[4][1] == "C");
}

TEST_CASE("is parsing text for determinate and indeterminate segments correct, start/end is determinate", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("AAA{A,C,G,GGT}AAA{A,C}{A,C}ACG", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 6);

    REQUIRE(segmentSizes[0] == 1);
    REQUIRE(segmentSizes[1] == 4);
    REQUIRE(segmentSizes[2] == 1);
    REQUIRE(segmentSizes[3] == 2);
    REQUIRE(segmentSizes[4] == 2);
    REQUIRE(segmentSizes[5] == 1);

    REQUIRE(segments[0][0] == "AAA");
    REQUIRE(segments[1][0] == "A");
    REQUIRE(segments[1][1] == "C");
    REQUIRE(segments[1][2] == "G");
    REQUIRE(segments[1][3] == "GGT");
    REQUIRE(segments[2][0] == "AAA");
    REQUIRE(segments[3][0] == "A");
    REQUIRE(segments[3][1] == "C");
    REQUIRE(segments[4][0] == "A");
    REQUIRE(segments[4][1] == "C");
    REQUIRE(segments[5][0] == "ACG");
}

TEST_CASE("is parsing text for repeated indeterminate segment correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("{A,C}{A,C}{A,C}{A,C}", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 4);

    REQUIRE(segmentSizes[0] == 2);
    REQUIRE(segmentSizes[1] == 2);
    REQUIRE(segmentSizes[2] == 2);
    REQUIRE(segmentSizes[3] == 2);

    REQUIRE(segments[0][0] == "A");
    REQUIRE(segments[0][1] == "C");
    REQUIRE(segments[1][0] == "A");
    REQUIRE(segments[1][1] == "C");
    REQUIRE(segments[2][0] == "A");
    REQUIRE(segments[2][1] == "C");
    REQUIRE(segments[3][0] == "A");
    REQUIRE(segments[3][1] == "C");
}

TEST_CASE("is parsing text for indeterminate segments with empty words correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("{A,C,}{A,,C}{,A,C}", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 3);

    REQUIRE(segmentSizes[0] == 3);
    REQUIRE(segmentSizes[1] == 3);
    REQUIRE(segmentSizes[2] == 3);

    REQUIRE(segments[0][0] == "A");
    REQUIRE(segments[0][1] == "C");
    REQUIRE(segments[0][2] == "");
    REQUIRE(segments[1][0] == "A");
    REQUIRE(segments[1][1] == "");
    REQUIRE(segments[1][2] == "C");
    REQUIRE(segments[2][0] == "");
    REQUIRE(segments[2][1] == "A");
    REQUIRE(segments[2][2] == "C");
}

TEST_CASE("is parsing patterns for an empty string correct", "[parsing]")
{
    vector<string> empty = Sopang::parsePatterns("");
    REQUIRE(empty.size() == 0);
}

TEST_CASE("is parsing patterns for a single pattern correct", "[parsing]")
{
    string str = "1";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 1);
    REQUIRE(patterns[0] == str);
}

TEST_CASE("is parsing patterns correct", "[parsing]")
{
    string str = "1\n2\n3\n4\n5";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 5);
    REQUIRE(Helpers::join(patterns, "") == "12345");
}

TEST_CASE("is parsing patterns correct for multiple empty and trailing lines", "[parsing]")
{
    string str = "\n\n\n\n1\n\n2\n3\n4\n\n\n5\n\n\n\n\n\n";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 5);
    REQUIRE(Helpers::join(patterns, "") == "12345");
}

TEST_CASE("is parsing patterns correct for trailing whitespace", "[parsing]")
{
    string str = "  1   \t\n\n2  \n 3\n  4\n\n 5\t\t  ";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 5);
    REQUIRE(Helpers::join(patterns, "") == "12345");
}

TEST_CASE("is matching for a single segment correct, whole segment match", "[matching]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "ACGT", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching for a single segment correct, partial segment match", "[matching]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res1 = sopang.match(segments, nSegments, segmentSizes, "ACG", alphabet);

    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(0) == 1); // 0 = index of the first segment.

    unordered_set<unsigned> res2 = sopang.match(segments, nSegments, segmentSizes, "CGT", alphabet);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching single letter pattern correct, single match", "[matching]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,CA}ACN{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "N", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching single letter pattern correct, multiple matches", "[matching]")
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

TEST_CASE("is matching multiple determinate segments correct, whole segment match", "[matching]")
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

TEST_CASE("is matching multiple determinate segments correct, partial segment match", "[matching]")
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

TEST_CASE("is matching single indeterminate and determinate segments spanning correct", "[matching]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, "CAC", alphabet);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching multiple indeterminate and determinate segments spanning correct", "[matching]")
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

TEST_CASE("is filling mask buffer correct for a predefined pattern", "[matching]")
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

TEST_CASE("is filling mask buffer correct for repeated same character in pattern", "[matching]")
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
