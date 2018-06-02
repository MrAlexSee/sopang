CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++11

# Try testing both without and with optimization.
# OPTFLAGS  	  = -DNDEBUG -O3

TEST_FILES    = catch.hpp repeat.hpp

EXE 	      = main_tests
OBJ           = main_tests.o helpers_tests.o sopang_tests.o sopang.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c main_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c helpers_tests.cpp

sopang_tests.o: sopang_tests.cpp ../sopang.hpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c sopang_tests.cpp

sopang.o: ../sopang.cpp ../sopang.hpp ../helpers.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) -c ../sopang.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
