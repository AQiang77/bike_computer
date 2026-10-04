#include "charger_service.h"

#include "sgm41562b_rt_device.h"
#include <rtdevice.h>

static rt_device_t g_charger_device;
static bike_service_state_t g_state = BIKE_SERVICE_UNINITIALIZED;

rt_err_t charger_service_read(charger_service_status_t *status)
{
    struct sgm41562b_rt_status raw_status = {0};
    rt_err_t result;

    if (status == RT_NULL)
    {
        return -RT_EINVAL;
    }
    if (g_charger_device == RT_NULL)
    {
        return -RT_ENOSYS;
    }

    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_GET_DEVICE_ID,
                               &status->device_id);
    if (result != RT_EOK)
    {
        return result;
    }
    if (rt_device_read(g_charger_device, 0, &raw_status, 1) != 1)
    {
        return -RT_ERROR;
    }

    status->system_status = raw_status.system_status;
    status->fault_status = raw_status.fault_status;
    status->charge_status = raw_status.charge_status;
    status->power_good = raw_status.power_good != 0;
    return RT_EOK;
}

rt_err_t charger_service_init(void)
{
    charger_service_status_t status = {0};
    rt_err_t result;

    if (g_state == BIKE_SERVICE_READY || g_state == BIKE_SERVICE_RUNNING)
    {
        return RT_EOK;
    }

    g_charger_device = rt_device_find(SGM41562B_RT_DEVICE_NAME);
    if (g_charger_device == RT_NULL)
    {
        g_state = BIKE_SERVICE_ERROR;
        rt_kprintf("SGM41562B device not found\n");
        return -RT_ENOSYS;
    }

    result = rt_device_open(g_charger_device, RT_DEVICE_OFLAG_RDWR);
    if (result != RT_EOK)
    {
        g_state = BIKE_SERVICE_ERROR;
        g_charger_device = RT_NULL;
        rt_kprintf("SGM41562B open failed: %d\n", result);
        return result;
    }

    result = charger_service_read(&status);
    if (result != RT_EOK)
    {
        g_state = BIKE_SERVICE_ERROR;
        rt_kprintf("SGM41562B read status failed: %d\n", result);
        return result;
    }

    g_state = BIKE_SERVICE_RUNNING;
    rt_kprintf("SGM41562B ready: id=0x%02x, power_good=%u, charge=%u, "
               "status=0x%02x, fault=0x%02x\n",
               status.device_id, status.power_good, status.charge_status,
               status.system_status, status.fault_status);
    return RT_EOK;
}

rt_err_t charger_service_start(void)
{
    rt_bool_t enable = RT_TRUE;
    rt_err_t result;

    result = charger_service_init();
    if (result != RT_EOK)
    {
        return result;
    }
    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_ENABLE_CHARGING,
                               &enable);
    if (result == RT_EOK)
    {
        g_state = BIKE_SERVICE_RUNNING;
    }
    return result;
}

rt_err_t charger_service_stop(void)
{
    rt_bool_t enable = RT_FALSE;
    rt_err_t result;

    if (g_charger_device == RT_NULL)
    {
        return -RT_ENOSYS;
    }
    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_ENABLE_CHARGING,
                               &enable);
    if (result == RT_EOK)
    {
        g_state = BIKE_SERVICE_READY;
    }
    return result;
}

bike_service_state_t charger_service_get_status(void)
{
    return g_state;
}
