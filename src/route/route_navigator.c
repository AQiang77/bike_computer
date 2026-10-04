#include "route_navigator.h"

rt_err_t route_navigator_init(void) { return -RT_ENOSYS; }

rt_err_t route_navigator_start(const char *route_name)
{
    if (route_name == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

rt_err_t route_navigator_update(const ride_sample_t *position,
                                navigation_instruction_t *instruction)
{
    if (position == RT_NULL || instruction == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

rt_err_t route_navigator_stop(void) { return -RT_ENOSYS; }
