/*
 ***
 SOPanG, a simple tool for exact pattern matching over a so-called elastic-degenerate string, a recently proposed simplified model for the pan-genome.
 ***
 Authors: Aleksander Cisłak, Szymon Grabowski, Jan Holub.
 ***
 Set BOOST_DIR in makefile and type "make" for optimized compile.
 ***
 */

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "helpers.hpp"
#include "params.hpp"
#include "sopang.hpp"

using namespace inverted_basilisk;
using namespace std;

namespace po = boost::program_options;

namespace
{
    Params params;
}

namespace inverted_basilisk
{

int handleParams(int argc, const char **argv);
bool checkInputFiles(const char *execName);
int run();

string readInputText();
vector<string> readPatterns();

void runSopang(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
               const vector<string> &patterns);

void calcTextSize(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
                  int *textSize, double *textSizeMB);
double measure(const string *const *segments, unsigned nSegments,
               const unsigned *segmentSizes, const string &pattern);

void dumpMedians(const vector<double> &elapsedSecVec, double textSizeMB);
void dumpIndexes(const unordered_set<unsigned> &indexes);

void clearMemory(const string *const *segments, unsigned nSegments, unsigned *segmentSizes);
}

int main(int argc, const char **argv)
{
    int paramsRes = handleParams(argc, argv);
    if (paramsRes != -1)
    {
        return paramsRes;
    }

    if (checkInputFiles(argv[0]) == false)
    {
        return params.errorExitCode;
    }

    return run();
}

namespace inverted_basilisk
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
       ("out-file,o", po::value<string>(&params.outFile), "output file path")
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
    catch (const po::error& e)
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

    return -1;
}

bool checkInputFiles(const char *execName)
{
    if (Helpers::isFileReadable(params.inTextFile) == false)
    {
        cerr << "Cannot access input text file (doesn't exist or insufficient permissions): " << params.inTextFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    if (Helpers::isFileReadable(params.inPatternFile) == false)
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

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

        runSopang(segments, nSegments, segmentSizes, patterns);
        clearMemory(segments, nSegments, segmentSizes);
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

    if (params.nPatterns > 0)
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


void runSopang(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
               const vector<string> &patterns)
{
    assert(segments != nullptr and segmentSizes != nullptr and nSegments > 0);
    assert(patterns.size() > 0);

    int textSize;
    double textSizeMB;

    calcTextSize(segments, nSegments, segmentSizes, &textSize, &textSizeMB);
    cout << boost::format("EDS length = %1%, EDS size = %2% (%3% MB)") % nSegments % textSize % textSizeMB << endl;

    vector<double> elapsedSecVec;

    for (size_t iP = 0; iP < patterns.size(); ++iP)
    {
        int percProg = round(static_cast<double>(iP + 1) * 100.0 / patterns.size());

        string pattern = patterns[iP];
        cout << endl << boost::format("Querying pattern %1%/%2% (%3%%%) = \"%4%\"") %
                                     (iP + 1) % patterns.size() % round(percProg) % pattern << endl;

        double elapsedSec = measure(segments, nSegments, segmentSizes, pattern);
        elapsedSecVec.push_back(elapsedSec);
    }

    if (params.dumpToFile)
    {
        dumpMedians(elapsedSecVec, textSizeMB);
    }
}

void calcTextSize(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
                  int *textSize, double *textSizeMB)
{
    *textSize = 0;

    for (unsigned iS = 0; iS < nSegments; ++iS)
    {
        for (unsigned iSS = 0; iSS < segmentSizes[iS]; ++iSS)
        {
            *textSize += segments[iS][iSS].size();
        }
    }

    *textSizeMB = static_cast<double>(*textSize) / 1000.0 / 1000.0;
}

double measure(const string *const *segments, unsigned nSegments,
               const unsigned *segmentSizes, const string &pattern)
{
    Sopang sopang;

    clock_t start = std::clock();
    unordered_set<unsigned> res = sopang.match(segments, nSegments, segmentSizes, pattern, params.alphabet);
    clock_t end = std::clock();

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

void clearMemory(const string *const *segments, unsigned nSegments, unsigned *segmentSizes)
{
    assert(segments != nullptr and segmentSizes != nullptr and nSegments > 0);

    for (unsigned iSeg = 0; iSeg < nSegments; ++iSeg)
    {
        delete[] segments[iSeg];
    }

    delete[] segments;
    delete[] segmentSizes;

    cout << endl << "Cleared memory" << endl;
}
}
