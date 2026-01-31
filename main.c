#include <stdio.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "registry.h"
#include "utils.h"

static uint64_t now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

static void print_registry(const adsb_registry_t *reg)
{
    printf("\nICAO     TC  LAST SEEN (ms ago)\n");
    printf("-------  --  -----------------\n");

    uint64_t t = now_ns();
    for (uint32_t i = 0; i < reg->count; ++i) {
        const adsb_flugzeug_t *a = &reg->items[i];
        uint64_t age_ms = (t - a->last_seen_nsec) / 1000000ull;

        printf("%06X   %2u  %10llu\n",
               a->icao,
               a->last_tc,
               (unsigned long long)age_ms);
    }
}

int main(void)
{
    adsb_registry_t reg = {0};

    char line[256];
    uint8_t raw[ADS_B_FRAME_BYTES];
    adsb_frame_t frame;

    printf("Paste ADS-B hex frames (Ctrl+D to quit)\n");

    while (fgets(line, sizeof(line), stdin)) {
        int n = hex_to_bytes(line, raw, ADS_B_FRAME_BYTES);
        if (n != ADS_B_FRAME_BYTES) {
            printf("Invalid frame (need %d bytes, got %d)\n",
                   ADS_B_FRAME_BYTES, n);
            continue;
        }

        adsb_parse_frame(&frame, raw);

        if (frame.df != 17 && frame.df != 18) {
            printf("Not ADS-B (DF=%u)\n", frame.df);
            continue;
        }

        adsb_flugzeug_t *a = pull_from_registry(&reg, frame.icao);
        if (!a) {
            printf("Registry full!\n");
            continue;
        }

        a->last_seen_nsec = now_ns();
        a->last_tc = frame.tc;

        print_registry(&reg);
    }

    return 0;
}
