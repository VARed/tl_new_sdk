#include "drivers/hal_gpio.h"
#include "gpio.h"  // 现有B85 GPIO驱动

struct hal_gpio_dev {
    uint8_t pin;
};

hal_gpio_dev_t* hal_gpio_init(uint8_t pin, hal_gpio_dir_t dir) {
    static struct hal_gpio_dev gpio_devs[32];  // 假设支持32个GPIO
    hal_gpio_dev_t* dev = &gpio_devs[pin];
    dev->pin = pin;

    // 调用现有B85 GPIO初始化函数
    gpio_function_en(pin);
    if (dir == HAL_GPIO_DIR_OUTPUT) {
        gpio_output_en(pin);
        gpio_input_dis(pin);
    } else {
        gpio_input_en(pin);
        gpio_output_dis(pin);
    }
    return dev;
}

void hal_gpio_set(hal_gpio_dev_t* dev, bool level) {
    gpio_write(dev->pin, level ? 1 : 0);  // 现有B85 GPIO写函数
}

bool hal_gpio_get(hal_gpio_dev_t* dev) {
    return gpio_read(dev->pin) ? true : false;  // 现有B85 GPIO读函数
}
