#pragma once
#ifndef REGISTRY_H
#define REGISTRY_H
#include "types.h"

#define MAX_FLUGZEUG 512

typedef struct {
    adsb_flugzeug_t items[MAX_FLUGZEUG];
    uint32_t count;
} adsb_registry_t;

#endif 