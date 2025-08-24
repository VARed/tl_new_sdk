#ifndef BOARD_B91_EVK_H
#define BOARD_B91_EVK_H

// 引脚定义（映射到芯片实际GPIO）
#define BOARD_LED0_PIN    12  // B91_EVK的LED连接到GPIO12
#define BOARD_BUTTON0_PIN 13 // B91_EVK的按键连接到GPIO13

// GPIO配置（LED为输出，按键为输入）
#define LED0_CONFIG { .dir = HAL_GPIO_DIR_OUTPUT }
#define BUTTON0_CONFIG { .dir = HAL_GPIO_DIR_INPUT }

#endif // BOARD_B91_EVK_H
