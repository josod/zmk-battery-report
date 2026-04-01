# Copyright (c) 2025 The ZMK Contributors
# SPDX-License-Identifier: MIT

target_sources_ifdef(CONFIG_ZMK_FEATURE_SPLITBATTERY_REPORT app PRIVATE src/battery_report.c)
zephyr_include_directories(include)
