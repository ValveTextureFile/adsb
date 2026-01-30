#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdlib.h>
#include "types.h"

#define btby(b)  b*8
#define bytb(by) by/8


static inline uint32_t get_bits_from_index(const uint8_t *buffer, int start, int nbits) {
    uint32_t v;
    for (int i=0; i<nbits; ++i){
        int b = (start-1) + i;
        int by = btby(b);
        int bitbyte = 7 - (b % 8);
        v = (v << 1) | ((buffer[by]>>bitbyte)&1u);
    }
    return v;
}
#endif // UTILS_H