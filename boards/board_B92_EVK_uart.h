#ifndef BOARD_B92_EVK_UART_H
#define BOARD_B92_EVK_UART_H

#include "hal/hal_uart.h"

// UART引脚定义
#define BOARD_UART_TX_PIN    4
#define BOARD_UART_RX_PIN    5


// 默认UART配置
#define BOARD_UART_DEFAULT_BAUDRATE   115200
#define BOARD_UART_DEFAULT_PARITY     HAL_UART_PARITY_NONE
#define BOARD_UART_DEFAULT_STOPBIT    HAL_UART_STOPBIT_1
#define BOARD_UART_DEFAULT_MODE       HAL_UART_MODE_POLLING

/**
 * @brief 初始化板级UART引脚

 */
static inline void board_uart_pin_init(void) {
    gpio_set_func(BOARD_UART_TX_PIN, GPIO_FUN_UART);
    gpio_set_func(BOARD_UART_RX_PIN, GPIO_FUN_UART);
}


#endif // BOARD_B92_EVK_UART_H
