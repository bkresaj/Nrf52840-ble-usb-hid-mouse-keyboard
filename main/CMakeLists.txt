cmake_minimum_required(VERSION 3.20)

set(BOARD nrf52840dk_nrf52840)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf-usb)

target_sources(app PRIVATE main.c)

target_sources_ifdef(CONFIG_BT_NUS_UART_ASYNC_ADAPTER app PRIVATE
  uart_async_adapter.c
)

zephyr_library_include_directories(.)