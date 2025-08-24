#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import argparse
import json
from pathlib import Path

def show_info(msg):
    """打印信息提示"""
    print(f"[INFO] {msg}")

def create_directories(root_dir):
    """创建必要的目录结构"""
    dirs = [
        os.path.join(root_dir, "drivers"),
        os.path.join(root_dir, "chip", args.chip_name, "drivers"),
        os.path.join(root_dir, "boards")
    ]
    for dir_path in dirs:
        Path(dir_path).mkdir(parents=True, exist_ok=True)
    show_info("目录结构创建完成")

def generate_hal_uart(root_dir, support_multi_uart):
    """生成HAL层UART接口头文件"""
    hal_dir = os.path.join(root_dir, "drivers")
    hal_path = os.path.join(hal_dir, "hal_uart.h")
    
    # 多UART实例相关代码段
    multi_uart_code = """
typedef enum {
    HAL_UART_NUM_0,
    HAL_UART_NUM_1
} hal_uart_num_t;

typedef struct hal_uart_dev {
    hal_uart_num_t uart_num;
    uint32_t baudrate;
    void* chip_data; // 芯片私有数据
} hal_uart_dev_t;
""" if support_multi_uart else """
typedef struct hal_uart_dev {
    uint32_t baudrate;
    void* chip_data; // 芯片私有数据
} hal_uart_dev_t;
"""

    # 初始化函数声明
    init_declaration = """
hal_uart_dev_t* hal_uart_init(hal_uart_num_t uart_num, uint32_t baudrate, 
                             hal_uart_parity_t parity, hal_uart_stopbit_t stopbit,
                             hal_uart_mode_t mode);
""" if support_multi_uart else """
hal_uart_dev_t* hal_uart_init(uint32_t baudrate, hal_uart_parity_t parity, 
                             hal_uart_stopbit_t stopbit, hal_uart_mode_t mode);
"""

    # 函数参数说明
    param_doc = " @param uart_num UART编号" if support_multi_uart else ""

    # 完整头文件内容
    content = f"""#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {{
    HAL_UART_PARITY_NONE,
    HAL_UART_PARITY_EVEN,
    HAL_UART_PARITY_ODD
}} hal_uart_parity_t;

typedef enum {{
    HAL_UART_STOPBIT_1,
    HAL_UART_STOPBIT_2
}} hal_uart_stopbit_t;

typedef enum {{
    HAL_UART_MODE_POLLING,
    HAL_UART_MODE_DMA,
    HAL_UART_MODE_IRQ
}} hal_uart_mode_t;

{multi_uart_code}

/**
 * @brief 初始化UART
 * @param baudrate 波特率
 * @param parity 校验位
 * @param stopbit 停止位
 * @param mode 工作模式
{param_doc}
 * @return 设备句柄，NULL表示失败
 */
{init_declaration}

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
"""

    with open(hal_path, "w", encoding="utf-8") as f:
        f.write(content)
    show_info(f"HAL层UART接口生成完成: {hal_path}")

