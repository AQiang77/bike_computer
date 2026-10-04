#ifndef PHONE_NAV_MODE_H
#define PHONE_NAV_MODE_H

#include <rtthread.h>

rt_err_t phone_nav_mode_start(void);
rt_err_t phone_nav_mode_pause(void);
rt_err_t phone_nav_mode_stop(void);

#endif /* PHONE_NAV_MODE_H */
