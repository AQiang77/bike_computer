#ifndef CHARGER_SERVICE_H
#define CHARGER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <rtthread.h>

typedef enum
{
    CHARGER_SERVICE_UNINITIALIZED = 0,
    CHARGER_SERVICE_READY,
    CHARGER_SERVICE_ERROR,
} charger_service_state_t;

typedef enum
{
    CHARGE_PHASE_NO_INPUT = 0,
    CHARGE_PHASE_NOT_CHARGING,
    CHARGE_PHASE_PRECHARGE,
    CHARGE_PHASE_FAST_CHARGE,
    CHARGE_PHASE_DONE,
    CHARGE_PHASE_FAULT,
} charger_phase_t;

typedef enum
{
    CHARGER_THERMAL_REGULATION_60C = 0,
    CHARGER_THERMAL_REGULATION_80C,
    CHARGER_THERMAL_REGULATION_100C,
    CHARGER_THERMAL_REGULATION_120C,
} charger_thermal_regulation_t;

enum
{
    CHARGER_FAULT_NONE = 0,
    CHARGER_FAULT_WATCHDOG = (1UL << 0),
    CHARGER_FAULT_INPUT = (1UL << 1),
    CHARGER_FAULT_THERMAL_SHUTDOWN = (1UL << 2),
    CHARGER_FAULT_BATTERY = (1UL << 3),
    CHARGER_FAULT_SAFETY_TIMER = (1UL << 4),
    CHARGER_FAULT_NTC_HOT = (1UL << 5),
    CHARGER_FAULT_NTC_COLD = (1UL << 6),
};

enum
{
    CHARGER_EVENT_NONE = 0,
    CHARGER_EVENT_STATUS_CHANGED = (1UL << 0),
    CHARGER_EVENT_INPUT_CONNECTED = (1UL << 1),
    CHARGER_EVENT_INPUT_DISCONNECTED = (1UL << 2),
    CHARGER_EVENT_PHASE_CHANGED = (1UL << 3),
    CHARGER_EVENT_CHARGE_DONE = (1UL << 4),
    CHARGER_EVENT_FAULT = (1UL << 5),
    CHARGER_EVENT_RECOVERED = (1UL << 6),
    CHARGER_EVENT_SERVICE_ERROR = (1UL << 7),
    CHARGER_EVENT_CONFIG_CHANGED = (1UL << 8),
};

typedef struct
{
    uint16_t input_voltage_limit_mv;
    uint16_t input_current_limit_ma;
    uint16_t charge_voltage_mv;
    uint16_t charge_current_ma;
    charger_thermal_regulation_t thermal_regulation;
    bool charging_enabled_on_boot;
} charger_config_t;

typedef struct
{
    charger_service_state_t service_state;
    charger_phase_t phase;
    uint32_t fault_flags;
    uint32_t updated_tick;
    uint32_t io_error_count;
    uint16_t input_voltage_limit_mv;
    uint16_t input_current_limit_ma;
    uint16_t charge_voltage_mv;
    uint16_t charge_current_ma;
    uint8_t device_id;
    uint8_t raw_system_status;
    uint8_t raw_fault_status;
    bool input_present;
    bool charging_enabled;
    bool thermal_regulation;
} charger_status_t;

typedef void (*charger_event_callback_t)(uint32_t event_mask,
                                         const charger_status_t *status,
                                         void *user_data);

void charger_service_get_default_config(charger_config_t *config);
rt_err_t charger_service_init(const charger_config_t *config);
rt_err_t charger_service_set_enabled(bool enabled);
rt_err_t charger_service_set_input_current(uint16_t current_ma);
rt_err_t charger_service_set_charge_current(uint16_t current_ma);
rt_err_t charger_service_get_snapshot(charger_status_t *status);
rt_err_t charger_service_register_listener(charger_event_callback_t callback,
                                           void *user_data);
rt_err_t charger_service_enter_shipping_mode(void);

#endif /* CHARGER_SERVICE_H */
