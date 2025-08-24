#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

// GPIO方向枚举
typedef enum {
    HAL_GPIO_DIR_INPUT,
    HAL_GPIO_DIR_OUTPUT
} hal_gpio_dir_t;

// GPIO设备句柄（芯片适配层实现）
typedef struct hal_gpio_dev hal_gpio_dev_t;

// 初始化GPIO
hal_gpio_dev_t* hal_gpio_init(uint8_t pin, hal_gpio_dir_t dir);

// 设置GPIO输出电平
void hal_gpio_set(hal_gpio_dev_t* dev, bool level);

// 读取GPIO输入电平
bool hal_gpio_get(hal_gpio_dev_t* dev);

#endif // HAL_GPIO_H
