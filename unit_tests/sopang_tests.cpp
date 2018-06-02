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

TEST_CASE("is parsing patterns for an empty string correct", "[parsing]")
{
    
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
    REQUIRE(Helpers::join(patterns, "") == "12345" );
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
