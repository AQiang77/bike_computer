#ifndef MOTION_SERVICE_H
#define MOTION_SERVICE_H

#include "bike_types.h"
#include <rtthread.h>

typedef struct
{
    int32_t accel_mg[3];
    int32_t gyro_mdps[3];
    bool moving;
} motion_sample_t;

rt_err_t motion_service_init(void);
rt_err_t motion_service_start(void);
rt_err_t motion_service_stop(void);
rt_err_t motion_service_read(motion_sample_t *sample);
bike_service_state_t motion_service_get_status(void);

#endif /* MOTION_SERVICE_H */
