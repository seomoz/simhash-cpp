#include "catch.hpp"

/* Include internals */
#include "tokenizers/strspn.h"

TEST_CASE("We can get tokens from a string", "[tokenizers]") {
    /* This is a string that we'd like to use for testing the tokenizer */
    const char* test_string = "what's new?How5is _ the ^# stuff";

    SECTION("We can use the strspn tokenizer") {
        /* We need an instance of our actual tokenizer */
        Simhash::Strspn tokenizer;

        const char *current(NULL);
        const char *next(NULL);
        current = test_string;

        int64_t expected[] = {
            4, // 'what'
            1, // 's'
            3, // 'new'
            3, // 'How'
            2, // 'is'
            0, // '_'
            0, // ' '
            3, // 'the'
            0, // '^'
            0, // '#'
            0, // ' '
            5, // 'stuff'
        };

        for (size_t i = 0; i < 12; ++i) {
            next = tokenizer(current);
            REQUIRE((next - current) == expected[i]);
            current = next + (*next != '\0');
        }

        /* Now we should be at the end */
        next = tokenizer(current);
        REQUIRE((next == NULL));
    }
}
