#include "catch.hpp"

/* Include simhash */
#include "hash.hpp"

static int bit_count(uint64_t value) {
    int count = 0;
    while (value) {
        value &= value - 1;
        ++count;
    }
    return count;
}

TEST_CASE("We can perform simhash", "[simhash]") {
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
        Simhash::Simhash<> hasher;
        uint64_t a = hasher.hash_tokenizer(jabberwocky.c_str(), Simhash::Strspn());
        /* Update jabberwocky to include who wrote it */
        jabberwocky += " - Lewis Carroll 'Alice In Wonderland'";
        uint64_t b = hasher.hash_tokenizer(jabberwocky.c_str(), Simhash::Strspn());

        /* Now, make sure that the number of bits by which they differ is
         * small */
        size_t count = bit_count(a ^ b);
        REQUIRE(count != 0);
        REQUIRE(count <= 3);

        /* Now, let's make sure that we can verify that two strings are
         * nothing alike */
        count = bit_count(a ^ hasher.hash_tokenizer(pope.c_str(), Simhash::Strspn()));
        REQUIRE(count > 5);
    }

    SECTION("simhash has specific value") {
        // We cannot change value arbitrary, since Freshscape stores it in HBase
        Simhash::Simhash<> hasher;
        uint64_t a = hasher.hash_tokenizer(jabberwocky.c_str(), Simhash::Strspn());

        // The expected value is taken from pre-refactoring implementation.
        REQUIRE(a == 0x041379A587C66880ul);
    }
}
