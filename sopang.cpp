#include "sopang.hpp"

#include "helpers.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

namespace sopang
{

Sopang::Sopang()
{
    dBuffer = new uint64_t[dBufferSize];
    initCounterPositionMasks();
}

Sopang::~Sopang()
{
    delete[] dBuffer;
}

const string *const *Sopang::parseTextArray(string text, unsigned *nSegments, unsigned **segmentSizes)
{
    boost::trim(text);

    vector<vector<string>> segments;
    bool inSegment = false;

    vector<string> curSegment;
    string curStr = "";

    for (size_t i = 0; text[i] != '\0'; ++i)
    {
        if (text[i] != '{' and text[i] != '}') // Inside a string or a segment: comma or string character.
        {
            if (not inSegment)
            {
                if (text[i] == ',')
                {
                    throw runtime_error("bad input text formatting: comma outside a segment: char index = " + to_string(i));
                }

                curStr += text[i];
            }
            else
            {
                if (text[i] == ',')
                {
                    curSegment.emplace_back(move(curStr));
                    curStr.clear();
                }
                else
                {
                    curStr += text[i];
                }
            }
        }
        else if (text[i] == '{') // Segment start.
        {
            assert(not inSegment and curSegment.size() == 0);

            if (not curStr.empty()) // If we enter the segment from the determinate string.
            {
                segments.emplace_back(vector<string> { move(curStr) });
                curStr.clear();
            }

            inSegment = true;
        }
        else // Segment end.
        {
            assert(text[i] == '}');
            assert(inSegment == true and curSegment.size() >= 1);

            if (curSegment.empty())
            {
                throw runtime_error("degenerate segment cannot be empty: char index = " + to_string(i));
            }

            curSegment.emplace_back(move(curStr));
            curStr.clear();

            segments.emplace_back(move(curSegment));
            curSegment.clear();

            inSegment = false;
        }
    }

    if (not curStr.empty()) // If the file ended with a determinate segment.
    {
        assert(not inSegment and curSegment.empty());

        curSegment.push_back(curStr);
        segments.push_back(vector<string>(curSegment));
    }

    *nSegments = segments.size();

    *segmentSizes = new unsigned[segments.size()];
    string **res = new string *[segments.size()];

    for (size_t iSeg = 0; iSeg < segments.size(); ++iSeg)
    {
        assert(segments[iSeg].size() > 0 and segments[iSeg].size() <= dBufferSize);

        (*segmentSizes)[iSeg] = segments[iSeg].size();
        res[iSeg] = new string[segments[iSeg].size()];

        for (size_t iVar = 0; iVar < segments[iSeg].size(); ++iVar) // We iterate segment variants.
        {
            res[iSeg][iVar] = segments[iSeg][iVar];
        }
    }

    return const_cast<const string *const *>(res);
}

vector<string> Sopang::parsePatterns(string patternsStr)
{
    boost::trim(patternsStr);
    vector<string> res;

    vector<string> splitRes;
    boost::split(splitRes, patternsStr, boost::is_any_of("\n"));

    for (string &pattern : splitRes)
    {
        boost::trim(pattern);
    }

    Helpers::removeEmptyStrings(splitRes);
    return splitRes;
}

namespace // Contains helpers for parsing sources.
{

int parseSourceCount(const string &text, size_t &startIdx)
{
    string numberStr = "";

    for (startIdx = 0; text[startIdx] != '\n'; ++startIdx)
    {
        if (not isdigit(text[startIdx]))
        {
            throw runtime_error("bad source count formatting, index = " + to_string(startIdx));
        }

        numberStr += text[startIdx];
    }

    startIdx += 1;
    return stoi(numberStr);
}

void handleSourceNumberEnd(string &curNumber, Sopang::SourceSet &curVariant, size_t charIdx)
{
    if (curNumber.empty())
    {
        throw runtime_error("bad empty number, index = " + to_string(charIdx));
    }

    const int sourceIdx = stoi(curNumber);

    if (curVariant.test(sourceIdx))
    {
        throw runtime_error((boost::format("duplicate source index = %1%, text index = %2%")
            % sourceIdx % charIdx).str());
    }

    curVariant.set(sourceIdx);
    curNumber.clear();
}

void handleSourceVariantEnd(Sopang::SourceSet &curVariant, vector<Sopang::SourceSet> &curSegment)
{
    curSegment.emplace_back(move(curVariant));
    curVariant.reset();
}

void addReferenceSources(vector<Sopang::SourceSet> &segment, int sourceCount)
{
    Sopang::SourceSet referenceVariant;

    for (int sourceIdx = 0; sourceIdx < sourceCount; ++sourceIdx)
    {
        bool exists = false;

        for (const Sopang::SourceSet &variant : segment)
        {
            if (variant.test(sourceIdx))
            {
                exists = true;
                break;
            }
        }

        if (not exists)
        {
            referenceVariant.set(sourceIdx);
        }
    }

    segment.emplace_back(move(referenceVariant));
}

void handleSourceSegmentEnd(vector<Sopang::SourceSet> &curSegment,
    Sopang::SourceSet &curVariant, string &curNumber,
    vector<vector<Sopang::SourceSet>> &sources,
    int sourceCount, size_t charIdx)
{
    handleSourceNumberEnd(curNumber, curVariant, charIdx);
    handleSourceVariantEnd(curVariant, curSegment);

    addReferenceSources(curSegment, sourceCount);

    sources.emplace_back(move(curSegment));
    curSegment.clear();
}

} // namespace (anonymous)

// We will return a vector with size equal to the number of non-deterministic segments.
// For each segment, we will store a vector with size equal to the number of variants.
// For each variant, we will store a set with source indexes, with reference sources stored in the last set.
vector<vector<Sopang::SourceSet>> Sopang::parseSources(string text, int &sourceCount)
{
    if (text.empty())
    {
        return {};
    }

    boost::trim(text);

    bool inSegment = false;
    bool inSingleVariant = false;
    bool inMultipleVariants = false;

    vector<vector<SourceSet>> ret;
    vector<SourceSet> curSegment;
    SourceSet curVariant;
    
    string curNumber;

    size_t startIdx;
    sourceCount = parseSourceCount(text, startIdx);

    for (size_t charIdx = startIdx; charIdx < text.size(); ++charIdx)
    {
        const char curChar = text[charIdx];

        if (inSegment)
        {
            if (inSingleVariant)
            {
                if (isdigit(curChar))
                {
                    curNumber += curChar;
                }
                else if (curChar == ',')
                {
                    handleSourceNumberEnd(curNumber, curVariant, charIdx);
                }
                else if (curChar == '}')
                {
                    handleSourceSegmentEnd(curSegment, curVariant, curNumber, ret, sourceCount, charIdx);
                    
                    inSingleVariant = false;
                    inSegment = false;
                }
                else
                {
                    throw runtime_error((boost::format("bad character (in a single source variant) = %1%, index = %2%")
                        % curChar % charIdx).str());
                }
            }
            else if (inMultipleVariants)
            {
                if (isdigit(curChar))
                {
                    curNumber += curChar;
                }
                else if (curChar == ',')
                {
                    handleSourceNumberEnd(curNumber, curVariant, charIdx);
                }
                else if (curChar == '}' and text[charIdx + 1] == '}') // Segment end.
                {
                    handleSourceSegmentEnd(curSegment, curVariant, curNumber, ret, sourceCount, charIdx);
                    
                    inMultipleVariants = false;
                    inSegment = false;

                    charIdx += 1;
                }
                else if (curChar == '}') // Variant end.
                {    
                    handleSourceNumberEnd(curNumber, curVariant, charIdx);
                    handleSourceVariantEnd(curVariant, curSegment);
                }
                else if (curChar != '{')
                {
                    throw runtime_error((boost::format("bad character (in multiple source variants) = %1%, index = %2%")
                        % curChar % charIdx).str());
                }
            }
            else // Not in any variant.
            {
                if (curChar == '{')
                {
                    inMultipleVariants = true;
                }
                else if (isdigit(curChar))
                {
                    curNumber += curChar;
                    inSingleVariant = true;
                }
                else
                {
                    throw runtime_error((boost::format("bad character (not in a source variant) = %1%, index = %2%")
                        % curChar % charIdx).str());
                }
            }
        }
        else // Not in a segment.
        {
            if (curChar == '{')
            {
                inSegment = true;
            }
            else
            {
                throw runtime_error((boost::format("bad character (not in a source segment) = %1%, index = %2%") 
                    % curChar % charIdx).str());
            }
        }
    }

    if (inSegment or inSingleVariant or inMultipleVariants)
    {
        throw runtime_error("the last segment is not closed with \"}\"");
    }

    return ret;
}

namespace // Contains helpers for parsing compressed sources.
{

int unpackNumber(const unsigned char first, const unsigned char second, size_t &shift)
{
    const int firstVal = static_cast<int>(first);

    if (firstVal >= 128)
    {
        shift = 1;
        return firstVal - 128;
    }

    const int secondVal = static_cast<int>(second);

    shift = 2;
    return firstVal * 128 + secondVal - 128;
}

} // namespace (anonymous)

// The same output as for Sopang::parseSources.
vector<vector<Sopang::SourceSet>> Sopang::parseSourcesCompressed(string text, int &sourceCount)
{
    if (text.empty())
    {
        return {};
    }

    boost::trim(text);
    vector<vector<SourceSet>> ret;

    vector<SourceSet> curSegment;
    SourceSet curVariant;

    size_t charIdx, shift;
    sourceCount = parseSourceCount(text, charIdx);

    while (charIdx < text.size())
    {
        const char curChar = text[charIdx];

        if (curChar == segmentStartMark)
        {
            if (not curSegment.empty())
            {
                addReferenceSources(curSegment, sourceCount);
                ret.emplace_back(move(curSegment));

                curSegment.clear();
            }

            charIdx += 1;
            continue;
        }

        assert(charIdx < text.size());

        // Note that even if (charIdx + 1) spills, it should be contained by the terminating '\0'.
        const int variantSize = unpackNumber(text[charIdx], text[charIdx + 1], shift);
        charIdx += shift;

        int prevVal = 0;

        for (int variantIdx = 0; variantIdx < variantSize; ++variantIdx)
        {
            int sourceVal = unpackNumber(text[charIdx], text[charIdx + 1], shift);
            sourceVal += prevVal;

            assert(not curVariant.test(sourceVal));
            curVariant.set(sourceVal);

            prevVal = sourceVal;
            charIdx += shift;
        }

        curSegment.emplace_back(move(curVariant));
        curVariant.reset();
    }

    assert(not curSegment.empty());

    addReferenceSources(curSegment, sourceCount);
    ret.emplace_back(move(curSegment));

    return ret;
}

unordered_map<unsigned, vector<Sopang::SourceSet>> Sopang::sourcesToSourceMap(unsigned nSegments,
    const unsigned *segmentSizes, const vector<vector<Sopang::SourceSet>> &sources)
{
    unordered_map<unsigned, vector<SourceSet>> ret;
    size_t arrayIdx = 0;

    for (unsigned iS = 0; iS < nSegments; ++iS)
    {
        if (segmentSizes[iS] > 1)
        {
            ret[iS] = sources[arrayIdx];
            arrayIdx += 1;
        }
    }

    return ret;
}

unordered_set<unsigned> Sopang::match(const string *const *segments,
    unsigned nSegments, const unsigned *segmentSizes,
    const string &pattern, const string &alphabet)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= wordSize);

    fillPatternMaskBuffer(pattern, alphabet);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

    unordered_set<unsigned> res;

    for (unsigned iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and segmentSizes[iS] <= dBufferSize);

        for (unsigned iD = 0; iD < segmentSizes[iS]; ++iD)
        {
            dBuffer[iD] = D;

            for (size_t iC = 0; iC < segments[iS][iD].size(); ++iC)
            {
                const char c = segments[iS][iD][iC];

                assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
                assert(alphabet.find(c) != string::npos);

                dBuffer[iD] <<= 1;
                dBuffer[iD] |= maskBuffer[static_cast<unsigned char>(c)];

                // Match occurred. Note: we still continue in order to fill the whole d-buffer.
                if ((dBuffer[iD] & hitMask) == 0x0ULL)
                {
                    res.insert(iS);
                }
            }
        }

        D = dBuffer[0];

        for (unsigned iD = 1; iD < segmentSizes[iS]; ++iD)
        {
            // As a join operation we want to preserve 0s (active states):
            // a match can occur in any segment alternative.
            D &= dBuffer[iD];
        }
    }

    return res;
}

