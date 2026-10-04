#ifndef OFFLINE_MODE_H
#define OFFLINE_MODE_H

#include <rtthread.h>

rt_err_t offline_mode_start(void);
rt_err_t offline_mode_pause(void);
rt_err_t offline_mode_stop(void);

#endif /* OFFLINE_MODE_H */
