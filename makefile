CC        = g++
EXE       = sopang
CCFLAGS   = -Wall -pedantic -std=c++11
OPTFLAGS  = -DNDEBUG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"

INCLUDE   = -I$(BOOST_DIR)
LDFLAGS   = -L$(BOOST_DIR)
LDLIBS    = -lboost_program_options -lm

all: $(EXE)

$(EXE): main.o sopang.o
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) $(LDFLAGS) $^ -o $@ $(LDLIBS)

main.o: main.cpp helpers.hpp params.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main.cpp

sopang.o: sopang.cpp sopang.hpp helpers.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang.cpp

.PHONY: clean

clean:
	rm -f main.o sopang.o $(EXE)

rebuild: clean all