unordered_set<unsigned> Sopang::matchApprox(const string *const *segments,
    unsigned nSegments, const unsigned *segmentSizes,
    const string &pattern, const string &alphabet,
    unsigned k)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= maxPatternApproxSize);
    assert(k > 0);

    unordered_set<unsigned> res;

    fillPatternMaskBufferApprox(pattern, alphabet);

    // This is the initial position of each counter, after k + 1 errors the most significant bit will be set.
    const uint64_t counterMask = 0xFULL - k;
    // Hit mask indicates whether the most significant bit in the last counter is set.
    const uint64_t hitMask = (0x1ULL << ((pattern.size() * saCounterSize) - 1));
    
    uint64_t D = 0x0ULL;

    for (size_t i = 0; i < pattern.size(); ++i)
    {
        // We initialize the state with full counters which allow us to effectively start matching
        // after m characters.
        D |= (saFullCounter << (i * saCounterSize));
    }

    for (unsigned iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and segmentSizes[iS] <= dBufferSize);

        for (unsigned iD = 0; iD < segmentSizes[iS]; ++iD)
        {
            dBuffer[iD] = D;

            for (size_t iC = 0; iC < segments[iS][iD].size(); ++iC)
            {
                const char c = segments[iS][iD][iC];

                assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
                assert(alphabet.find(c) != string::npos);

                dBuffer[iD] <<= saCounterSize;
                dBuffer[iD] += counterMask;

                dBuffer[iD] += maskBuffer[static_cast<unsigned char>(c)];

                if ((dBuffer[iD] & hitMask) == 0x0ULL)
                {
                    res.insert(iS);
                }
            }
        }

        D = 0x0ULL;
        
        // As a join operation, we take the minimum (the most promising alternative) from each counter.
        for (size_t i = 0; i < pattern.size(); ++i)
        {
            uint64_t min = (dBuffer[0] & counterPosMasks[i]);

            for (unsigned iD = 1; iD < segmentSizes[iS]; ++iD)
            {
                uint64_t cur = (dBuffer[iD] & counterPosMasks[i]);
                
                if (cur < min)
                {
                    min = cur;
                }
            }

            D |= min;
        }
    }

    return res;
}

