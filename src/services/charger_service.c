#include "charger_service.h"

#include "sgm41562b_rt_device.h"
#include <rtdevice.h>
#include <string.h>

#define CHARGER_MONITOR_PERIOD_MS 5000
#define CHARGER_MONITOR_STACK_SIZE 1536
#define CHARGER_MONITOR_TIMESLICE 10
#define CHARGER_IO_ERROR_LIMIT 3

static rt_device_t g_charger_device;
static rt_thread_t g_monitor_thread;
static struct rt_mutex g_status_lock;
static struct rt_semaphore g_update_sem;
static bool g_ipc_initialized;
static charger_status_t g_status;
static charger_event_callback_t g_listener;
static void *g_listener_user_data;

static bool charger_config_valid(const charger_config_t *config)
{
    return config != RT_NULL &&
           config->input_voltage_limit_mv >= 3880 &&
           config->input_voltage_limit_mv <= 5080 &&
           config->input_current_limit_ma >= 50 &&
           config->input_current_limit_ma <= 500 &&
           config->charge_voltage_mv >= 3600 &&
           config->charge_voltage_mv <= 4545 &&
           config->charge_current_ma >= 8 &&
           config->charge_current_ma <= 456 &&
           config->thermal_regulation <= CHARGER_THERMAL_REGULATION_120C;
}

static charger_phase_t charger_decode_phase(
    const struct sgm41562b_rt_status *raw_status)
{
    if (raw_status->fault_flags != SGM41562B_RT_FAULT_NONE)
        return CHARGE_PHASE_FAULT;
    if (!raw_status->power_good)
        return CHARGE_PHASE_NO_INPUT;

    switch (raw_status->charge_status)
    {
    case 1:
        return CHARGE_PHASE_PRECHARGE;
    case 2:
        return CHARGE_PHASE_FAST_CHARGE;
    case 3:
        return CHARGE_PHASE_DONE;
    case 0:
    default:
        return CHARGE_PHASE_NOT_CHARGING;
    }
}

static uint32_t charger_decode_faults(uint32_t driver_faults)
{
    uint32_t faults = CHARGER_FAULT_NONE;

    if (driver_faults & SGM41562B_RT_FAULT_WATCHDOG)
        faults |= CHARGER_FAULT_WATCHDOG;
    if (driver_faults & SGM41562B_RT_FAULT_INPUT)
        faults |= CHARGER_FAULT_INPUT;
    if (driver_faults & SGM41562B_RT_FAULT_THERMAL_SHUTDOWN)
        faults |= CHARGER_FAULT_THERMAL_SHUTDOWN;
    if (driver_faults & SGM41562B_RT_FAULT_BATTERY)
        faults |= CHARGER_FAULT_BATTERY;
    if (driver_faults & SGM41562B_RT_FAULT_SAFETY_TIMER)
        faults |= CHARGER_FAULT_SAFETY_TIMER;
    if (driver_faults & SGM41562B_RT_FAULT_NTC_HOT)
        faults |= CHARGER_FAULT_NTC_HOT;
    if (driver_faults & SGM41562B_RT_FAULT_NTC_COLD)
        faults |= CHARGER_FAULT_NTC_COLD;
    return faults;
}

static uint32_t charger_build_events(const charger_status_t *old_status,
                                     const charger_status_t *new_status)
{
    uint32_t events = CHARGER_EVENT_NONE;

    if (old_status->input_present != new_status->input_present)
        events |= new_status->input_present
                      ? CHARGER_EVENT_INPUT_CONNECTED
                      : CHARGER_EVENT_INPUT_DISCONNECTED;
    if (old_status->phase != new_status->phase)
    {
        events |= CHARGER_EVENT_PHASE_CHANGED;
        if (new_status->phase == CHARGE_PHASE_DONE)
            events |= CHARGER_EVENT_CHARGE_DONE;
    }
    if (old_status->fault_flags != new_status->fault_flags)
    {
        if (new_status->fault_flags != CHARGER_FAULT_NONE)
            events |= CHARGER_EVENT_FAULT;
        else if (old_status->fault_flags != CHARGER_FAULT_NONE)
            events |= CHARGER_EVENT_RECOVERED;
    }
    if (old_status->service_state != new_status->service_state)
    {
        if (new_status->service_state == CHARGER_SERVICE_ERROR)
            events |= CHARGER_EVENT_SERVICE_ERROR;
        else if (old_status->service_state == CHARGER_SERVICE_ERROR)
            events |= CHARGER_EVENT_RECOVERED;
    }
    if (events != CHARGER_EVENT_NONE ||
        old_status->raw_system_status != new_status->raw_system_status ||
        old_status->raw_fault_status != new_status->raw_fault_status ||
        old_status->thermal_regulation != new_status->thermal_regulation)
        events |= CHARGER_EVENT_STATUS_CHANGED;
    return events;
}

