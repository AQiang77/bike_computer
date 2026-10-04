#include "bike_app.h"

#include "bike_mode_manager.h"
#include "bike_ui.h"
#include "charger_service.h"
#include "navigation_service.h"
#include "ride_engine.h"
#include "littlevgl2rtt.h"
#include "lvgl.h"
#include <rtthread.h>

int bike_app_run(void)
{
    uint32_t delay_ms;
    rt_err_t result;
    const navigation_instruction_t demo_instruction = {
        .source = NAV_SOURCE_PHONE,
        .maneuver = NAV_MANEUVER_RIGHT,
        .next_distance_m = 120,
        .remaining_distance_m = 8600,
        .remaining_time_s = 24 * 60,
        .link_connected = false,
        .current_road = "Huancheng East Road",
        .next_road = "Riverside Road",
    };

    rt_kprintf("Bike navigation UI starting...\n");

    bike_mode_manager_init();
    ride_engine_init();
    navigation_service_init();

    result = charger_service_init();
    if (result != RT_EOK)
    {
        rt_kprintf("Bike navigation continues without charger control\n");
    }

    result = littlevgl2rtt_init("lcd");
    if (result != RT_EOK)
    {
        rt_kprintf("littlevgl2rtt_init failed: %d\n", result);
        return result;
    }

    bike_ui_init();
    navigation_service_publish(&demo_instruction);
    bike_ui_show_navigation(&demo_instruction);

    while (1)
    {
        delay_ms = lv_timer_handler();
        if (delay_ms < 2)
        {
            delay_ms = 2;
        }
        else if (delay_ms > 20)
        {
            delay_ms = 20;
        }
        rt_thread_mdelay(delay_ms);
    }
}
