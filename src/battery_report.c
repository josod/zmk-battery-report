
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/bluetooth/service.h>

#include "battery_report.h"

/* =========================
* Shared state
* ========================= */

uint8_t zmk_central_battery = 0;
uint16_t zmk_central_voltage = 0;

uint8_t zmk_peripheral_battery = 0;
uint16_t zmk_peripheral_voltage = 0;

/* =========================
* Sync (central only)
* ========================= */

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
K_SEM_DEFINE(bat_sem, 0, 1);
bool peripheral_connected = false;
#endif

/* =========================
* Battery event (local)
* ========================= */

static int battery_listener(const zmk_event_t *eh) {
const struct zmk_battery_state_changed *ev =
as_zmk_battery_state_changed(eh);

if (!ev) return 0;

zmk_central_battery = ev->state_of_charge;
zmk_central_voltage = ev->voltage;

return 0;
}

ZMK_LISTENER(battery_report_listener, battery_listener);
ZMK_SUBSCRIPTION(battery_report_listener, zmk_battery_state_changed);

/* =========================
* Split messaging
* ========================= */

enum bat_cmd {
BAT_REQ = 1,
BAT_RESP = 2,
};

struct bat_msg {
uint8_t cmd;
uint8_t level;
uint16_t voltage_mv;
};

static void bat_received(const void *data, size_t len) {
if (len != sizeof(struct bat_msg)) return;

const struct bat_msg *msg = data;

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_PERIPHERAL)

if (msg->cmd == BAT_REQ) {
LOG_INF("REQ → send %d%% %dmV",
zmk_central_battery,
zmk_central_voltage);

struct bat_msg resp = {
.cmd = BAT_RESP,
.level = zmk_central_battery,
.voltage_mv = zmk_central_voltage,
};

zmk_split_bt_service_send(&resp, sizeof(resp));
}

#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

if (msg->cmd == BAT_RESP) {
zmk_peripheral_battery = msg->level;
zmk_peripheral_voltage = msg->voltage_mv;

peripheral_connected = true;

LOG_INF("RESP: %d%% %dmV",
msg->level,
msg->voltage_mv);

k_sem_give(&bat_sem);
}

#endif
}

ZMK_SPLIT_BLE_SERVICE_REGISTER(battery_service, bat_received);

/* =========================
* Request API (central)
* ========================= */

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

void request_peripheral_battery(void) {
struct bat_msg req = {
.cmd = BAT_REQ,
.level = 0,
.voltage_mv = 0,
};

LOG_INF("Sending BAT_REQ");

zmk_split_bt_service_send(&req, sizeof(req));
}
