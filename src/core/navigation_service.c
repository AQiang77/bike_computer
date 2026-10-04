#include "navigation_service.h"

#include <string.h>

static navigation_instruction_t g_instruction;
static bool g_has_instruction;

rt_err_t navigation_service_init(void)
{
    navigation_service_clear();
    return RT_EOK;
}

rt_err_t navigation_service_publish(const navigation_instruction_t *instruction)
{
    if (instruction == RT_NULL)
    {
        return -RT_EINVAL;
    }
    memcpy(&g_instruction, instruction, sizeof(g_instruction));
    g_instruction.current_road[BIKE_NAV_TEXT_MAX_LEN - 1] = '\0';
    g_instruction.next_road[BIKE_NAV_TEXT_MAX_LEN - 1] = '\0';
    g_has_instruction = true;
    return RT_EOK;
}

rt_err_t navigation_service_get_latest(navigation_instruction_t *instruction)
{
    if (instruction == RT_NULL)
    {
        return -RT_EINVAL;
    }
    if (!g_has_instruction)
    {
        return -RT_EEMPTY;
    }
    memcpy(instruction, &g_instruction, sizeof(*instruction));
    return RT_EOK;
}

void navigation_service_clear(void)
{
    memset(&g_instruction, 0, sizeof(g_instruction));
    g_has_instruction = false;
}
