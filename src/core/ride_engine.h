#ifndef RIDE_ENGINE_H
#define RIDE_ENGINE_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t ride_engine_init(void);
rt_err_t ride_engine_start(void);
rt_err_t ride_engine_pause(void);
rt_err_t ride_engine_stop(void);
ride_session_state_t ride_engine_get_state(void);

#endif /* RIDE_ENGINE_H */
