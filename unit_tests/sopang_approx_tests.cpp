#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "catch.hpp"
#include "repeat.hpp"
#include "sopang_whitebox.hpp"

#include "../helpers.hpp"
#include "../sopang.hpp"

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

TEST_CASE("is approx matching for a single segment exact correct for partial segment match", "[approx]")
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

TEST_CASE("is approx matching for a single segment correct for 1 error for whole segment match", "[approx]")
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

TEST_CASE("is approx matching for a single segment correct for 1 error for partial segment match", "[approx]")
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

TEST_CASE("is approx matching for a single longer segment correct for 1 error for partial segment match", "[approx]")
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

TEST_CASE("is approx matching for a single longer segment at 2nd position correct for 1 error for partial segment match", "[approx]")
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

TEST_CASE("is approx matching for a single segment correct for 2 errors for partial segment match", "[approx]")
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

TEST_CASE("is approx matching multiple determinate segments exact correct for partial segment match", "[approx]")
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

TEST_CASE("is approx matching multiple determinate segments correct for 1 error for whole segment match", "[approx]")
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

TEST_CASE("is approx matching multiple determinate segments correct for 1 error for partial segment match", "[approx]")
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

TEST_CASE("is approx matching for a single segment correct for 2 errors for whole segment match", "[approx]")
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

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors for whole segment match", "[approx]")
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

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for exact", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, "CTATGCTC", alphabet, 1);
   
    REQUIRE(res.size() == 1);
    REQUIRE(res.count(7) == 1);
}

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang;

    string basePattern = "CTATGCTC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(7) == 1);
    }
}

TEST_CASE("is approx matching multiple short contiguous indeterminate segments correct for 2 errors", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("AC{A,G,C}T{,A}{A,T}{C,GC}{A,CA,T}{CA,GG}", &nSegments, &segmentSizes);

    Sopang sopang;
    string basePattern = "ACCTATGCTCA";

    repeat(nRandIter, [&] {
        for (size_t i = 0; i < basePattern.size(); ++i) 
        {
            string pattern = basePattern;

            pattern[i] = 'N';
            pattern[Helpers::randIntRangeExcluded(0, pattern.size() - 1, i)] = 'N';

            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 2);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(7) == 1);
        }
    });
}

TEST_CASE("is approx matching multiple determinate segments correct for 2 errors for partial segment match", "[approx]")
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

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple empty words at different positions for exact for spanning correct", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,AA,CC,GG}{AA,,CC,GG}{AA,CC,,GG}{AA,CC,GG,}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    // Only letters from the first and the last segment.
    unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, "ACGTACGT", alphabet, 1);

    REQUIRE(res.size() == 1);
    REQUIRE(res.count(5) == 1);

    // Letters from the first, the last, and one (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAACGT", "ACGTCCACGT", "ACGTGGACGT" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }

    // Letters from the first, the last, and two (any) of the duplicated segments located in the middle.
    for (const string &pattern : { "ACGTAAAAACGT", "ACGTCCCCACGT", "ACGTGGGGACGT", "ACGTAACCACGT", "ACGTCCAAACGT", "ACGTCCGGACGT", "ACGTGGCCACGT", "ACGTAAGGACGT", "ACGTGGAAACGT" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(5) == 1);
    }
}

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple empty words at different positions for 1 error for spanning correct", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,AA,CC,GG}{AA,,CC,GG}{AA,CC,,GG}{AA,CC,GG,}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    // Only letters from the first and the last segment.
    string basePattern1 = "ACGTACGT";

    for (size_t i = 0; i < basePattern1.size(); ++i)
    {
        string pattern = basePattern1;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

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

            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

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

            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

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

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, det, alphabet, 1);
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

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "AACTN", "NAACT" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
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

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;
    
    for (size_t i = 0; i < det.size(); ++i)
    {
        string pattern = det;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
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

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, det, alphabet, 1);
    REQUIRE(res.size() == nTextRepeats);

    for (unsigned i = 1; i < 2 * nTextRepeats; i += 2)
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

    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);

    Sopang sopang;

    for (size_t i = 0; i < det.size(); ++i)
    {
        string pattern = det;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == nTextRepeats);

        for (unsigned i = 1; i < 2 * nTextRepeats; i += 2)
        {
            REQUIRE(res.count(i) == 1);
        }
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for spanning correct for exact", "[approx]")
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

TEST_CASE("is approx matching single indeterminate and determinate segments for repeated spanning multiple variants correct for exact", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C,G,T}{AAA,CCC,GGG,TTT}{ACGT,TGCA}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &pattern : { "TAAAAAC", "TAAAATG", "TCCCCAC", "TCCCCTG", "TGGGGAC", "TGGGGTG", "TTTTTAC", "TTTTTTG" })
    {
        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 1);
        REQUIRE(res.count(3) == 1);
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for repeated spanning multiple variants correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{A,C,G,T}{AAA,CCC,GGG,TTT}{ACGT,TGCA}ACGT", &nSegments, &segmentSizes);

    Sopang sopang;

    for (const string &basePattern : { "TAAAAAC", "TAAAATG", "TCCCCAC", "TCCCCTG", "TGGGGAC", "TGGGGTG", "TTTTTAC", "TTTTTTG" })
    {
        for (size_t i = 0; i < basePattern.size(); ++i)
        {
            string pattern = basePattern;
            pattern[i] = 'N';

            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

            REQUIRE(res.size() == 1);
            REQUIRE(res.count(3) == 1);
        }
    }
}

