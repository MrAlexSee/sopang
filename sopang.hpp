#ifndef SOPANG_HPP
#define SOPANG_HPP

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#ifndef SOPANG_WHITEBOX
#define SOPANG_WHITEBOX
#endif

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

    static const std::string *const *parseTextArray(std::string text, unsigned *nSegments, unsigned **segmentSizes);
    static std::vector<std::string> parsePatterns(std::string patternsStr);
    static std::vector<std::vector<std::set<int>>> parseSources(std::string sourcesStr);

    /*
     *** MATCHING
     */

    std::unordered_set<unsigned> match(const std::string *const *segments,
        unsigned nSegments, const unsigned *segmentSizes,
        const std::string &pattern, const std::string &alphabet);

    std::unordered_set<unsigned> matchApprox(const std::string *const *segments,
        unsigned nSegments, const unsigned *segmentSizes,
        const std::string &pattern, const std::string &alphabet, 
        unsigned k);

    std::unordered_set<unsigned> matchWithSources(const std::string *const *segments,
        unsigned nSegments, const unsigned *segmentSizes,
        const std::vector<std::vector<std::set<int>>> &sources,
        const std::string &pattern, const std::string &alphabet);
private:
    void initCounterPositionMasks();

    void fillPatternMaskBuffer(const std::string &pattern, const std::string &alphabet);
    void fillPatternMaskBufferApprox(const std::string &pattern, const std::string &alphabet);

    /** Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants) 
     * from the input file cannot be larger than this value. */
    static constexpr unsigned dBufferSize = 262144;
    /** Buffer size for Shift-Or masks for the input alphabet, must be larger than the largest input character ASCII code, 
     * up to 'Z' = 90. */
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
