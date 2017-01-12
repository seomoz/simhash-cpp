#include <gtest/gtest.h>

#include "permutation.h"

#include <cstdlib>

TEST(PermutationTest, Choose3From6) {
    std::vector<Simhash::hash_t> population = { 0, 1, 2, 3, 4, 5 };
    std::vector<std::vector<Simhash::hash_t> > expected = {
        { 0, 1, 2 }, { 0, 1, 3 }, { 0, 1, 4 }, { 0, 1, 5 },
        { 0, 2, 3 }, { 0, 2, 4 }, { 0, 2, 5 }, { 0, 3, 4 },
        { 0, 3, 5 }, { 0, 4, 5 }, { 1, 2, 3 }, { 1, 2, 4 },
        { 1, 2, 5 }, { 1, 3, 4 }, { 1, 3, 5 }, { 1, 4, 5 },
        { 2, 3, 4 }, { 2, 3, 5 }, { 2, 4, 5 }, { 3, 4, 5 }
    };
    std::vector<std::vector<Simhash::hash_t> > actual =
        Simhash::Permutation::choose(population, 3);
    EXPECT_EQ(20, actual.size());
    EXPECT_EQ(expected, actual);
}

TEST(PermutationTest, ChooseTooMany)
{
    std::vector<Simhash::hash_t> population = { 0, 1, 2, 3, 4, 5 };
    ASSERT_THROW(Simhash::Permutation::choose(population, 7), std::invalid_argument);
}

TEST(PermutationTest, Create)
{
    std::vector<Simhash::Permutation> permutations = Simhash::Permutation::create(6, 3);
    EXPECT_EQ(20, permutations.size());
}

TEST(PermutationTest, CreateTooManyBlocks)
{
    ASSERT_THROW(
        Simhash::Permutation::create(Simhash::BITS + 1, 3), std::invalid_argument);
}

TEST(PermutationTest, CreateTooFewBlocks)
{
    ASSERT_THROW(
        Simhash::Permutation::create(2, 3), std::invalid_argument);
}

TEST(PermutationTest, Apply)
{
    std::vector<Simhash::Permutation> permutations = Simhash::Permutation::create(4, 3);
    for (size_t i = 0; i < 1; ++i) {
        Simhash::hash_t num = static_cast<Simhash::hash_t>(rand());
        for (size_t j = 0; j < permutations.size(); ++j) {
            Simhash::hash_t permuted = permutations[j].apply(num);
            EXPECT_NE(num, permuted);
            EXPECT_EQ(num, permutations[j].reverse(permuted));
        }
    }
}
