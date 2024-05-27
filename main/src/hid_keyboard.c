#include "hid_keyboard.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <zephyr/random/random.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(hid_keyboard);

static const uint8_t hid_kbd_report_desc[] = HID_KEYBOARD_REPORT_DESC();
static const struct device *hid1_dev = NULL;

static K_SEM_DEFINE(usb_sem, 1, 1);

static void in_ready_cb(const struct device *dev)
{
    ARG_UNUSED(dev);

    k_sem_give(&usb_sem);
}

static const struct hid_ops ops = {
    .int_in_ready = in_ready_cb,
};

static int ascii_to_hid(uint8_t ascii)
{
    if (ascii < 32)
    {
        /* Character not supported */
        return -1;
    }
    else if (ascii < 48)
    {
        /* Special characters */
        switch (ascii)
        {
        case 32:
            return HID_KEY_SPACE;
        case 33:
            return HID_KEY_1;
        case 34:
            return HID_KEY_APOSTROPHE;
        case 35:
            return HID_KEY_3;
        case 36:
            return HID_KEY_4;
        case 37:
            return HID_KEY_5;
        case 38:
            return HID_KEY_7;
        case 39:
            return HID_KEY_APOSTROPHE;
        case 40:
            return HID_KEY_9;
        case 41:
            return HID_KEY_0;
        case 42:
            return HID_KEY_8;
        case 43:
            return HID_KEY_EQUAL;
        case 44:
            return HID_KEY_COMMA;
        case 45:
            return HID_KEY_MINUS;
        case 46:
            return HID_KEY_DOT;
        case 47:
            return HID_KEY_SLASH;
        default:
            return -1;
        }
    }
    else if (ascii < 58)
    {
        /* Numbers */
        if (ascii == 48U)
        {
            return HID_KEY_0;
        }
        else
        {
            return ascii - 19;
        }
    }
    else if (ascii < 65)
    {
        /* Special characters #2 */
        switch (ascii)
        {
        case 58:
            return HID_KEY_SEMICOLON;
        case 59:
            return HID_KEY_SEMICOLON;
        case 60:
            return HID_KEY_COMMA;
        case 61:
            return HID_KEY_EQUAL;
        case 62:
            return HID_KEY_DOT;
        case 63:
            return HID_KEY_SLASH;
        case 64:
            return HID_KEY_2;
        default:
            return -1;
        }
    }
    else if (ascii < 91)
    {
        /* Uppercase characters */
        return ascii - 61U;
    }
    else if (ascii < 97)
    {
        /* Special characters #3 */
        switch (ascii)
        {
        case 91:
            return HID_KEY_LEFTBRACE;
        case 92:
            return HID_KEY_BACKSLASH;
        case 93:
            return HID_KEY_RIGHTBRACE;
        case 94:
            return HID_KEY_6;
        case 95:
            return HID_KEY_MINUS;
        case 96:
            return HID_KEY_GRAVE;
        default:
            return -1;
        }
    }
    else if (ascii < 123)
    {
        /* Lowercase letters */
        return ascii - 93;
    }
    else if (ascii < 128)
    {
        /* Special characters #4 */
        switch (ascii)
        {
        case 123:
            return HID_KEY_LEFTBRACE;
        case 124:
            return HID_KEY_BACKSLASH;
        case 125:
            return HID_KEY_RIGHTBRACE;
        case 126:
            return HID_KEY_GRAVE;
        case 127:
            return HID_KEY_DELETE;
        default:
            return -1;
        }
    }

    return -1;
}

static bool needs_shift(uint8_t ascii)
{
    if ((ascii < 33) || (ascii == 39U))
    {
        return false;
    }
    else if ((ascii >= 33U) && (ascii < 44))
    {
        return true;
    }
    else if ((ascii >= 44U) && (ascii < 58))
    {
        return false;
    }
    else if ((ascii == 59U) || (ascii == 61U))
    {
        return false;
    }
    else if ((ascii >= 58U) && (ascii < 91))
    {
        return true;
    }
    else if ((ascii >= 91U) && (ascii < 94))
    {
        return false;
    }
    else if ((ascii == 94U) || (ascii == 95U))
    {
        return true;
    }
    else if ((ascii > 95) && (ascii < 123))
    {
        return false;
    }
    else if ((ascii > 122) && (ascii < 127))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool init_hid_keyboard()
{
    hid1_dev = device_get_binding("HID_1");
    if (hid1_dev == NULL)
    {
        LOG_ERR("Cannot get USB HID 1 Device");
        return false;
    }

    usb_hid_register_device(hid1_dev, hid_kbd_report_desc,
                            sizeof(hid_kbd_report_desc), &ops);

    if (usb_hid_init(hid1_dev) != 0)
    {
        LOG_ERR("Failed to init USB");
        return false;
    }

    k_sleep(K_MSEC(2000));

    return true;
}

bool hid_heyboard_send(const char *input_string, size_t data_len)
{
    if (input_string == NULL || data_len == 0)
    {
        LOG_ERR("Wrong parameters, function %s", __func__);
    }

    bool character_invalid = false;

    for (size_t i = 0; i < data_len; i++)
    {
        int hid_code = ascii_to_hid(*(input_string + i));

        if (hid_code != -1)
        {
            uint8_t rep[] = {0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00};

            if (needs_shift(*(input_string + i)))
            {
                rep[0] |= HID_KBD_MODIFIER_RIGHT_SHIFT;
            }

            rep[7] = (uint8_t)hid_code;

            k_sem_take(&usb_sem, K_FOREVER);

            hid_int_ep_write(hid1_dev, rep, sizeof(rep), NULL);

            uint8_t rep2[] = {0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00};

            k_sem_take(&usb_sem, K_FOREVER);
            hid_int_ep_write(hid1_dev, rep2, sizeof(rep2), NULL);
        }
        else
        {
            character_invalid = true;
        }
    }

    if (character_invalid)
    {
        return false;
    }
    else
    {
        return true;
    }
}
