#ifndef CHARGER_SERVICE_H
#define CHARGER_SERVICE_H

#include "bike_types.h"
#include <rtthread.h>

typedef struct
{
    uint8_t device_id;
    uint8_t system_status;
    uint8_t fault_status;
    uint8_t charge_status;
    bool power_good;
} charger_service_status_t;

rt_err_t charger_service_init(void);
rt_err_t charger_service_start(void);
rt_err_t charger_service_stop(void);
rt_err_t charger_service_read(charger_service_status_t *status);
bike_service_state_t charger_service_get_status(void);

#endif /* CHARGER_SERVICE_H */
