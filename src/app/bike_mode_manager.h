#ifndef BIKE_MODE_MANAGER_H
#define BIKE_MODE_MANAGER_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t bike_mode_manager_init(void);
bike_mode_t bike_mode_manager_get(void);
rt_err_t bike_mode_manager_set(bike_mode_t mode);
rt_err_t bike_mode_manager_start(void);
rt_err_t bike_mode_manager_pause(void);
rt_err_t bike_mode_manager_stop(void);

#endif /* BIKE_MODE_MANAGER_H */
