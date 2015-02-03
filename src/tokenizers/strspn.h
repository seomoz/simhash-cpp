#ifndef SIMHASH_TOKENIZERS_STRSPN_H
#define SIMHASH_TOKENIZERS_STRSPN_H

#include <string.h>

namespace Simhash {
    struct Strspn {
        /* Return the length of the token starting at last */
        const char* operator()(const char* last) {
            size_t s = strspn(last,
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
            return (*last != '\0') ? last + s : NULL;
        }
    };
}

#endif
