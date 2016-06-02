#include "catch.hpp"

/* Include simhash */
#include "hash.hpp"

#define VECTOR_INITIALIZER(data) (data), \
    ((data) + sizeof(data) / sizeof((data)[0]))

namespace {
    using namespace Simhash;

    TEST_CASE("Simhash", "[simhash]") {
        SECTION("Yields 0 for empty collection") {
            Simhash::Simhash hasher;

            REQUIRE(hasher.result() == 0);
        }

        SECTION("Yields expected result for even number of samples") {
            Simhash::Simhash hasher;

            for (hash_t mask = 1; mask < 0x4000000000000000ul; mask = 2 * mask + 1) {
                hasher.update(mask);
            }

            REQUIRE(hasher.result() == 0xFFFFFFFE00000000ul);
        }

        SECTION("Yields expected result for odd number of samples") {
            Simhash::Simhash hasher;

            for (hash_t mask = 0xFFFFFFFFFFFFFFFFul; mask > 1; mask >>= 1) {
                hasher.update(mask);
            }

            REQUIRE(hasher.result() == 0xFFFFFFFF80000000ul);
        }
    }

    class Recorder: public HasherInterface {
    public:
        std::vector<hash_t> samples;

        virtual void update(hash_t hash) /* override */ {
            samples.push_back(hash);
        }

        virtual hash_t result() /* override */ {
            return 0x123ul;
        }
    };

    TEST_CASE("MovingWindowShingler", "[simhash]") {
        SECTION("pushes expected samples to the provided hasher") {
            Recorder recorder;
            MovingWindowShingler hasher(recorder, 2);

            static const hash_t v[] = {
                0x0001, 0x0012, 0x0120,
                0x0200 // this sample is from clearing up window on the tail
            };
            std::vector<hash_t> expected(VECTOR_INITIALIZER(v));

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            hasher.result();

            REQUIRE(recorder.samples == expected);
        }

        SECTION("respects provided window size") {
            Recorder recorder;
            MovingWindowShingler hasher(recorder, 3);

            static const hash_t v[] = {
                0x0001, 0x0012, 0x0124,
                0x0240, 0x0400 // this samples are from clearing up window on the tail
            };
            std::vector<hash_t> expected(VECTOR_INITIALIZER(v));

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            hasher.result();

            REQUIRE(recorder.samples == expected);
        }

        SECTION("returns provided hasher's return") {
            Recorder recorder;
            MovingWindowShingler hasher(recorder, 3);

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            REQUIRE(hasher.result() == 0x123ul);
        }
    }

    TEST_CASE("MovingWindowLegacyShingler", "[simhash]") {
        SECTION("pushes expected samples to the provided hasher") {
            Recorder recorder;
            MovingWindowLegacyShingler hasher(recorder, 2);

            static const hash_t v[] = {0x0001, 0x0012, 0x0120};
            std::vector<hash_t> expected(VECTOR_INITIALIZER(v));

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            hasher.result();

            REQUIRE(recorder.samples == expected);
        }

        SECTION("respects provided window size") {
            Recorder recorder;
            MovingWindowLegacyShingler hasher(recorder, 3);

            static const hash_t v[] = {0x0001, 0x0012, 0x0124};
            std::vector<hash_t> expected(VECTOR_INITIALIZER(v));

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            hasher.result();

            REQUIRE(recorder.samples == expected);
        }

        SECTION("returns provided hasher's return") {
            Recorder recorder;
            MovingWindowLegacyShingler hasher(recorder, 3);

            hasher.update(0x0001);
            hasher.update(0x0010);
            hasher.update(0x0100);

            REQUIRE(hasher.result() == 0x123ul);
        }
    }

    struct PseudoHash {
        uint64_t operator()(const char* data, size_t len, uint64_t seed) {
            return data[0] + 10 * len;
        }
    };

    TEST_CASE("DataHasher", "[simhash]") {
        SECTION("invokes data hasher with proper params, and passes results to hasher") {
            Recorder recorder;
            DataHasher<PseudoHash> hasher(recorder);

            const char a[] = "aaaa";
            const char b[] = "bb";

            hash_t v[] = {'a' + 40, 'b' + 20};
            std::vector<hash_t> expected(VECTOR_INITIALIZER(v));

            hasher.update(a, 4);
            hasher.update(b, 2);
            hasher.result();

            REQUIRE(recorder.samples == expected);
        }

        SECTION("returns provided hasher's return") {
            Recorder recorder;
            DataHasher<PseudoHash> hasher(recorder);

            const char a[] = "aaaa";
            const char b[] = "bb";

            hasher.update(a, 4);
            hasher.update(b, 2);

            REQUIRE(hasher.result() == 0x123ul);
        }
    }

    int bit_count(uint64_t value) {
        int count = 0;
        while (value) {
            value &= value - 1;
            ++count;
        }
        return count;
    }

    TEST_CASE("Simhash integrated test", "[simhash]") {
        std::string jabberwocky = "Twas brillig, and the slithy toves"
            "  Did gyre and gimble in the wabe:"
            "All mimsy were the borogoves,"
            "  And the mome raths outgrabe."
            "Beware the Jabberwock, my son!"
            "  The jaws that bite, the claws that catch!"
            "Beware the Jubjub bird, and shun"
            "  The frumious Bandersnatch!"
            "He took his vorpal sword in hand:"
            "  Long time the manxome foe he sought --"
            "So rested he by the Tumtum tree,"
            "  And stood awhile in thought."
            "And, as in uffish thought he stood,"
            "  The Jabberwock, with eyes of flame,"
            "Came whiffling through the tulgey wood,"
            "  And burbled as it came!"
            "One, two! One, two! And through and through"
            "  The vorpal blade went snicker-snack!"
            "He left it dead, and with its head"
            "  He went galumphing back."
            "And, has thou slain the Jabberwock?"
            "  Come to my arms, my beamish boy!"
            "O frabjous day! Callooh! Callay!'"
            "  He chortled in his joy.";

        std::string pope = "There once was a man named Pope"
            "who loved an oscilloscope."
            "  and the cyclical trace"
            "  of their carnal embrace"
            "had a damned-near-infinite slope";

        SECTION("Can find near-duplicates") {
            /* Let's instantiate a simhasher */
            uint64_t a = hash_text<jenkins, Strspn>(jabberwocky.c_str());
            /* Update jabberwocky to include who wrote it */
            jabberwocky += " - Lewis Carroll 'Alice In Wonderland'";
            uint64_t b = hash_text<jenkins, Strspn>(jabberwocky.c_str());

            /* Now, make sure that the number of bits by which they differ is
             * small */
            size_t count = bit_count(a ^ b);
            REQUIRE(count != 0);
            REQUIRE(count <= 3);

            /* Now, let's make sure that we can verify that two strings are
             * nothing alike */
            count = bit_count(a ^ hash_text<jenkins, Strspn>(pope.c_str()));
            REQUIRE(count > 5);
        }

        SECTION("simhash has specific value") {
            // We cannot change value arbitrary, since Freshscape stores it in HBase
            uint64_t a = hash_text<jenkins, Strspn>(jabberwocky.c_str());

            // The expected value is taken from pre-refactoring implementation.
            REQUIRE(a == 0x041379A587C66880ul);
        }
    }
}
