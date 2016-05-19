#include "catch.hpp"

#include <string.h>

/* Include internals */
#include "tokenizers/strspn.h"

void test_tokenizer(const char* string, const char* expected_tokens[]) {
    Simhash::Strspn tokenizer;
    Simhash::StringToken token;

    const char** expected = expected_tokens;
    for (tokenizer(string, token)
        ; token.length != 0
        ; tokenizer(token.start + token.length, token)) {
        REQUIRE(((*expected) != NULL));
        REQUIRE((token.start != NULL));
        REQUIRE((token.length == strlen(*expected)));
        REQUIRE((strncmp(token.start, *expected, token.length)== 0));
        ++expected;
    }
    REQUIRE((token.start == NULL));
    REQUIRE((token.length == 0));
}

TEST_CASE("We can get tokens from a string", "[tokenizers]") {
    SECTION("We can tokenize an empty string") {
        const char* string = "";
        const char* expected_tokens[] = {NULL};

        test_tokenizer(string, expected_tokens);
    }

    SECTION("We can tokenize long string") {
        /* This is a string that we'd like to use for testing the tokenizer */
        const char* string = "what's new?How5is _ the ^# stuff";
        const char* expected_tokens[] = {
            "what", "s", "new", "How", "is", "the", "stuff", NULL
        };

        test_tokenizer(string, expected_tokens);
    }

    SECTION("We can tokenize string that starts with delimiters") {
        /* This is a string that we'd like to use for testing the tokenizer */
        const char* string = "@#$@$! what's new?How5is _ the ^# stuff";
        const char* expected_tokens[] = {
            "what", "s", "new", "How", "is", "the", "stuff", NULL
        };

        test_tokenizer(string, expected_tokens);
    }

    SECTION("We can tokenize string that ends with delimiters") {
        /* This is a string that we'd like to use for testing the tokenizer */
        const char* string = "what's new?How5is _ the ^# stuff !~@#!@$";
        const char* expected_tokens[] = {
            "what", "s", "new", "How", "is", "the", "stuff", NULL
        };

        test_tokenizer(string, expected_tokens);
    }

    SECTION("We do not overstep end-of-string") {
        /* This is a string that we'd like to use for testing the tokenizer */
        const char* string = "a b c\0d e f\0\0";
        const char* expected_tokens[] = {
            "a", "b", "c", NULL
        };

        test_tokenizer(string, expected_tokens);
    }
}
