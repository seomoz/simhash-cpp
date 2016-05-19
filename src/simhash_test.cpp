#include "catch.hpp"

/* Include internals */
#include "simhash.h"

TEST_CASE("const_iterator_t", "[iterator]") {
    void *judy = NULL;
    uint64_t values[] = {1, 5, 10, 20};
    const int values_length = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < values_length; ++i) {
        uint64_t result = 0;
        uint64_t index = values[i];
        J1S(result, judy, index);
    }

    SECTION("default constructed should be equal") {
        Simhash::const_iterator_t default1;
        Simhash::const_iterator_t default2;
        REQUIRE(default1 == default2);
    }

    SECTION("default constructed should equal initial empty") {
        Simhash::const_iterator_t empty(NULL, 0);
        REQUIRE(Simhash::const_iterator_t() == empty);
    }

    SECTION("default constructed should not equal initial non-empty") {
        Simhash::const_iterator_t initial(judy, 0);
        REQUIRE(Simhash::const_iterator_t() != initial);
      }

    SECTION("preincremented should not equal initial non-empty") {
        Simhash::const_iterator_t initial(judy, 0);
        Simhash::const_iterator_t incremented(judy, 0);
        ++incremented;
        REQUIRE(initial != incremented);
      }

    SECTION("iterators for different trees should not be equal") {
        void *other = NULL;
        uint64_t result = 0;
        uint64_t index = 1;
        J1S(result, other, index);
        REQUIRE(Simhash::const_iterator_t(judy, 0) !=
            Simhash::const_iterator_t(other, 0));
    }

    SECTION("dereference produces right value") {
        Simhash::const_iterator_t initial(judy, 0);
        REQUIRE(*initial == 1);
    }

    SECTION("preincrement followed by dereference produces right value") {
        Simhash::const_iterator_t incremented(judy, 0);
        ++incremented;
        REQUIRE(*incremented == 5);
    }

    SECTION("postincrement followed by dereference produces right value") {
        Simhash::const_iterator_t incremented(judy, 0);
        incremented++;
        REQUIRE(*incremented == 5);
    }

    SECTION("preincremented advances before dereference") {
        Simhash::const_iterator_t incremented(judy, 0);
        REQUIRE(*++incremented == 5);
        REQUIRE(*incremented == 5);
    }

    SECTION("postincrement advances after dereference") {
        Simhash::const_iterator_t incremented(judy, 0);
        REQUIRE(*incremented++ == 1);
        REQUIRE(*incremented == 5);
    }

    SECTION("can iterate over sequence") {
        Simhash::const_iterator_t begin(judy, 0);
        Simhash::const_iterator_t end;
        int i = 0;
        for (Simhash::const_iterator_t begin(judy, 0); begin != end; ++begin, ++i) {
            REQUIRE(i < values_length);
            REQUIRE(*begin == values[i]);
        }
        REQUIRE(i == values_length);
      }

    SECTION("copy constructor should preserve position") {
        Simhash::const_iterator_t incremented(judy, 0);
        ++incremented;
        Simhash::const_iterator_t copied(incremented);
        REQUIRE(*copied == *incremented);
    }

    SECTION("predecrement should move back before dereference") {
        Simhash::const_iterator_t twice_incremented(judy, 0);
        ++twice_incremented;
        ++twice_incremented;
        REQUIRE(*--twice_incremented == 5);
        REQUIRE(*twice_incremented == 5);
    }

    SECTION("postdecrement should move back after dereference") {
        Simhash::const_iterator_t twice_incremented(judy, 0);
        ++twice_incremented;
        ++twice_incremented;
        REQUIRE(*twice_incremented-- == 10);
        REQUIRE(*twice_incremented == 5);
    }

    SECTION("decrement followed by increment should leave you where you started") {
        Simhash::const_iterator_t iter(judy, 0);
        ++iter;
        --iter;
        ++iter;
        REQUIRE(*iter == 5);
    }
}
