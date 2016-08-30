#ifndef SIMHASH_SIMHASH_H
#define SIMHASH_SIMHASH_H

#include <cstddef>
#include <stdint.h>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Simhash {

    /**
     * The type of all hashes.
     */
    typedef uint64_t hash_t;

    /**
     * The type of a match of two hashes.
     */
    typedef std::pair<hash_t, hash_t> match_t;

    /**
     * For use with matches_t.
     */
    struct match_t_hash {
        inline std::size_t operator()(const std::pair<hash_t,hash_t>& v) const {
            return static_cast<hash_t>(v.first * 31 + v.second);
        }
    };

    /**
     * The type for matches what we've returned.
     */
    typedef std::unordered_set<match_t, match_t_hash> matches_t;

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

    /**
     * Find the set of all matches within the provided vector of hashes.
     *
     * The provided hashes are manipulated in place, but upon completion are
     * restored to their original state.
     */
    matches_t find_all(std::unordered_set<hash_t>& hashes,
                       size_t number_of_blocks,
                       size_t different_bits);
}

#endif