def generate_chip_uart(root_dir, chip_name, support_multi_uart):
    """生成芯片适配层UART驱动代码"""
    chip_dir = os.path.join(root_dir, "chip", chip_name, "drivers")
    chip_path = os.path.join(chip_dir, f"uart_{chip_name}_hal.c")

    # 芯片特定代码段
    chip_specific = """#include "dma.h"
static dma_chn_t uart_tx_dma_chn[2] = {DMA_CHN0, DMA_CHN1};
static dma_chn_t uart_rx_dma_chn[2] = {DMA_CHN2, DMA_CHN3};
""" if support_multi_uart else """static unsigned char uart_TxIndex = 0;
"""

    # 初始化函数实现
    init_impl = f"""hal_uart_dev_t* hal_uart_init(hal_uart_num_t uart_num, uint32_t baudrate, 
                             hal_uart_parity_t parity, hal_uart_stopbit_t stopbit,
                             hal_uart_mode_t mode) {{
    static hal_uart_dev_t uart_devs[2] = {{0}};
    hal_uart_dev_t* dev = &uart_devs[uart_num];
    dev->uart_num = uart_num;
    dev->baudrate = baudrate;

    unsigned short div;
    unsigned char bwpc;
    uart_cal_div_and_bwpc(baudrate, sys_clk.pclk, &div, &bwpc);
    
    uart_parity_e chip_parity = (parity == HAL_UART_PARITY_NONE) ? 0 : 
                               (parity == HAL_UART_PARITY_EVEN) ? UART_PARITY_EVEN : UART_PARITY_ODD;
    uart_stop_bit_e chip_stopbit = (stopbit == HAL_UART_STOPBIT_1) ? UART_STOP_BIT_1 : UART_STOP_BIT_2;
    
    uart_init(uart_num, div, bwpc, chip_parity, chip_stopbit);

    // 模式配置
    if (mode == HAL_UART_MODE_DMA) {{
        uart_dma_enable(uart_num, 1, 1);
        dma_config(uart_tx_dma_chn[uart_num], &uart_tx_dma_config[uart_num]);
        dma_config(uart_rx_dma_chn[uart_num], &uart_rx_dma_config[uart_num]);
    }} else if (mode == HAL_UART_MODE_IRQ) {{
        uart_dma_enable(uart_num, 0, 0);
        uart_irq_enable(uart_num, 1, 1);
        uart_ndma_irq_triglevel(uart_num, 1, 1);
    }}

    return dev;
}}
""" if support_multi_uart else """hal_uart_dev_t* hal_uart_init(uint32_t baudrate, hal_uart_parity_t parity, 
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
"""

    # 发送接收函数实现
    send_recv_impl = f"""uint32_t hal_uart_send(hal_uart_dev_t* dev, const uint8_t* data, uint32_t len) {{
    if (dev->mode == HAL_UART_MODE_DMA) {{
        dma_set_address(uart_tx_dma_chn[dev->uart_num], (uint32_t)data, (uint32_t)&reg_uart_data(dev->uart_num));
        dma_set_transfer_count(uart_tx_dma_chn[dev->uart_num], len);
        dma_chn_en(uart_tx_dma_chn[dev->uart_num]);
        while(dma_chn_is_busy(uart_tx_dma_chn[dev->uart_num]));
        return len;
    }} else {{
        for (uint32_t i = 0; i < len; i++) {{
            while(uart_tx_is_busy(dev->uart_num));
            reg_uart_data(dev->uart_num) = data[i];
        }}
        return len;
    }}
}}

uint32_t hal_uart_recv(hal_uart_dev_t* dev, uint8_t* data, uint32_t len, uint32_t timeout) {{
    // 实现接收逻辑（简化版）
    uint32_t recv_len = 0;
    while (recv_len < len && timeout--) {{
        if (uart_rx_data_ready(dev->uart_num)) {{
            data[recv_len++] = reg_uart_data(dev->uart_num);
        }}
        delay_us(1000); // 1ms延迟
    }}
    return recv_len;
}}
""" if support_multi_uart else """uint32_t hal_uart_send(hal_uart_dev_t* dev, const uint8_t* data, uint32_t len) {
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
"""

    # 关闭函数实现
    deinit_impl = """
void hal_uart_deinit(hal_uart_dev_t* dev) {
    uart_irq_enable(dev->uart_num, 0, 0);
    uart_dma_enable(dev->uart_num, 0, 0);
}
""" if support_multi_uart else """
void hal_uart_deinit(hal_uart_dev_t* dev) {
    uart_irq_enable(0, 0);
    uart_dma_enable(0, 0);
}
"""

    # 完整C文件内容
    content = f"""#include "hal/hal_uart.h"
#include "uart.h"
#include "gpio.h"
#include "clock.h"
{chip_specific}

{init_impl}

{send_recv_impl}

{deinit_impl}
"""

    with open(chip_path, "w", encoding="utf-8") as f:
        f.write(content)
    show_info(f"芯片适配层代码生成完成: {chip_path}")

