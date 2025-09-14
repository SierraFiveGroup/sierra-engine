#include "hash.hpp"

namespace Sierra {
    uint64_t combineHashes(uint64_t h1, uint64_t h2) {
        h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2); // stolen from boost
        return h1;
    }
}