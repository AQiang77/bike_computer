#include "bike_ui.h"

#include "screen_navigation.h"

rt_err_t bike_ui_init(void)
{
    return screen_navigation_create();
}

rt_err_t bike_ui_show_navigation(const navigation_instruction_t *instruction)
{
    return screen_navigation_update(instruction);
}