static void charger_notify(uint32_t event_mask,
                           const charger_status_t *status)
{
    charger_event_callback_t listener;
    void *user_data;

    if (event_mask == CHARGER_EVENT_NONE)
        return;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    listener = g_listener;
    user_data = g_listener_user_data;
    rt_mutex_release(&g_status_lock);

    if (listener != RT_NULL)
        listener(event_mask, status, user_data);
}

static rt_err_t charger_read_hardware(charger_status_t *status)
{
    struct sgm41562b_rt_status raw_status = {0};

    if (rt_device_read(g_charger_device, 0, &raw_status, 1) != 1)
        return -RT_ERROR;

    status->raw_system_status = raw_status.system_status;
    status->raw_fault_status = raw_status.fault_status;
    status->input_present = raw_status.power_good != 0;
    status->thermal_regulation = raw_status.thermal_regulation != 0;
    status->fault_flags = charger_decode_faults(raw_status.fault_flags);
    status->phase = charger_decode_phase(&raw_status);
    status->updated_tick = rt_tick_get();
    return RT_EOK;
}

static rt_err_t charger_refresh(void)
{
    charger_status_t old_status;
    charger_status_t new_status;
    uint32_t events;
    rt_err_t result;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    old_status = g_status;
    new_status = g_status;
    rt_mutex_release(&g_status_lock);

    result = charger_read_hardware(&new_status);
    if (result != RT_EOK)
    {
        new_status.io_error_count++;
        if (new_status.io_error_count >= CHARGER_IO_ERROR_LIMIT)
            new_status.service_state = CHARGER_SERVICE_ERROR;
    }
    else
    {
        new_status.io_error_count = 0;
        new_status.service_state = CHARGER_SERVICE_READY;
    }

    events = charger_build_events(&old_status, &new_status);
    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status = new_status;
    rt_mutex_release(&g_status_lock);
    charger_notify(events, &new_status);
    return result;
}

static rt_err_t charger_rx_indicate(rt_device_t device, rt_size_t size)
{
    (void)device;
    (void)size;
    rt_sem_release(&g_update_sem);
    return RT_EOK;
}

static void charger_monitor_entry(void *parameter)
{
    (void)parameter;
    while (1)
    {
        rt_sem_take(&g_update_sem,
                    rt_tick_from_millisecond(CHARGER_MONITOR_PERIOD_MS));
        charger_refresh();
    }
}

static rt_err_t charger_control_u16(int command, uint16_t value)
{
    rt_uint16_t driver_value = value;
    return rt_device_control(g_charger_device, command, &driver_value);
}

static rt_err_t charger_apply_config(const charger_config_t *config)
{
    rt_uint8_t thermal = (rt_uint8_t)config->thermal_regulation;
    rt_bool_t enable = config->charging_enabled_on_boot ? RT_TRUE : RT_FALSE;
    rt_err_t result;

    result = charger_control_u16(SGM41562B_RT_CTRL_SET_INPUT_VOLTAGE,
                                 config->input_voltage_limit_mv);
    if (result != RT_EOK)
        return result;
    result = charger_control_u16(SGM41562B_RT_CTRL_SET_INPUT_CURRENT,
                                 config->input_current_limit_ma);
    if (result != RT_EOK)
        return result;
    result = charger_control_u16(SGM41562B_RT_CTRL_SET_CHARGE_VOLTAGE,
                                 config->charge_voltage_mv);
    if (result != RT_EOK)
        return result;
    result = charger_control_u16(SGM41562B_RT_CTRL_SET_CHARGE_CURRENT,
                                 config->charge_current_ma);
    if (result != RT_EOK)
        return result;
    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_SET_THERMAL_REGULATION,
                               &thermal);
    if (result != RT_EOK)
        return result;
    return rt_device_control(g_charger_device,
                             SGM41562B_RT_CTRL_ENABLE_CHARGING, &enable);
}

