#ifndef NAVIGATION_SERVICE_H
#define NAVIGATION_SERVICE_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t navigation_service_init(void);
rt_err_t navigation_service_publish(const navigation_instruction_t *instruction);
rt_err_t navigation_service_get_latest(navigation_instruction_t *instruction);
void navigation_service_clear(void);

#endif /* NAVIGATION_SERVICE_H */
