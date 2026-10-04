#ifndef RIDE_STORAGE_H
#define RIDE_STORAGE_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t ride_storage_init(void);
rt_err_t ride_storage_start(void);
rt_err_t ride_storage_stop(void);
rt_err_t ride_storage_append(const ride_sample_t *sample);
bike_service_state_t ride_storage_get_status(void);

#endif /* RIDE_STORAGE_H */
