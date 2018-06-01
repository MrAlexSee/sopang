CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++11

TEST_FILES    = catch.hpp repeat.hpp

EXE 	      = main_tests
OBJ           = main_tests.o helpers_tests.o sopang_tests.o sopang.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CCFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) -c main_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) -c helpers_tests.cpp

sopang_tests.o: sopang_tests.cpp ../sopang.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) -c sopang_tests.cpp

sopang.o: ../sopang.cpp ../sopang.hpp ../helpers.hpp
	$(CC) $(CCFLAGS) -c ../sopang.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
