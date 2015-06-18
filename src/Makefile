CPP     = g++
CPPOPTS = -g -O3 -Wall
DEPS    = hashes/* tokenizers/* hash.hpp simhash.h simhash.cpp

all: bench hash test

bench: simhash.o bench.cpp hashes/* tokenizers/*
	$(CPP) $(CPPOPTS) -o bench bench.cpp simhash.o -lJudy

hash: hash.cpp $(DEPS)
	$(CPP) $(CPPOPTS) -o hash hash.cpp

simhash.o: simhash.h simhash.cpp
	$(CPP) $(CPPOPTS) -o simhash.o -c simhash.cpp

test: test.cpp simhash.o $(DEPS)
	$(CPP) $(CPPOPTS) -o test test.cpp -Ideps/Catch/single_include simhash.o -lJudy

clean:
	rm -rf driver test *.o
