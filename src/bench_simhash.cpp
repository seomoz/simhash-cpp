#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "hashes/jenkins.h"
#include "simhash.h"

static float ms_timer() {
    static struct timeval* start = NULL;
    if (!start) {
        start = new struct timeval;
        gettimeofday(start, NULL);
    }
    struct timeval now;
    gettimeofday(&now, NULL);
    long seconds = now.tv_sec - start->tv_sec;
    long useconds = now.tv_usec - start->tv_usec;
    *start = now;
    return (seconds * 1000.0 + useconds/1000.0) + 0.5;
}

namespace Simhash {
    class SimhashNew {
    private:
        static const size_t BITS = sizeof(hash_t) * 8;

        std::vector<size_t> v;
        size_t hash_count;
    public:
        SimhashNew() : v(BITS, 0), hash_count(0) {}

        void update(hash_t hash) {
            ++hash_count;
            for (int j = BITS - 1; j >= 0; --j) {
                v[j] += hash & 1;
                hash >>= 1;
            }
        }

        /* With counts appropriately tallied, create a 1 bit for each of
         * the counts that's over threshold. That result is the hash. */
        hash_t result() {
            size_t threshold = hash_count / 2;
            hash_t hash(0);
            for (size_t j = 0; j < BITS; ++j) {
                if (v[j] > threshold) {
                    hash |= (static_cast<hash_t>(1) << j);
                }
            }
            return hash;
        }
    };

    class SimhashOld {
    private:
        static const size_t BITS = sizeof(hash_t) * 8;
        std::vector<int64_t> v;
    public:
        SimhashOld() : v(BITS, 0) {}

        void update(hash_t hash) {
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
}

int main(int argc, char* argv[]) {
    uint64_t limit(100000000);
    Simhash::jenkins hasher;
    Simhash::SimhashNew simhash_new;
    Simhash::SimhashOld simhash_old;
    static volatile Simhash::hash_t h;

    ms_timer();
    for (uint64_t i = 0; i < limit; ++i) {
        h = hasher(reinterpret_cast<const char*>(&i), sizeof(i), 0);
    }
    printf("no simhash:  %8.3f ms\n", ms_timer());

    ms_timer();
    for (uint64_t i = 0; i < limit; ++i) {
        h = hasher(reinterpret_cast<const char*>(&i), sizeof(i), 0);
        simhash_old.update(h);
    }
    printf("old simhash: %8.3f ms\n", ms_timer());

    ms_timer();
    for (uint64_t i = 0; i < limit; ++i) {
        h = hasher(reinterpret_cast<const char*>(&i), sizeof(i), 0);
        simhash_new.update(h);
    }
    printf("new simhash: %8.3f ms\n", ms_timer());
}
