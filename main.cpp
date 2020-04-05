/*
 *** SOPanG, a simple tool for pattern matching over an elastic-degenerate string, a recently proposed simplified model for the pan-genome.
 *** Authors for the current release version: Aleksander Cislak, Szymon Grabowski.
 *** Authors of the SOPanG algorithm: Aleksander Cislak, Szymon Grabowski, Jan Holub.
 *** License: GNU LGPL v3.
 *** Type "make" for optimized compile.
 */

#include "helpers.hpp"
#include "params.hpp"
#include "parsing.hpp"
#include "sopang.hpp"
#include "zstd_helper.hpp"

#include <cassert>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

using namespace sopang;
using namespace std;

namespace po = boost::program_options;

namespace sopang
{

namespace
{

Params params;

/** Indicates that program execution should continue after checking parameters. */
constexpr int paramsResContinue = -1;

}

struct SegmentData
{
    const string *const *segments;
    int nSegments; // Number of segments.
    const int *segmentSizes; // Size of each segment (number of variants).
};

/** Handles cmd-line parameters, returns paramsResContinue if program execution should continue. */
int handleParams(int argc, const char **argv);
/** Returns true if input files are readable, false otherwise. */
bool checkInputFiles();
/** Runs the main program and returns the program exit code. */
int run();

string readInputText();
vector<string> readPatterns();
vector<vector<Sopang::SourceSet>> readSources(int nSegments, const int *segmentSizes, int &sourceCount);

/** Runs sopang for [segmentData] and [sourceMap] (which may be empty) having [sourceCount] sources, searching for [patterns]. */
void runSopang(const SegmentData &segmentData, 
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const vector<string> &patterns);

/** Calculates total [textSize] in bytes and corresponding [textSizeMB] in megabytes (10^6) for [segmentData]. */
void calcTextSize(const SegmentData &segmentData, int &textSize, double &textSizeMB);

/** Searches for [pattern] in [segmentData] and [sourceMap] (which may be empty) having [sourceCount] sources and returns elapsed time in seconds. */
double measure(const SegmentData &segmentData,
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const string &pattern);

void dumpMedians(const vector<double> &elapsedSecVec, double textSizeMB);

void dumpSources(int index, const Sopang::SourceSet &sources);
void dumpIndexes(const unordered_set<int> &indexes);

void clearMemory(SegmentData &segmentData);

} // namespace sopang

int main(int argc, const char **argv)
{
    const int paramsRes = handleParams(argc, argv);
    if (paramsRes != paramsResContinue)
    {
        return paramsRes;
    }

    if (not checkInputFiles())
    {
        cerr << "Run " << argv[0] << " -h for more information" << endl << endl;
        return params.errorExitCode;
    }

    return run();
}

namespace sopang
{

int handleParams(int argc, const char **argv)
{
    po::options_description options("Parameters");
    options.add_options()
       ("dump,d", "dump input file info and throughput to output file (useful for throughput testing)")
       ("dump-indexes,D", "dump resulting indexes (full results) to stdout")
       ("full-sources-output", "when matching with sources, return all matching source (strain) indexes rather than only verify if the match is correct")
       ("help,h", "display help message")
       ("help-verbose", "display verbose help message")
       ("in-text-file,i", po::value<string>(&params.inTextFile)->required(), "input text file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("in-sources-file,S", po::value<string>(&params.inSourcesFile), "input sources file path")
       ("in-compressed", "parse compressed input text or sources file")
       ("approx,k", po::value<int>(&params.kApprox), "perform approximate search (Hamming distance) for k errors (preliminary, max pattern length = 12, not compatible with matching with sources)")
       ("out-file,o", po::value<string>(&params.outFile)->default_value("timings.txt"), "output file path")
       ("pattern-count,p", po::value<int>(&params.nPatterns), "maximum number of patterns read from top of the patterns file (non-positive values are ignored)")
       ("version,v", "display version info");

    po::positional_options_description positionalOptions;

    positionalOptions.add("in-text-file", 1);
    positionalOptions.add("in-pattern-file", 1);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                  options(options).
                  positional(positionalOptions).run(), vm);

        if (vm.count("help"))
        {
            cout << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
            cout << options << endl;

            return 0;
        }
        if (vm.count("help-verbose"))
        {
            cout << params.verboseInfoString << endl << endl;
            cout << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;

            cout << options << endl;
            cout << params.verboseParamsString << endl;

            return 0;
        }
        if (vm.count("version"))
        {
            cout << params.versionInfo << endl;
            return 0;
        }

        po::notify(vm);
    }
    catch (const po::error &e)
    {
        cerr << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
        cerr << options << endl;

        cerr << "Error: " << e.what() << endl;
        return params.errorExitCode;
    }

