#include "parsing.hpp"

#include "helpers.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <cassert>
#include <stdexcept>

using namespace std;

namespace sopang::parsing
{

const string *const *parseTextArray(string text, int *nSegments, int **segmentSizes)
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

            if (not curStr.empty()) // If we enter the non-deterministic segment from a deterministic segment (string).
            {
                segments.emplace_back(vector<string>{ move(curStr) });
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
                throw runtime_error("non-deterministic segment cannot be empty: char index = " + to_string(i));
            }

            curSegment.emplace_back(move(curStr));
            curStr.clear();

            segments.emplace_back(move(curSegment));
            curSegment.clear();

            inSegment = false;
        }
    }

    if (not curStr.empty()) // If the file ended with a deterministic segment.
    {
        assert(not inSegment and curSegment.empty());
        segments.emplace_back(vector<string>{ move(curStr) });
    }

    *nSegments = segments.size();

    *segmentSizes = new int[segments.size()];
    string **res = new string *[segments.size()];

    for (size_t iSeg = 0; iSeg < segments.size(); ++iSeg)
    {
        (*segmentSizes)[iSeg] = segments[iSeg].size();
        res[iSeg] = new string[segments[iSeg].size()];

        for (size_t iVar = 0; iVar < segments[iSeg].size(); ++iVar) // We iterate segment variants.
        {
            res[iSeg][iVar] = segments[iSeg][iVar];
        }
    }

    return const_cast<const string *const *>(res);
}

vector<string> parsePatterns(string patternsStr)
{
    boost::trim(patternsStr);
    vector<string> res;

    vector<string> splitRes;
    boost::split(splitRes, patternsStr, boost::is_any_of("\n"));

    for (string &pattern : splitRes)
    {
        boost::trim(pattern);
    }

    helpers::removeEmptyStrings(splitRes);
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

    const int ret = stoi(numberStr);
    return ret;
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
// For each segment, we will store a vector with size equal to the number of variants in that segment.
// For each variant, we will store a set with source indexes, with reference sources stored in the last element (set).
vector<vector<Sopang::SourceSet>> parseSources(string text, int &sourceCount)
{
    if (text.empty())
    {
        return {};
    }

    boost::trim(text);

    bool inSegment = false;
    bool inSingleVariant = false;
    bool inMultipleVariants = false;

    vector<vector<Sopang::SourceSet>> ret;
    vector<Sopang::SourceSet> curSegment;
    Sopang::SourceSet curVariant;
    
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
vector<vector<Sopang::SourceSet>> parseSourcesCompressed(string text, int &sourceCount)
{
    if (text.empty())
    {
        return {};
    }

    boost::trim(text);
    vector<vector<Sopang::SourceSet>> ret;

    vector<Sopang::SourceSet> curSegment;
    Sopang::SourceSet curVariant;

    size_t charIdx, shift;
    sourceCount = parseSourceCount(text, charIdx);

    /** Segment start mark value in the compressed sources file. */
    constexpr char segmentStartMark = static_cast<char>(127);

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

Sopang::SourceMap sourcesToSourceMap(int nSegments, const int *segmentSizes,
    const vector<vector<Sopang::SourceSet>> &sources)
{
    unordered_map<int, vector<Sopang::SourceSet>> ret;
    size_t arrayIdx = 0;

    for (int iS = 0; iS < nSegments; ++iS)
    {
        if (segmentSizes[iS] > 1)
        {
            ret[iS] = sources[arrayIdx];
            arrayIdx += 1;
        }
    }

    return ret;
}

} // namespace sopang::parsing
