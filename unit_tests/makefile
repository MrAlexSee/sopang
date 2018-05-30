CC            = g++
CCFLAGS       = -Wall -pedantic -std=c++11

CATCH_FILES   = catch.hpp

HELPERS_TESTS = helpers_tests

all: $(HELPERS_TESTS)

$(HELPERS_TESTS): helpers_tests.cpp ../helpers.hpp repeat.hpp $(CATCH_FILES)
	$(CC) $(CCFLAGS) helpers_tests.cpp -o $@

run: all
	./$(HELPERS_TESTS)

.PHONY: clean

clean:
	rm -f $(HELPERS_TESTS)

rebuild: clean all
