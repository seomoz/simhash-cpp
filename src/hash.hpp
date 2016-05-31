#ifndef SIMHASH_HASH_H
#define SIMHASH_HASH_H

#include <vector>

/* Simply holds references to all of our hash functions */
#include "hashes/jenkins.h"
#include "hashes/murmur.h"
#include "hashes/fnv.h"

/* Include a reference to the tokenizer to use */
#include "tokenizers/strspn.h"

/* For hash_t */
#include "common.h"
#include "cyclic.hpp"

namespace Simhash {
    class HasherInterface {
    public:
        virtual void update(hash_t) = 0;
        virtual hash_t result() = 0;
    };

    class Simhash: public HasherInterface {
    private:
        static const size_t BITS = sizeof(hash_t) * 8;

        std::vector<size_t> bit_counts;
        size_t hash_count;
    public:
        Simhash(): bit_counts(BITS, 0), hash_count(0) {}

        /**
         * Updates bit counts with pre-calculated hash.
         */
        virtual void update(hash_t hash) /* override */ {
            ++hash_count;
            for (int j = BITS - 1; j >= 0; --j) {
                bit_counts[j] += hash & 1;
                hash >>= 1;
            }
        }

        /* With bit counts appropriately tallied, create a 1 bit for each of
         * the counts that's over threshold. That result is the hash. */
        virtual hash_t result() /* override */ {
            size_t threshold = hash_count / 2;
            hash_t hash(0);
            for (size_t j = 0; j < BITS; ++j) {
                if (bit_counts[j] > threshold) {
                    hash |= (static_cast<hash_t>(1) << j);
                }
            }
            return hash;
        }
    };

    class MovingWindowShingler: public HasherInterface {
    private:
        HasherInterface& hasher;
        int window_size;
        Cyclic<hash_t> cyclic;

    public:
        MovingWindowShingler(HasherInterface& hasher_, int window_size_)
            : hasher(hasher_), window_size(window_size_), cyclic(window_size_) {
        }

        virtual void update(hash_t hash) /* override */ {
            hasher.update(cyclic.push(hash));
        }

        /* With bit counts appropriately tallied, create a 1 bit for each of
         * the counts that's over threshold. That result is the hash. */
        virtual hash_t result() /* override */ {
            for (int j = 0; j < window_size - 1; ++j) {
                hasher.update(cyclic.push(0));
            }
            return hasher.result();
        }
    };

    class MovingWindowLegacyShingler: public HasherInterface {
    private:
        HasherInterface& hasher;
        Cyclic<hash_t> cyclic;

    public:
        MovingWindowLegacyShingler(HasherInterface& hasher_, int window_size_)
            : hasher(hasher_), cyclic(window_size_) {
        }

        virtual void update(hash_t hash) /* override */ {
            hasher.update(cyclic.push(hash));
        }

        /* With bit counts appropriately tallied, create a 1 bit for each of
         * the counts that's over threshold. That result is the hash. */
        virtual hash_t result() /* override */ {
            return hasher.result();
        }
    };

    template <typename HashFunctor>
    class DataHasher {
    private:
        HasherInterface& hasher;
        HashFunctor data_hash;
    public:
        DataHasher(HasherInterface& hasher_)
            : hasher(hasher_) {
        }

        void update(const void* data, size_t data_size) {
            hash_t hash =
                data_hash(static_cast<const char*>(data), data_size, 0);
            hasher.update(hash);
        }

        hash_t result() {
            return hasher.result();
        }
    };

    typedef DataHasher<fnv> FnvDataHasher;
    typedef DataHasher<jenkins> JenkinsDataHasher;
    typedef DataHasher<murmur> MurmurDataHasher;


    /**
     * Calculates simhash of the `text` using provided
     * SimHash and Tokenizer classes.
     */
    template<typename HashFunctor, typename Tokenizer>
    hash_t hash_text(const char* string) {
        Simhash simhash;
        MovingWindowLegacyShingler shingler(simhash, 3);
        DataHasher<HashFunctor> hasher(shingler);
        Tokenizer tokenizer;
        StringToken token;

        for (tokenizer(string, token)
            ; token.length != 0
            ; tokenizer(token.start + token.length, token)) {
            hasher.update(token.start, token.length);
        }

        return hasher.result();
    }
}
#endif
