#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>

LOG_MODULE_REGISTER(EXAMPLE, LOG_LEVEL_DBG);

int main(void)
{
    while (1)
    {   
        k_sleep(K_MSEC(2000));
        LOG_INF("While loop :)\n");
        LOG_ERR("While loop :)\n");
    }
    return 0;
}