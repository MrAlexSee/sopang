CC        = g++
CCFLAGS   = -Wall -pedantic -std=c++11
OPTFLAGS  = -DNDEBUG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"
ZSTD_DIR = "/home/alex/zstd/lib"

INCLUDE   = -I$(BOOST_DIR) -I$(ZSTD_DIR)
LDFLAGS   = -L$(BOOST_DIR) -L$(ZSTD_DIR) -static
LDLIBS    = -lboost_program_options -lzstd -lm

EXE       = sopang
OBJ       = main.o sopang.o zstd_helper.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

main.o: main.cpp helpers.hpp params.hpp sopang.hpp zstd_helper.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main.cpp

sopang.o: sopang.cpp sopang.hpp bitset.hpp helpers.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang.cpp

zstd_helper.o: zstd_helper.cpp zstd_helper.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c zstd_helper.cpp

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
