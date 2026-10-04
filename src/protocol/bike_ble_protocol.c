#include "bike_ble_protocol.h"

bool bike_ble_protocol_version_supported(uint8_t version)
{
    return version == BIKE_BLE_PROTOCOL_VERSION;
}
