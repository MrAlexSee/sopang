#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

using namespace std;

namespace inverted_basilisk
{

struct Params
{
    // COMPILE-TIME PARAMS

    const string alphabet = "ACGTNUVISMEL<>:0123456789"; // Matches all characters in EDSO (Grossi et al. CPM '17) tool output.

    // COMMAND-LINE PARAMS
    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    bool dumpToFile = false; // Dump input file info and throughput to output file (outFile). Cmd arg -d.
    bool dumpIndexes = false; // Dump resulting indexes (full results) to stdout. Cmd arg -D.

    int nPatterns = -1; // Maximum number of patterns read from top of the patterns file; -1 = ignore the pattern count limit. Cmd arg -p.

    string inTextFile = ""; // Input text file path (positional arg 1).
    string inPatternFile = ""; // Input pattern file path (positional arg 2).

    string outFile = "res.txt"; // Output file path.

    // CONSTANTS
    static constexpr int errorExitCode = 1; // Returned from main on failure.

    const string versionInfo = "sopang v1.2.0"; // Current version: major.minor.patch
    const string usageInfoString = "[options] <input text file> <input pattern file>";

    const string verboseInfoString =
        "This software is called SOPanG (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pangenome model). Authors: Aleksander Cislak, Szymon Grabowski, Jan Holub.\n"
        "ED text is in the format: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each indeterminate segment (i.e. segment having multiple variants), and commas delimit segment variants. Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.\n"
        "SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).";

    const string verboseParamsString =
        "Input text file (positional parameter 1 or named parameter -i or --in-text-file) should contain the elastic-degenerate text in the format {A,C,}GAAT{AT,A}ATT.\n"
        "Input pattern file (positional parameter 2 or named parameter -I or --in-pattern-file) should contain the list of patterns, each of the same length, separated with newline characters.\n"
        "Attached as part of the sopang package is a script run_all.sh, which allows for processing multiple input text (chromosome) and pattern files.";
};
}

#endif // PARAMS_HPP
