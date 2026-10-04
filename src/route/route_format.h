#ifndef ROUTE_FORMAT_H
#define ROUTE_FORMAT_H

#include "bike_types.h"
#include <stdint.h>

#define BIKE_ROUTE_MAGIC 0x42525445UL /* BRTE */
#define BIKE_ROUTE_FORMAT_VERSION 1U
#define BIKE_ROUTE_NAME_MAX_LEN 48U

typedef struct
{
    uint32_t magic;
    uint16_t version;
    uint16_t header_size;
    uint32_t point_count;
    uint32_t total_distance_m;
    char name[BIKE_ROUTE_NAME_MAX_LEN];
} bike_route_header_t;

typedef struct
{
    int32_t latitude_e7;
    int32_t longitude_e7;
    int32_t altitude_dm;
    uint32_t distance_from_start_m;
    uint16_t road_name_index;
    uint8_t maneuver;
    uint8_t flags;
} bike_route_point_t;

#endif /* ROUTE_FORMAT_H */
