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

TEST(SimhashTest, FindAll)
{
    std::unordered_set<Simhash::hash_t> hashes = {
        0x000000FF, 0x000000EF, 0x000000EE, 0x000000CE, 0x00000033,
        0x0000FF00, 0x0000EF00, 0x0000EE00, 0x0000CE00, 0x00003300,
        0x00FF0000, 0x00EF0000, 0x00EE0000, 0x00CE0000, 0x00330000,
        0xFF000000, 0xEF000000, 0xEE000000, 0xCE000000, 0x33000000
    };
    Simhash::matches_t expected = {
        { 0x000000EF, 0x000000FF },
        { 0x000000EE, 0x000000EF },
        { 0x000000EE, 0x000000FF },
        { 0x000000CE, 0x000000EE },
        { 0x000000CE, 0x000000EF },
        { 0x000000CE, 0x000000FF },
        { 0x0000EF00, 0x0000FF00 },
        { 0x0000EE00, 0x0000EF00 },
        { 0x0000EE00, 0x0000FF00 },
        { 0x0000CE00, 0x0000EE00 },
        { 0x0000CE00, 0x0000EF00 },
        { 0x0000CE00, 0x0000FF00 },
        { 0x00EF0000, 0x00FF0000 },
        { 0x00EE0000, 0x00EF0000 },
        { 0x00EE0000, 0x00FF0000 },
        { 0x00CE0000, 0x00EE0000 },
        { 0x00CE0000, 0x00EF0000 },
        { 0x00CE0000, 0x00FF0000 },
        { 0xEF000000, 0xFF000000 },
        { 0xEE000000, 0xEF000000 },
        { 0xEE000000, 0xFF000000 },
        { 0xCE000000, 0xEE000000 },
        { 0xCE000000, 0xEF000000 },
        { 0xCE000000, 0xFF000000 }
    };

    for (size_t blocks = 4; blocks < 10; ++blocks)
    {
        EXPECT_EQ(expected, Simhash::find_all(hashes, blocks, 3));
    }
}

TEST(SimhashTest, FindAllDiverse)
{
    std::unordered_set<Simhash::hash_t> hashes = {
        0x00000000, 0x10101000, 0x10100010, 0x10001010, 0x00101010,
                    0x01010100, 0x01010001, 0x01000101, 0x00010101
    };
    Simhash::matches_t expected = {
        { 0x00000000, 0x10101000 },
        { 0x00000000, 0x10100010 },
        { 0x00000000, 0x10001010 },
        { 0x00000000, 0x00101010 },
        { 0x00000000, 0x01010100 },
        { 0x00000000, 0x01010001 },
        { 0x00000000, 0x01000101 },
        { 0x00000000, 0x00010101 },
        { 0x00101010, 0x10001010 },
        { 0x00101010, 0x10100010 },
        { 0x00101010, 0x10101000 },
        { 0x10001010, 0x10100010 },
        { 0x10001010, 0x10101000 },
        { 0x10100010, 0x10101000 },
        { 0x00010101, 0x01000101 },
        { 0x00010101, 0x01010001 },
        { 0x00010101, 0x01010100 },
        { 0x01000101, 0x01010001 },
        { 0x01000101, 0x01010100 },
        { 0x01010001, 0x01010100 }
    };

    for (size_t blocks = 4; blocks < 10; ++blocks)
    {
        EXPECT_EQ(expected, Simhash::find_all(hashes, blocks, 3));
    }
}
