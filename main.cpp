// Add Boost library to the path for compilation.
// Regular compile: g++ -Wall -pedantic -std=c++11 main.cpp sopang.cpp -o sopang
// Optimized compile: g++ -Wall -pedantic -std=c++11 -DNDEBUG -O3 main.cpp sopang.cpp -o sopang

#include <boost/format.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <set>
#include <string>

#include "helpers.hpp"
#include "params.hpp"
#include "sopang.hpp"

using namespace std;

void parseParams(int argc, char **argv);
void calcTextSize(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
                  int *textSize, double *textSizeMB);

void doTest(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
            const vector<string> &patterns);
double measure(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes, const string &pattern);

void clearMemory(const string *const *segments, unsigned nSegments, unsigned *segmentSizes);

namespace
{
    Params params;
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        cout << "Using command-line args" << endl;
        parseParams(argc, argv);
    }
    else if (argc > 1)
    {
        cerr << "Usage: sopang [chromosome index] [pattern length]" << endl;
        return 1;
    }

    cout << boost::format("Started, using alphabet = \"%1%\" (make sure it matches the input file!)")
            % params.alphabet << endl;
    cout << "If the alphabet does not match the input file, undefined behavior occurs" << endl;

    string text = Helpers::readFile(params.inTextFile);
    cout << "Read file: " << params.inTextFile << endl;

    float textSizeMB = text.size() / 1000.0 / 1000.0;

    if (params.dumpToFile)
    {
        Helpers::dumpToFile(to_string(textSizeMB) + " ", params.outFile, false);
    }

    cout << boost::format("Read text, #chars = %1%, MB = %2%") % text.size() % textSizeMB << endl;
    cout << "Parsing segments..." << endl;

    unsigned nSegments = 0;
    unsigned *segmentSizes = nullptr;

    const string *const *segments = Sopang::parseTextArray(text, &nSegments, &segmentSizes);
    cout << "Parsed #segments = " << nSegments << endl;

    string patternsStr = Helpers::readFile(params.inPatternFile);
    cout << "Read patterns, #chars = " << patternsStr.size() << endl;

    vector<string> patterns = Sopang::parsePatterns(patternsStr);

    if (params.nPatterns > 0)
    {
        patterns.resize(params.nPatterns);
    }

    cout << "Parsed #patterns = " << patterns.size() << endl;
    doTest(segments, nSegments, segmentSizes, patterns);

    clearMemory(segments, nSegments, segmentSizes);
}

void parseParams(int argc, char **argv)
{
    assert(argc == 3);

    string chromoIndex = argv[1];
    string patternLength = argv[2];

    params.inTextFile = params.inDataFolder + "/" + "chr" + chromoIndex + ".eds";
    params.inPatternFile = params.inDataFolder + "/" + "patterns" + patternLength + ".txt";
    params.outFile = "res" + chromoIndex + "_" + patternLength + ".txt";

    cout << boost::format("Updated parameters, chromo index = %1%, pattern length = %2%")
            % chromoIndex % patternLength << endl;
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

void doTest(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes,
            const vector<string> &patterns)
{
    int textSize;
    double textSizeMB;

    calcTextSize(segments, nSegments, segmentSizes, &textSize, &textSizeMB);
    cout << boost::format("EDS length = %1%, EDS size = %2% (%3% MB)") % nSegments % textSize % textSizeMB << endl;

    vector<double> elapsedTotal;

    for (size_t iP = 0; iP < patterns.size(); ++iP)
    {
        string pattern = patterns[iP];
        cout << boost::format("Querying pattern %1%/%2% = \"%3%\"") % (iP + 1) % patterns.size() % pattern << endl;

        double elapsed = measure(segments, nSegments, segmentSizes, pattern);
        elapsedTotal.push_back(elapsed);
    }

    if (params.dumpToFile)
    {
        double medianTotal;

        Helpers::calcStatsMedian(elapsedTotal, &medianTotal);
        Helpers::dumpToFile(to_string(medianTotal) + " ", params.outFile);

        double throughputMedian = textSizeMB / medianTotal;
        Helpers::dumpToFile(to_string(throughputMedian) + "\n", params.outFile);
    }
}

double measure(const string *const *segments, unsigned nSegments, const unsigned *segmentSizes, const string &pattern)
{
    Sopang sopang;

    clock_t start = std::clock();
    unordered_set<unsigned> res = sopang.matchArray(segments, nSegments, segmentSizes, pattern, params.alphabet);
    clock_t end = std::clock();

    // Make sure that the number of results is printed in order to
    // prevent the compiler from overoptimizing unused results.
    cout << endl << boost::format("#results = %1%, pattern = \"%2%\"") % res.size() % pattern << endl;

    if (params.dumpIndexes)
    {
        set<unsigned> resSorted(res.begin(), res.end()); // ordered set

        for (const unsigned r : resSorted)
        {
            cout << r << " ";
        }
    }

    double elapsed = (end - start) / static_cast<double>(CLOCKS_PER_SEC);

    if (elapsed == 0.0)
    {
        cerr << "[ERROR] Elapsed is 0" << endl;
    }

    return elapsed;
}

void clearMemory(const string *const *segments, unsigned nSegments, unsigned *segmentSizes)
{
    assert(segments != nullptr and segmentSizes != nullptr);
    assert(nSegments > 0);

    for (unsigned iSeg = 0; iSeg < nSegments; ++iSeg)
    {
        delete[] segments[iSeg];
    }

    delete[] segmentSizes;
    delete[] segments;

    cout << "Cleared memory" << endl;
}
