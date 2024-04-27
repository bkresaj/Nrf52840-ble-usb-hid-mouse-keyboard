cmake_minimum_required(VERSION 3.20)

set(BOARD nrf52840dk_nrf52840)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(nrf-usb)

target_sources(app PRIVATE main.c)
