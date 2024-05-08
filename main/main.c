
int main(void)
{
	int blink_status = 0;
	int err = 0;

	configure_gpio();

	err = uart_init();
	if (err) {
		error();
	}

	init_nus_ble();

	for (;;) {
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL * 2));
	}
}
