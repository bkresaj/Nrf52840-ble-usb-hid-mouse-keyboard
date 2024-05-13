cmake_minimum_required(VERSION 3.20)

set(BOARD nrf52840dk_nrf52840)
set(CONF_FILE "prj.conf")
set(DTC_OVERLAY_FILE "app.overlay")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf-usb)

target_sources(app PRIVATE 
              main.c
              src/hid_keyboard.c
              src/hid_mouse.c
              src/ble_service.c)

zephyr_library_include_directories(inc)