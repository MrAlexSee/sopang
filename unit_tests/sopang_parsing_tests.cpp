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

TEST_CASE("is parsing text for determinate and indeterminate segments correct where start and end are indeterminate", "[parsing]")
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

TEST_CASE("is parsing text for determinate and indeterminate segments correct where start and end is determinate", "[parsing]")
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

TEST_CASE("is parsing text for a repeated indeterminate segment correct", "[parsing]")
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

TEST_CASE("is parsing text with spaces correct", "[parsing]")
{
    unsigned nSegments = 1000;
    unsigned *segmentSizes;

    const string *const *segments = Sopang::parseTextArray("{AC, CG}{A,,C C C}", &nSegments, &segmentSizes);

    REQUIRE(nSegments == 2);

    REQUIRE(segmentSizes[0] == 2);
    REQUIRE(segmentSizes[1] == 3);
    
    // Spaces are not ignored.
    REQUIRE(segments[0][0] == "AC");
    REQUIRE(segments[0][1] == " CG");
    REQUIRE(segments[1][0] == "A");
    REQUIRE(segments[1][1] == "");
    REQUIRE(segments[1][2] == "C C C");
}

TEST_CASE("is parsing patterns for an empty string correct", "[parsing]")
{
    vector<string> empty = Sopang::parsePatterns("");
    REQUIRE(empty.empty());
}

TEST_CASE("is parsing patterns for a single pattern correct", "[parsing]")
{
    string str = "1";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 1);
    REQUIRE(patterns[0] == str);
}

TEST_CASE("is parsing patterns with single newlines correct", "[parsing]")
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

TEST_CASE("is parsing sources for an empty string correct", "[parsing]")
{
    int sourceCount;
    const auto empty = Sopang::parseSources("", sourceCount);

    REQUIRE(empty.empty());
}

TEST_CASE("does parsing sources throw for bad strings", "[parsing]")
{
    int sourceCount;

    for (const string &sources : vector<string>{ "{{0,1,2}{3,5}{7}}", "5\n{{0,X,2}{3,5}{7}}",
        "5\n{{0,1,2}{3,5}{7}", "5\n{{0;2}{3,5}{7}" })
    {
        REQUIRE_THROWS_AS(Sopang::parseSources(sources, sourceCount), runtime_error);
    }
}

TEST_CASE("is parsing sources for a single segment correct", "[parsing]")
{
    int sourceCount;
    const auto sources = Sopang::parseSources("8\n{{0,1,2}{3,5}{7}}", sourceCount);
    
    REQUIRE(sourceCount == 8);
    REQUIRE(sources.size() == 1);

    const auto variants = sources[0];
    REQUIRE(variants.size() == 4);

    REQUIRE(variants[0] == set<int>{ 0, 1, 2 });
    REQUIRE(variants[1] == set<int>{ 3, 5 });
    REQUIRE(variants[2] == set<int>{ 7 });
    REQUIRE(variants[3] == set<int>{ 4, 6 }); // Reference sequence = remaining sources.
}

TEST_CASE("is parsing sources correct for a single segment with leading and trailing whitespace correct", "[parsing]")
{
    int sourceCount;
    const auto sources = Sopang::parseSources("    \t    \t\t   8\n{{0,1,2}{3,5}{7}}            \t", sourceCount);

    REQUIRE(sourceCount == 8);
    REQUIRE(sources.size() == 1);

    const auto variants = sources[0];
    REQUIRE(variants.size() == 4);

    REQUIRE(variants[0] == set<int>{ 0, 1, 2 });
    REQUIRE(variants[1] == set<int>{ 3, 5 });
    REQUIRE(variants[2] == set<int>{ 7 });
    REQUIRE(variants[3] == set<int>{ 4, 6 }); // Reference sequence = remaining sources.
}

TEST_CASE("is parsing sources for multiple single segments correct", "[parsing]")
{
    int sourceCount;
    const auto sources = Sopang::parseSources("3\n{0,2}{0}{1,2}", sourceCount);

    REQUIRE(sourceCount == 3);
    REQUIRE(sources.size() == 3);

    REQUIRE(sources[0].size() == 2);
    REQUIRE(sources[1].size() == 2);
    REQUIRE(sources[2].size() == 2);

    REQUIRE(sources[0][0] == set<int>{ 0, 2 });
    REQUIRE(sources[0][1] == set<int>{ 1 });

    REQUIRE(sources[1][0] == set<int>{ 0 });
    REQUIRE(sources[1][1] == set<int>{ 1, 2 });

    REQUIRE(sources[2][0] == set<int>{ 1, 2 });
    REQUIRE(sources[2][1] == set<int>{ 0 });
}

TEST_CASE("is parsing sources for multiple segments correct", "[parsing]")
{
    int sourceCount;
    const auto sources = Sopang::parseSources("5\n{{1,2}{0}}{0}{{0}{1}{2}}", sourceCount);

    REQUIRE(sourceCount == 5);
    REQUIRE(sources.size() == 3);

    REQUIRE(sources[0].size() == 3);
    REQUIRE(sources[1].size() == 2);
    REQUIRE(sources[2].size() == 4);

    REQUIRE(sources[0][0] == set<int>{ 1, 2 });
    REQUIRE(sources[0][1] == set<int>{ 0 });
    REQUIRE(sources[0][2] == set<int>{ 3, 4 });

    REQUIRE(sources[1][0] == set<int>{ 0 });
    REQUIRE(sources[1][1] == set<int>{ 1, 2, 3, 4 });

    REQUIRE(sources[2][0] == set<int>{ 0 });
    REQUIRE(sources[2][1] == set<int>{ 1 });
    REQUIRE(sources[2][2] == set<int>{ 2 });
    REQUIRE(sources[2][3] == set<int>{ 3, 4 });
}

TEST_CASE("is parsing sources for multiple segments correct 2", "[parsing]")
{
    int sourceCount;
    const auto sources = Sopang::parseSources("10\n{{3}{7}{8}{1}{2}}{{6}{7}{0}{4}{1}{8}}", sourceCount);

    REQUIRE(sourceCount == 10);
    REQUIRE(sources.size() == 2);

    REQUIRE(sources[0].size() == 6);
    REQUIRE(sources[1].size() == 7);

    REQUIRE(sources[0] == vector<set<int>>{ { 3 }, { 7 }, { 8 }, { 1 }, { 2 }, { 0, 4, 5, 6, 9 } });
    REQUIRE(sources[1] == vector<set<int>>{ { 6 }, { 7 }, { 0 }, { 4 }, { 1 }, { 8 }, { 2, 3, 5, 9 } });
}

} // namespace sopang
