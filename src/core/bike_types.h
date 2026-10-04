#ifndef BIKE_TYPES_H
#define BIKE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BIKE_NAV_TEXT_MAX_LEN 64

typedef enum
{
    BIKE_MODE_OFFLINE = 0,
    BIKE_MODE_ROUTE,
    BIKE_MODE_PHONE_NAV,
    BIKE_MODE_COUNT,
} bike_mode_t;

typedef enum
{
    RIDE_SESSION_IDLE = 0,
    RIDE_SESSION_RECORDING,
    RIDE_SESSION_PAUSED,
    RIDE_SESSION_FINISHED,
} ride_session_state_t;

typedef enum
{
    BIKE_SERVICE_UNINITIALIZED = 0,
    BIKE_SERVICE_READY,
    BIKE_SERVICE_RUNNING,
    BIKE_SERVICE_DISABLED,
    BIKE_SERVICE_ERROR,
} bike_service_state_t;

typedef enum
{
    NAV_SOURCE_NONE = 0,
    NAV_SOURCE_ROUTE,
    NAV_SOURCE_PHONE,
} navigation_source_t;

typedef enum
{
    NAV_MANEUVER_STRAIGHT = 0,
    NAV_MANEUVER_LEFT,
    NAV_MANEUVER_RIGHT,
    NAV_MANEUVER_UTURN,
    NAV_MANEUVER_ARRIVE,
} navigation_maneuver_t;

enum
{
    RIDE_SAMPLE_FLAG_LOCATION_VALID = (1U << 0),
    RIDE_SAMPLE_FLAG_ALTITUDE_VALID = (1U << 1),
    RIDE_SAMPLE_FLAG_SPEED_VALID = (1U << 2),
    RIDE_SAMPLE_FLAG_MOTION_VALID = (1U << 3),
};

typedef struct
{
    uint32_t timestamp_s;
    int32_t latitude_e7;
    int32_t longitude_e7;
    int32_t altitude_dm;
    uint32_t speed_cms;
    uint16_t course_cdeg;
    uint16_t hdop_x10;
    uint8_t satellites;
    uint8_t flags;
} ride_sample_t;

typedef struct
{
    navigation_source_t source;
    navigation_maneuver_t maneuver;
    uint32_t next_distance_m;
    uint32_t remaining_distance_m;
    uint32_t remaining_time_s;
    bool link_connected;
    char current_road[BIKE_NAV_TEXT_MAX_LEN];
    char next_road[BIKE_NAV_TEXT_MAX_LEN];
} navigation_instruction_t;

#ifdef __cplusplus
}
#endif

#endif /* BIKE_TYPES_H */
