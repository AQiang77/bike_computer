#include "ride_engine.h"

static ride_session_state_t g_ride_state = RIDE_SESSION_IDLE;

rt_err_t ride_engine_init(void)
{
    g_ride_state = RIDE_SESSION_IDLE;
    return RT_EOK;
}

rt_err_t ride_engine_start(void)
{
    if (g_ride_state != RIDE_SESSION_IDLE &&
        g_ride_state != RIDE_SESSION_PAUSED &&
        g_ride_state != RIDE_SESSION_FINISHED)
    {
        return -RT_EBUSY;
    }
    g_ride_state = RIDE_SESSION_RECORDING;
    return RT_EOK;
}

rt_err_t ride_engine_pause(void)
{
    if (g_ride_state != RIDE_SESSION_RECORDING)
    {
        return -RT_ERROR;
    }
    g_ride_state = RIDE_SESSION_PAUSED;
    return RT_EOK;
}

rt_err_t ride_engine_stop(void)
{
    if (g_ride_state != RIDE_SESSION_RECORDING &&
        g_ride_state != RIDE_SESSION_PAUSED)
    {
        return -RT_ERROR;
    }
    g_ride_state = RIDE_SESSION_FINISHED;
    return RT_EOK;
}

ride_session_state_t ride_engine_get_state(void)
{
    return g_ride_state;
}
