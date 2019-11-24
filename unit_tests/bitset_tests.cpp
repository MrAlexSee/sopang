#include <set>

#include "catch.hpp"

#include "../bitset.hpp"

using namespace std;

namespace sopang
{

namespace
{

constexpr int N = 100;

}

TEST_CASE("is any/empty/count correct for empty bitset", "[bitset]")
{
    BitSet<N> bitset;

    REQUIRE(not bitset.any());
    REQUIRE(bitset.empty());

    REQUIRE(bitset.count() == 0);
}

TEST_CASE("is any/empty/count correct for filled bitset", "[bitset]")
{
    BitSet<N> bitset{ 1, 2, 4 };

    REQUIRE(bitset.any());
    REQUIRE(not bitset.empty());

    REQUIRE(bitset.count() == 3);
}

TEST_CASE("is set/reset and test correct", "[bitset]")
{
    BitSet<N> bitset;

    REQUIRE(not bitset.test(1));
    REQUIRE(not bitset.test(2));
    REQUIRE(not bitset.test(4));

    bitset.set(1);
    bitset.set(2);
    bitset.set(4);

    REQUIRE(bitset.test(1));
    REQUIRE(bitset.test(2));
    REQUIRE(bitset.test(4));

    bitset.reset(2);
    
    REQUIRE(bitset.test(1));
    REQUIRE(not bitset.test(2));
    REQUIRE(bitset.test(4));

    bitset.set(2);

    REQUIRE(bitset.test(1));
    REQUIRE(bitset.test(2));
    REQUIRE(bitset.test(4));

    bitset.reset(1);
    bitset.reset(2);
    bitset.reset(4);

    REQUIRE(not bitset.test(1));
    REQUIRE(not bitset.test(2));
    REQUIRE(not bitset.test(4));
}

TEST_CASE("is a set/reset all sequence correct", "[bitset]")
{
    BitSet<N> bitset;
    REQUIRE(bitset.empty());
    
    bitset.set();
    REQUIRE(not bitset.empty());

    bitset.reset();
    REQUIRE(bitset.empty());

    bitset.reset();
    REQUIRE(bitset.empty());

    bitset.set();
    REQUIRE(not bitset.empty());

    bitset.set();
    REQUIRE(not bitset.empty());

    bitset.reset();
    REQUIRE(bitset.empty());
}

TEST_CASE("is equality correct", "[bitset]")
{
    BitSet<N> bitset1{ 8, 50, 51 };
    BitSet<N> bitset2{ 8, 50, 52 };
    BitSet<N> bitset3;

    bitset3.set(8);
    bitset3.set(50);
    bitset3.set(51);

    REQUIRE(bitset1 == bitset3);
    REQUIRE(not (bitset1 == bitset2));
    REQUIRE(not (bitset2 == bitset3));
}

TEST_CASE("is conversion to set correct", "[bitset]")
{
    REQUIRE(BitSet<N>().toSet() == set<int>());

    BitSet<N> bitset{ 8, 50, 51 };
    REQUIRE(bitset.toSet() == set<int>{ 8, 50, 51 });
}

TEST_CASE("is AND operator correct", "[bitset]")
{
    REQUIRE((BitSet<N>() & BitSet<N>()) == BitSet<N>());

    BitSet<N> bitset1{ 8, 12, 14, 15, 16, 50, 51 };
    BitSet<N> bitset2{ 1, 2, 7, 9, 10, 11, 12, 13, 15, 49, 52, 53, 77 };

    BitSet<N> res = (bitset1 & bitset2);
    REQUIRE(res == BitSet<N>{ 12, 15 });
}

TEST_CASE("is OR equal operator correct", "[bitset]")
{
    BitSet<N> bitset1;
    bitset1 |= BitSet<N>();

    REQUIRE(bitset1 == BitSet<N>());

    BitSet<N> bitset2{ 8, 14, 15 };
    BitSet<N> bitset3{ 2, 8, 13, 16, 55 };

    bitset2 |= bitset3;

    REQUIRE(bitset2 == BitSet<N>{ 2, 8, 13, 14, 15, 16, 55 });
    REQUIRE(bitset3 == BitSet<N>{ 2, 8, 13, 16, 55 });

    bitset2 |= bitset1;
    bitset3 |= bitset1;

    REQUIRE(bitset2 == BitSet<N>{ 2, 8, 13, 14, 15, 16, 55 });
    REQUIRE(bitset3 == BitSet<N>{ 2, 8, 13, 16, 55 });
}

} // namespace sopang
