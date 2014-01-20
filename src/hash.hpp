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
            /* Simhash works by calculating the hashes of overlaping windows
             * of the input, and then for each bit of that hash, increments a
             * corresponding count. At the end, each of the counts is
             * transformed back into a bit by whether or not the count is
             * positive or negative */

            // Counts
            int64_t v[64] = {
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            };

            hash_t hash(0);             // The hash we're trying to produce
            size_t   j(0);              // Counter
            size_t   window(3);         // How many tokens in rolling hash?
            char     **tp;              // For stepping through tokens

            /* Create a tokenizer, hash function, and cyclic */
            Hash hasher;
            Cyclic<hash_t> cyclic(window);

            for (tp = tokens; *tp != NULL; tp++) {
                /* puts(*tp);  /* debug */
                hash_t r = cyclic.push(hasher(*tp, strlen(*tp), 0));
                for (j = 63; j > 0; --j) {
                    v[j] += (r & 1) ? 1 : -1;
                    r = r >> 1;
                }
                v[j] += (r & 1) ? 1 : -1;
            }

            /* With counts appropriately tallied, create a 1 bit for each of
             * the counts that's positive. That result is the hash. */
            for (j = 0; j < 64; ++j) {
                if (v[j] > 0) {
                    hash = hash | (static_cast<hash_t>(1) << j);
                }
            }
            return hash;
        }

        /* As above, but operate on a vector of unsigned 64-bit numbers,
           not strings. */
        hash_t hash_fp(uint64_t *vec, int len)
        {
            // Counts
            int64_t v[64] = {
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            };

            hash_t hash(0);             // The hash we're trying to produce
            size_t   j(0);              // Counter
            size_t   window(3);         // How many tokens in rolling hash?
            int     i;              // For stepping through tokens

            /* Create a tokenizer, hash function, and cyclic */
            Hash hasher;
            Cyclic<hash_t> cyclic(window);

            for (i = 0; i < len; i++) {
                hash_t r = cyclic.push(hasher(reinterpret_cast<char*>(vec+i), sizeof(uint64_t), 0));
                for (j = 63; j > 0; --j) {
                    v[j] += (r & 1) ? 1 : -1;
                    r = r >> 1;
                }
                v[j] += (r & 1) ? 1 : -1;
            }

            /* With counts appropriately tallied, create a 1 bit for each of
             * the counts that's positive. That result is the hash. */
            for (j = 0; j < 64; ++j) {
                if (v[j] > 0) {
                    hash = hash | (static_cast<hash_t>(1) << j);
                }
            }
            return hash;
        }

        /* As above, but operate on a vector of signed 64-bit integers. For
         * some language bindings, casting and function overloading can be
         * difficult to use, which is why it's given a new name */
        hash_t hash_fps(int64_t *vec, int len) {
            return hash_fp(reinterpret_cast<uint64_t*>(vec), len);
        }

    private:
        /* Internal stuffs */
        hash_type      hasher;
    };
}

#endif