static void charger_cleanup_failed_init(void)
{
    if (g_charger_device != RT_NULL)
    {
        rt_bool_t disable = RT_FALSE;

        rt_device_control(g_charger_device,
                          SGM41562B_RT_CTRL_ENABLE_CHARGING, &disable);
        rt_device_set_rx_indicate(g_charger_device, RT_NULL);
        rt_device_close(g_charger_device);
        g_charger_device = RT_NULL;
    }

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status.service_state = CHARGER_SERVICE_ERROR;
    rt_mutex_release(&g_status_lock);
}

void charger_service_get_default_config(charger_config_t *config)
{
    if (config == RT_NULL)
        return;

    /* Validate these defaults against the production cell and power design. */
    config->input_voltage_limit_mv = 4600;
    config->input_current_limit_ma = 500;
    config->charge_voltage_mv = 4200;
    config->charge_current_ma = 350;
    config->thermal_regulation = CHARGER_THERMAL_REGULATION_100C;
    config->charging_enabled_on_boot = true;
}

rt_err_t charger_service_init(const charger_config_t *config)
{
    charger_config_t default_config;
    charger_status_t initial_status = {0};
    rt_err_t result;

    if (config == RT_NULL)
    {
        charger_service_get_default_config(&default_config);
        config = &default_config;
    }
    if (!charger_config_valid(config))
        return -RT_EINVAL;

    if (!g_ipc_initialized)
    {
        rt_mutex_init(&g_status_lock, "chg_stat", RT_IPC_FLAG_PRIO);
        rt_sem_init(&g_update_sem, "chg_evt", 0, RT_IPC_FLAG_FIFO);
        g_ipc_initialized = true;
    }

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    if (g_status.service_state == CHARGER_SERVICE_READY)
    {
        rt_mutex_release(&g_status_lock);
        return RT_EOK;
    }
    memset(&g_status, 0, sizeof(g_status));
    g_status.service_state = CHARGER_SERVICE_UNINITIALIZED;
    rt_mutex_release(&g_status_lock);

    g_charger_device = rt_device_find(SGM41562B_RT_DEVICE_NAME);
    if (g_charger_device == RT_NULL)
    {
        charger_cleanup_failed_init();
        rt_kprintf("SGM41562B device not found\n");
        return -RT_ENOSYS;
    }

    result = rt_device_open(g_charger_device, RT_DEVICE_OFLAG_RDWR);
    if (result != RT_EOK)
    {
        charger_cleanup_failed_init();
        rt_kprintf("SGM41562B open failed: %d\n", result);
        return result;
    }

    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_GET_DEVICE_ID,
                               &initial_status.device_id);
    if (result != RT_EOK)
        goto init_failed;

    result = charger_apply_config(config);
    if (result != RT_EOK)
        goto init_failed;

    initial_status.service_state = CHARGER_SERVICE_READY;
    initial_status.charging_enabled = config->charging_enabled_on_boot;
    initial_status.input_voltage_limit_mv = config->input_voltage_limit_mv;
    initial_status.input_current_limit_ma = config->input_current_limit_ma;
    initial_status.charge_voltage_mv = config->charge_voltage_mv;
    initial_status.charge_current_ma = config->charge_current_ma;
    result = charger_read_hardware(&initial_status);
    if (result != RT_EOK)
        goto init_failed;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status = initial_status;
    rt_mutex_release(&g_status_lock);

    rt_device_set_rx_indicate(g_charger_device, charger_rx_indicate);
    if (g_monitor_thread == RT_NULL)
    {
        g_monitor_thread = rt_thread_create(
            "chg_srv", charger_monitor_entry, RT_NULL,
            CHARGER_MONITOR_STACK_SIZE, RT_THREAD_PRIORITY_MIDDLE + 1,
            CHARGER_MONITOR_TIMESLICE);
        if (g_monitor_thread == RT_NULL)
        {
            result = -RT_ENOMEM;
            goto init_failed;
        }
        rt_thread_startup(g_monitor_thread);
    }

    rt_kprintf("SGM41562B ready: id=0x%02x, input=%u, phase=%u, "
               "faults=0x%08x\n",
               initial_status.device_id, initial_status.input_present,
               initial_status.phase, initial_status.fault_flags);
    return RT_EOK;

