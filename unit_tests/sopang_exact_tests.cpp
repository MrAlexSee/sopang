#include "catch.hpp"
#include "repeat.hpp"
#include "sopang_whitebox.hpp"

#include "../helpers.hpp"
#include "../parsing.hpp"
#include "../sopang.hpp"

#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

namespace sopang
{

namespace
{

const string alphabet = "ACGTN";

constexpr int maxPatSize = 64;
constexpr int nRandIter = 100;

constexpr int nTextRepeats = 1000;

}

TEST_CASE("is matching a single segment correct for whole segment match", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGT");

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
}

TEST_CASE("is matching a single segment correct for partial segment match", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
    }
}

TEST_CASE("is matching single letter pattern correct for single match", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,CA}ACN{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "N");

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(2) == 1);
}

TEST_CASE("is matching single letter pattern correct for multiple matches", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,CA}ACN{,A}CGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "A");
    REQUIRE(res.size() == 6);
    
    for (int i : { 0, 1, 2, 3, 5, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for whole segment match", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGT");
    REQUIRE(res.size() == 4);

    for (int i : { 0, 2, 4, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple determinate segments correct for partial segment match", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
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

    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, det);
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

    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "AACTA", "ACTA", "CTA", "GAACT", "GAAC" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);
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

    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, det);
    REQUIRE(res.size() == nTextRepeats);

    for (int i = 1; i < 2 * nTextRepeats; i += 2)
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

    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "GAACTA", "AACTA", "ACTA", "CTA", "GAACT", "GAAC" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);
        REQUIRE(res.size() == nTextRepeats);

        for (int i = 1; i < 2 * nTextRepeats; i += 2)
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is matching single indeterminate and determinate segments for spanning correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "CAC", "AAC", "TAA", "TCA" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is matching single indeterminate and determinate segments for repeated spanning multiple variants correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C,G,T}{AAA,CCC,GGG,TTT}{ACGT,TGCA}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "TAAAAAC", "TAAAATG", "TCCCCAC", "TCCCCTG", "TGGGGAC", "TGGGGTG", "TTTTTAC", "TTTTTTG" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(3) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments for spanning correct for determinate start and end", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGTA");
    REQUIRE(res.size() == 4);
    
    for (int i : { 1, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments for spanning correct for indeterminate start and end", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{A,C}ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT,C}ACGT{A,C}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "CAC");
    REQUIRE(res.size() == 3);
    
    for (int i : { 1, 3, 7 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple short contiguous indeterminate segments correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "CTATGCTC");
   
    REQUIRE(res.size() == 1);
    REQUIRE(res.count(7) == 1);
}

TEST_CASE("is matching pattern starting and ending with text correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{A,C}ACGT{G,C}ACGT{,T}ACGT{GGGG,TTTT,C}AAC{A,G}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "AACG");
    REQUIRE(res.size() == 2);

    REQUIRE(res.count(1) == 1);
    REQUIRE(res.count(8) == 1);
}

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple empty words for spanning correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "TAC");
    REQUIRE(res.size() == 4);
    
    for (int i : { 2, 4, 6, 8 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple empty words at different positions for spanning correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,AA,CC,GG}{AA,,CC,GG}{AA,CC,,GG}{AA,CC,GG,}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    // Only letters from the first and the last segment.
    for (const string &pattern : { "ACGTACGT", "CGTACG", "GTACGT", "GTACG", "GTAC", "TACGT", "TACG", "TAC" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }

    // Letters from the first, the last, and one (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAACGT", "ACGTCCACGT", "ACGTGGACGT" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }

    // Letters from the first, the last, and two (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAAAACGT", "ACGTCCCCACGT", "ACGTGGGGACGT", "ACGTAACCACGT", "ACGTCCAAACGT", "ACGTCCGGACGT", "ACGTGGCCACGT", "ACGTAAGGACGT", "ACGTGGAAACGT" })
    {
        unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, pattern);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }
}

TEST_CASE("is matching pattern for contiguous indeterminate segments correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}{,AA}{,AAAAA,TTTT}{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "AAA");
    REQUIRE(res.size() == 3);
    
    for (int i : { 2, 3, 4 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching multiple indeterminate and determinate segments with multiple words for spanning correct for non-dna alphabet", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AACABBCBBC{A,AAB,ACCA}BB{C,ACABBCBB,CBA}BACABBC{B,CABB,BBC,AACABB,CBC}", &nSegments, &segmentSizes);

    Sopang sopang("ABC");

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "CABBCB");
    REQUIRE(res.size() == 4);
    
    for (int i : { 0, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern length 8 correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGT");
    REQUIRE(res.size() == 3);

    for (int i : { 2, 4, 6 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is matching pattern length 16 correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGT");

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(6) == 1);
}

TEST_CASE("is matching pattern length 32 correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}CGTACGT{A,}CGTACGT{A,CGT}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGTACGTACGTACGTACGT");

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(10) == 1);
}

TEST_CASE("is matching pattern length 64 correct", "[exact]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}CGTACGT{A,}CGTACGT{A,}CGTACGTACGTACGTACGTACGTACGTACGT{A,CGT}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT");

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(12) == 1);
}

TEST_CASE("is matching pattern equal to text correct", "[exact]")
{
    repeat(nRandIter, [] {
        for (int size = 1; size <= maxPatSize; ++size)
        {    
            string text = helpers::genRandomString(size, alphabet);
            Sopang sopang(alphabet);

            int nSegments;
            int *segmentSizes;
            const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);
        
            REQUIRE(nSegments == 1);
            REQUIRE(segmentSizes[0] == 1);

            unordered_set<int> res = sopang.match(segments, nSegments, segmentSizes, text);
        
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    });
}

TEST_CASE("is filling mask buffer correct for a predefined pattern", "[exact]")
{
    const string pattern = "ACAACGT";

    Sopang sopang(alphabet);
    SopangWhitebox::fillPatternMaskBuffer(sopang, pattern);

    const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

    uint64_t maskA = maskBuffer[static_cast<size_t>('A')];
    const vector<uint64_t> expectedA { 0x0, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1 };

    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskA & (0x1ULL << i)) >> i) == expectedA[i]);
    }

    uint64_t maskC = maskBuffer[static_cast<size_t>('C')];
    const vector<uint64_t> expectedC { 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskC & (0x1ULL << i)) >> i) == expectedC[i]);
    }

    uint64_t maskG = maskBuffer[static_cast<size_t>('G')];
    const vector<uint64_t> expectedG { 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskG & (0x1ULL << i)) >> i) == expectedG[i]);
    }

    uint64_t maskT = maskBuffer[static_cast<size_t>('T')];
    const vector<uint64_t> expectedT { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskT & (0x1ULL << i)) >> i) == expectedT[i]);
    }

    uint64_t maskN = maskBuffer[static_cast<size_t>('N')];
    const vector<uint64_t> expectedN { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1 };
    
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
            repeat(size, [c, &pattern] { pattern += c; });

            Sopang sopang(alphabet);
            SopangWhitebox::fillPatternMaskBuffer(sopang, pattern);

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

} // namespace sopang
