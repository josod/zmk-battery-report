
#pragma once
#include <stdint.h>

/* Central values */
extern uint8_t zmk_central_battery;
extern uint16_t zmk_central_voltage;

/* Peripheral values */
extern uint8_t zmk_peripheral_battery;
extern uint16_t zmk_peripheral_voltage;

/* Request API (central only) */
void request_peripheral_battery(void);
