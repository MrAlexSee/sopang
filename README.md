## Basic information

This software is called **SOPanG** (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pangenome model).

ED text is in a format: {A,C,}GAAT{AT,A}ATT. Braces determine the start and end of each indeterminate segment (i.e. segments having multiple variants), and commas delimit segment variants. Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.

SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).

## Compilation

Add Boost library to the path for compilation.

* Regular compile: g++ -Wall -pedantic -std=c++11 main.cpp sopang.cpp -o sopang
* Optimized compile: g++ -Wall -pedantic -std=c++11 -DNDEBUG -O3 main.cpp sopang.cpp -o sopang
* Or set BOOST_DIR in makefile and type "make" for optimized compile.

Tested with gcc 64-bit 7.2.0 and Boost 1.65.1 (not time-critical, used only for
data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

## Usage

The program can be run in two following modes of operation.

1. In order to run the program on a single input elastic degenerate file and a single patterns file, set parameters inTextFile and inPatternFile (in params.hpp) to their paths, respectively (either absolute or relative paths to the root folder). Run without arguments as ./sopangi.e. human genome and synthetic data in order to perform a run on these two files. In this mode, changing input files requires program recompilation.

1. The program can be also run with command line arguments for automated processing. Firstly, the parameter inDataFolder (in params.hpp) should be set to point to the folder containing the data -- changing this parameter requires recompilation, by default it points to the sample folder containing a single elastic-degenerate text and a single patterns file. Each input chromosome file name should be in the form chrN.eds, where N is the chromosome number, e.g., 1, 2, etc., and each input patterns file name should be in a form patternsM.txt, where M is the length of patterns in this file, e.g., 8, 16, etc. The program can be then run as follows:

`./sopang [chromosome index] [pattern length]`

e.g., ./sopang 10 8 for chromosome 10 (input text file chr10.eds) and pattern length 8 (input patterns file patterns8.txt). Attached is also a script run all.sh, which allows for running sopang over multiple chromosomes and pattern files.

## Testing

Testing on human genome and synthetic data:

1. Download edso: https://github.com/webmasterar/edso/.

1. Download data from the 1000 Genomes project: ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/.

1. Run edso for each chromosome and variants file. Rename resulting files as chr1.eds, chr2.eds, ..., chr23.eds.

1. Generate synthetic data by running python generate synth.py (requires Python 2.7) 4 times for the number of segments (parameter nSegments) set to 100, 500, 1000, and 1600. Rename files as chr24.eds, chr25.eds, chr26.eds, chr27.eds.

1. Set parameter inDataFolder to the folder containing .eds files and pattern files (all pattern files are located in the sample/ folder as part of this package).

1. Compile SoPanG (see above).

1. Run python run_all.sh.

## Compile-time parameter description

#### params.hpp

Parameter name | Parameter description
inTextFile     | input text (elastic-degenerate format) file path for processing without command line arguments
inPatternFile  | input file path with a list of patterns, each of the same length, separated with newline characters for processing without command line arguments
inDataFolder   | input root folder path for processing with command line arguments
outFile        | output file path
alphabet       | a set of symbols occurring in input text or patterns
dumpToFile     | set to true in order to dump file size information and throughput to output file (outFile)
dumpIndexes    | set to true in order to dump all results, i.e., resulting indexes (end positions of segments where a match occurred) to standard output
nPatterns      | maximum number of patterns from the input pattern file (inPatternFile) which are processed, set to -1 to ignore

#### sopang.hpp

Parameter name | Parameter description
dBufferSize    | buffer size for processing segment variants, the size of the
largest segment (i.e., the number of variants) from the input file cannot
be larger than this value,
maskBufferSize | buffer size for shift-or masks for the input alphabet,
must be larger than the largest input character ASCII code,
wordSize       | shift-or word size in bits.

#### generate_synth.py

Parameter name       | Parameter description
nSegments            | total number of segments
alphabet             | alphabet for character sampling
nDegeneratePositions | number of segments (must be smaller or equal to nSegments) which are degenerate (indeterminate), i.e., contain multiple variants
nMaxSegmentVariants  | maximum number of variants (a), the number of variants for each degenerate segment will be sampled from the interval [1, a]
nMaxVariantLength    | maximum length of each segment variant (b), the length for each variant will be sampled from the interval [0, b] (segments might contain empty words)
outFile              | output file path