namespace
{
    bool verifyMatch(const string *const *segments,
        const unsigned *segmentSizes,
        const unordered_map<unsigned, vector<Sopang::SourceSet>> &sourceMap,
        const string &pattern,
        unsigned matchIdx,
        const pair<unsigned, size_t> &match)
    {
        using SourceSet = Sopang::SourceSet;
        int patternCharIdx = static_cast<int>(pattern.size()) - match.second - 2;

        if (patternCharIdx < 0) // The match is fully contained within a single segment.
            return true;

        vector<pair<SourceSet, int>> leaves;
        SourceSet rootSources;
        
        if (sourceMap.count(matchIdx) > 0)
        {
            rootSources = sourceMap.at(matchIdx)[match.first];
        }
        else
        {
            rootSources.set();
        }

        leaves.emplace_back(make_pair(move(rootSources), patternCharIdx));
        int segmentIdx = static_cast<int>(matchIdx - 1);

        while (not leaves.empty() and segmentIdx >= 0)
        {
            if (segmentSizes[segmentIdx] == 1)
            {
                for (auto &leaf : leaves)
                {
                    leaf.second -= segments[segmentIdx][0].size();

                    if (leaf.second < 0)
                        return true;
                }
            }
            else
            {
                assert(sourceMap.count(segmentIdx) > 0 and sourceMap.at(segmentIdx).size() == segmentSizes[segmentIdx]);

                vector<pair<SourceSet, int>> newLeaves;
                newLeaves.reserve(leaves.size() * segmentSizes[segmentIdx]);
                
                for (const auto &leaf : leaves)
                {
                    for (unsigned variantIdx = 0; variantIdx < segmentSizes[segmentIdx]; ++variantIdx)
                    {
                        const SourceSet &variantSources = sourceMap.at(segmentIdx)[variantIdx];

                        if (segments[segmentIdx][variantIdx].empty())
                        {
                            const SourceSet newSources = (variantSources & leaf.first);

                            if (newSources.any())
                            {
                                newLeaves.emplace_back(make_pair(move(newSources), leaf.second));
                            }
                        }
                        else
                        {
                            int curCharIdx = static_cast<int>(segments[segmentIdx][variantIdx].size() - 1);
                            int curPatternIdx = static_cast<int>(leaf.second);

                            while (curCharIdx >= 0)
                            {
                                if (curPatternIdx < 0)
                                {
                                    const SourceSet newSources = (variantSources & leaf.first);

                                    if (newSources.any())
                                        return true;
                                }

                                if (pattern[curPatternIdx] != segments[segmentIdx][variantIdx][curCharIdx])
                                    break;

                                curCharIdx -= 1;
                                curPatternIdx -= 1;
                            }

                            if (curPatternIdx < 0)
                            {
                                const SourceSet newSources = (variantSources & leaf.first);

                                if (newSources.any())
                                    return true;
                            }

                            if (curCharIdx < 0)
                            {
                                const SourceSet newSources = (variantSources & leaf.first);

                                if (newSources.any())
                                {
                                    newLeaves.emplace_back(make_pair(move(newSources), curPatternIdx));
                                }
                            }
                        }
                    }
                }

                leaves = newLeaves;
            }

            segmentIdx -= 1;
        }

        return false;
    }
} // namespace (anonymous)

