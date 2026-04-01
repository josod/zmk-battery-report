/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_report

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

LOG_MODULE_REGISTER(battery_report, CONFIG_ZMK_LOG_LEVEL);

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
  LOG_INF("Battery report key pressed");
  return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
  return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_battery_report_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

static int behavior_battery_report_init(const struct device *dev) { return 0; }

#define BAT_REPORT_INST(n)                                                     \
  BEHAVIOR_DT_INST_DEFINE(n, behavior_battery_report_init, NULL, NULL, NULL,   \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,    \
                          &behavior_battery_report_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BAT_REPORT_INST)
