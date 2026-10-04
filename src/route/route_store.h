#ifndef ROUTE_STORE_H
#define ROUTE_STORE_H

#include "route_format.h"
#include <rtthread.h>

rt_err_t route_store_init(void);
rt_err_t route_store_open(const char *route_name);
rt_err_t route_store_close(void);
rt_err_t route_store_read_point(uint32_t index, bike_route_point_t *point);

#endif /* ROUTE_STORE_H */
