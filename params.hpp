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
        "This software is called SOPanG (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pangenome model).\n"
        "ED text is in a format: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each indeterminate segment (i.e. segment having multiple variants), and commas delimit segment variants. Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.\n"
        "SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).\n\n"
        "The program can be run in two following modes of operation.\n\n"
        "1. In order to run the program on a single input elastic degenerate file and a single patterns file, set parameters inTextFile and inPatternFile (in params.hpp) to their paths, respectively (either absolute or relative paths to the root folder). Run without arguments as ./sopang in order to perform a run on these two files. In this mode, changing input files requires program recompilation.\n\n"
        "2. The program can be also run with command line arguments for automated processing. Firstly, the parameter inDataFolder (in params.hpp) should be set to point to the folder containing the data -- changing this parameter requires recompilation, by default it points to the sample folder containing a single elastic-degenerate text and a single patterns file. Each input chromosome file name should be in the form chrN.eds, where N is the chromosome number, e.g., 1, 2, etc., and each input patterns file name should be in a form patternsM.txt, where M is the length of patterns in this file, e.g., 8, 16, etc. The program can be then run as follows:\n"
        "./sopang [chromosome index] [pattern length]\n"
        "e.g., ./sopang 10 8 for chromosome 10 (input text file chr10.eds) and pattern length 8 (input patterns file patterns8.txt). Attached is also a script run all.sh, which allows for running sopang over multiple chromosomes and pattern files.";
};

#endif // PARAMS_HPP
