#ifndef SIMHASH_HASH_H
#define SIMHASH_HASH_H

/* Simply holds references to all of our hash functions */
#include "hashes/jenkins.h"
#include "hashes/murmur.h"
#include "hashes/fnv.h"

/* Include a reference to the tokenizer to use */
#include "tokenizers/strspn.h"

/* For hash_t */
#include "common.h"
#include "cyclic.hpp"

#include <vector>

namespace Simhash {
    /**
     * Compute a similarity hash for the provided string using a rolling hash
     * function and a provided hash window
     *
     * @param tokens - An NULL-terminated array of character pointers to the
     *                 nul-terminated tokens comprising the text.
     *
     * @return hash representative of the content of the text */
    template <typename Hash=jenkins>
    class Simhash {
    private:
        class Accumulator {
        private:
            static const size_t BITS = sizeof(hash_t) * 8;

            size_t window;
            Cyclic<hash_t> cyclic;
            std::vector<int64_t> v;
        public:
            Accumulator() : window(3), cyclic(window), v(BITS, 0) {}

            void update(hash_t hash) {
                hash = cyclic.push(hash);
                for (int j = (BITS - 1); j >= 0; --j) {
                    v[j] += (hash & 1) ? 1 : -1;
                    hash >>= 1;
                }
            }

            /* With counts appropriately tallied, create a 1 bit for each of
             * the counts that's positive. That result is the hash. */
            hash_t result() {
                hash_t hash(0);
                for (size_t j = 0; j < BITS; ++j) {
                    if (v[j] > 0) {
                        hash |= (static_cast<hash_t>(1) << j);
                    }
                }
                return hash;
            }
        };
    public:
        typedef Hash      hash_type;

        /* Alias of operator()
         *
         * Some languages have difficulty making use of operator(), and so this
         * it made available for those languages */
        inline hash_t hash(char **tokens) {
            return operator()(tokens);
        }

        /* Return a simhash value using a moving window */
        hash_t operator()(char **tokens) {
            Hash hasher;
            Accumulator accumulator;

            for (char **tp = tokens; *tp != NULL; ++tp) {
                size_t len = strlen(*tp);
                if (len > 0) {
                    accumulator.update(hasher(*tp, len, 0));
                }
            }

            return accumulator.result();
        }

        /* As above, but operate on a vector of unsigned 64-bit numbers,
           not strings. */
        hash_t hash_fp(uint64_t *vec, int len) {
            Hash hasher;
            Accumulator accumulator;

            for (int i = 0; i < len; ++i) {
                accumulator.update(
                    hasher(reinterpret_cast<char*>(vec+i), sizeof(uint64_t), 0));
            }

            return accumulator.result();
        }

        /* As above, but operate on a vector of signed 64-bit integers. For
         * some language bindings, casting and function overloading can be
         * difficult to use, which is why it's given a new name */
        hash_t hash_fps(int64_t *vec, int len) {
            return hash_fp(reinterpret_cast<uint64_t*>(vec), len);
        }

        template <typename Tokenizer>
        hash_t hash_tokenizer(const char* string, const Tokenizer& tokenizer) {
            Hash hasher;
            Accumulator accumulator;

            for (const char *current = string, *next = tokenizer(current)
                ; next != NULL
                ; next = tokenizer(current)) {
                if (next != current) {
                    accumulator.update(hasher(current, next - current, 0));
                }
                current = next + 1;
            }

            return accumulator.result();
        }
    };
}

#endif
