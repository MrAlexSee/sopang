CC         = g++
CCFLAGS    = -Wall -pedantic -std=c++17

BOOST_DIR  = "/home/alex/boost_1_67_0"
INCLUDE    = -I$(BOOST_DIR)

TEST_FILES = catch.hpp repeat.hpp

EXE 	   = main_tests
OBJ        = main_tests.o bitset_tests.o helpers_tests.o parsing_tests.o sopang_approx_tests.o sopang_exact_tests.o sopang_sources_tests.o parsing.o sopang.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(INCLUDE) -c main_tests.cpp

bitset_tests.o: bitset_tests.cpp ../bitset.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c bitset_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c helpers_tests.cpp

parsing_tests.o: parsing_tests.cpp ../parsing.hpp ../sopang.hpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c parsing_tests.cpp

sopang_approx_tests.o: sopang_approx_tests.cpp sopang_whitebox.hpp ../sopang.hpp ../helpers.hpp ../parsing.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c sopang_approx_tests.cpp

sopang_exact_tests.o: sopang_exact_tests.cpp sopang_whitebox.hpp ../sopang.hpp ../helpers.hpp ../parsing.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c sopang_exact_tests.cpp

sopang_sources_tests.o: sopang_sources_tests.cpp ../sopang.hpp ../parsing.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(INCLUDE) -c sopang_sources_tests.cpp

parsing.o: ../parsing.cpp ../parsing.hpp ../helpers.hpp ../sopang.hpp ../bitset.hpp
	$(CC) $(CCFLAGS) $(INCLUDE) -c ../parsing.cpp

sopang.o: ../sopang.cpp ../sopang.hpp ../bitset.hpp
	$(CC) $(CCFLAGS) $(INCLUDE) -c ../sopang.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
