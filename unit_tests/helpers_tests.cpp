#include "catch.hpp"
#include "repeat.hpp"

#include "../helpers.hpp"

#include <cctype>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

namespace sopang
{

namespace
{

constexpr int nRandIter = 100;

constexpr int nRandMedianElems = 101;
constexpr int minVal = 0;
constexpr int maxVal = 1000;

constexpr int nVectorStrings = 10;
constexpr int maxStrSize = 10;

const string testText = "Ala ma kota, a Jarek ma psa.";

}

TEST_CASE("is median calculation correct for { 1, 2, 3 }", "[collections]")
{
    int median1;
    helpers::calcStatsMedian({ 1, 2, 3 }, &median1);
    REQUIRE(median1 == 2);

    int median2;
    helpers::calcStatsMedian({ 3, 2, 1 }, &median2);
    REQUIRE(median1 == 2);
}

TEST_CASE("is median calculation correct for { 1, 2, 3, 4, 5 }", "[collections]")
{
    int median1;
    helpers::calcStatsMedian({ 1, 2, 3, 4, 5 }, &median1);
    REQUIRE(median1 == 3);

    int median2;
    helpers::calcStatsMedian({ 5, 4, 3, 2, 1 }, &median2);
    REQUIRE(median1 == 3);
}

TEST_CASE("is median calculation correct for { 1, 2, 3, 4, 5, 6 }", "[collections]")
{
    int median1;
    helpers::calcStatsMedian({ 1, 2, 3, 4, 5, 6 }, &median1);
    REQUIRE(median1 == 4);

    int median2;
    helpers::calcStatsMedian({ 6, 5, 4, 3, 2, 1 }, &median2);
    REQUIRE(median1 == 4);
}

TEST_CASE("is median calculation correct for randomized odd #elems", "[collections]")
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(minVal + 1, maxVal - 1);

    repeat(nRandIter, [&dist, &mt] {
        int randMedian = dist(mt);
        vector<int> vec { randMedian };

        uniform_int_distribution<int> low(minVal, randMedian - 1);
        uniform_int_distribution<int> high(randMedian + 1, maxVal);

        // We add either a smaller or a larger number to a median, so that this is always our median.
        for (int i = 0; i < (nRandMedianElems - 1) / 2; ++i)
        {
            vec.push_back(low(mt));
            vec.push_back(high(mt));
        }

        int median;
        helpers::calcStatsMedian(vec, &median);

        REQUIRE(median == randMedian);
    });
}

TEST_CASE("is file dumping-reading symmetric for random file names", "[files]")
{
    repeat(nRandIter, [] {
        string fileName = helpers::genRandomStringAlphNum(maxStrSize);

        helpers::dumpToFile(testText, fileName);
        string read = helpers::readFile(fileName);

        REQUIRE(testText == read);
        helpers::removeFile(fileName);
    });
}

TEST_CASE("is file removing correct", "[files]")
{
    repeat(nRandIter, [] {
        string fileName = helpers::genRandomStringAlphNum(maxStrSize);
        helpers::dumpToFile(testText, fileName);

        REQUIRE(helpers::isFileReadable(fileName));

        helpers::removeFile(fileName);
        REQUIRE(helpers::isFileReadable(fileName) == false);
    });
}

TEST_CASE("is random generation from range without excluded correct", "[random]")
{
    const int start = 0, end = 100;

    repeat(nRandIter, [start, end] {
        int n = helpers::randIntRangeExcluded(start, end, -1);
        REQUIRE(n >= start);
        REQUIRE(n <= end);
    });
}

TEST_CASE("is random generation from range with excluded correct", "[random]")
{
    repeat(nRandIter, [] {
        int n0 = helpers::randIntRangeExcluded(0, 1, 1);
        REQUIRE(n0 == 0);

        int n1 = helpers::randIntRangeExcluded(0, 1, 0);
        REQUIRE(n1 == 1);

        int n2 = helpers::randIntRangeExcluded(0, 5, 3);
        REQUIRE(n2 != 3);
    });
}

TEST_CASE("does random generation from bad range throw", "[random]")
{
    REQUIRE_THROWS_AS(helpers::randIntRangeExcluded(10, -5, 0), invalid_argument);
    REQUIRE_THROWS_AS(helpers::randIntRangeExcluded(0, 0, 0), invalid_argument);
}

TEST_CASE("is join correct for empty vector", "[strings]")
{
    REQUIRE(helpers::join(vector<int> {}, "") == "");
    REQUIRE(helpers::join(vector<string> {}, "") == "");

    REQUIRE(helpers::join(vector<int> {}, ",") == "");
    REQUIRE(helpers::join(vector<string> {}, ",") == "");
}

TEST_CASE("is join correct", "[strings]")
{
    REQUIRE(helpers::join(vector<int> {1, 2, 3}, "") == "123");
    REQUIRE(helpers::join(vector<int> {1, 2, 3}, ",") == "1,2,3");

    REQUIRE(helpers::join(vector<string> {"ala", "ma", "kota"}, "") == "alamakota");
    REQUIRE(helpers::join(vector<string> {"ala", "ma", "kota"}, ",") == "ala,ma,kota");
    REQUIRE(helpers::join(vector<string> {"ala", "ma", "kota"}, "--") == "ala--ma--kota");
}

TEST_CASE("is removing empty strings correct", "[strings]")
{
    vector<string> v1 { "ala", "", "ma", "", "kota", "" };
    helpers::removeEmptyStrings(v1);

    REQUIRE(v1.size() == 3);
    REQUIRE(helpers::join(v1, "") == "alamakota");

    vector<string> v2 { "ala", "", "ma", "", "kota", "", "", "", "", "", "", "", "a nie psa", "" };
    helpers::removeEmptyStrings(v2);

    REQUIRE(v2.size() == 4);
    REQUIRE(helpers::join(v2, "") == "alamakotaa nie psa");
}

TEST_CASE("is removing empty strings correct for randomized", "[strings]")
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(minVal + 1, maxVal - 1);

    repeat(nRandIter, [&dist, &mt] {
        vector<string> vec;

        for (int i = 0; i < nVectorStrings; ++i)
        {
            vec.push_back(helpers::genRandomStringAlphNum(maxStrSize));
        }

        for (int i = 0; i < nVectorStrings; ++i)
        {
            vec.push_back("");
        }

        REQUIRE(vec.size() == 2 * nVectorStrings);

        helpers::removeEmptyStrings(vec);
        REQUIRE(vec.size() == nVectorStrings);
    });
}

TEST_CASE("is generated random alphanumeric string of correct size and sampled from correct alphabet", "[strings]")
{
    repeat(nRandIter, [] {
        for (int size = 0; size <= maxStrSize; ++size)
        {
            string str = helpers::genRandomStringAlphNum(size);
            REQUIRE(str.size() == size);

            for (const char c : str)
            {
                REQUIRE(isalnum(c));
            }
        }
    });
}

TEST_CASE("is generated random string of correct size and sampled from correct alphabet", "[strings]")
{
    string alphabet = "ACGTN";
    unordered_set<char> chars(alphabet.begin(), alphabet.end());

    repeat(nRandIter, [&alphabet, &chars] {
        for (int size = 0; size <= maxStrSize; ++size)
        {
            string str = helpers::genRandomString(size, alphabet);
            REQUIRE(str.size() == size);

            for (const char c : str)
            {
                REQUIRE(chars.count(c) == 1);
            }
        }
    });
}

} // namespace sopang