    if (vm.count("dump"))
    {
        params.dumpToFile = true;
    }
    if (vm.count("dump-indexes"))
    {
        params.dumpIndexes = true;
    }
    if (vm.count("full-sources-output"))
    {
        params.fullSourcesOutput = true;
    }
    if (vm.count("in-compressed"))
    {
        params.decompressInput = true;
    }

    return paramsResContinue;
}

bool checkInputFiles()
{
    if (not helpers::isFileReadable(params.inTextFile))
    {
        cerr << "Cannot access input text file (doesn't exist or insufficient permissions): " << params.inTextFile << endl;
        return false;
    }

    if (not helpers::isFileReadable(params.inPatternFile))
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
        return false;
    }

    if (not params.inSourcesFile.empty() and
        not helpers::isFileReadable(params.inSourcesFile))
    {
        cerr << "Cannot access input sources file (doesn't exist or insufficient permissions): " << params.inSourcesFile << endl;
        return false;
    }

    cout << boost::format("Started, using alphabet = \"%1%\" (make sure it matches input files, otherwise undefined behavior occurs!)") % params.alphabet << endl << endl;

    return true;
}

int run()
{
    try
    {
        const string text = readInputText();
        cout << "Parsing segments..." << endl;

        int nSegments = 0;
        int *segmentSizes = nullptr;

        const string *const *segments = parsing::parseTextArray(text, &nSegments, &segmentSizes);
        cout << "Parsed #segments = " << nSegments << endl;

        if (nSegments == 0)
        {
            throw runtime_error("cannot run for empty segments");
        }

        vector<string> patterns = readPatterns();

        SegmentData segmentData{ segments, nSegments, segmentSizes };
        Sopang::SourceMap sourceMap;

        int sourceCount = 0;

        if (not params.inSourcesFile.empty())
        {
            const vector<vector<Sopang::SourceSet>> sources = readSources(nSegments, segmentSizes, sourceCount);
            sourceMap = parsing::sourcesToSourceMap(nSegments, segmentSizes, sources);
        }

        runSopang(segmentData, sourceMap, sourceCount, patterns);
        clearMemory(segmentData);
    }
    catch (const exception &e)
    {
        cerr << endl << "Fatal error occurred: " << e.what() << endl;
        return params.errorExitCode;
    }

    return 0;
}

string readInputText()
{
    string text = helpers::readFile(params.inTextFile);
    cout << "Read file: " << params.inTextFile << endl;

    if (params.decompressInput)
    {
        text = zstd::decompress(text);
        cout << "Decompressed input text" << endl;
    }

    const double textSizeMB = text.size() / 1'000'000.0;

    if (params.dumpToFile)
    {
        string outStr = params.inTextFile + " " + to_string(textSizeMB) + " ";
        helpers::dumpToFile(outStr, params.outFile, false);
    }

    cout << boost::format("Read input text, #chars = %1%, MB = %2%") % text.size() % textSizeMB << endl;
    return text;
}

vector<string> readPatterns()
{
    const string patternsStr = helpers::readFile(params.inPatternFile);
    cout << "Read file: " << params.inPatternFile << endl;

    vector<string> patterns = parsing::parsePatterns(patternsStr);

    if (params.nPatterns > 0 and static_cast<size_t>(params.nPatterns) < patterns.size())
    {
        patterns.resize(params.nPatterns);
    }

    cout << boost::format("Parsed #patterns = %1%, #chars = %2%") % patterns.size() % patternsStr.size() << endl;

    if (patterns.empty())
    {
        throw runtime_error("cannot run for empty patterns");
    }

    return patterns;
}

