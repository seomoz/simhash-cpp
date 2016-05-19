#ifndef SIMHASH_TOKENIZERS_STRSPN_H
#define SIMHASH_TOKENIZERS_STRSPN_H

#include <string.h>

namespace Simhash {
    static const char* alphas = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    struct StringToken {
        const char* start;
        size_t      length;
    };

    struct Strspn {
        /**
         * Sets fields of the `output` to the first character of the
         * token and its length, skipping preceding non-token characters.
         * If the `string` does not contain any token (empty or constists
         * of separators only), `output.start` is set to NULL, and
         * `output.length` is set to 0.
         *
         * Note that for valid token length is never 0.
         */
        void operator()(const char* string, StringToken& output) const {
            const char* start = strpbrk(string, alphas);
            output.start = start;
            output.length = (start ? strspn(start, alphas) : 0);
        }
    };
}
#endif
