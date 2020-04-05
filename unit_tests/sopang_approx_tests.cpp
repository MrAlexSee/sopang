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

constexpr int maxPatSize = 12;
constexpr int nRandIter = 100;

constexpr int nTextRepeats = 1000;

}

TEST_CASE("is approx matching for a single segment exact correct for whole segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (int k : { 1, 2, 3, 4 })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGT", k);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
    }
}

TEST_CASE("is approx matching for a single segment exact correct for partial segment match", "[approx]")
{    
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACG", "CGT" })
    {
        for (int k : { 1, 2, 3, 4 })
        {
            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, k);
            
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1); // 0 = index of the first segment.
        }
    }
}

TEST_CASE("is approx matching for a single segment correct for 1 error for whole segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NCGT", "ANGT", "ACNT", "ACGN" })
    {
        for (int k : { 1, 2, 3, 4 })
        {
            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, k);
          
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching for a single segment correct for 1 error for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACN", "ANG", "NCG", "NGT", "CNT", "CGN" })
    {
        for (int k : { 1, 2, 3 })
        {
            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, k);
          
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching for a single longer segment correct for 1 error for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGTAAGGCTTTAAGCTTA", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ANGCT", "AGNCT", "AGGNT", "AGGCN" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(0) == 1);
    }
}

TEST_CASE("is approx matching for a single longer segment at 2nd position correct for 1 error for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{ACC,AAAC}ACGTAAGGCTTTAAGCTTA{CC,AA}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ANGCT", "AGNCT", "AGGNT", "AGGCN" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(1) == 1);
    }
}

TEST_CASE("is approx matching for a single segment correct for 2 errors for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NNG", "ANN", "NCN", "NNT", "NGN", "CNN" })
    {
        unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res1.size() == 0);
        
        for (int k : { 2, 3, 4 })
        {
            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, k);
          
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments exact correct for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACG", "CGT" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 1 error for whole segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NCGT", "ANGT", "ACNT", "ACGN" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 1 error for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "ACN", "ANG", "NCG", "NGT", "CNT", "CGN" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching for a single segment correct for 2 errors for whole segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NNGT", "ANNT", "ACNN" })
    {
        unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res1.size() == 0);
        
        for (int k : { 2, 3, 4 })
        {
            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, k);
            
            REQUIRE(res.size() == 1);
            REQUIRE(res.count(0) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors for whole segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT{,A}ACGT{AAAAA,TTTT}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NNGT", "ANNT", "ACNN" })
    {
        unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res1.size() == 0);

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 2);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for exact", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, "CTATGCTC", 1);
   
    REQUIRE(res.size() == 1);
    REQUIRE(res.count(7) == 1);
}

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    string basePattern = "CTATGCTC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(7) == 1);
    }
}

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for 2 errors", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);
    string basePattern = "ACCTATGCTCA";

    repeat(nRandIter, [&] {
        for (size_t i = 0; i < basePattern.size(); ++i) 
        {
            string pattern = basePattern;

            pattern[i] = 'N';
            pattern[helpers::randIntRangeExcluded(0, pattern.size() - 1, i)] = 'N';

            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 2);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(7) == 1);
        }
    });
}

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors for partial segment match", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGTT{A,C}ACGTT{,A}ACGTT{AAAAA,GGGG}ACGTT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "NNGT", "ANNT", "NCNT", "NNTT", "NGNT", "CNNT" })
    {
        unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res1.size() == 0);

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 2);
        REQUIRE(res.size() == 4);

        for (int i : { 0, 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }            
    }
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple empty words at different positions for exact for spanning correct", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,AA,CC,GG}{AA,,CC,GG}{AA,CC,,GG}{AA,CC,GG,}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    // Only letters from the first and the last segment.
    unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTACGT", 1);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(5) == 1);

    // Letters from the first, the last, and one (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAACGT", "ACGTCCACGT", "ACGTGGACGT" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }

    // Letters from the first, the last, and two (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAAAACGT", "ACGTCCCCACGT", "ACGTGGGGACGT", "ACGTAACCACGT", "ACGTCCAAACGT", "ACGTCCGGACGT", "ACGTGGCCACGT", "ACGTAAGGACGT", "ACGTGGAAACGT" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple empty words at different positions for 1 error for spanning correct", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,AA,CC,GG}{AA,,CC,GG}{AA,CC,,GG}{AA,CC,GG,}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    // Only letters from the first and the last segment.
    string basePattern1 = "ACGTACGT";

    for (size_t i = 0; i < basePattern1.size(); ++i)
    {
        string pattern = basePattern1;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }

    // Letters from the first, the last, and one (any) of the duplicated segments located in the middle.
    for (const string &basePattern2 : { "ACGTAAACGT", "ACGTCCACGT", "ACGTGGACGT" })
    {
        for (size_t i = 0; i < basePattern2.size(); ++i)
        {
            string pattern = basePattern2;
            pattern[i] = 'N';

            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(5) == 1);
        }
    }

    // Letters from the first, the last, and two (any) of the duplicated segments located in the middle.
    for (const string &basePattern3 : { "ACGTAAAAACGT", "ACGTCCCCACGT", "ACGTGGGGACGT", "ACGTAACCACGT", "ACGTCCAAACGT", "ACGTCCGGACGT", "ACGTGGCCACGT", "ACGTAAGGACGT", "ACGTGGAAACGT" })
    {
        for (size_t i = 0; i < basePattern3.size(); ++i)
        {
            string pattern = basePattern3;
            pattern[i] = 'N';

            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(5) == 1);
        }
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for whole segment match for exact for long generated text where start and end are determinate", "[approx]")
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

    unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, det, 1);
    REQUIRE(res.size() == 2);

    REQUIRE(res.count(0) == 1);
    REQUIRE(res.count(nTextRepeats + 1) == 1);
}

