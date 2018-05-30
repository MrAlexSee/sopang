CC        = g++
CCFLAGS   = -Wall -pedantic -std=c++11

HELPERS_TESTS = helpers_tests

all: $(HELPERS_TESTS)

$(HELPERS_TESTS): helpers_tests.cpp catch.hpp repeat.hpp
	$(CC) $(CCFLAGS) helpers_tests.cpp -o $@

.PHONY: clean

run: all
	./$(HELPERS_TESTS)

clean:
	rm -f $(HELPERS_TESTS)

rebuild: clean all
