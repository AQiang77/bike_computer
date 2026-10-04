#ifndef BIKE_BLE_PROTOCOL_H
#define BIKE_BLE_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define BIKE_BLE_PROTOCOL_MAGIC 0xB1C5U
#define BIKE_BLE_PROTOCOL_VERSION 1U

typedef enum
{
    BIKE_BLE_MSG_COMMAND = 1,
    BIKE_BLE_MSG_COMMAND_RESPONSE,
    BIKE_BLE_MSG_NAVIGATION,
    BIKE_BLE_MSG_LIVE_SAMPLE,
    BIKE_BLE_MSG_FILE_INFO,
    BIKE_BLE_MSG_FILE_DATA,
    BIKE_BLE_MSG_FILE_ACK,
} bike_ble_message_type_t;

typedef struct
{
    uint16_t magic;
    uint8_t version;
    uint8_t message_type;
    uint16_t sequence;
    uint16_t payload_length;
    uint32_t checksum;
} bike_ble_packet_header_t;

bool bike_ble_protocol_version_supported(uint8_t version);

#endif /* BIKE_BLE_PROTOCOL_H */