TEST_CASE("is approx matching multiple determinate segments correct for partial segment match for exact for long generated text where start and end are determinate", "[approx]")
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

    for (const string &pattern : { "AACTN", "NAACT" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 2);

        REQUIRE(res.count(0) == 1);
        REQUIRE(res.count(nTextRepeats + 1) == 1);
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for whole segment match for 1 error for long generated text where start and end are determinate", "[approx]")
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
    
    for (size_t i = 0; i < det.size(); ++i)
    {
        string pattern = det;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 2);

        REQUIRE(res.count(0) == 1);
        REQUIRE(res.count(nTextRepeats + 1) == 1);
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for whole segment match for exact for long generated text where segments are once determinate once indeterminate", "[approx]")
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

    unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, det, 1);
    REQUIRE(res.size() == nTextRepeats);

    for (int i = 1; i < 2 * nTextRepeats; i += 2)
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is approx matching multiple determinate segments correct for whole segment match for 1 error for long generated text where segments are once determinate once indeterminate", "[approx]")
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

    for (size_t i = 0; i < det.size(); ++i)
    {
        string pattern = det;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == nTextRepeats);

        for (int i = 1; i < 2 * nTextRepeats; i += 2)
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for spanning correct for exact", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "TAAC", "TCAC" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for repeated spanning multiple variants correct for exact", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C,G,T}{AAA,CCC,GGG,TTT}{ACGT,TGCA}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "TAAAAAC", "TAAAATG", "TCCCCAC", "TCCCCTG", "TGGGGAC", "TGGGGTG", "TTTTTAC", "TTTTTTG" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(3) == 1);
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for repeated spanning multiple variants correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C,G,T}{AAA,CCC,GGG,TTT}{ACGT,TGCA}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &basePattern : { "TAAAAAC", "TAAAATG", "TCCCCAC", "TCCCCTG", "TGGGGAC", "TGGGGTG", "TTTTTAC", "TTTTTTG" })
    {
        for (size_t i = 0; i < basePattern.size(); ++i)
        {
            string pattern = basePattern;
            pattern[i] = 'N';

            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(3) == 1);
        }
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for spanning correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{A,C}ACGT", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    for (const string &pattern : { "TAAN", "TANC", "TNAC", "NAAC", "TCAN", "TCNC", "NCAC" })
    {
        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(2) == 1);
    }
}

TEST_CASE("is approx matching for contiguous indeterminate segments correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}{,AA}{,AAAAA,TTTT}{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, "CGTCAANA", 1);
    
    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(3) == 1);

    unordered_set<int> res2 = sopang.matchApprox(segments, nSegments, segmentSizes, "AAAANAAAC", 1);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(5) == 1);

    unordered_set<int> res3 = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTANT", 1);

    REQUIRE(res3.size() == 1);
    REQUIRE(res3.count(3) == 1);
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words for spanning correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{AA,CCAA}ACGT{AAA,CCC,TTT}{,AA}{A,C,}{AAA,CCC,T}TTCC{AA,CC}AAA", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    unordered_set<int> res1 = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTAAN", 1);
    
    REQUIRE(res1.size() == 1);
    REQUIRE(res1.count(2) == 1);

    unordered_set<int> res2 = sopang.matchApprox(segments, nSegments, segmentSizes, "AAAANAAA", 1);

    REQUIRE(res2.size() == 1);
    REQUIRE(res2.count(5) == 1);

    string basePattern = "TTTAACCCCTTC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res3 = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res3.size() == 1);
        REQUIRE(res3.count(6) == 1);
    }
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words for spanning correct for 1 error for non-dna alphabet", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("AACABBCBBC{A,AAB,ACCA}BB{C,ACABBCBB,CBA}BACABBC{B,CABB,BBC,AACABB,CBC}", &nSegments, &segmentSizes);

    Sopang sopang("ABCN");

    unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, "CABNCB", 1);
    REQUIRE(res.size() == 4);
    
    for (int i : { 0, 3, 4, 5 })
    {
        REQUIRE(res.count(i) == 1);
    }
}