vector<vector<Sopang::SourceSet>> readSources(int nSegments, const int *segmentSizes, int &sourceCount)
{
    string sourcesStr = helpers::readFile(params.inSourcesFile);
    cout << "Read file: " << params.inSourcesFile << endl;

    vector<vector<Sopang::SourceSet>> sources;

    if (params.decompressInput)
    {
        sourcesStr = zstd::decompress(sourcesStr);
        cout << "Decompressed sources text, parsing sources..." << endl;

        sources = parsing::parseSourcesCompressed(sourcesStr, sourceCount);
    }
    else
    {
        cout << "Parsing sources..." << endl;
        sources = parsing::parseSources(sourcesStr, sourceCount);
    }

    cout << boost::format("Parsed sources for non-deterministic #segments = %1%, #chars = %2%")
        % sources.size() % sourcesStr.size() << endl;

    cout << "Source count = " << sourceCount << endl;

    if (sources.empty())
    {
        throw runtime_error("cannot run for empty sources");
    }

    // We check whether the source counts match the segments in text.
    size_t sourceIdx = 0;

    for (int segmentIdx = 0; segmentIdx < nSegments; ++segmentIdx)
    {
        if (segmentSizes[segmentIdx] == 1)
        {
            continue;
        }

        if (static_cast<size_t>(segmentSizes[segmentIdx]) != sources[sourceIdx].size())
        {
            throw runtime_error("source segment variant count does not match text segment variant count, source segment index = "
                + to_string(sourceIdx));
        }

        // We check whether all sources are present for the current segment.
        Sopang::SourceSet sourcesForSegment(sourceCount);

        for (const Sopang::SourceSet &sourcesForVariant : sources[sourceIdx])
        {
            sourcesForSegment |= sourcesForVariant;
        }

        if (sourcesForSegment.count() != sourceCount)
        {
            throw runtime_error("not all sources are present for segment: " + to_string(sourceIdx));
        }

        sourceIdx += 1;

        if (sourceIdx > sources.size())
        {
            throw runtime_error("there are fewer source segments than non-deterministic segments in text");
        }
    }

    if (sourceIdx < sources.size())
    {
        throw runtime_error("there are more source segments than non-deterministic segments in text");
    }

    cout << "Sanity check for sources passed" << endl;
    return sources;
}

void runSopang(const SegmentData &segmentData,
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const vector<string> &patterns)
{
    assert(segmentData.nSegments > 0);
    assert(segmentData.segments != nullptr and segmentData.segmentSizes != nullptr);
    assert(patterns.size() > 0);

    int textSize;
    double textSizeMB;

    calcTextSize(segmentData, textSize, textSizeMB);
    cout << boost::format("EDS length = %1%, EDS size = %2% (%3% MB)") 
        % segmentData.nSegments % textSize % textSizeMB << endl;

    vector<double> elapsedSecVec;

    for (size_t iP = 0; iP < patterns.size(); ++iP)
    {
        const double percProg = static_cast<double>(iP + 1) * 100.0 / patterns.size();
        const string pattern = patterns[iP];

        string msg;

        if (params.kApprox > 0)
        {
            msg = (boost::format("Querying pattern %d/%d (%.2f) = \"%s\" for k = %d") %
                (iP + 1) % patterns.size() % percProg % pattern % params.kApprox).str();
        }
        else
        {
            msg = (boost::format("Querying pattern %d/%d (%.2f%%) = \"%s\"") %
                (iP + 1) % patterns.size() % percProg % pattern).str();
        }

        cout << endl << msg << endl;

        const double elapsedSec = measure(segmentData, sourceMap, sourceCount, pattern);
        elapsedSecVec.push_back(elapsedSec);
    }

    if (params.dumpToFile)
    {
        dumpMedians(elapsedSecVec, textSizeMB);
    }
}

void calcTextSize(const SegmentData &segmentData, int &textSize, double &textSizeMB)
{
    textSize = 0;

    for (int iS = 0; iS < segmentData.nSegments; ++iS)
    {
        for (int iSS = 0; iSS < segmentData.segmentSizes[iS]; ++iSS)
        {
            if (segmentData.segments[iS][iSS].empty())
            {
                // Empty segment is regarded as having size 1 when counted towards the total size.
                textSize += 1;
            }
            else
            {
                textSize += segmentData.segments[iS][iSS].size();
            }
        }
    }

    textSizeMB = static_cast<double>(textSize) / 1'000'000.0;
}

