#ifndef BIKE_UI_H
#define BIKE_UI_H

#include "bike_types.h"
#include <rtthread.h>

rt_err_t bike_ui_init(void);
rt_err_t bike_ui_show_navigation(const navigation_instruction_t *instruction);

#endif /* BIKE_UI_H */
