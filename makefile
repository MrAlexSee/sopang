CC        = g++
EXE		  = sopang
CCFLAGS   = -Wall -pedantic -std=c++11
OPTFLAGS  = -DNDEBUG -O3

INCLUDE   = -I$(BOOST_DIR)
BOOST_DIR = "/home/alex/boost_1_65_1"

all: $(EXE)

$(EXE): main.o sopang.o
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) main.o sopang.o -o $@

main.o: main.cpp params.hpp helpers.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main.cpp

sopang.o: sopang.cpp sopang.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang.cpp

.PHONY: clean

clean:
	rm -f main.o sopang.o $(EXE)

rebuild: clean all
