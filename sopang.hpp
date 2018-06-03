#ifndef SOPANG_HPP
#define SOPANG_HPP

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#ifndef SOPANG_WHITEBOX
#define SOPANG_WHITEBOX
#endif

using namespace std;

namespace inverted_basilisk
{

class Sopang
{
public:
    Sopang();
    ~Sopang();

    /*
     *** PARSING
     */

    static const string *const *parseTextArray(string text, unsigned *nSegments, unsigned **segmentSizes);
    static vector<string> parsePatterns(string patternsStr);

    /*
     *** MATCHING
     */

    unordered_set<unsigned> match(const string *const *segments,
                                  unsigned nSegments, const unsigned *segmentSizes,
                                  const string &pattern, const string &alphabet);
private:
    void fillPatternMaskBuffer(const string &pattern, const string &alphabet);

    static constexpr unsigned dBufferSize = 262144; // Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants) from the input file cannot be larger than this value.
    static constexpr unsigned maskBufferSize = 91; // Buffer size for shift-or masks for the input alphabet, must be larger than the largest input character ASCII code, up to 'Z' = 90.
    static constexpr unsigned wordSize = 64; // Shift-or word size in bits.

    static constexpr uint64_t allOnes = ~(0x0ULL);

    uint64_t *dBuffer;
    uint64_t maskBuffer[maskBufferSize];

    SOPANG_WHITEBOX
};
}

#endif // SOPANG_HPP
