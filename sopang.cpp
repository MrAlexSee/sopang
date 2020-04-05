#include "sopang.hpp"

#include <algorithm>
#include <cassert>

using namespace std;

namespace sopang
{

Sopang::Sopang(const std::string &alphabet, int sourceCount)
    :alphabet(alphabet),
     sourceCount(sourceCount)
{
    dBuffer = new uint64_t[dBufferSize];
    initCounterPositionMasks();
}

Sopang::~Sopang()
{
    delete[] dBuffer;
}

unordered_set<int> Sopang::match(const string *const *segments,
    int nSegments,
    const int *segmentSizes,
    const string &pattern)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= wordSize);

    fillPatternMaskBuffer(pattern);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

    unordered_set<int> res;

    for (int iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and static_cast<size_t>(segmentSizes[iS]) <= dBufferSize);

        for (int iD = 0; iD < segmentSizes[iS]; ++iD)
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

        for (int iD = 1; iD < segmentSizes[iS]; ++iD)
        {
            // As a join operation we want to preserve 0s (active states):
            // a match can occur in any segment alternative.
            D &= dBuffer[iD];
        }
    }

    return res;
}

unordered_set<int> Sopang::matchApprox(const string *const *segments,
    int nSegments,
    const int *segmentSizes,
    const string &pattern,
    int k)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= maxPatternApproxSize);
    assert(k > 0);

    unordered_set<int> res;

    fillPatternMaskBufferApprox(pattern);

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

    for (int iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and static_cast<size_t>(segmentSizes[iS]) <= dBufferSize);

        for (int iD = 0; iD < segmentSizes[iS]; ++iD)
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

            for (int iD = 1; iD < segmentSizes[iS]; ++iD)
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
    const int *segmentSizes,
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const string &pattern,
    int matchIdx,
    const pair<int, int> &match)
{
    using SourceSet = Sopang::SourceSet;
    const int patternCharIdx = static_cast<int>(pattern.size()) - match.second - 2;

    if (patternCharIdx < 0) // The match is fully contained within a single segment.
        return true;

    vector<pair<SourceSet, int>> leaves;
    SourceSet rootSources(sourceCount);
    
    if (sourceMap.count(matchIdx) > 0)
    {
        rootSources = sourceMap.at(matchIdx)[match.first];
    }
    else
    {
        rootSources.set();
    }

    leaves.emplace_back(move(rootSources), patternCharIdx);
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
            assert(sourceMap.count(segmentIdx) > 0 and sourceMap.at(segmentIdx).size() == static_cast<size_t>(segmentSizes[segmentIdx]));

            vector<pair<SourceSet, int>> newLeaves;
            newLeaves.reserve(leaves.size() * segmentSizes[segmentIdx]);
            
            for (const auto &leaf : leaves)
            {
                for (int variantIdx = 0; variantIdx < segmentSizes[segmentIdx]; ++variantIdx)
                {
                    const SourceSet &variantSources = sourceMap.at(segmentIdx)[variantIdx];

                    if (segments[segmentIdx][variantIdx].empty())
                    {
                        const SourceSet newSources = (variantSources & leaf.first);

                        if (newSources.any())
                        {
                            newLeaves.emplace_back(move(newSources), leaf.second);
                        }
                    }
                    else
                    {
                        int curCharIdx = static_cast<int>(segments[segmentIdx][variantIdx].size()) - 1;
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
                                newLeaves.emplace_back(move(newSources), curPatternIdx);
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

Sopang::SourceSet calcMatchSources(const string *const *segments,
    const int *segmentSizes,
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const string &pattern,
    int matchIdx,
    const pair<int, int> &match,
    bool &deterministicSegmentMatch)
{
    using SourceSet = Sopang::SourceSet;
    const int patternCharIdx = static_cast<int>(pattern.size()) - match.second - 2;

    if (patternCharIdx < 0) // The match is fully contained within a single segment.
    {
        if (sourceMap.count(matchIdx) > 0)
        {
            assert(match.first >= 0 and match.first < static_cast<int>(sourceMap.at(matchIdx).size()));
            return sourceMap.at(matchIdx)[match.first];
        }

        deterministicSegmentMatch = true;
        return { };
    }

    vector<pair<SourceSet, int>> leaves;
    SourceSet rootSources(sourceCount);
    
    if (sourceMap.count(matchIdx) > 0)
    {
        assert(match.first >= 0 and match.first < static_cast<int>(sourceMap.at(matchIdx).size()));
        rootSources = sourceMap.at(matchIdx)[match.first];
    }
    else
    {
        rootSources.set();
    }

    leaves.emplace_back(move(rootSources), patternCharIdx);
    int segmentIdx = static_cast<int>(matchIdx - 1);

    SourceSet res(sourceCount);

    while (not leaves.empty() and segmentIdx >= 0)
    {
        vector<pair<SourceSet, int>> newLeaves;
        newLeaves.reserve(leaves.size() * segmentSizes[segmentIdx]);

        if (segmentSizes[segmentIdx] == 1)
        {
            for (auto &leaf : leaves)
            {
                leaf.second -= segments[segmentIdx][0].size();

                if (leaf.second < 0)
                {
                    res |= leaf.first;
                }
                else
                {
                    newLeaves.push_back(leaf);
                }
            }

            leaves = newLeaves;
        }
        else
        {
            assert(sourceMap.count(segmentIdx) > 0 and sourceMap.at(segmentIdx).size() == static_cast<size_t>(segmentSizes[segmentIdx]));

            for (const auto &leaf : leaves)
            {
                for (int variantIdx = 0; variantIdx < segmentSizes[segmentIdx]; ++variantIdx)
                {
                    const SourceSet &variantSources = sourceMap.at(segmentIdx)[variantIdx];

                    if (segments[segmentIdx][variantIdx].empty())
                    {
                        const SourceSet newSources = (variantSources & leaf.first);

                        if (newSources.any())
                        {
                            newLeaves.emplace_back(move(newSources), leaf.second);
                        }
                    }
                    else
                    {
                        int curCharIdx = static_cast<int>(segments[segmentIdx][variantIdx].size()) - 1;
                        int curPatternIdx = static_cast<int>(leaf.second);

                        while (curCharIdx >= 0)
                        {
                            if (curPatternIdx < 0)
                                break;

                            if (pattern[curPatternIdx] != segments[segmentIdx][variantIdx][curCharIdx])
                                break;

                            curCharIdx -= 1;
                            curPatternIdx -= 1;
                        }

                        if (curPatternIdx < 0)
                        {
                            const SourceSet newSources = (variantSources & leaf.first);

                            if (newSources.any())
                            {
                                res |= newSources;
                                continue;
                            }
                        }

                        if (curCharIdx < 0)
                        {
                            const SourceSet newSources = (variantSources & leaf.first);

                            if (newSources.any())
                            {
                                newLeaves.emplace_back(move(newSources), curPatternIdx);
                            }
                        }
                    }
                }
            }

            leaves = newLeaves;
        }

        segmentIdx -= 1;
    }

    return res;
}

} // namespace (anonymous)

unordered_set<int> Sopang::matchWithSourcesVerify(const string *const *segments,
    int nSegments,
    const int *segmentSizes,
    const Sopang::SourceMap &sourceMap,
    const string &pattern)
{
    const IndexToMatchMap indexToMatch = calcIndexToMatchMap(segments, nSegments, segmentSizes, pattern);
    unordered_set<int> res;

    for (const auto &kv : indexToMatch)
    {
        for (const auto &match : kv.second)
        {
            if (verifyMatch(segments, segmentSizes, sourceMap, sourceCount, pattern, kv.first, match))
            {
                res.insert(kv.first);
                break;
            }
        }
    }

    return res;
}

unordered_map<int, Sopang::SourceSet> Sopang::matchWithSources(const string *const *segments,
    int nSegments,
    const int *segmentSizes,
    const Sopang::SourceMap &sourceMap,
    const string &pattern)
{
    const IndexToMatchMap indexToMatch = calcIndexToMatchMap(segments, nSegments, segmentSizes, pattern);

    unordered_map<int, SourceSet> res;

    for (const auto &kv : indexToMatch)
    {
        for (const auto &match : kv.second)
        {
            bool deterministicSegmentMatch = false;
            const SourceSet curSources = calcMatchSources(segments, segmentSizes, sourceMap, sourceCount, pattern, kv.first, match, deterministicSegmentMatch);

            if (not curSources.empty())
            {
                if (res.count(kv.first) > 0)
                {
                    res.find(kv.first)->second |= curSources;
                }
                else
                {
                    res.emplace(kv.first, move(curSources));
                }
            }
            else if (deterministicSegmentMatch)
            {
                res.emplace(kv.first, sourceCount);
            }
        }
    }

    return res;
}

Sopang::IndexToMatchMap Sopang::calcIndexToMatchMap(const string *const *segments,
    int nSegments,
    const int *segmentSizes,
    const string &pattern)
{
    assert(nSegments > 0 and pattern.size() > 0 and pattern.size() <= wordSize);
    fillPatternMaskBuffer(pattern);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

    // segment index -> [(variant index, char in variant index)]
    IndexToMatchMap res;
    res.reserve(matchMapReserveSize);

    for (int iS = 0; iS < nSegments; ++iS)
    {
        assert(segmentSizes[iS] > 0 and static_cast<size_t>(segmentSizes[iS]) <= dBufferSize);

        for (int iD = 0; iD < segmentSizes[iS]; ++iD)
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
                    res[iS].emplace_back(iD, static_cast<int>(iC));
                }
            }
        }

        D = dBuffer[0];

        for (int iD = 1; iD < segmentSizes[iS]; ++iD)
        {
            D &= dBuffer[iD];
        }
    }

    return res;
}

void Sopang::initCounterPositionMasks()
{
    for (size_t i = 0; i < maxPatternApproxSize; ++i)
    {
        counterPosMasks[i] = (saCounterAllSet << (i * saCounterSize));
    }
}

void Sopang::fillPatternMaskBuffer(const string &pattern)
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

void Sopang::fillPatternMaskBufferApprox(const string &pattern)
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
