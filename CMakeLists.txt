if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  target_sources_ifdef(CONFIG_ZMK_FEATURE_SPLITBATTERY_REPORT app PRIVATE src/battery_report.c src/split_battery.c)
  zephyr_include_directories(include)
endif()
