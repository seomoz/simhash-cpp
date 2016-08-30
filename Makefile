CPP          = g++
CPPOPTS      = -g -Wall -Werror -std=c++11 -Iinclude/
DEBUG_OPTS   = -fprofile-arcs -ftest-coverage -O0 -g -fPIC
RELEASE_OPTS = -O3

all: test

# Release libraries
release/libsimhash.o: release/simhash.o
	mkdir -p release
	ld -r -o $@ $^

release/%.o: src/%.cpp include/%.h
	mkdir -p release
	$(CPP) $(CPPOPTS) $(RELEASE_OPTS) -o $@ -c $<

# Debug libraries
debug/libsimhash.o: debug/simhash.o
	mkdir -p debug
	ld -r -o $@ $^

debug/%.o: src/%.cpp include/%.h
	mkdir -p debug
	$(CPP) $(CPPOPTS) $(DEBUG_OPTS) -o $@ -c $<

test/%.o: test/%.cpp
	$(CPP) $(CPPOPTS) $(DEBUG_OPTS) -o $@ -c $<

# Tests
test-all: test/test-all.o test/test-simhash.o debug/libsimhash.o
	$(CPP) $(CPPOPTS) $(DEBUG_OPTS) -o $@ $^ -lgtest -lpthread

.PHONY: test
test: test-all
	./test-all

clean:
	find . -name '*.o' -o -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \
		| xargs --no-run-if-empty rm
	rm -f test-all bench
