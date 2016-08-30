#ifndef SIMHASH_SIMHASH_H
#define SIMHASH_SIMHASH_H

#include <cstddef>
#include <stdint.h>
#include <vector>

namespace Simhash {
    /**
     * The type of all hashes.
     */
    typedef uint64_t hash_t;

    /**
     * The number of bits in a hash_t.
     */
    static const size_t BITS = sizeof(hash_t) * 8;

    /**
     * Compute the number of bits that are flipped between two numbers
     *
     * @param a - reference number
     * @param b - number to compare
     *
     * @return number of bits that differ between a and b */
    size_t num_differing_bits(hash_t a, hash_t b);

    /**
     * Compute the simhash of a vector of hashes.
     */
    hash_t compute(const std::vector<hash_t>& hashes);
}

#endif
