/*
 *** SOPanG, a simple tool for pattern matching over an elastic-degenerate string, a recently proposed simplified model for the pan-genome.
 *** Authors for release versions before 1.4.0: Aleksander Cisłak, Szymon Grabowski, Jan Holub.
 *** Authors for release versions after (including) 1.4.0: Aleksander Cisłak, Szymon Grabowski.
 *** License: GNU LGPL v3.
 *** Set BOOST_DIR in makefile and type "make" for optimized compile.
 */

#include "helpers.hpp"
#include "params.hpp"
#include "sopang.hpp"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
    unsigned nSegments; // Number of segments.
    const unsigned *segmentSizes; // Size of each segment (number of variants).
};

/** Handles cmd-line parameters, returns paramsResContinue if program execution should continue. */
int handleParams(int argc, const char **argv);
/** Returns true if input files are readable, false otherwise. */
bool checkInputFiles();
/** Runs the main program and returns the program exit code. */
int run();

string readInputText();
vector<string> readPatterns();
vector<vector<set<int>>> readSources(unsigned nSegments, const unsigned *segmentSizes);

/** Runs sopang for [segmentData] and [sources] (which may be empty), searching for [patterns]. */
void runSopang(const SegmentData &segmentData, const vector<vector<set<int>>> &sources,
    const vector<string> &patterns);

/** Calculates total [textSize] in bytes and corresponding [textSizeMB] in megabytes (10^6) for [segmentData]. */
void calcTextSize(const SegmentData &segmentData, int *textSize, double *textSizeMB);

/** Searches for [pattern] in [segmentData] and [sources] (which may be empty) and returns elapsed time in seconds. */
double measure(const SegmentData &segmentData, const vector<vector<set<int>>> &sources,
    const string &pattern);

void dumpMedians(const vector<double> &elapsedSecVec, double textSizeMB);
void dumpIndexes(const unordered_set<unsigned> &indexes);

void clearMemory(SegmentData &segmentData);

} // namespace sopang

int main(int argc, const char **argv)
{
    int paramsRes = handleParams(argc, argv);
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
       ("help,h", "display help message")
       ("help-verbose", "display verbose help message")
       ("in-text-file,i", po::value<string>(&params.inTextFile)->required(), "input text file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("in-sources-file,S", po::value<string>(&params.inSourcesFile), "input sources file path")
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

    return paramsResContinue;
}

bool checkInputFiles()
{
    if (not Helpers::isFileReadable(params.inTextFile))
    {
        cerr << "Cannot access input text file (doesn't exist or insufficient permissions): " << params.inTextFile << endl;
        return false;
    }

    if (not Helpers::isFileReadable(params.inPatternFile))
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
        return false;
    }

    if (not params.inSourcesFile.empty() and
        not Helpers::isFileReadable(params.inSourcesFile))
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
        string text = readInputText();
        cout << "Parsing segments..." << endl;

        unsigned nSegments = 0;
        unsigned *segmentSizes = nullptr;

        const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);
        cout << "Parsed #segments = " << nSegments << endl;

        if (nSegments == 0)
        {
            throw runtime_error("cannot run for empty segments");
        }

        vector<string> patterns = readPatterns();

        SegmentData segmentData{ segments, nSegments, segmentSizes };
        vector<vector<set<int>>> sources;

        if (not params.inSourcesFile.empty())
        {
            sources = readSources(nSegments, segmentSizes);
        }

        runSopang(segmentData, sources, patterns);
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
    string text = Helpers::readFile(params.inTextFile);
    cout << "Read file: " << params.inTextFile << endl;

    double textSizeMB = text.size() / 1000.0 / 1000.0;
    if (params.dumpToFile)
    {
        string outStr = params.inTextFile + " " + to_string(textSizeMB) + " ";
        Helpers::dumpToFile(outStr, params.outFile, false);
    }

    cout << boost::format("Read input text, #chars = %1%, MB = %2%") % text.size() % textSizeMB << endl;
    return text;
}

vector<string> readPatterns()
{
    string patternsStr = Helpers::readFile(params.inPatternFile);
    cout << "Read patterns, #chars = " << patternsStr.size() << endl;

    vector<string> patterns = Sopang::parsePatterns(patternsStr);

    if (params.nPatterns > 0 and static_cast<size_t>(params.nPatterns) < patterns.size())
    {
        patterns.resize(params.nPatterns);
    }

    cout << "Parsed #patterns = " << patterns.size() << endl;

    if (patterns.empty())
    {
        throw runtime_error("cannot run for empty patterns");
    }

    return patterns;
}

