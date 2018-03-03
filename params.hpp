#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

using namespace std;

struct Params
{
    string inTextFile = "sample/chr1337.eds";
    string inPatternFile = "sample/patterns8.txt";

    string inDataFolder = "sample/"; // Used with command line args.
    string outFile = "res.txt";

    const string alphabet = "ACGTNUVISMEL<>:0123456789"; // matches characters in edso output

    const bool dumpToFile = true; // Dumps input file info and throughput to outFile.
    const bool dumpIndexes = false; // Dumps resulting indexes to outFile.

    const int nPatterns = -1; // Set to -1 to ignore.
};

#endif // PARAMS_HPP
