CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++11

EXE 	      = main_tests
TEST_FILES    = catch.hpp repeat.hpp

all: $(EXE)

$(EXE): main_tests.o helpers_tests.o #sopang_tests.o
	$(CC) $(CCFLAGS) $^ -o $@

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) -c main_tests.cpp

helpers_tests.o: helpers_tests.cpp ../helpers.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) -c helpers_tests.cpp

# sopang_tests.o: sopang_tests.cpp ../sopang.hpp ../sopang.cpp $(TEST_FILES)
	# $(CC) $(CCFLAGS) -c sopang_tests.cpp ../sopang.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f main_tests.o helpers_tests.o sopang_tests.o sopang.o $(EXE)

rebuild: clean all
