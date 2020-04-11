## Basic information

This software is called **SOPanG** (Shift-Or for Pan-Genome).
It can be used for matching patterns in elastic-degenerate (ED) text (simplified pan-genome model).
Authors for the current release version: Aleksander Cisłak, Szymon Grabowski.
Authors of the SOPanG algorithm: Aleksander Cisłak, Szymon Grabowski, Jan Holub.

* Published as an applications note entitled *SOPanG: online text searching over a pan-genome* (Cisłak, Grabowski, Holub), Bioinformatics, Vol. 34, Issue 24, 12/2018, pp. 4290-4292. DOI link: https://doi.org/10.1093/bioinformatics/bty506
* For the second version (follow-up work) see arXiv preprint entitled *SOPanG 2: online searching over a pan-genome without false positives* (Cisłak and Grabowski). DOI link:

ED text is in the following format: `{A,C,}GAAT{AT,A}ATT`.
Braces determine the start and end of each non-deterministic segment (a segment having multiple variants), and commas delimit segment variants.
If a comma is not preceded by a string of letters or it is a trailing symbol in a segment, it indicates an empty word.
To give an example, all three notations: `{,A,C}`, `{A,,C}`, and `{A,C,}` mean the same, which is a segment which accepts either a string `A`, or a string `C`, or an empty word.
Deterministic segments (i.e. segments having a single variant) are stored as regular contiguous strings.
Note that, e.g., `{AC,CG}` and `{AC, CG}` are not the same (the latter would expect a space in its second variant).
Therefore, you should not use whitespace characters in the ED text if not intended.
For more information regarding this format, consult, e.g., *Efficient pattern matching in elastic-degenerate texts* (Iliopoulos et al., 2017).

SOPanG returns the end positions of pattern occurrences in the ED text.
More precisely, it returns the set of segment indexes in which pattern occurrences end (without possible duplicates).

Below you can see the pattern search speed for individual human chromosomes (1-22) for matching with and without sources (see the following sections for more information).
Pattern sizes of 8, 16, 32, and 64 characters were tested, without any noticeable differences between them.
1 MB = 10^6 B, throughput measured with regard to the ED string size (delimiters not included).
Intel i7-4930K@3.4 GHz, 64 GB DDR3 RAM.

<p align="center">
  <img src="pics/performance.png?raw=true" alt="Performance diagram" width="650"/>
</p>

#### Sources

The elastic-degenerate format which, in its original form, might contain paths not appearing in any genome can be extended with the sources file.
Sources describe which individuals (strains) are associated with a given variant from the ED text.
This allows us to report only true positive matches, i.e. omit matches not occurring in any of the individuals.

For instance, for 4 individuals, we might have the elastic-degenerate text `AA{AG,G}{CG,N,TT}` and the corresponding sources text `{0}{{0,2}{3}}`.
Note that the sources are 0-indexed.
We assume that the last variant of each segment (in the above example: `G` and `TT`, respectively) describes the reference sequence.
This indicates the following.

* For the 1st non-deterministic segment, `AG` is associated with source `0`, and `G` (the reference sequence) is associated with the remaining sources `1`, `2` and `3`.

* For the 2nd non-deterministic segment, `CG` is associated with sources `0` and `2`, `N` with source `3`, and `TT` (the reference sequence) is associated with the remaining source `1`.

In every segment, each variant is associated with some sources. Moreover, each source must be associated with exactly one variant from every segment.
The proposed human-readable format of the sources data was defined in such a way as to be consistent with the ED text, that is, it also consists of curly braces and commas as delimiters.

The sources file should have two lines in total.

* The first line contains the source count, e.g., 10 if there are 10 individuals.
* The second line contains a contiguous string with the data described above.

