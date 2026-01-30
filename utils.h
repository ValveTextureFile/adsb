#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdlib.h>
#include "types.h"

static inline uint32_t get_bits_from_index(const uint8_t *buffer, int start, int nbits) {
    uint32_t res;
    for (int i=0; i<nbits; ++i){
        int b = (start-1)
    }

    return res;
}
#endif // UTILS_H