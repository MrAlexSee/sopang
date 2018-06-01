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
}

TEST_CASE("is parsing patterns single correct", "[parsing]")
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

TEST_CASE("is parsing patterns multiple empty and trailing correct", "[parsing]")
{
    string str = "\n\n\n\n1\n\n2\n3\n4\n\n\n5\n\n\n\n\n\n";
    vector<string> patterns = Sopang::parsePatterns(str);

    REQUIRE(patterns.size() == 5);
    REQUIRE(Helpers::join(patterns, "") == "12345" );
}

TEST_CASE("is filling mask buffer correct", "[matching]")
{
    Sopang sopang;
    SopangWhitebox::fillPatternMaskBuffer(sopang, "AAAAA", alphabet);

    uint64_t *maskBuffer = SopangWhitebox::getMaskBuffer(sopang);

    for (uint64_t m = 0x1; m <= 0x5; ++m)
    {
        REQUIRE((maskBuffer['A'] & m) == 0x0);

        REQUIRE((maskBuffer['C'] & m) != 0x0);
        REQUIRE((maskBuffer['G'] & m) != 0x0);
        REQUIRE((maskBuffer['T'] & m) != 0x0);
        REQUIRE((maskBuffer['N'] & m) != 0x0);
    }
}

}
