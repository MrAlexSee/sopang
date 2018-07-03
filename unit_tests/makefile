CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++11
# Try testing both without and with the optimization.
# OPTFLAGS  	  = -DNDEBUG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"
INCLUDE   = -I$(BOOST_DIR)

TEST_FILES    = catch.hpp repeat.hpp

EXE 	      = main_tests
OBJ           = main_tests.o helpers_tests.o sopang_approx_tests.o sopang_exact_tests.o sopang_parsing_tests.o sopang.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c helpers_tests.cpp

sopang_approx_tests.o: sopang_approx_tests.cpp sopang_whitebox.hpp ../sopang.hpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang_approx_tests.cpp

sopang_exact_tests.o: sopang_exact_tests.cpp sopang_whitebox.hpp ../sopang.hpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang_exact_tests.cpp

sopang_parsing_tests.o: sopang_parsing_tests.cpp ../sopang.hpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c sopang_parsing_tests.cpp

sopang.o: ../sopang.cpp ../sopang.hpp ../helpers.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c ../sopang.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
