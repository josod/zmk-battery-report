#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(battery_report, LOG_LEVEL_INF);

#include <zmk/behavior.h>
#include <zmk/hid.h>

#include "battery_report.h"

/* externs from split file */
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
extern struct k_sem bat_sem;
extern bool peripheral_connected;
#endif

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
struct zmk_behavior_binding_event event) {

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

LOG_INF("Battery report key pressed");

/* Reset sync */
k_sem_reset(&bat_sem);
peripheral_connected = false;

/* Send request */
request_peripheral_battery();

/* Wait for response */
int ret = k_sem_take(&bat_sem, K_MSEC(200));

char buf[48];

if (ret == 0 && peripheral_connected) {
LOG_INF("Print L:%d(%dmV) R:%d(%dmV)",
zmk_central_battery,
zmk_central_voltage,
zmk_peripheral_battery,
zmk_peripheral_voltage);

snprintk(buf, sizeof(buf),
"L:%d%%(%dmV) R:%d%%(%dmV)",
zmk_central_battery,
zmk_central_voltage,
zmk_peripheral_battery,
zmk_peripheral_voltage);

} else {
LOG_INF("Timeout / NC");

snprintk(buf, sizeof(buf),
"L:%d%%(%dmV) R:NC",
zmk_central_battery,
zmk_central_voltage);
}

/* Type string */
for (int i = 0; buf[i]; i++) {
zmk_hid_keyboard_press(buf[i]);
k_msleep(5);
zmk_hid_keyboard_release(buf[i]);
}

#endif

return 0;
}

static const struct behavior_driver_api behavior_api = {
.binding_pressed = on_keymap_binding_pressed,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
POST_KERNEL,
CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
&behavior_api);
