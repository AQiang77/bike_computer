#ifndef GPS_SERVICE_H
#define GPS_SERVICE_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t gps_service_init(void);
rt_err_t gps_service_start(void);
rt_err_t gps_service_stop(void);
rt_err_t gps_service_read(ride_sample_t *sample);
bike_service_state_t gps_service_get_status(void);

#endif /* GPS_SERVICE_H */