vector<vector<set<int>>> readSources(unsigned nSegments, const unsigned *segmentSizes)
{
    string sourcesStr = Helpers::readFile(params.inSourcesFile);
    cout << "Read sources, #chars = " << sourcesStr.size() << endl;

    int sourceCount;
    vector<vector<set<int>>> sources = Sopang::parseSources(sourcesStr, sourceCount);

    cout << "Parsed sources for non-deterministic #segments = " << sources.size() << endl;
    cout << "Source count = " << sourceCount << endl;

    if (sources.empty())
    {
        throw runtime_error("cannot run for empty sources");
    }

    // We check whether the source counts match the segments in text.
    size_t sourceIdx = 0;

    for (unsigned segmentIdx = 0; segmentIdx < nSegments; ++segmentIdx)
    {
        if (segmentSizes[segmentIdx] == 1)
        {
            continue;
        }

        if (segmentSizes[segmentIdx] != sources[sourceIdx].size())
        {
            throw runtime_error("source segment variant count does not match text segment variant count, source segment index = "
                + to_string(sourceIdx));
        }

        // We check whether all sources are present for the current segment.
        set<int> sourcesForSegment;

        for (const set<int> &sourcesForVariant : sources[sourceIdx])
        {
            sourcesForSegment.insert(sourcesForVariant.begin(), sourcesForVariant.end());
        }

        if (sourcesForSegment.size() != static_cast<size_t>(sourceCount))
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

void runSopang(const SegmentData &segmentData, const vector<vector<set<int>>> &sources, 
    const vector<string> &patterns)
{
    assert(segmentData.nSegments > 0);
    assert(segmentData.segments != nullptr and segmentData.segmentSizes != nullptr);
    assert(patterns.size() > 0);

    int textSize;
    double textSizeMB;

    calcTextSize(segmentData, &textSize, &textSizeMB);
    cout << boost::format("EDS length = %1%, EDS size = %2% (%3% MB)") 
        % segmentData.nSegments % textSize % textSizeMB << endl;

    vector<double> elapsedSecVec;

    for (size_t iP = 0; iP < patterns.size(); ++iP)
    {
        double percProg = static_cast<double>(iP + 1) * 100.0 / patterns.size();
        string pattern = patterns[iP];

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

        double elapsedSec = measure(segmentData, sources, pattern);
        elapsedSecVec.push_back(elapsedSec);
    }

    if (params.dumpToFile)
    {
        dumpMedians(elapsedSecVec, textSizeMB);
    }
}

void calcTextSize(const SegmentData &segmentData, int *textSize, double *textSizeMB)
{
    *textSize = 0;

    for (unsigned iS = 0; iS < segmentData.nSegments; ++iS)
    {
        for (unsigned iSS = 0; iSS < segmentData.segmentSizes[iS]; ++iSS)
        {
            *textSize += segmentData.segments[iS][iSS].size();
        }
    }

    *textSizeMB = static_cast<double>(*textSize) / 1000.0 / 1000.0;
}

double measure(const SegmentData &segmentData, const vector<vector<set<int>>> &sources,
    const string &pattern)
{
    unordered_set<unsigned> res;
    clock_t start, end;

    {
        Sopang sopang;

        if (params.kApprox > 0)
        {
            if (not sources.empty())
            {
                throw runtime_error("matching with sources is not supported for approximate matching");
            }

            start = std::clock();
            res = sopang.matchApprox(segmentData.segments, segmentData.nSegments, segmentData.segmentSizes,
                pattern, params.alphabet, params.kApprox);
            end = std::clock();
        }
        else
        {
            if (sources.empty())
            {
                start = std::clock();
                res = sopang.match(segmentData.segments, segmentData.nSegments, segmentData.segmentSizes,
                    pattern, params.alphabet);
                end = std::clock();
            }
            else
            {
                start = std::clock();
                res = sopang.matchWithSources(segmentData.segments, segmentData.nSegments, segmentData.segmentSizes,
                    sources, pattern, params.alphabet);
                end = std::clock();
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

    Helpers::calcStatsMedian(elapsedSecVec, &medianTotal);
    Helpers::dumpToFile(to_string(medianTotal) + " ", params.outFile);

    double throughputMedian = textSizeMB / medianTotal;
    Helpers::dumpToFile(to_string(throughputMedian) + "\n", params.outFile);

    cout << endl << "Dumped to: " << params.outFile << " ([input file name] [input text size MB] [median elapsed sec] [median throughput MB/s])" << endl;
}

void dumpIndexes(const unordered_set<unsigned> &indexes)
{
    if (indexes.size() == 0)
    {
        return;
    }

    set<unsigned> resSorted(indexes.begin(), indexes.end()); // ordered set

    for (const unsigned r : resSorted)
    {
        cout << r << " ";
    }

    cout << endl;
}

void clearMemory(SegmentData &segmentData)
{
    assert(segmentData.nSegments > 0);
    assert(segmentData.segments != nullptr and segmentData.segmentSizes != nullptr);

    for (unsigned iSeg = 0; iSeg < segmentData.nSegments; ++iSeg)
    {
        delete[] segmentData.segments[iSeg];
    }

    delete[] segmentData.segments;
    delete[] segmentData.segmentSizes;

    cout << endl << "Cleared memory" << endl;
}

} // namespace sopang