We also offer a compressed sources format, which is not human-readable.
It is recommended for the practical use, as the resulting files are expected to be roughly an order of magnitude smaller.
It is based on variable-length differential coding and the use of [zstd](https://github.com/facebook/zstd) compression library.

We choose the following naming convention: `.eds` for ED text and `.edss` for the corresponding sources file, or `.edz` and `.edsz` for compressed versions of these files.
In order to use SOPanG for matching with sources, supply the path to the sources file in the format described above via the parameter `-S` (see below for more information regarding the usage).

## Compilation

Add Boost and zstd libraries to the path for compilation by setting `BOOST_DIR` and `ZSTD_DIR` in the makefile.
Requires Boost program options module and zstd to be compiled for static linking.
Requires support for the C++17 standard.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in the makefile in order to disable optimization.

Tested with gcc 64-bit 7.4.0 and Boost 1.67.0 (the latter is not performance-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

A binary (compiled executable) for Linux is available in the release (file name `sopang`).

## Usage

Basic usage: `./sopang [options] <input text file> <input pattern file>`

Input text file (positional parameter 1 or named parameter `-i` or `--in-text-file`) should contain the elastic-degenerate text in the format `{A,C,}GAAT{AT,A}ATT`.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, each of the same length, separated with newline characters.

* End-to-end tests are located in the `end_to_end_tests` folder and they can be run using the `run_tests.sh` script in that folder.

* Performance testing and data generation tools are located in the `performance_tests` folder, see below for details.

* Python prototypes are located in the `prototypes` folder.

* Additional tools are located in the `scripts` folder, see below for details.

* Unit tests are located in the `unit_tests` folder and they can be run by issuing the `make run` command in that folder.

#### Command-line parameter description

Short name | Long name               | Parameter description
---------- | ----------------------- | ---------------------
`-d`       | `--dump`                | dump input file info and throughput to output file (useful for throughput testing)
`-D`       | `--dump-indexes`        | dump resulting indexes (full results) to stdout
&nbsp;     | `--full-sources-output` | when matching with sources, return all matching source (strain) indexes rather than only verify if the match is correct
`-h`       | `--help`                | display help message
&nbsp;     | `--help-verbose`        | display verbose help message
`-i`       | `--in-text-file arg`    | input text file path (positional arg 1)
`-I`       | `--in-pattern-file arg` | input pattern file path (positional arg 2)
`-S`       | `--in-sources-file arg` | input sources file path
&nbsp;     | `--in-compressed`       | parse compressed input text or sources file
`-k`       | `--approx arg`          | perform approximate search (Hamming distance) for k errors (preliminary, max pattern length = 12, not compatible with matching with sources)
`-o`       | `--out-file arg`        | output file path (default = timings.txt)
`-p`       | `--pattern-count arg`   | maximum number of patterns read from top of the patterns file (non-positive values are ignored)
`-v`       | `--version`             | display version info

## Compile-time parameter description

#### params.hpp

Parameter name   | Parameter description
---------------- | ---------------------
`alphabet`       | A set of symbols occurring in input (ED) text file or input pattern file.

#### sopang.hpp

Parameter name         | Parameter description
---------------------- | ---------------------
`dBufferSize`          | Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants) from the input file cannot be larger than this value.
`maskBufferSize`       | Buffer size for Shift-Or masks for the input alphabet, must be larger than the largest input character ASCII code.
`matchMapReserveSize`  | Initial memory reserve size for a map storing matches for verification with sources.
`maxPatternApproxSize` | Maximum pattern size for approximate search.
`maxSourceCount`       | Maximum number of sources (upper bound on source set size).
`saCounterSize`        | Shift-Add counter size in bits.
`wordSize`             | Word size (in bits) used by the Shift-Or algorithm.

## Testing

Testing can be automated with the use of scripts from the `performance_tests` folder.

Script name                     | Description
------------------------------- | ------------------------------
`download_genome_data.sh`       | Downloads 1000 Genomes Project data.
`generate_real_source_data.sh`  | Generates compressed ED-text and source files for the reference .fasta file and a set of .vcf variant files.
`generate_synthetic_data.sh`    | Generates synthetic ED-text and source files for various output file sizes.
`run_all_real_data.sh`          | Performs experiments for regular ED-text matching and ED-text matching with sources on real-world data.
`run_all_synthetic_data.sh`     | Performs experiments for regular ED-text matching and ED-text matching with sources on synthetic data.

A complete testing procedure including data generation is as follows.

1. Run `generate_synthetic_data.sh`.

1. Download data from the 1000 Genomes Project.

    * Reference file: ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/technical/reference/phase2_reference_assembly_sequence/hs37d5.fa.gz
    * Corresponding variant files: ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ (*.vcf.gz files)

1. Unpack the files. Move the reference file `hs37d5.fa` to the `data` folder. Move all .vcf files to the `data` folder, rename them as `chr1.vcf`, `chr2.vcf`, ..., `chr24.vcf`. The above 2 points can be automated with the `download_genome_data.sh` script, however, note that it would download and unpack a huge amount of data in a single run.

1. Run `generate_real_source_data.sh`. For all chromosomes, this will take a very long time. However, it only needs to be performed once in order to obtain elastic-degenerate data with sources. Instead of generating the data on your own, it is possible to download `.edz` and `.edsz` files for chromosomes 1-22 here: http://szgrabowski.kis.p.lodz.pl/ed/ed_chr1-22.zip.

1. Compile SoPanG (see above) or download a compiled version from the latest release (please note though that it is compiled using default settings).

1. Run `run_all_real_data.sh` and `run_all_synthetic_data.sh`.

## Scripts

All Python (Python 3, tested with Python 3.6.8) scripts are located in the `scripts` folder.
Their command line parameters are described as docopt interfaces at the top of each file.

Script name                         | Description
----------------------------------- | ------------------------------
`ed_histogram.py`                   | Prints the histogram of character counts in deterministic/non-deterministic segments.
`generate_synth_ed_text.py`         | Generates synthetic elastic-degenerate text.
`generate_synth_sources.py`         | Generates a synthetic sources file for elastic-degenerate text.
`parse_fasta_vcf.py`                | Parses a .fasta and .vcf file pair in order to obtain elastic-degenerate text with sources.

The scripts folder also contains a C++ program in the `parse_fasta_vcf_cpp` folder.
It offers roughly the same functionality as the `parse_fasta_vcf.py` script but dumps only the compressed output.
The C++ version is faster and consumes less memory, however, it needs to be compiled.
