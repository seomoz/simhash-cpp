CXX          ?= g++
CXXOPTS      ?= -g -Wall -Werror -std=c++11 -Iinclude/
DEBUG_OPTS   ?= -fprofile-arcs -ftest-coverage -O0 -fPIC
RELEASE_OPTS ?= -O3
BINARIES      = release/bin/simhash-find-all release/bin/simhash-find-clusters

all: test release/libsimhash.o $(BINARIES)

# Release libraries
release:
	mkdir -p release

release/libsimhash.o: release/simhash.o release/permutation.o
	ld -r -o $@ $^

release/%.o: src/%.cpp include/%.h release
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ -c $<

release/bin/%: src/bin/%.cpp release/libsimhash.o
	mkdir -p release/bin
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ $^

# Debug libraries
debug:
	mkdir -p debug

debug/libsimhash.o: debug/simhash.o debug/permutation.o
	ld -r -o $@ $^

debug/%.o: src/%.cpp include/%.h debug
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

debug/bin/%: src/bin/%.cpp debug/libsimhash.o
	mkdir -p debug/bin
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ $^

test/%.o: test/%.cpp
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

# Tests
test-all: test/test-all.o test/test-simhash.o test/test-permutation.o debug/libsimhash.o
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ $^ -lgtest -lpthread

.PHONY: test
test: test-all
	./test-all
	./scripts/check-coverage.sh $(PWD)

clean:
	rm -rf debug release test-all bench