unordered_set<unsigned> Sopang::matchWithSources(const string *const *segments,
    unsigned nSegments, const unsigned *segmentSizes,
    const unordered_map<unsigned, vector<Sopang::SourceSet>> &sourceMap,
    const string &pattern, const string &alphabet)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= wordSize);
    fillPatternMaskBuffer(pattern, alphabet);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

    // segment index -> [(variant index, char in variant index)]
    unordered_map<unsigned, vector<pair<unsigned, size_t>>> indexToMatch;
    indexToMatch.reserve(matchMapReserveSize);

    for (unsigned iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and segmentSizes[iS] <= dBufferSize);

        for (unsigned iD = 0; iD < segmentSizes[iS]; ++iD)
        {
            dBuffer[iD] = D;

            for (size_t iC = 0; iC < segments[iS][iD].size(); ++iC)
            {
                const char c = segments[iS][iD][iC];

                assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
                assert(alphabet.find(c) != string::npos);

                dBuffer[iD] <<= 1;
                dBuffer[iD] |= maskBuffer[static_cast<unsigned char>(c)];

                if ((dBuffer[iD] & hitMask) == 0x0ULL)
                {
                    if (indexToMatch.count(iS) == 0)
                    {
                        indexToMatch[iS] = {};
                    }

                    indexToMatch[iS].emplace_back(make_pair(iD, iC));
                }
            }
        }

        D = dBuffer[0];

        for (unsigned iD = 1; iD < segmentSizes[iS]; ++iD)
        {
            D &= dBuffer[iD];
        }
    }

    unordered_set<unsigned> res;
    for (const auto &kv : indexToMatch)
    {
        for (const auto &match : kv.second)
        {
            if (verifyMatch(segments, segmentSizes, sourceMap, pattern, kv.first, match))
            {
                res.insert(kv.first);
            }
        }
    }

    return res;
}

