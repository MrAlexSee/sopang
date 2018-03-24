CC        = g++
EXE		  = sopang
CCFLAGS   = -Wall -pedantic -std=c++11
OPTFLAGS  = -DNDEBUG -O3

INCLUDE   = -I$(BOOST_DIR)
BOOST_DIR = "/home/alex/boost_1_65_1"

all: $(EXE)

$(EXE):
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) main.cpp sopang.cpp -o $@

.PHONY: clean

clean:
	rm -f $(EXE)

rebuild: clean all
