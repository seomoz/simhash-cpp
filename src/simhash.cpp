#include "simhash.h"
#include "permutation.h"

#include <algorithm>

size_t Simhash::num_differing_bits(Simhash::hash_t a, Simhash::hash_t b)
{
    size_t count(0);
    Simhash::hash_t n = a ^ b;
    while (n)
    {
        ++count;
        n = n & (n - 1);
    }
    return count;
}

Simhash::hash_t Simhash::compute(const std::vector<Simhash::hash_t>& hashes)
{
    // Initialize counts to 0
    std::vector<long> counts(Simhash::BITS, 0);

    // Count the number of 1's, 0's in each position of the hashes
    for (auto it = hashes.begin(); it != hashes.end(); ++it)
    {
        Simhash::hash_t hash = *it;
        for (size_t i = 0; i < BITS; ++i)
        {
            counts[i] += (hash & 1) ? 1 : -1;
            hash >>= 1;
        }
    }

    // Produce the result
    Simhash::hash_t result(0);
    for (size_t i = 0; i < BITS; ++i) {
        if (counts[i] > 0)
        {
            result |= (static_cast<Simhash::hash_t>(1) << i);
        }
    }
    return result;
}

/**
 * Find all near-matches in a set of hashes.
 *
 * This works by putting the provided hashes into a vector. Then, for each permutation,
 * apply the permutation and sort the permuted hashes. Then walk the hashes, finding each
 * unique prefix.
 *
 * For each unique prefix, consider all hashes sharing that prefix, adding matches with
 * the lower number first (to avoid duplication; suppose a < b -- we will only emit (a, b)
 * as a match, but (b, a) will not be emitted).
 */
Simhash::matches_t Simhash::find_all(
    std::unordered_set<Simhash::hash_t>& hashes,
    size_t number_of_blocks,
    size_t different_bits)
{
    std::vector<Simhash::hash_t> copy(hashes.begin(), hashes.end());
    Simhash::matches_t results;
    auto permutations = Simhash::Permutation::create(number_of_blocks, different_bits);
    for (Simhash::Permutation& permutation : permutations)
    {
        // Apply the permutation to the set of hashes and sort
        auto op = [permutation](Simhash::hash_t h) -> Simhash::hash_t {
            return permutation.apply(h);
        };
        std::transform(hashes.begin(), hashes.end(), copy.begin(), op);
        std::sort(copy.begin(), copy.end());

        // Walk through and find regions that have the same prefix subject to the mask
        Simhash::hash_t mask = permutation.search_mask();
        auto start = copy.begin();
        while (start != copy.end())
        {
            // Find the end of the range that starts with this prefix
            Simhash::hash_t prefix = (*start) & mask;
            std::vector<Simhash::hash_t>::iterator end = start;
            for (; end != copy.end() && (*end & mask) == prefix; ++end) { }
            
            // For all the hashes that are between start and end, consider them all
            for (auto a = start; a != end; ++a)
            {
                for (auto b = a + 1; b != end; ++b)
                {
                    if (Simhash::num_differing_bits(*a, *b) <= different_bits)
                    {
                        Simhash::hash_t a_raw = permutation.reverse(*a);
                        Simhash::hash_t b_raw = permutation.reverse(*b);
                        // Insert the result keyed on the smaller of the two
                        results.insert(
                            std::make_pair(
                                std::min(a_raw, b_raw),
                                std::max(a_raw, b_raw)));
                    }
                }
            }

            // Advance start to after the block
            start = end;
        }
    }

    return results;
}
