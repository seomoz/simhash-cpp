#include "permutation.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace Simhash {

    std::vector<std::vector<hash_t> > Permutation::choose(
            const std::vector<hash_t>& population, size_t r)
    {
        // This algorithm is cribbed from python's itertools page.
        size_t n = population.size();
        if (r > n)
        {
            throw std::invalid_argument("R cannot be greater than population size.");
        }

        std::vector<size_t> indices(r);
        for (size_t i = 0; i < r; ++i)
        {
            indices[i] = i;
        }

        std::vector<std::vector<hash_t> > results;
        std::vector<hash_t> result(r);

        for (size_t i = 0; i < r; ++i)
        {
            result[i] = population[indices[i]];
        }
        results.push_back(result);

        while (true)
        {
            int i = r - 1;
            for (; i >= 0; --i)
            {
                if (indices[i] != i + n - r)
                {
                    break;
                }
            }
            if (i < 0)
            {
                return results;
            }

            indices[i] += 1;
            for (size_t j = i + 1; j < r; ++j)
            {
                indices[j] = indices[j-1] + 1;
            }
            for (size_t j = 0; j < r; ++j)
            {
                result[j] = population[indices[j]];
            }
            results.push_back(result);
        }
    }

    std::vector<Permutation> Permutation::create(size_t number_of_blocks,
                                                 size_t different_bits)
    {
        if (number_of_blocks > Simhash::BITS)
        {
            std::stringstream message;
            message << "Number of blocks must not exceed " << sizeof(hash_t) * 8;
            throw std::invalid_argument(message.str());
        }

        if (number_of_blocks <= different_bits)
        {
            std::stringstream message;
            message << "Number of blocks (" << number_of_blocks
                    << ") must be greater than different_bits (" << different_bits
                    << ")";
            throw std::invalid_argument(message.str());
        }

        /* These are the blocks, in mask form. */
        std::vector<hash_t> blocks;
        for (size_t i = 0; i < number_of_blocks; ++i)
        {
            hash_t mask(0);
            size_t start = (   i    * Simhash::BITS) / number_of_blocks;
            size_t end   = ((i + 1) * Simhash::BITS) / number_of_blocks;
            for (size_t j = start; j < end; ++j)
            {
                mask |= (static_cast<hash_t>(1) << j);
            }
            blocks.push_back(mask);
        }

        /* This is the number of blocks in the leading prefix. */
        size_t count = static_cast<size_t>(number_of_blocks - different_bits);

        /* All the mask choices. */
        std::vector<Permutation> results;
        for (std::vector<hash_t>& choice : choose(blocks, count))
        {
            // Add the remaining masks -- those that were not part of choice
            for (hash_t block : blocks)
            {
                if (find(choice.begin(), choice.end(), block) == choice.end())
                {
                    choice.push_back(block);
                }
            }

            results.push_back(Permutation(different_bits, choice));
        }
        
        return results;
    }
    
    Permutation::Permutation(size_t different_bits, std::vector<hash_t>& masks)
        : forward_masks(masks)
        , reverse_masks()
        , offsets()
        , search_mask_(0)
    {
        int j(0), i(0), width(0); // counters

        // All of these are O(forward_masks)
        std::vector<size_t> widths;
        widths.reserve(forward_masks.size());
        reverse_masks.reserve(forward_masks.size());
        offsets.reserve(forward_masks.size());

        std::vector<hash_t>::iterator mask_it(forward_masks.begin());

        /* To more easily and reasonably-efficiently calculate the permutations
         * of each of the hashes we insert, and since each block is just
         * contiguous set bits, we will calculate the widths of each of these
         * blocks, and the offset of their rightmost bit. With this, we'll
         * generate net offsets between their positions in the unpermuted and
         * permuted forms, and simultaneously generate reverse masks */
        for(; mask_it != forward_masks.end(); ++mask_it)
        {
            hash_t mask = *mask_it;
            /* Find where the 1's start, and where they end. After this, `i` is
             * the position to the right of the rightmost set bit. `j` is the
             * position of the leftmost set bit. In `width`, we keep a running
             * tab of the widths of the bitmasks so far. */
            for (i = 0;          !((1UL << i) & mask); ++i) {}
            for (j = i; j < 64 && ((1UL << j) & mask); ++j) {}

            /* Just to prove that I'm sane, and in case that I'm ever running
             * circles around this logic in the future, consider:
             *
             *     63---53|52---42|41---32|31---21|20---10|09---00|
             *     |  A   |   B   |   C   |   D   |   E   |   F   |
             *
             *                       permuted to
             *     63---53|52---42|41---32|31---21|20---10|09---00|
             *     |  C   |   D   |   E   |   A   |   B   |   F   |
             *
             * The first loop, we'll have width = 0, and examining the mask for
             * C, we'll find that i = 31 and j = 41, so we find that its width
             * is 10. In the end, the bit in position 32 needs to move to be in
             * position 53. Width serves as an accumulator of the widths of the
             * blocks inserted into the permuted value, so we increment it by
             * the width of C (j-i) = 10. Now the end offset is 63 - width = 53,
             * and the original offset for that bit was (i+1) = 32, and we find
             * that we need an offset of 63 - width - i - 1 = 62 - width - i:
             *
             *    C: i = 31 | j = 41 | width = 0  | end = 53
             *       width += (j-i)          => 10
             *       offset = 62 - width - i => 21
             *
             *    D: i = 20 | j = 31 | width = 10 | end = 42
             *       width += (j-i)          => 21
             *       offset = 62 - width - i => 21 */
            width += (j - i);
            widths.push_back(j - i);

            int offset = 64 - width - i;
            offsets.push_back(offset);

            /* It's a trivial transformation, but we'll pre-compute our reverse
             * masks so that we don't have to compute for after the every time
             * we unpermute a number */
            if (offset > 0)
            {
                reverse_masks.push_back(mask <<  offset);
            }
            else
            {
                reverse_masks.push_back(mask >> -offset);
            }
        }

        /* Alright, we have to determine the low and high masks for this
         * particular table. If we are searching for up to /d/ differing bits,
         * then we should  include all but the last /d/ blocks in our mask.
         *
         * After this, width should hold the number of bits that are in all but
         * the last d blocks */
        std::vector<size_t>::iterator width_it(widths.begin());
        for (width = 0; different_bits < widths.size(); ++different_bits, ++width_it)
        {
            width += *width_it;
        }

        /* Set the first /width/ bits in the low mask to 1, and then shift it up
         * until it's a full 64-bit number. */
        for(i = 0    ; i < width; ++i) { search_mask_ = (search_mask_ << 1) | 1; }
        for(i = width; i < 64   ; ++i) { search_mask_ =  search_mask_ << 1;      }
    }

    hash_t Permutation::apply(hash_t hash) const
    {
        std::vector<hash_t>::const_iterator masks_it(forward_masks.begin());
        std::vector<int   >::const_iterator offset_it(     offsets.begin());

        hash_t result(0);
        for (; masks_it != forward_masks.end(); ++masks_it, ++offset_it)
        {
            if (*offset_it > 0)
            {
                result = result | ((hash & *masks_it) <<   *offset_it );
            }
            else
            {
                result = result | ((hash & *masks_it) >> -(*offset_it));
            }
        }
        return result;
    }

    hash_t Permutation::reverse(hash_t hash) const
    {
        std::vector<hash_t>::const_iterator masks_it(reverse_masks.begin());
        std::vector<int   >::const_iterator offset_it(     offsets.begin());

        hash_t result(0);
        for (; masks_it != reverse_masks.end(); ++masks_it, ++offset_it)
        {
            if (*offset_it > 0)
            {
                result = result | ((hash & *masks_it) >>   *offset_it );
            }
            else
            {
                result = result | ((hash & *masks_it) << -(*offset_it));
            }
        }
        return result;
    }

    hash_t Permutation::search_mask() const
    {
        return search_mask_;
    }
}
