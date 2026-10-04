#include "gps_service.h"

static bike_service_state_t g_state = BIKE_SERVICE_DISABLED;

rt_err_t gps_service_init(void)
{
    g_state = BIKE_SERVICE_DISABLED;
    return -RT_ENOSYS;
}

rt_err_t gps_service_start(void) { return -RT_ENOSYS; }
rt_err_t gps_service_stop(void) { return RT_EOK; }

rt_err_t gps_service_read(ride_sample_t *sample)
{
    if (sample == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

bike_service_state_t gps_service_get_status(void) { return g_state; }
