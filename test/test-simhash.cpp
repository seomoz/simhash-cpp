#include <gtest/gtest.h>

#include "simhash.h"

TEST(NumDifferingBitsTest, Basic)
{
    Simhash::hash_t a = 0xDEADBEEF;
    Simhash::hash_t b = 0xDEADBEAD;
    size_t expected = 2;
    EXPECT_EQ(expected, Simhash::num_differing_bits(a, b));
}

TEST(SimhashTest, Empty)
{
    std::vector<Simhash::hash_t> hashes = {};
    Simhash::hash_t expected = 0;
    EXPECT_EQ(expected, Simhash::compute(hashes));
}

TEST(SimhashTest, Repeat)
{
    std::vector<Simhash::hash_t> hashes(100, 0xDEADBEEF);
    Simhash::hash_t expected = 0xDEADBEEF;
    EXPECT_EQ(expected, Simhash::compute(hashes));
}

TEST(SimhashTest, Inverse)
{
    std::vector<Simhash::hash_t> hashes = {
        0xDEADBEEF, ~0xDEADBEEF
    };
    Simhash::hash_t expected = 0;
    EXPECT_EQ(expected, Simhash::compute(hashes));
}

TEST(SimhashTest, Basic)
{
    std::vector<Simhash::hash_t> hashes = {
        0xABCD, 0xBCDE, 0xCDEF
    };
    Simhash::hash_t expected = 0xADCF;
    EXPECT_EQ(expected, Simhash::compute(hashes));
}
