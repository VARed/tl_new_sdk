#include "hal/hal_uart.h"
#include "uart.h"
#include "gpio.h"
#include "clock.h"
static unsigned char uart_TxIndex = 0;


hal_uart_dev_t* hal_uart_init(uint32_t baudrate, hal_uart_parity_t parity, 
                             hal_uart_stopbit_t stopbit, hal_uart_mode_t mode) {
    static hal_uart_dev_t uart_dev = {0};
    uart_dev.baudrate = baudrate;

    uart_init_baudrate(baudrate, sys_clk.pclk, 
                      0,  // 无校验
                      1); // 1个停止位

    // 模式配置
    if (mode == HAL_UART_MODE_DMA) {
        uart_dma_enable(1, 1);
    } else if (mode == HAL_UART_MODE_IRQ) {
        uart_dma_enable(0, 0);
        uart_irq_enable(1, 1);
        uart_ndma_irq_triglevel(1, 1);
    }

    return &uart_dev;
}


uint32_t hal_uart_send(hal_uart_dev_t* dev, const uint8_t* data, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        while(uart_tx_is_busy());
        reg_uart_data = data[i];
    }
    return len;
}

uint32_t hal_uart_recv(hal_uart_dev_t* dev, uint8_t* data, uint32_t len, uint32_t timeout) {
    uint32_t recv_len = 0;
    while (recv_len < len && timeout--) {
        if (uart_rx_data_ready()) {
            data[recv_len++] = reg_uart_data;
        }
        delay_us(1000);
    }
    return recv_len;
}



void hal_uart_deinit(hal_uart_dev_t* dev) {
    uart_irq_enable(0, 0);
    uart_dma_enable(0, 0);
}

