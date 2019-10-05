#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

namespace sopang
{

struct Params
{
    /* 
     *** COMPILE-TIME PARAMS
     */

    /** A set of symbols occurring in input (ED) text file or input pattern file.
      * Restricted to DNA characters, matches the output of the data generation tools from the scripts folder. */
    const std::string alphabet = "ACGTN";

    /** Input and output buffer size used for Zstandard library streaming decompression. */
    const size_t zstdBufferSize = 16384;

    /*
     *** COMMAND-LINE PARAMS
     */

    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    /** Decompress input files (zstd lib compression and custom sources file format). */
    bool decompressInput = false;
    /** Dump input file info and throughput to output file (outFile). Cmd arg -d. */
    bool dumpToFile = false;
    /** Dump resulting indexes (full results) to stdout. Cmd arg -D. */
    bool dumpIndexes = false;

    /** Number of errors for approximate search (Hamming distance). noValue = perform exact search. Cmd arg -k. */
    int kApprox = noValue;
    /** Maximum number of patterns read from top of the patterns file. noValue = ignore the pattern count limit. Cmd arg -p. */
    int nPatterns = noValue;

    /** Input text file path (positional arg 1). */
    std::string inTextFile;
    /** Input pattern file path (positional arg 2). */
    std::string inPatternFile;
    /** Input sources file path. Cmd arg -S. */
    std::string inSourcesFile;

    /** Output file path. Cmd arg -o. */
    std::string outFile;

    /*
     *** CONSTANTS
     */

    /** Returned from main on failure. */
    static constexpr int errorExitCode = 1;
    /** Indicates that a given non-negative integer is not set. */
    static constexpr int noValue = -1;

    /** Current version: major.minor.patch */
    const std::string versionInfo = "sopang v1.4.0";
    const std::string usageInfoString = "[options] <input text file> <input pattern file>";

    const std::string verboseInfoString =
        "This software is called SOPanG (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pan-genome model).\n"
        "Authors for the current release version: Aleksander Cislak, Szymon Grabowski. Authors of the SOPanG algorithm: Aleksander Cislak, Szymon Grabowski, Jan Holub.\n"
        "ED text is in a format possibly best explained with an example: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each non-deterministic segment (i.e. segment having multiple variants), and commas delimit segment variants.\n"
        "If a comma is not preceded by a string of letters or it is a trailing symbol in a segment, it indicates an empty word.\n"
        "To give an example, all three notations: {,A,C}, {A,,C}, and {A,C,} mean the same, which is a segment which accepts either a string A, or a string C, or an empty word.\n"
        "Deterministic segments (i.e. segments having a single variant) are stored as regular contiguous strings.\n"
        "Note that, e.g., {AC,CG} and {AC, CG} are not the same (the latter would expect a space in its second variant). Therefore, you should not use whitespaces in the ED text if not intended.\n"
        "SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).\n"
        "See README.md for more information.";

    const std::string verboseParamsString =
        "Input text file (positional parameter 1 or named parameter -i or --in-text-file) should contain the elastic-degenerate text in the format {A,C,}GAAT{AT,A}ATT.\n"
        "Input pattern file (positional parameter 2 or named parameter -I or --in-pattern-file) should contain the list of patterns, each of the same length, separated with newline characters.\n"
        "Attached as part of the sopang package is a folder performance_tests, which allows for generating and processing multiple input text (chromosome) and pattern files.";
};

} // namespace sopang

#endif // PARAMS_HPP
