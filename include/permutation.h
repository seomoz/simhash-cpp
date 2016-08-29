#ifndef SIMHASH_PERMUTATION_H
#define SIMHASH_PERMUTATION_H

#include "simhash.h"

#include <vector>

namespace Simhash {
    
    class Permutation {
    public:
        /**
         * Create a vector of permutations necessary to do all simhash near-dup
         * detection.
         */
        static std::vector<Permutation> create(size_t number_of_blocks,
                                               size_t different_bits);

        /**
         * Generate combinations of length r from population.
         */
        static std::vector<std::vector<hash_t> > choose(
            const std::vector<hash_t>& population, size_t r);

        /**
         * Construct a permutation from its permutation masks and the maximum
         * number of bits that may differ.
         */
        Permutation(size_t different_bits, std::vector<hash_t>& masks);

        /**
         * Apply this permutation.
         */
        hash_t apply(hash_t hash) const;

        /**
         * Reverse this permutation, getting the original.
         */
        hash_t reverse(hash_t hash) const;

        /**
         * Search mask
         *
         * When searching for a potential match, a match may be less than
         * the query, so it's insufficient to simply search for the query.
         * This mask sets all the bits in the last _differing_bits_ blocks
         * to 0, which is smaller than is necessary, but the easiest correct
         * number to compute. The highest number which must be potentially
         * searched is the query with all the bits in the last
         * _differing_bits_ blocks set to 1. */
        hash_t search_mask() const;
    private:
        std::vector<hash_t> forward_masks;
        std::vector<hash_t> reverse_masks;
        std::vector<int> offsets;
        hash_t search_mask_;
    };
}

#endif