init_failed:
    charger_cleanup_failed_init();
    rt_kprintf("SGM41562B initialization failed: %d\n", result);
    return result;
}

rt_err_t charger_service_set_enabled(bool enabled)
{
    charger_status_t status;
    rt_bool_t driver_enabled = enabled ? RT_TRUE : RT_FALSE;
    rt_err_t result;

    if (g_charger_device == RT_NULL)
        return -RT_ENOSYS;

    result = rt_device_control(g_charger_device,
                               SGM41562B_RT_CTRL_ENABLE_CHARGING,
                               &driver_enabled);
    if (result != RT_EOK)
        return result;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status.charging_enabled = enabled;
    status = g_status;
    rt_mutex_release(&g_status_lock);
    charger_notify(CHARGER_EVENT_STATUS_CHANGED | CHARGER_EVENT_CONFIG_CHANGED,
                   &status);
    rt_sem_release(&g_update_sem);
    return RT_EOK;
}

rt_err_t charger_service_set_input_current(uint16_t current_ma)
{
    charger_status_t status;
    rt_err_t result;

    if (g_charger_device == RT_NULL)
        return -RT_ENOSYS;
    result = charger_control_u16(SGM41562B_RT_CTRL_SET_INPUT_CURRENT,
                                 current_ma);
    if (result != RT_EOK)
        return result;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status.input_current_limit_ma = current_ma;
    status = g_status;
    rt_mutex_release(&g_status_lock);
    charger_notify(CHARGER_EVENT_STATUS_CHANGED | CHARGER_EVENT_CONFIG_CHANGED,
                   &status);
    return RT_EOK;
}

rt_err_t charger_service_set_charge_current(uint16_t current_ma)
{
    charger_status_t status;
    rt_err_t result;

    if (g_charger_device == RT_NULL)
        return -RT_ENOSYS;
    result = charger_control_u16(SGM41562B_RT_CTRL_SET_CHARGE_CURRENT,
                                 current_ma);
    if (result != RT_EOK)
        return result;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_status.charge_current_ma = current_ma;
    status = g_status;
    rt_mutex_release(&g_status_lock);
    charger_notify(CHARGER_EVENT_STATUS_CHANGED | CHARGER_EVENT_CONFIG_CHANGED,
                   &status);
    return RT_EOK;
}

rt_err_t charger_service_get_snapshot(charger_status_t *status)
{
    if (status == RT_NULL)
        return -RT_EINVAL;
    if (!g_ipc_initialized)
        return -RT_ENOSYS;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    *status = g_status;
    rt_mutex_release(&g_status_lock);
    return RT_EOK;
}

rt_err_t charger_service_register_listener(charger_event_callback_t callback,
                                           void *user_data)
{
    if (!g_ipc_initialized)
        return -RT_ENOSYS;

    rt_mutex_take(&g_status_lock, RT_WAITING_FOREVER);
    g_listener = callback;
    g_listener_user_data = user_data;
    rt_mutex_release(&g_status_lock);
    return RT_EOK;
}

rt_err_t charger_service_enter_shipping_mode(void)
{
    if (g_charger_device == RT_NULL)
        return -RT_ENOSYS;
    return rt_device_control(g_charger_device,
                             SGM41562B_RT_CTRL_ENTER_SHIPPING, RT_NULL);
}
