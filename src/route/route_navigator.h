#ifndef ROUTE_NAVIGATOR_H
#define ROUTE_NAVIGATOR_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t route_navigator_init(void);
rt_err_t route_navigator_start(const char *route_name);
rt_err_t route_navigator_update(const ride_sample_t *position,
                                navigation_instruction_t *instruction);
rt_err_t route_navigator_stop(void);

#endif /* ROUTE_NAVIGATOR_H */
