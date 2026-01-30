#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define ADS_B_FRAME_BYTES 14 // 112 bits

typedef struct {
    uint8_t  raw[ADS_B_FRAME_BYTES]; // as received (MSB-first per spec)
    uint64_t rx_time_ns;             

    uint8_t  df;      // 5 bits
    uint8_t  ca;      // 3 bits
    uint32_t icao;    // 24 bits

    uint8_t  tc;      // 5 bits (first 5 bits of ME)
    uint8_t  me[7];   // 56-bit ME field (bytes 4..10 in the frame)

    uint32_t pi;      // 24 bits

    bool crc_ok;      // if you verify parity/CRC
} ads_b_frame_t;

typedef struct {
    uint32_t icao;

    char callsign[9];
    bool has_callsign;

    int32_t alt;
    bool has_alt;

    double lat_deg, lng_deg;
    bool has_position;

    double groundspeed_kt, track_deg;
    int32_t vr_fpm;
    bool has_vel;

    uint64_t last_seen_nsec;
} ads_b_flugzeug_t;

#endif // TYPES_H
