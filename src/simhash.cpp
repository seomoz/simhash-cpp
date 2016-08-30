#include "simhash.h"

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
