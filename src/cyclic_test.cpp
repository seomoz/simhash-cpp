#include "catch.hpp"

/* Include internals */
#include "cyclic.hpp"

TEST_CASE("The cyclic hashing function stuff", "[cyclic]") {
    SECTION("It can rotate integers by one bit") {
        /* Given these inputs... */
        uint64_t inputs[] = {
            0xDEADBEEFDEADBEEF, 0xAAAAAAAAAAAAAAAA, 0x5555555555555555
        };
        /* We should get these outputs */
        uint64_t outputs[] = {
            0xBD5B7DDFBD5B7DDF, 0x5555555555555555, 0xAAAAAAAAAAAAAAAA
        };
        /* How many examples do I have? */
        size_t count = 3;

        for (size_t i = 0; i < count; ++i) {
            REQUIRE(
                Simhash::Cyclic<uint64_t>::rotate(inputs[i], 1) == outputs[i]);
        }
    }

    SECTION("It can rotate by many bits") {
        REQUIRE(Simhash::Cyclic<uint64_t>::rotate(0xDEADBEEFDEADBEEF, 10)
            == 0xB6FBBF7AB6FBBF7A);
        REQUIRE(Simhash::Cyclic<uint64_t>::rotate(0xABCDEFFFABCDEFFF, 32)
            == 0xABCDEFFFABCDEFFF);
        REQUIRE(Simhash::Cyclic<uint64_t>::rotate(0xABCDEFFFABCDEFFF, 64)
            == 0xABCDEFFFABCDEFFF);
        /* Really large offsets should be rounded down and treated as ok */
        REQUIRE(Simhash::Cyclic<uint64_t>::rotate(0xABCDEFFFABCDEFFF, 1024)
            == 0xABCDEFFFABCDEFFF);

        for (size_t count = 0; count <= 1024; ++count) {
            REQUIRE(Simhash::Cyclic<uint64_t>::rotate(1ul, count)
                == 1ul << (count % 64));
        }
    }

    SECTION("It can keep track of a cyclic hash correctly") {
        /* First, we'll instantiate a cyclic hasher with a window of 5 */
        Simhash::Cyclic<uint64_t> cyclic(5);
        /* Next, we're going to give this hash a sequence of longer than the
         * window, and then we'd like to make sure that the hashes that come
         * up are in fact different, and furthermore, that repeated sequences
         * that are sufficiently long will yield the same values */
        uint64_t values[] = {
            0xCAFEBABECAFEBABE,
            0xAAAAAAAAAAAAAAAA,
            0x5555555555555555,
            0xDEADBEEFDEADBEEF,
            0xFEEDEDFEEDEDFEED,
            0xBD5B7DDFBD5B7DDF,
            0xABEDABEDABEDABED,
            0xBADEBADEBADEBADE
        };
        const size_t count = 8;
        /* Now, we'll push each of these onto the cyclic hasher twice. The
         * first pass through, we expect the numbers to be different from the
         * second pass for the first four numbers. After that point, though,
         * the window should have sufficient time to reset */
        uint64_t results[2*count];
        for (size_t i = 0; i < 2; ++i) {
            for (size_t j = 0; j < count; ++j) {
                results[i*count+j] = cyclic.push(values[j]);
            }
        }
        REQUIRE(results[0] != results[ 8]); /* Unequal */
        REQUIRE(results[1] != results[ 9]);
        REQUIRE(results[2] != results[10]);
        REQUIRE(results[3] != results[11]);
        REQUIRE(results[4] == results[12]); /* Equal */
        REQUIRE(results[5] == results[13]);
        REQUIRE(results[6] == results[14]);
        REQUIRE(results[7] == results[15]);
    }
}
