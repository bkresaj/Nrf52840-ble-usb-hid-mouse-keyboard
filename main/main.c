#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <ble_service.h>
#include <dk_buttons_and_leds.h>
#include "hid_mouse.h"
#include "hid_keyboard.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define RUN_STATUS_LED DK_LED1

int main(void)
{
	int blink_status = 0;
	bool is_usb_enabled = false;

	if (!init_hid_mouse(&is_usb_enabled))
	{
		LOG_ERR("Failed to init hid mouse");
		return 0;
	}

	configure_gpio();

	init_ble();

	if (!init_hid_keyboard(&is_usb_enabled))
	{
		LOG_ERR("Failed to init hid keyboard");
		return 0;
	}

	for (;;)
	{
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		LOG_INF("while loop");
		k_sleep(K_MSEC(5000));
	}
}
