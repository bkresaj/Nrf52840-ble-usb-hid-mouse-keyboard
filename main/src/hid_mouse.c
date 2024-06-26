#include "hid_mouse.h"

#include <zephyr/kernel.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hid_mouse, LOG_LEVEL_INF);

#define MOUSE_DEV_NAME "HID_0"
#define MOUSE_REPORT_SIZE 4
#define MOUSE_BTN_LEFT 0
#define MOUSE_BTN_RIGHT 1

#define MAX_BUTTON_MOVE_VALUE 1000

static uint8_t mouse_report[MOUSE_REPORT_SIZE];

static const struct device *hid_dev = NULL;
static const uint8_t hid_report_desc[] = HID_MOUSE_REPORT_DESC(1);
static enum usb_dc_status_code usb_status;

static K_SEM_DEFINE(report_sem, 1, 1);

enum mouse_report_idx
{
    MOUSE_BTN_REPORT_IDX = 0,
    MOUSE_X_REPORT_IDX = 1,
    MOUSE_Y_REPORT_IDX = 2,
    MOUSE_WHEEL_REPORT_IDX = 3,
    MOUSE_REPORT_COUNT = 4,
};

static void in_ready_cb(const struct device *dev)
{
    ARG_UNUSED(dev);

    k_sem_give(&report_sem);
}

static const struct hid_ops ops = {
    .int_in_ready = in_ready_cb,
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

    hid_dev = device_get_binding(MOUSE_DEV_NAME);
    if (hid_dev == NULL)
    {
        LOG_ERR("Cannot get USB HID Device");
        return false;
    }

    usb_hid_register_device(hid_dev,
                            hid_report_desc, sizeof(hid_report_desc),
                            &ops);

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

    k_sleep(K_MSEC(2000));

    return true;
}

bool hid_mouse_left_click()
{
    LOG_INF("%s", __func__);

    mouse_report[1] = 0;
    mouse_report[2] = 0;
    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_LEFT, 1);

    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);

    k_sleep(K_MSEC(10));

    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_LEFT, 0);

    if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool hid_mouse_right_click()
{
    LOG_INF("%s", __func__);

    mouse_report[1] = 0;
    mouse_report[2] = 0;
    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_RIGHT, 1);

    hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL);

    k_sleep(K_MSEC(10));

    WRITE_BIT(mouse_report[MOUSE_BTN_REPORT_IDX], MOUSE_BTN_RIGHT, 0);

    if (hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool send_mapped_x_value(uint8_t mapped_value)
{
    mouse_report[MOUSE_X_REPORT_IDX] = mapped_value;
    k_sem_take(&report_sem, K_FOREVER);
    LOG_INF("Move x value %d", mapped_value);
    return hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0;
}

static bool send_mapped_y_value(uint8_t mapped_value)
{
    mouse_report[MOUSE_Y_REPORT_IDX] = mapped_value;
    k_sem_take(&report_sem, K_FOREVER);
    LOG_INF("Move y value %d", mapped_value);
    return hid_int_ep_write(hid_dev, mouse_report, sizeof(mouse_report), NULL) == 0;
}

static bool hid_mouse_move(int move_x, int move_y)
{
    if (move_x >= -MAX_BUTTON_MOVE_VALUE && move_x <= MAX_BUTTON_MOVE_VALUE && move_y >= -MAX_BUTTON_MOVE_VALUE && move_y <= MAX_BUTTON_MOVE_VALUE)
    {
        if (move_y == 0 && move_x != 0)
        {
            mouse_report[MOUSE_BTN_REPORT_IDX] = 0;
            mouse_report[MOUSE_Y_REPORT_IDX] = 0;
            if (move_x < 0)
            {
                while (move_x < -128)
                {
                    if (!send_mapped_x_value(128))
                    {
                        return false;
                    }
                    move_x += 128;
                }

                if (move_x != 0)
                {
                    move_x = 255 + move_x;
                    if (!send_mapped_x_value(move_x))
                    {
                        return false;
                    }
                }
            }
            else
            {
                while (move_x >= 128)
                {
                    if (!send_mapped_x_value(127))
                    {
                        return false;
                    }
                    move_x -= 128;
                }
                if (move_x >= 0)
                {
                    if (!send_mapped_x_value(move_x))
                    {
                        return false;
                    }
                }
            }
            return true;
        }
        else if (move_x == 0 && move_y != 0)
        {
            mouse_report[MOUSE_BTN_REPORT_IDX] = 0;
            mouse_report[MOUSE_X_REPORT_IDX] = 0;
            if (move_y < 0)
            {
                while (move_y < -128)
                {
                    if (!send_mapped_y_value(128))
                    {
                        return false;
                    }
                    move_y += 128;
                }

                if (move_y != 0)
                {
                    move_y = 255 + move_y;
                    if (!send_mapped_y_value(move_y))
                    {
                        return false;
                    }
                }
            }
            else
            {
                while (move_y >= 128)
                {
                    if (!send_mapped_y_value(127))
                    {
                        return false;
                    }
                    move_y -= 128;
                }
                if (move_y >= 0)
                {
                    if (!send_mapped_y_value(move_y))
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }
    else
    {
        LOG_ERR("Wrong move value");
    }
    return false;
}

bool hid_mouse_move_up(const size_t move_value)
{
    LOG_INF("%s", __func__);

    return hid_mouse_move(0, -move_value);
}

bool hid_mouse_move_down(const size_t move_value)
{
    LOG_INF("%s", __func__);

    return hid_mouse_move(0, move_value);
}

bool hid_mouse_move_left(const size_t move_value)
{
    LOG_INF("%s", __func__);

    return hid_mouse_move(-move_value, 0);
}

bool hid_mouse_move_right(const size_t move_value)
{
    LOG_INF("%s", __func__);

    return hid_mouse_move(move_value, 0);
}