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

    // Matches all characters in EDSO (Grossi et al. CPM '17) tool output.
    const string alphabet = "ACGTNUVISMEL<>:0123456789";

    const bool dumpToFile = true; // Dumps input file info and throughput to outFile.
    const bool dumpIndexes = false; // Dumps resulting indexes to stdout.

    const int nPatterns = -1; // Set to -1 to ignore.

    const string verNumber = "1.0.1"; // Current version: major.minor.patch
    const string helpString =
        "This software is called SOPanG (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pangenome model).\n\n"
        "ED text is in a format: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each indeterminate segment (i.e. segment having multiple variants), and commas delimit segment variants. Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.\n\n"
        "SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).\n";
};

#endif // PARAMS_HPP
