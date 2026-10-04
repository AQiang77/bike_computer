#include "bike_mode_manager.h"

#include "ride_engine.h"

static bike_mode_t g_current_mode = BIKE_MODE_OFFLINE;

rt_err_t bike_mode_manager_init(void)
{
    g_current_mode = BIKE_MODE_OFFLINE;
    return RT_EOK;
}

bike_mode_t bike_mode_manager_get(void)
{
    return g_current_mode;
}

rt_err_t bike_mode_manager_set(bike_mode_t mode)
{
    if (mode >= BIKE_MODE_COUNT)
    {
        return -RT_EINVAL;
    }
    if (ride_engine_get_state() == RIDE_SESSION_RECORDING ||
        ride_engine_get_state() == RIDE_SESSION_PAUSED)
    {
        return -RT_EBUSY;
    }
    g_current_mode = mode;
    return RT_EOK;
}

rt_err_t bike_mode_manager_start(void)
{
    return -RT_ENOSYS;
}

rt_err_t bike_mode_manager_pause(void)
{
    return -RT_ENOSYS;
}

rt_err_t bike_mode_manager_stop(void)
{
    return -RT_ENOSYS;
}
