#ifndef SCREEN_NAVIGATION_H
#define SCREEN_NAVIGATION_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t screen_navigation_create(void);
rt_err_t screen_navigation_update(const navigation_instruction_t *instruction);

#endif /* SCREEN_NAVIGATION_H */
