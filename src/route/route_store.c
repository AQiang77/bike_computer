#include "route_store.h"

rt_err_t route_store_init(void) { return -RT_ENOSYS; }

rt_err_t route_store_open(const char *route_name)
{
    if (route_name == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}

rt_err_t route_store_close(void) { return -RT_ENOSYS; }

rt_err_t route_store_read_point(uint32_t index, bike_route_point_t *point)
{
    (void)index;
    if (point == RT_NULL)
    {
        return -RT_EINVAL;
    }
    return -RT_ENOSYS;
}
