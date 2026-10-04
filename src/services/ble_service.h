#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t ble_service_init(void);
rt_err_t ble_service_start(void);
rt_err_t ble_service_stop(void);
rt_err_t ble_service_send_sample(const ride_sample_t *sample);
bike_service_state_t ble_service_get_status(void);

#endif /* BLE_SERVICE_H */
