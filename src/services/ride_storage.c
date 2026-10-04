#include "ride_storage.h"

static bike_service_state_t g_state = BIKE_SERVICE_DISABLED;

rt_err_t ride_storage_init(void)
{
    g_state = BIKE_SERVICE_DISABLED;
    return -RT_ENOSYS;
}

rt_err_t ride_storage_start(void) { return -RT_ENOSYS; }
rt_err_t ride_storage_stop(void) { return RT_EOK; }

rt_err_t ride_storage_append(const ride_sample_t *sample)
{
    if (sample == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

bike_service_state_t ride_storage_get_status(void) { return g_state; }
