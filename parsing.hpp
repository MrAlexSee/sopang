#ifndef PARSING_HPP
#define PARSING_HPP

#include "sopang.hpp"

#include <unordered_map>
#include <string>
#include <vector>

namespace sopang::parsing
{

const std::string *const *parseTextArray(std::string text, int *nSegments, int **segmentSizes);
std::vector<std::string> parsePatterns(std::string patternsStr);

std::vector<std::vector<Sopang::SourceSet>> parseSources(std::string text, int &sourceCount);
std::vector<std::vector<Sopang::SourceSet>> parseSourcesCompressed(std::string text, int &sourceCount);

std::unordered_map<int, std::vector<Sopang::SourceSet>> sourcesToSourceMap(int nSegments,
    const int *segmentSizes, const std::vector<std::vector<Sopang::SourceSet>> &sources);

} // namespace sopang::parsing

#endif // PARSING_HPP