void Sopang::initCounterPositionMasks()
{
    for (unsigned i = 0; i < maxPatternApproxSize; ++i)
    {
        counterPosMasks[i] = (saCounterAllSet << (i * saCounterSize));
    }
}

void Sopang::fillPatternMaskBuffer(const string &pattern, const string &alphabet)
{
    assert(pattern.size() > 0 and pattern.size() <= wordSize);
    assert(alphabet.size() > 0);

    for (const char c : alphabet)
    {
        assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
        maskBuffer[static_cast<unsigned char>(c)] = allOnes;
    }

    for (size_t iC = 0; iC < pattern.size(); ++iC)
    {
        assert(pattern[iC] > 0 and static_cast<unsigned char>(pattern[iC]) < maskBufferSize);
        maskBuffer[static_cast<unsigned char>(pattern[iC])] &= (~(0x1ULL << iC));
    }
}

void Sopang::fillPatternMaskBufferApprox(const string &pattern, const string &alphabet)
{
    assert(pattern.size() > 0 and pattern.size() <= wordSize);
    assert(alphabet.size() > 0);

    for (const char c : alphabet)
    {
        assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
        maskBuffer[static_cast<unsigned char>(c)] = 0x0ULL;

        for (size_t iC = 0; iC < pattern.size(); ++iC)
        {
            maskBuffer[static_cast<unsigned char>(c)] |= (0x1ULL << (iC * saCounterSize));
        }    
    }

    for (size_t iC = 0; iC < pattern.size(); ++iC)
    {
        assert(pattern[iC] > 0 and static_cast<unsigned char>(pattern[iC]) < maskBufferSize);
        // We zero the bit at the counter position corresponding to the current character in the pattern.
        maskBuffer[static_cast<unsigned char>(pattern[iC])] &= (~(0x1ULL << (iC * saCounterSize)));
    }
}

} // namespace sopang
