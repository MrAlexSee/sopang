## Basic information

This software is called **SOPanG** (Shift-Or for Pan-Genome). It can be used for matching patterns in elastic-degenerate (ED) text (simplified pangenome model). Authors: Aleksander Cisłak, Szymon Grabowski, Jan Holub.

ED text is in the following format: `{A,C,}GAAT{AT,A}ATT`. 
Braces determine the start and end of each indeterminate segment (degenerate segments, i.e. segments having multiple variants), and commas delimit segment variants.
If a comma is not preceded by a string of letters or it is a trailing symbol in a segment, it indicates an empty word.
To give an example, all three notations: `{,A,C}`, `{A,,C}`, and `{A,C,}` mean the same, which is a segment which accepts either a string `A`, or a string `C`, or an empty word.
Determinate segments (i.e. segments having a single variant) are stored as regular contiguous strings.
Note that, e.g., `{AC,CG}` and `{AC, CG}` are not the same (the latter would expect a space in its second variant).
Therefore, you should not use whitespaces in the ED text if not intended.

SOPanG returns the end positions of pattern occurrences in the ED text. More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).

## Compilation

Add Boost library to the path for compilation by setting `BOOST_DIR` in the makefile. Requires Boost program options module to be compiled for static linking.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in the makefile in order to disable optimization.

Tested with gcc 64-bit 7.2.0 and Boost 1.67.0 (the latter is not time-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

A binary (compiled executable) for Linux is available in the release (file name `sopang`).

## Usage

Basic usage: `./sopang [options] <input text file> <input pattern file>`

Input text file (positional parameter 1 or named parameter `-i` or `--in-text-file`) should contain the elastic-degenerate text in the format `{A,C,}GAAT{AT,A}ATT`.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, each of the same length, separated with newline characters.
Attached as part of this package is a script `run_all.sh`, which allows for processing multiple input text (chromosome) and pattern files.

#### Command-line parameter description

Short name | Long name               | Parameter description
---------- | ----------------------- | ---------------------
`-d`       | `--dump`                | dump input file info and throughput to output file (useful for throughput testing)
`-D`       | `--dump-indexes`        | dump resulting indexes (full results) to stdout
`-h`       | `--help`                | display help message
&nbsp;     | `--help-verbose`        | display verbose help message
`-i`       | `--in-text-file arg`    | input text file path (positional arg 1)
`-I`       | `--in-pattern-file arg` | input pattern file path (positional arg 2)
`-k`       | `--approx`              | perform approximate search (Hamming distance) for k errors (preliminary, max pattern length = 12)
`-o`       | `--out-file arg`        | output file path
`-p`       | `--pattern-count arg`   | maximum number of patterns read from top of the patterns file
`-v`       | `--version`             | display version info

## Testing

Testing on human genome and synthetic data.

1. Download [EDSO](https://github.com/webmasterar/edso/) by Ahmad Retha.

1. Download data from the 1000 Genomes project: ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/.

1. Run EDSO for each chromosome and variants file. Rename resulting files as `chr1.eds`, `chr2.eds`, ..., `chr23.eds`.

1. Generate synthetic data by running `python generate_synth.py` (requires Python 2.7) 4 times for the number of segments (parameter `nSegments`) set to 100, 500, 1000, and 1600 thousands. Rename files as `chr24.eds`, `chr25.eds`, `chr26.eds`, `chr27.eds`.

1. Set parameter `inputDir` in `run_all.sh` to the folder containing `.eds` files and pattern files (all pattern files are located in the `sample/` folder as part of this package).

1. Compile SoPanG (see above).

1. Run `run_all.sh`.

## Compile-time parameter description

#### params.hpp

Parameter name  | Parameter description
--------------- | ---------------------
`alphabet`      | a set of symbols occurring in input (ED) text file or input pattern file

#### sopang.hpp

Parameter name   | Parameter description
---------------- | ---------------------
`dBufferSize`    | buffer size for processing segment variants, the size of the largest segment (i.e., the number of variants) from the input file cannot be larger than this value
`maskBufferSize` | buffer size for shift-or masks for the input alphabet, must be larger than the largest input character ASCII code
`wordSize`       | shift-or word size in bits

## Script parameter description

#### run_all.sh

Parameter name   | Parameter description
---------------- | ---------------------
`inputDir`       | input directory containing `.eds` input ED text files and `.txt` input pattern files
`outFile`        | base name for output files

#### generate_synth.py

Parameter name         | Parameter description
---------------------- | ---------------------
`nSegments`            | total number of segments
`alphabet`             | alphabet for character sampling
`nDegeneratePositions` | number of segments (must be smaller than or equal to `nSegments`) which are degenerate (indeterminate), i.e., contain multiple variants
`nMaxSegmentVariants`  | maximum number of variants (`a`), the number of variants for each degenerate segment will be sampled from the interval `[2, a]`
`nMaxVariantLength`    | maximum length of each segment variant (`b`), the length for each variant will be sampled from the interval `[0, b]` (segments might contain empty words)
`outFile`              | output file path
