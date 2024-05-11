#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <nus_service.h>
#include <dk_buttons_and_leds.h>
// #include "hid_mouse.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define RUN_STATUS_LED DK_LED1

int main(void)
{
	int blink_status = 0;
	int err = 0;

	configure_gpio();

	err = uart_init();
	if (err)
	{
		error();
	}

	init_nus_ble();

	// if (!init_hid_mouse())
	// {
	// 	LOG_ERR("Failed to init hid mouse");
	// 	return 0;
	// }

	for (;;)
	{
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		LOG_INF("while loop");
		k_sleep(K_MSEC(5000));
	}
}
