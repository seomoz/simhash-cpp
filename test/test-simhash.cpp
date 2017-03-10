#include <gtest/gtest.h>

#include <algorithm>
#include <map>

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

/**
 * Turn a Simhash::clusters_t into a sorted vector of sorted clusters.
 */
std::vector<std::set<Simhash::hash_t> > sortClusters(const Simhash::clusters_t& clusters)
{
    // Comparator for sorting the results
    struct {
        bool operator() (
            const std::set<Simhash::hash_t>& a,
            const std::set<Simhash::hash_t> b)
        {
            return min(a) < min(b);
        }

        Simhash::hash_t min(const std::set<Simhash::hash_t>& cluster)
        {
            return *(std::min_element(cluster.begin(), cluster.end()));
        }
    } comparator;

    // Assign clusters to results
    std::vector<std::set<Simhash::hash_t> > results;
    for (const auto& cluster : clusters) {
        results.push_back(std::set<Simhash::hash_t>(cluster.begin(), cluster.end()));
    }

    // Sort and return
    std::sort(results.begin(), results.end(), comparator);
    return results;
}

TEST(SimhashTest, FindClusters)
{
    std::unordered_set<Simhash::hash_t> hashes = {
        0x000000FF, 0x000000EF, 0x000000EE, 0x000000CE, 0x00000033,
        0x0000FF00, 0x0000EF00, 0x0000EE00, 0x0000CE00, 0x00003300,
        0x00FF0000, 0x00EF0000, 0x00EE0000, 0x00CE0000, 0x00330000,
        0xFF000000, 0xEF000000, 0xEE000000, 0xCE000000, 0x33000000
    };
    Simhash::clusters_t expected = {
        { 0x000000FF, 0x000000EF, 0x000000EE, 0x000000CE },
        { 0x0000FF00, 0x0000EF00, 0x0000EE00, 0x0000CE00 },
        { 0x00FF0000, 0x00EF0000, 0x00EE0000, 0x00CE0000 },
        { 0xFF000000, 0xEF000000, 0xEE000000, 0xCE000000 }
    };

    for (size_t blocks = 4; blocks < 10; ++blocks)
    {
        auto actual = Simhash::find_clusters(hashes, blocks, 3);
        EXPECT_EQ(sortClusters(expected), sortClusters(actual));
    }
}

TEST(SimhashTest, FindClustersDiverse)
{
    std::unordered_set<Simhash::hash_t> hashes = {
        0x00000000, 0x10101000, 0x10100010, 0x10001010, 0x00101010,
                    0x01010100, 0x01010001, 0x01000101, 0x00010101
    };

    Simhash::clusters_t expected = {
        {
            0x00000000, 0x10101000, 0x10100010, 0x10001010, 0x00101010,
                        0x01010100, 0x01010001, 0x01000101, 0x00010101
        }
    };

    for (size_t blocks = 4; blocks < 10; ++blocks)
    {
        auto actual = Simhash::find_clusters(hashes, blocks, 3);
        EXPECT_EQ(sortClusters(expected), sortClusters(actual));
    }
}

TEST(SimhashTest, FindClustersUnique)
{
    std::unordered_set<Simhash::hash_t> hashes = { 20, 10 };
    Simhash::clusters_t expected = { { 10L, 20L } };
    // 10 and 20 are 4 bits different
    EXPECT_EQ(sortClusters(expected), sortClusters(Simhash::find_clusters(hashes, 5, 4)));
}
