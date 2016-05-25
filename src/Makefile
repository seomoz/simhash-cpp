CPP     = g++
CPPOPTS = -g -O3 -Wall
DEPS    = hashes/* tokenizers/* hash.hpp simhash.h simhash.cpp

all: bench hash test

bench: bench.cpp simhash.o hashes/* tokenizers/*
	$(CPP) $(CPPOPTS) -o $@ $< simhash.o -lJudy

bench_simhash: bench_simhash.cpp
	$(CPP) $(CPPOPTS) -o $@ $<
	./$@

hash: hash.cpp $(DEPS)
	$(CPP) $(CPPOPTS) -o $@ $<

simhash.o: simhash.cpp simhash.h
	$(CPP) $(CPPOPTS) -o $@ -c $<

test_catch: *_test.cpp test_catch_config.o simhash.o $(DEPS)
	$(CPP) $(CPPOPTS) -o $@ *_test.cpp -Ideps/Catch/single_include test_catch_config.o simhash.o -lJudy

# speed up tests (compile Catch as a separate rule)
test_catch_config.o: test_catch_config.cpp
	$(CPP) $(CPPOPTS) -o $@ -c $< -Ideps/Catch/single_include

test: test_catch
	./$<

clean:
	rm -rf bench hash test_catch *.o