def generate_board_config(root_dir, board_name, support_multi_uart):
    """生成板级UART配置头文件"""
    boards_dir = os.path.join(root_dir, "boards")
    board_path = os.path.join(boards_dir, f"board_{board_name}_uart.h")

    # 引脚定义
    pin_defs = """// UART0引脚定义
#define BOARD_UART0_TX_PIN    4
#define BOARD_UART0_RX_PIN    5

// UART1引脚定义
#define BOARD_UART1_TX_PIN    6
#define BOARD_UART1_RX_PIN    7
""" if support_multi_uart else """// UART引脚定义
#define BOARD_UART_TX_PIN    4
#define BOARD_UART_RX_PIN    5
"""

    # 引脚初始化函数
    pin_init_func = """static inline void board_uart_pin_init(hal_uart_num_t uart_num) {
    if (uart_num == HAL_UART_NUM_0) {
        gpio_set_func(BOARD_UART0_TX_PIN, GPIO_FUN_UART);
        gpio_set_func(BOARD_UART0_RX_PIN, GPIO_FUN_UART);
    } else {
        gpio_set_func(BOARD_UART1_TX_PIN, GPIO_FUN_UART);
        gpio_set_func(BOARD_UART1_RX_PIN, GPIO_FUN_UART);
    }
}
""" if support_multi_uart else """static inline void board_uart_pin_init(void) {
    gpio_set_func(BOARD_UART_TX_PIN, GPIO_FUN_UART);
    gpio_set_func(BOARD_UART_RX_PIN, GPIO_FUN_UART);
}
"""

    # 完整头文件内容
    content = f"""#ifndef BOARD_{board_name.upper()}_UART_H
#define BOARD_{board_name.upper()}_UART_H

#include "hal/hal_uart.h"

{pin_defs}

// 默认UART配置
#define BOARD_UART_DEFAULT_BAUDRATE   115200
#define BOARD_UART_DEFAULT_PARITY     HAL_UART_PARITY_NONE
#define BOARD_UART_DEFAULT_STOPBIT    HAL_UART_STOPBIT_1
#define BOARD_UART_DEFAULT_MODE       HAL_UART_MODE_POLLING

/**
 * @brief 初始化板级UART引脚
{(" @param uart_num UART编号" if support_multi_uart else "")}
 */
{pin_init_func}

#endif // BOARD_{board_name.upper()}_UART_H
"""

    with open(board_path, "w", encoding="utf-8") as f:
        f.write(content)
    show_info(f"板级配置文件生成完成: {board_path}")

def update_cmake_config(root_dir, chip_name):
    """更新CMake配置文件"""
    # 更新CMakeLists.txt
    cmake_path = os.path.join(root_dir, "CMakeLists.txt")
    if os.path.exists(cmake_path):
        with open(cmake_path, "r", encoding="utf-8") as f:
            content = f.read()
        
        uart_config = f"""
# UART驱动框架
set(UART_FRAMEWORK_DIRS
    "drivers"
    "chip/{chip_name}/drivers"
    "boards"
)
list(APPEND SOURCE_DIR_LIST ${{UART_FRAMEWORK_DIRS}})
"""

        if "UART_FRAMEWORK_DIRS" not in content:
            # 在驱动源文件目录后插入
            new_content = content.replace("# 驱动源文件目录", f"# 驱动源文件目录{uart_config}")
            with open(cmake_path, "w", encoding="utf-8") as f:
                f.write(new_content)
            show_info("CMakeLists.txt更新完成")
        else:
            show_info("CMakeLists.txt已包含UART配置，无需更新")
    else:
        show_info(f"未找到CMakeLists.txt: {cmake_path}")

    # 更新JSON配置文件
    json_path = os.path.join(root_dir, "cmake_configs", f"TL_PLATFORM_SDK_B92_cmake.json")
    if os.path.exists(json_path):
        with open(json_path, "r", encoding="utf-8") as f:
            try:
                config = json.load(f)
            except json.JSONDecodeError:
                show_info(f"JSON配置文件解析错误: {json_path}")
                return

        # 添加UART目录到directories
        if "directories" in config:
            uart_dirs = [
                "drivers",
                f"chip/{chip_name}/drivers",
                "boards"
            ]
            for dir_path in uart_dirs:
                if dir_path not in config["directories"]:
                    config["directories"].append(dir_path)
            
            with open(json_path, "w", encoding="utf-8") as f:
                json.dump(config, f, indent=4)
            show_info(f"JSON配置文件更新完成: {json_path}")
        else:
            show_info(f"JSON配置文件中未找到directories字段: {json_path}")
    else:
        show_info(f"未找到JSON配置文件: {json_path}")

def main(args):
    # 确定SDK根目录（假设脚本在tools目录，根目录为上两级）
    script_dir = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.abspath(os.path.join(script_dir, ".."))  # 适配tools目录结构
    show_info(f"SDK根目录: {root_dir}")

    # 执行各步骤
    create_directories(root_dir)
    generate_hal_uart(root_dir, args.support_multi_uart)
    generate_chip_uart(root_dir, args.chip_name, args.support_multi_uart)
    generate_board_config(root_dir, args.board_name, args.support_multi_uart)
    update_cmake_config(root_dir, args.chip_name)

    show_info("UART驱动配置完成")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="UART驱动自动化配置脚本")
    parser.add_argument("--chip-name", required=True, help="目标芯片型号（如B80、B92、tl322x）")
    parser.add_argument("--board-name", required=True, help="开发板名称（如B92_EVK）")
    parser.add_argument("--support-multi-uart", action="store_true", 
                        help="是否支持多UART实例（B92/B91需指定）")
    
    args = parser.parse_args()
    main(args)