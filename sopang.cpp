#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "helpers.hpp"
#include "sopang.hpp"

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
    bool inSeg = false;

    vector<string> curSegment;
    string curStr = "";

    for (size_t i = 0; text[i] != '\0'; ++i)
    {
        if (text[i] != '{' and text[i] != '}') // Inside a string or segment: comma or string character.
        {
            if (not inSeg)
            {
                if (text[i] == ',')
                {
                    throw runtime_error("bad input text formatting: comma outside segment: char index = " + to_string(i));
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
            assert(not inSeg and curSegment.size() == 0);

            if (not curStr.empty()) // If we enter the segment from the determinate string.
            {
                segments.emplace_back(vector<string> { move(curStr) });
                curStr.clear();
            }

            inSeg = true;
        }
        else // Segment end.
        {
            assert(text[i] == '}');
            assert(inSeg == true and curSegment.size() >= 1);

            if (curSegment.empty())
            {
                throw runtime_error("degenerate segment cannot be empty: char index = " + to_string(i));
            }

            curSegment.emplace_back(move(curStr));
            curStr.clear();

            segments.emplace_back(move(curSegment));
            curSegment.clear();

            inSeg = false;
        }
    }

    if (not curStr.empty()) // If the file ended with a determinate segment.
    {
        assert(not inSeg and curSegment.empty());

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

vector<vector<vector<int>>> Sopang::parseSources(string text)
{
    boost::trim(text);
    vector<vector<vector<int>>> sources;

    bool inSourcesForSegment = false;
    bool inSourcesForVariant = false;

    vector<vector<int>> curSegment;
    vector<int> curVariant;

    string curNumber;

    for (size_t i = 0; text[i] != '\0'; ++i)
    {
        if (inSourcesForSegment)
        {
            if (inSourcesForVariant)
            {
                if (text[i] == '}')
                {
                    inSourcesForVariant = false;

                    if (curNumber.empty())
                    {
                        throw runtime_error("bad empty variant, index = " + to_string(i));
                    }

                    curVariant.push_back(stoi(curNumber));
                    curNumber.clear();

                    curSegment.emplace_back(move(curVariant));
                    curVariant.clear();

                    continue;
                }

                if (text[i] == ',')
                {
                    if (curNumber.empty())
                    {
                        throw runtime_error("bad empty variant, index = " + to_string(i));
                    }

                    curVariant.push_back(stoi(curNumber));
                    curNumber.clear();
                }
                else if (isdigit(text[i]))
                {
                    curNumber.push_back(text[i]);
                }
                else
                {
                    throw runtime_error((boost::format("bad character (not digit or comma in variant) = %1%, index = %2%") 
                        % text[i] % i).str());
                }
            }
            else // not in variant
            {
                if (text[i] == '{')
                {
                    inSourcesForVariant = true;
                }
                else if (text[i] == '}')
                {
                    inSourcesForSegment = false;

                    sources.emplace_back(move(curSegment));
                    curSegment.clear();
                }
                else
                {
                    throw runtime_error((boost::format("bad character (not in variant) = %1%, index = %2%") 
                        % text[i] % i).str());
                }
            }
        }
        else // not in segment
        {
            if (text[i] == '{')
            {
                inSourcesForSegment = true;
            }
            else
            {
                throw runtime_error((boost::format("bad character (not in segment) = %1%, index = %2%") 
                    % text[i] % i).str());
            }
        }
    }

    if (inSourcesForSegment or inSourcesForVariant)
    {
        throw runtime_error("last segment is not closed with \"}\"");
    }

    return sources;
}

unordered_set<unsigned> Sopang::match(const string *const *segments,
                                      unsigned nSegments, const unsigned *segmentSizes,
                                      const string &pattern, const string &alphabet)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= wordSize);
    unordered_set<unsigned> res;

    fillPatternMaskBuffer(pattern, alphabet);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

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

                // Match occurred.
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
