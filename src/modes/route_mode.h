#ifndef ROUTE_MODE_H
#define ROUTE_MODE_H

#include <rtthread.h>

rt_err_t route_mode_start(void);
rt_err_t route_mode_pause(void);
rt_err_t route_mode_stop(void);

#endif /* ROUTE_MODE_H */