double measure(const SegmentData &segmentData,
    const Sopang::SourceMap &sourceMap,
    int sourceCount,
    const string &pattern)
{
    unordered_set<int> res;
    clock_t start, end;

    {
        Sopang sopang(params.alphabet);

        if (params.kApprox > 0)
        {
            if (not sourceMap.empty())
            {
                throw runtime_error("matching with sources is not supported for approximate matching");
            }

            start = std::clock();
            res = sopang.matchApprox(
                segmentData.segments,
                segmentData.nSegments,
                segmentData.segmentSizes,
                pattern,
                params.kApprox);
            end = std::clock();
        }
        else
        {
            if (sourceMap.empty())
            {
                start = std::clock();
                res = sopang.match(
                    segmentData.segments,
                    segmentData.nSegments,
                    segmentData.segmentSizes,
                    pattern);
                end = std::clock();
            }
            else
            {
                if (params.fullSourcesOutput)
                {
                    start = std::clock();
                    const auto fullSourceMatches = sopang.matchWithSources(
                        segmentData.segments,
                        segmentData.nSegments,
                        segmentData.segmentSizes,
                        sourceMap,
                        sourceCount,
                        pattern);
                    end = std::clock();

                    for (const auto &kv : map<int, Sopang::SourceSet>(fullSourceMatches.begin(), fullSourceMatches.end())) // ordered map
                    {
                        res.insert(kv.first);

                        if (params.dumpIndexes)
                        {
                            dumpSources(kv.first, kv.second);
                        }
                    }
                }
                else
                {
                    start = std::clock();
                    res = sopang.matchWithSourcesVerify(
                        segmentData.segments,
                        segmentData.nSegments,
                        segmentData.segmentSizes,
                        sourceMap,
                        sourceCount,
                        pattern);
                    end = std::clock();
                }
            }
        }
    }

    // Make sure that the number of results is printed in order to
    // prevent the compiler from overoptimizing unused results.
    cout << "#results = " << res.size() << endl;

    if (params.dumpIndexes)
    {
        dumpIndexes(res);
    }

    double elapsedSec = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
    if (elapsedSec == 0.0)
    {
        cerr << "[ERROR] Elapsed is 0" << endl;
    }

    return elapsedSec;
}

void dumpMedians(const vector<double> &elapsedSecVec, double textSizeMB)
{
    assert(elapsedSecVec.size() > 0);
    double medianTotal = 0.0;

    helpers::calcStatsMedian(elapsedSecVec, &medianTotal);
    helpers::dumpToFile(to_string(medianTotal) + " ", params.outFile);

    const double throughputMedian = textSizeMB / medianTotal;
    helpers::dumpToFile(to_string(throughputMedian) + "\n", params.outFile);

    cout << endl << "Dumped to: " << params.outFile << " ([input file name] [input text size MB] [median elapsed sec] [median throughput MB/s])" << endl;
}

void dumpSources(int index, const Sopang::SourceSet &sources)
{
    cout << index << " -> ";

    if (sources.empty())
    {
        cout << "no sources (deterministic segment)";
    }
    else
    {
        for (const int sourceIndex : sources.toSet()) // ordered set
        {
            cout << sourceIndex << ' ';
        }
    }

    cout << endl;
}

void dumpIndexes(const unordered_set<int> &indexes)
{
    if (indexes.empty())
        return;

    for (const int index : set<int>(indexes.begin(), indexes.end())) // ordered set
    {
        cout << index << " ";
    }

    cout << endl;
}

void clearMemory(SegmentData &segmentData)
{
    assert(segmentData.nSegments > 0);
    assert(segmentData.segments != nullptr and segmentData.segmentSizes != nullptr);

    for (int iSeg = 0; iSeg < segmentData.nSegments; ++iSeg)
    {
        delete[] segmentData.segments[iSeg];
    }

    delete[] segmentData.segments;
    delete[] segmentData.segmentSizes;

    cout << endl << "Cleared memory" << endl;
}

} // namespace sopang
