#include "ble_service.h"

static bike_service_state_t g_state = BIKE_SERVICE_DISABLED;

rt_err_t ble_service_init(void)
{
    g_state = BIKE_SERVICE_DISABLED;
    return -RT_ENOSYS;
}

rt_err_t ble_service_start(void) { return -RT_ENOSYS; }
rt_err_t ble_service_stop(void) { return RT_EOK; }

rt_err_t ble_service_send_sample(const ride_sample_t *sample)
{
    if (sample == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

bike_service_state_t ble_service_get_status(void) { return g_state; }
