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

namespace sopang
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
    void initCounterPositionMasks();

    void fillPatternMaskBuffer(const string &pattern, const string &alphabet);
    void fillPatternMaskBufferApprox(const string &pattern, const string &alphabet);

    /** Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants) from the input file cannot be larger than this value. */
    static constexpr unsigned dBufferSize = 262144;
    /** Buffer size for Shift-Or masks for the input alphabet, must be larger than the largest input character ASCII code, up to 'Z' = 90. */
    static constexpr unsigned maskBufferSize = 91;
    /** Word size (in bits) used by the Shift-Or algorithm. */
    static constexpr unsigned wordSize = 64;

    /** Shift-Add counter size in bits. */
    static constexpr unsigned saCounterSize = 5;
    /** Maximum pattern size for approximate search. */
    static constexpr unsigned maxPatternApproxSize = 12;
    
    /** Full single Shift-Add counter indicating no match. */
    static constexpr uint64_t saFullCounter = 0x10ULL;
    /** Single Shift-Add counter with all bits set. */
    static constexpr uint64_t saCounterAllSet = 0x20ULL - 0x1ULL;

    static constexpr uint64_t allOnes = ~(0x0ULL);

    uint64_t counterPosMasks[maxPatternApproxSize];

    uint64_t *dBuffer;
    uint64_t maskBuffer[maskBufferSize];

    SOPANG_WHITEBOX
};

} // namespace sopang

#endif // SOPANG_HPP
