#ifndef SOPANG_HPP
#define SOPANG_HPP

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

class Sopang
{
public:
    static const string *const *parseTextArray(string text, unsigned *nSegments, unsigned **segmentSizes);
    static vector<string> parsePatterns(string patternsStr);

    unordered_set<unsigned> matchArray(const string *const *segments,
                                       unsigned nSegments, const unsigned *segmentSizes,
                                       const string &pattern, const string &alphabet);
private:
    void fillPatternMaskBuffer(const string &pattern, const string &alphabet);

    static constexpr unsigned dBufferSize = 262144;
    static constexpr unsigned maskBufferSize = 91; // Up to 'Z' = 90
    static constexpr unsigned wordSize = 64; // Bits

    static constexpr uint64_t allOnes = ~(0x0ULL);

    uint64_t dBuffer[dBufferSize];
    uint64_t maskBuffer[maskBufferSize];
};

#endif // SOPANG_HPP