TEST_CASE("is approx matching pattern starting and ending with text correct", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("{A,C}ACGT{G,C}ACGT{,T}ACGT{GGGG,TTTT,C}AAC{A,G}TGA", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    string basePattern = "AACGTGA";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);

        REQUIRE(res.size() == 2);
        
        REQUIRE(res.count(3) == 1);
        REQUIRE(res.count(9) == 1);
    }
}

TEST_CASE("is approx matching pattern length 8 correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    string basePattern = "ACGTACGT";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 3);

        for (int i : { 2, 4, 6 })
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching pattern length 8 correct for 2 errors", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,A}CGT{,AAAAA,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);
    string basePattern = "ACGTACGT";

    repeat(nRandIter, [&] {
        for (size_t i = 0; i < basePattern.size(); ++i)
        {
            string pattern = basePattern;
            
            pattern[i] = 'N';
            pattern[helpers::randIntRangeExcluded(0, pattern.size() - 1, i)] = 'N';

            unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 2);
            REQUIRE(res.size() == 3);

            for (int i : { 2, 4, 6 })
            {
                REQUIRE(res.count(i) == 1);
            }
        }
    });
}

TEST_CASE("is approx matching pattern length 10 correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,GGGG,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    string basePattern = "ACGTACGTAC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        REQUIRE(res.size() == 2);

        REQUIRE(res.count(4) == 1);
        REQUIRE(res.count(6) == 1);
    }
}

TEST_CASE("is approx matching pattern length 12 correct for 1 error", "[approx]")
{
    int nSegments;
    int *segmentSizes;
    const string *const *segments = parsing::parseTextArray("ACGT{,A,C}ACGT{,GGGG,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang(alphabet);

    string basePattern = "ACGTACGTACGT";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<int> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, 1);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(4) == 1);
    }
}

TEST_CASE("is filling approx mask buffer correct for a predefined pattern", "[approx]")
{
    const string pattern = "ACAACGT";
    
    Sopang sopang(alphabet);
    SopangWhitebox::fillPatternMaskBufferApprox(sopang, "ACAACGT");

    const size_t saCounterSize = SopangWhitebox::getSACounterSize(sopang);
    const size_t wordSize = SopangWhitebox::getWordSize(sopang);
    
    const size_t saBitShiftRight = wordSize - saCounterSize;
    const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);
    
    uint64_t maskA = maskBuffer[static_cast<size_t>('A')];
    const vector<uint64_t> expectedA { 0x0, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1 };

    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskA << (saBitShiftRight - i * saCounterSize)) >> saBitShiftRight) == expectedA[i]);
    }

    uint64_t maskC = maskBuffer[static_cast<size_t>('C')];
    const vector<uint64_t> expectedC { 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskC << (saBitShiftRight - i * saCounterSize)) >> saBitShiftRight) == expectedC[i]);
    }

    uint64_t maskG = maskBuffer[static_cast<size_t>('G')];
    const vector<uint64_t> expectedG { 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskG << (saBitShiftRight - i * saCounterSize)) >> saBitShiftRight) == expectedG[i]);
    }

    uint64_t maskT = maskBuffer[static_cast<size_t>('T')];
    const vector<uint64_t> expectedT { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskT << (saBitShiftRight - i * saCounterSize)) >> saBitShiftRight) == expectedT[i]);
    }

    uint64_t maskN = maskBuffer[static_cast<size_t>('N')];
    const vector<uint64_t> expectedN { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskN << (saBitShiftRight - i * saCounterSize)) >> saBitShiftRight) == expectedN[i]);
    }
}

TEST_CASE("is filling approx mask buffer correct for repeated same character in pattern", "[approx]")
{
    for (const char c : alphabet)
    {
        for (int size = 1; size <= maxPatSize; ++size)
        {
            string pattern = "";
            repeat(size, [c, &pattern] { pattern += c; });

            Sopang sopang(alphabet);

            const size_t saCounterSize = SopangWhitebox::getSACounterSize(sopang);
            const size_t wordSize = SopangWhitebox::getWordSize(sopang);
            
            const size_t saBitShiftRight = wordSize - saCounterSize;
            const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

            SopangWhitebox::fillPatternMaskBufferApprox(sopang, pattern);

            for (int shift = 0; shift < size; ++shift)
            {
                for (const char curC : alphabet)
                {
                    if (curC == c) // Corresponding occurrences should be set to 0.
                    {
                        REQUIRE(((maskBuffer[static_cast<size_t>(curC)] << (saBitShiftRight - shift * saCounterSize)) >> saBitShiftRight) == 0x0ULL);
                    }
                    else
                    {
                        REQUIRE(((maskBuffer[static_cast<size_t>(curC)] << (saBitShiftRight - shift * saCounterSize)) >> saBitShiftRight) == 0x1ULL);
                    }
                }
            }
        }
    }
}

} // namespace sopang