TEST_CASE("is approx matching single indeterminate and determinate segments for spanning correct for 1 error", "[approx]")
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

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words for spanning correct for 1 error", "[approx]")
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

TEST_CASE("is approx matching multiple indeterminate and determinate segments with multiple words for spanning correct for 1 error for non-dna alphabet", "[approx]")
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

TEST_CASE("is approx matching pattern starting and ending with text correct", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("{A,C}ACGT{G,C}ACGT{,T}ACGT{GGGG,TTTT,C}AAC{A,G}TGA", &nSegments, &segmentSizes);

    Sopang sopang;

    string basePattern = "AACGTGA";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);

        REQUIRE(res.size() == 2);
        
        REQUIRE(res.count(3) == 1);
        REQUIRE(res.count(9) == 1);
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
    string basePattern = "ACGTACGT";

    repeat(nRandIter, [&] {
        for (size_t i = 0; i < basePattern.size(); ++i)
        {
            string pattern = basePattern;
            
            pattern[i] = 'N';
            pattern[Helpers::randIntRangeExcluded(0, pattern.size() - 1, i)] = 'N';

            unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 2);
            REQUIRE(res.size() == 3);

            for (unsigned i : { 2, 4, 6 })
            {
                REQUIRE(res.count(i) == 1);
            }
        }
    });
}

TEST_CASE("is approx matching pattern length 10 correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,GGGG,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    string basePattern = "ACGTACGTAC";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        REQUIRE(res.size() == 2);

        REQUIRE(res.count(4) == 1);
        REQUIRE(res.count(6) == 1);
    }
}

TEST_CASE("is approx matching pattern length 12 correct for 1 error", "[approx]")
{
    unsigned nSegments;
    unsigned *segmentSizes;
    const string *const *segments = Sopang::parseTextArray("ACGT{,A,C}ACGT{,GGGG,TTTT}ACGT{A,}C", &nSegments, &segmentSizes);

    Sopang sopang;

    string basePattern = "ACGTACGTACGT";

    for (size_t i = 0; i < basePattern.size(); ++i)
    {
        string pattern = basePattern;
        pattern[i] = 'N';

        unordered_set<unsigned> res = sopang.matchApprox(segments, nSegments, segmentSizes, pattern, alphabet, 1);
        
        REQUIRE(res.size() == 1);
        REQUIRE(res.count(4) == 1);
    }
}

TEST_CASE("is filling approx mask buffer correct for a predefined pattern", "[approx]")
{
    const string pattern = "ACAACGT";
    
    Sopang sopang;
    SopangWhitebox::fillPatternMaskBufferApprox(sopang, "ACAACGT", alphabet);

    const unsigned saCounterSize = SopangWhitebox::getSACounterSize(sopang);
    const unsigned wordSize = SopangWhitebox::getWordSize(sopang);
    
    const unsigned saBitShiftRight = wordSize - saCounterSize;
    const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);
    
    uint64_t maskA = maskBuffer[static_cast<size_t>('A')];
    const vector<unsigned> expectedA { 0x0, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1 };

    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskA << saBitShiftRight - i * saCounterSize) >> saBitShiftRight) == expectedA[i]);
    }

    uint64_t maskC = maskBuffer[static_cast<size_t>('C')];
    const vector<unsigned> expectedC { 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskC << saBitShiftRight - i * saCounterSize) >> saBitShiftRight) == expectedC[i]);
    }

    uint64_t maskG = maskBuffer[static_cast<size_t>('G')];
    const vector<unsigned> expectedG { 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskG << saBitShiftRight - i * saCounterSize) >> saBitShiftRight) == expectedG[i]);
    }

    uint64_t maskT = maskBuffer[static_cast<size_t>('T')];
    const vector<unsigned> expectedT { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskT << saBitShiftRight - i * saCounterSize) >> saBitShiftRight) == expectedT[i]);
    }

    uint64_t maskN = maskBuffer[static_cast<size_t>('N')];
    const vector<unsigned> expectedN { 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1 };
    
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        REQUIRE(((maskN << saBitShiftRight - i * saCounterSize) >> saBitShiftRight) == expectedN[i]);
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

            Sopang sopang;

            const unsigned saCounterSize = SopangWhitebox::getSACounterSize(sopang);
            const unsigned wordSize = SopangWhitebox::getWordSize(sopang);
            
            const unsigned saBitShiftRight = wordSize - saCounterSize;
            const uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

            SopangWhitebox::fillPatternMaskBufferApprox(sopang, pattern, alphabet);

            for (int shift = 0; shift < size; ++shift)
            {
                for (const char curC : alphabet)
                {
                    if (curC == c) // Corresponding occurrences should be set to 0.
                    {
                        REQUIRE(((maskBuffer[static_cast<size_t>(curC)] << saBitShiftRight - shift * saCounterSize) >> saBitShiftRight) == 0x0);
                    }
                    else
                    {
                        REQUIRE(((maskBuffer[static_cast<size_t>(curC)] << saBitShiftRight - shift * saCounterSize) >> saBitShiftRight) == 0x1);
                    }
                }
            }
        }
    }
}

} // namespace sopang
