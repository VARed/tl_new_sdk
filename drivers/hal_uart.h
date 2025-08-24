#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HAL_UART_PARITY_NONE,
    HAL_UART_PARITY_EVEN,
    HAL_UART_PARITY_ODD
} hal_uart_parity_t;

typedef enum {
    HAL_UART_STOPBIT_1,
    HAL_UART_STOPBIT_2
} hal_uart_stopbit_t;

typedef enum {
    HAL_UART_MODE_POLLING,
    HAL_UART_MODE_DMA,
    HAL_UART_MODE_IRQ
} hal_uart_mode_t;


typedef struct hal_uart_dev {
    uint32_t baudrate;
    void* chip_data; // 芯片私有数据
} hal_uart_dev_t;


/**
 * @brief 初始化UART
 * @param baudrate 波特率
 * @param parity 校验位
 * @param stopbit 停止位
 * @param mode 工作模式

 * @return 设备句柄，NULL表示失败
 */

hal_uart_dev_t* hal_uart_init(uint32_t baudrate, hal_uart_parity_t parity, 
                             hal_uart_stopbit_t stopbit, hal_uart_mode_t mode);


/**
 * @brief 发送数据
 * @param dev UART设备句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 实际发送长度
 */
uint32_t hal_uart_send(hal_uart_dev_t* dev, const uint8_t* data, uint32_t len);

/**
 * @brief 接收数据
 * @param dev UART设备句柄
 * @param data 接收缓冲区
 * @param len 最大接收长度
 * @param timeout 超时时间(ms)，0表示阻塞
 * @return 实际接收长度
 */
uint32_t hal_uart_recv(hal_uart_dev_t* dev, uint8_t* data, uint32_t len, uint32_t timeout);

/**
 * @brief 关闭UART
 * @param dev UART设备句柄
 */
void hal_uart_deinit(hal_uart_dev_t* dev);

#endif // HAL_UART_H
