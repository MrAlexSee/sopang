#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

using namespace std;

namespace sopang
{

struct Params
{
    // COMPILE-TIME PARAMS

    const string alphabet = "ACGTNUVISMEL<>:0123456789"; // Matches all characters in EDSO (Grossi et al. CPM '17) tool output.

    // COMMAND-LINE PARAMS
    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    bool dumpToFile = false; // Dump input file info and throughput to output file (outFile). Cmd arg -d.
    bool dumpIndexes = false; // Dump resulting indexes (full results) to stdout. Cmd arg -D.

    int kApprox = noValue; // Number of errors for approximate search (Hamming). noValue = perform exact search. Cmd arg -k.
    int nPatterns = noValue; // Maximum number of patterns read from top of the patterns file; noValue = ignore the pattern count limit. Cmd arg -p.

    string inTextFile = ""; // Input text file path (positional arg 1).
    string inPatternFile = ""; // Input pattern file path (positional arg 2).

    string outFile = "res.txt"; // Output file path.

    // CONSTANTS
    static constexpr int errorExitCode = 1; // Returned from main on failure.
    static constexpr int noValue = -1; // Indicates that a given non-negative integer is not set.

    const string versionInfo = "sopang v1.3.0"; // Current version: major.minor.patch
    const string usageInfoString = "[options] <input text file> <input pattern file>";

    const string verboseInfoString =
        "This software is called SOPanG (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pan-genome model). Authors: Aleksander Cislak, Szymon Grabowski, Jan Holub.\n"
        "ED text is in the following format: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each indeterminate segment (i.e. segment having multiple variants), and commas delimit segment variants.\n"
        "If a comma is not preceded by a string of letters or it is a trailing symbol in a segment, it indicates an empty word.\n"
        "To give an example, all three notations: {,A,C}, {A,,C}, and {A,C,} mean the same, which is a segment which accepts either a string A, or a string C, or an empty word.\n"
        "Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.\n"
        "Note that, e.g., {AC,CG} and {AC, CG} are not the same (the latter would expect a space in its second variant). Therefore, you should not use whitespaces in the ED text if not intended.\n"
        "SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).";

    const string verboseParamsString =
        "Input text file (positional parameter 1 or named parameter -i or --in-text-file) should contain the elastic-degenerate text in the format {A,C,}GAAT{AT,A}ATT.\n"
        "Input pattern file (positional parameter 2 or named parameter -I or --in-pattern-file) should contain the list of patterns, each of the same length, separated with newline characters.\n"
        "Attached as part of the sopang package is a script run_all.sh, which allows for processing multiple input text (chromosome) and pattern files.";
};

}

#endif // PARAMS_HPP
