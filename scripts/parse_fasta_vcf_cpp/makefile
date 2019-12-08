CC        	   = g++
CCFLAGS   	   = -Wall -pedantic -std=c++17
# OPTFLAGS     = -DNDEBUG -O3

VCFLIB_DIR     = /home/alex/vcflib

VCFLIB_INCLUDE = -I$(VCFLIB_DIR)/include -I$(VCFLIB_DIR)/tabixpp/htslib
VCFLIB_LDFLAGS = -L$(VCFLIB_DIR)/tabixpp/htslib -L$(VCFLIB_DIR)/lib
VCFLIB_LDLIBS  = -lvcflib -lhts -lz -lm -llzma -lbz2 -pthread

INCLUDE        = $(VCFLIB_INCLUDE)
LDFLAGS        = $(VCFLIB_LDFLAGS)
LDLIBS	  	   = $(VCFLIB_LDLIBS)

EXE       	   = parse_fasta_vcf
OBJ       	   = parse_fasta_vcf.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

parse_fasta_vcf.o: parse_fasta_vcf.cpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c parse_fasta_vcf.cpp

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all