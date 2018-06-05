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

    unordered_set<unsigned> matchApprox(const string *const *segments,
                                        unsigned nSegments, const unsigned *segmentSizes,
                                        const string &pattern, const string &alphabet,
                                        unsigned k);
private:
    void fillPatternMaskBuffer(const string &pattern, const string &alphabet);
    void fillPatternMaskBufferApprox(const string &pattern, const string &alphabet);

    static constexpr unsigned dBufferSize = 262144; // Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants) from the input file cannot be larger than this value.
    static constexpr unsigned maskBufferSize = 91; // Buffer size for shift-or masks for the input alphabet, must be larger than the largest input character ASCII code, up to 'Z' = 90.
    static constexpr unsigned wordSize = 64; // Shift-or word size in bits.

    static constexpr unsigned saCounterSize = 0x5; // Shift-add counter size in bits.
    static constexpr unsigned saBitShiftRight = wordSize - saCounterSize;

    static constexpr uint64_t allOnes = ~(0x0ULL);
    static constexpr uint64_t saFullCounter = 0x10ULL; // Full shift-add counter (indicating no match).

    uint64_t *dBuffer;
    uint64_t maskBuffer[maskBufferSize];

    SOPANG_WHITEBOX
};
}

#endif // SOPANG_HPP
