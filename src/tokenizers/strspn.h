#ifndef SIMHASH_TOKENIZERS_STRSPN_H
#define SIMHASH_TOKENIZERS_STRSPN_H

#include <string.h>

namespace Simhash {
    struct Strspn {
        /**
         * Return a pointer to the end of the next token or NULL if
         * the end of string has been reached.
         */
        const char* operator()(const char* last) const {
            size_t s = strspn(last,
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
            return (*last != '\0') ? last + s : NULL;
        }
    };
}

#endif
