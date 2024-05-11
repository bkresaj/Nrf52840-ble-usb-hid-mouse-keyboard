#include "hid_mouse.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>
#include <zephyr/sys/util.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hid_mouse, LOG_LEVEL_INF);

#define MOUSE_DEV_NAME "HID_0"
#define MOUSE_REPORT_ID 0
#define MOUSE_REPORT_SIZE 4
#define MOUSE_BUTTON_LEFT 0x01
#define MOUSE_BUTTON_RIGHT 0x02

#define MOUSE_BTN_LEFT 0
#define MOUSE_BTN_RIGHT 1

static uint8_t mouse_report[MOUSE_REPORT_SIZE];

static const struct device *hid_dev = NULL;
static const uint8_t hid_report_desc[] = HID_MOUSE_REPORT_DESC(2);
static enum usb_dc_status_code usb_status;

enum mouse_report_idx
{
    MOUSE_BTN_REPORT_IDX = 0,
    MOUSE_X_REPORT_IDX = 1,
    MOUSE_Y_REPORT_IDX = 2,
    MOUSE_WHEEL_REPORT_IDX = 3,
    MOUSE_REPORT_COUNT = 4,
};

static ALWAYS_INLINE void rwup_if_suspended(void)
{
    if (IS_ENABLED(CONFIG_USB_DEVICE_REMOTE_WAKEUP))
    {
        if (usb_status == USB_DC_SUSPEND)
        {
            usb_wakeup_request();
            return;
        }
    }
}

static void status_cb(enum usb_dc_status_code status, const uint8_t *param)
{
    usb_status = status;
}

bool init_hid_mouse()
{
    int ret;

    hid_dev = device_get_binding("HID_0");
    if (hid_dev == NULL)
    {
        LOG_ERR("Cannot get USB HID Device");
        return false;
    }

    usb_hid_register_device(hid_dev,
                            hid_report_desc, sizeof(hid_report_desc),
                            NULL);

    ret = usb_hid_init(hid_dev);
    if (ret != 0)
    {
        LOG_ERR("Failed to init USB");
        return false;
    }

    ret = usb_enable(status_cb);
    if (ret != 0)
    {
        LOG_ERR("Failed to enable USB");
        return false;
    }

    return true;
}

bool mouse_left_click()
{
    LOG_INF("%s", __func__);

    mouse_report[1] = 0;
    mouse_report[2] = 0;
    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_LEFT, 1);
    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);

    k_sleep(K_MSEC(10));

    // mouse_report[1] = 0;
    // mouse_report[2] = 0;

    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_LEFT, 0);
    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);
}

bool mouse_right_click()
{
    LOG_INF("%s", __func__);

    mouse_report[1] = 0;
    mouse_report[2] = 0;
    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_RIGHT, 1);
    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);

    k_sleep(K_MSEC(10));

    // mouse_report[1] = 0;
    // mouse_report[2] = 0;

    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_RIGHT, 0);
    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);
}

bool mouse_move_up(const size_t move_value)
{
    LOG_INF("%s", __func__);

    if (move_value <= 500)
    {
        rwup_if_suspended();
        mouse_report[0] = 0;
        mouse_report[1] = 0;
        mouse_report[MOUSE_Y_REPORT_IDX] = -move_value;
        if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
        {
            return true;
        }
    }
    else
    {
        LOG_ERR("Wrong move value");
    }
    return false;
}

bool mouse_move_down(const size_t move_value)
{
    LOG_INF("%s", __func__);

    if (move_value <= 500)
    {
        rwup_if_suspended();
        mouse_report[0] = 0;
        mouse_report[1] = 0;
        mouse_report[MOUSE_Y_REPORT_IDX] = move_value;
        if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
        {
            return true;
        }
    }
    else
    {
        LOG_ERR("Wrong move value");
    }
    return false;
}

bool mouse_move_left(const size_t move_value)
{
    LOG_INF("%s", __func__);

    if (move_value <= 500)
    {
        rwup_if_suspended();
        mouse_report[0] = 0;
        mouse_report[MOUSE_X_REPORT_IDX] = -move_value;
        mouse_report[2] = 0;
        if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
        {
            return true;
        }
    }
    else
    {
        LOG_ERR("Wrong move value");
    }
    return false;
}

bool mouse_move_right(const size_t move_value)
{
    LOG_INF("%s", __func__);

    if (move_value <= 500)
    {
        rwup_if_suspended();
        mouse_report[0] = 0;
        mouse_report[MOUSE_X_REPORT_IDX] = move_value;
        mouse_report[2] = 0;
        if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
        {
            return true;
        }
    }
    else
    {
        LOG_ERR("Wrong move value");
    }
    return false;
}