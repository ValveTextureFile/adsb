#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "types.h"
#include "registry.h"

#define btby(b)  ((b) / 8)
#define bytb(by) ((by) * 8)
static inline int hexval(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static inline int hex_to_bytes(const char *hex, uint8_t *out, int max_out)
{
    int len = 0;
    while (*hex && len < max_out) {
        while (*hex && isspace((unsigned char)*hex)) hex++;
        if (!hex[0] || !hex[1]) break;

        int hi = hexval(hex[0]);
        int lo = hexval(hex[1]);
        if (hi < 0 || lo < 0) return -1;

        out[len++] = (uint8_t)((hi << 4) | lo);
        hex += 2;
    }
    return len;
}
static inline uint32_t get_bits_from_index(const uint8_t *buffer, int start, int nbits) {
    uint32_t v = 0;
    for (int i = 0; i < nbits; ++i) {
        int b = (start - 1) + i;
        int by = btby(b);
        int bitbyte = 7 - (b % 8);
        v = (v << 1) | ((buffer[by] >> bitbyte) & 1u);
    }
    return v;
}

static inline uint64_t get_bits_from_index64(const uint8_t *buffer, int start, int nbits) {
    uint64_t v = 0;
    for (int i = 0; i < nbits; ++i) {
        int b = (start - 1) + i;
        int by = btby(b);
        int bitbyte = 7 - (b % 8);
        v = (v << 1) | ((buffer[by] >> bitbyte) & 1u);
    }
    return v;
}

static inline void set_bits_from_index(uint8_t *buffer, int start, int nbits, uint64_t value) {
    for (int i = nbits - 1; i >= 0; --i) {
        int b = (start - 1) + (nbits - 1 - i);
        int by = btby(b);
        int bitbyte = 7 - (b % 8);
        uint8_t bit = (value >> i) & 1u;
        buffer[by] = (uint8_t)((buffer[by] & ~(1u << bitbyte)) | (bit << bitbyte));
    }
}

static inline void adsb_parse_frame(adsb_frame_t *out, const uint8_t raw[ADS_B_FRAME_BYTES])
{
    memcpy(out->raw, raw, ADS_B_FRAME_BYTES);

    out->df   = (uint8_t)get_bits_from_index(raw, 1, 5);
    out->ca   = (uint8_t)get_bits_from_index(raw, 6, 3);
    out->icao =          get_bits_from_index(raw, 9, 24);

    // ME payload (56 bits) is bytes 4..10
    memcpy(out->me, &raw[4], 7);

    out->tc = (uint8_t)get_bits_from_index(raw, 33, 5);
    out->pi =          get_bits_from_index(raw, 89, 24);
}


static inline adsb_flugzeug_t* pull_from_registry(adsb_registry_t* reg, uint32_t icao) {
    if (!reg) return NULL;

    for (uint32_t i = 0; i < reg->count; ++i)
        if (reg->items[i].icao == icao)
            return &reg->items[i];

    if (reg->count >= MAX_FLUGZEUG) return NULL;

    adsb_flugzeug_t *a = &reg->items[reg->count++];
    *a = (adsb_flugzeug_t){0};
    a->icao = icao;
    return a;
}

static inline void decode_callsign(adsb_flugzeug_t *a, const uint8_t me[7]) {
    const char *charset = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ????? ???????????????0123456789??????";
    for (int i=0; i<8; ++i) {
        int shift = 42-(i*6);
        uint8_t c = (uint8_t)get_bits_from_index64(me, shift++, 6);
        a->callsign[i] = charset[c & 0x3f];
    }
    a->callsign[8] = '\0';
    a->has_callsign = true;
}

static inline void adsb_update_aircraft(adsb_flugzeug_t *a, const adsb_frame_t *f, uint64_t now_ns)
{
    a->last_seen_nsec = now_ns;

    switch (f->tc) {
        case 1: case 2: case 3: case 4:
            decode_callsign(a, f->me);
            break;

        case 9: case 10: case 11: case 12:
        case 13: case 14: case 15: case 16:
        case 17: case 18:
            // airborne position (baro altitude) -> CPR decode later
            // decode_altitude_baro(a, f->me);
            break;

        case 19:
            // decode_velocity(a, f->me);
            break;

        default:
            break;
    }
}

#endif // UTILS_H
