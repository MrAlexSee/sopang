#define CATCH_CONFIG_MAIN

#include <random>

#include "catch.hpp"
#include "repeat.hpp"

#include "../helpers.hpp"

using namespace inverted_basilisk;
using namespace std;

const int nRandMedianElems = 101;
const int nRandIter = 100;

TEST_CASE("is median calculation correct for { 1, 2, 3 }", "[median]")
{
    int median1;
    Helpers::calcStatsMedian(vector<int> { 1, 2, 3 }, &median1);
    REQUIRE(median1 == 2);

    int median2;
    Helpers::calcStatsMedian(vector<int> { 3, 2, 1 }, &median2);
    REQUIRE(median1 == 2);
}

TEST_CASE("is median calculation correct for { 1, 2, 3, 4, 5 }", "[median]")
{
    int median1;
    Helpers::calcStatsMedian(vector<int> { 1, 2, 3, 4, 5 }, &median1);
    REQUIRE(median1 == 3);

    int median2;
    Helpers::calcStatsMedian(vector<int> { 5, 4, 3, 2, 1 }, &median2);
    REQUIRE(median1 == 3);
}

TEST_CASE("is median calculation correct for { 1, 2, 3, 4, 5, 6 }", "[median]")
{
    int median1;
    Helpers::calcStatsMedian(vector<int> { 1, 2, 3, 4, 5, 6 }, &median1);
    REQUIRE(median1 == 4);

    int median2;
    Helpers::calcStatsMedian(vector<int> { 6, 5, 4, 3, 2, 1 }, &median2);
    REQUIRE(median1 == 4);
}

TEST_CASE("is median calculation correct for randomized odd #elems", "[median]")
{
    const int minVal = 0, maxVal = 1000;

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(minVal + 1, maxVal - 1);

    repeat(nRandIter, [&dist, &mt]() {
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
        Helpers::calcStatsMedian(vec, &median);

        REQUIRE(median == randMedian);
    });
}

TEST_CASE("is removing empty strings correct", "[string]")
{
    
}
