#!/usr/bin/env python3
import os
import json
import argparse
from pathlib import Path

def create_directories(root_dir):
    """创建框架所需的目录结构"""
    dirs = [
        os.path.join(root_dir, "drivers"),
        os.path.join(root_dir, "chip", args.chip_name, "drivers"),
        os.path.join(root_dir, "boards")
    ]
    
    for dir_path in dirs:
        Path(dir_path).mkdir(parents=True, exist_ok=True)
    print("✅ 框架目录创建完成")

def generate_hal_files(root_dir):
    """生成硬件抽象层(HAL)文件"""
    hal_dir = os.path.join(root_dir, "drivers")
    hal_gpio_path = os.path.join(hal_dir, "hal_gpio.h")
    
    with open(hal_gpio_path, "w", encoding="utf-8") as f:
        f.write("""#ifndef HAL_GPIO_H
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
""")
    print("✅ HAL层文件生成完成")

def generate_chip_driver(root_dir, chip_name):
    """生成芯片适配层驱动文件"""
    driver_dir = os.path.join(root_dir, "chip", chip_name, "drivers")
    gpio_driver_path = os.path.join(driver_dir, f"gpio_{chip_name.lower()}.c")
    
    with open(gpio_driver_path, "w", encoding="utf-8") as f:
        f.write(f"""#include "drivers/hal_gpio.h"
#include "gpio.h"  // 现有{chip_name} GPIO驱动

struct hal_gpio_dev {{
    uint8_t pin;
}};

hal_gpio_dev_t* hal_gpio_init(uint8_t pin, hal_gpio_dir_t dir) {{
    static struct hal_gpio_dev gpio_devs[32];  // 假设支持32个GPIO
    hal_gpio_dev_t* dev = &gpio_devs[pin];
    dev->pin = pin;

    // 调用现有{chip_name} GPIO初始化函数
    gpio_function_en(pin);
    if (dir == HAL_GPIO_DIR_OUTPUT) {{
        gpio_output_en(pin);
        gpio_input_dis(pin);
    }} else {{
        gpio_input_en(pin);
        gpio_output_dis(pin);
    }}
    return dev;
}}

void hal_gpio_set(hal_gpio_dev_t* dev, bool level) {{
    gpio_write(dev->pin, level ? 1 : 0);  // 现有{chip_name} GPIO写函数
}}

bool hal_gpio_get(hal_gpio_dev_t* dev) {{
    return gpio_read(dev->pin) ? true : false;  // 现有{chip_name} GPIO读函数
}}
""")
    print(f"✅ {chip_name}芯片适配层文件生成完成")

def generate_board_config(root_dir, board_name):
    """生成板级配置文件"""
    boards_dir = os.path.join(root_dir, "boards")
    board_config_path = os.path.join(boards_dir, f"board_{board_name.lower()}.h")
    
    with open(board_config_path, "w", encoding="utf-8") as f:
        f.write(f"""#ifndef BOARD_{board_name.upper()}_H
#define BOARD_{board_name.upper()}_H

// 引脚定义（映射到芯片实际GPIO）
#define BOARD_LED0_PIN    12  // {board_name}的LED连接到GPIO12
#define BOARD_BUTTON0_PIN 13 // {board_name}的按键连接到GPIO13

// GPIO配置（LED为输出，按键为输入）
#define LED0_CONFIG {{ .dir = HAL_GPIO_DIR_OUTPUT }}
#define BUTTON0_CONFIG {{ .dir = HAL_GPIO_DIR_INPUT }}

#endif // BOARD_{board_name.upper()}_H
""")
    print(f"✅ {board_name}板级配置文件生成完成")

def generate_driver_manager(root_dir):
    """生成驱动管理层文件"""
    driver_dir = os.path.join(root_dir, "drivers")
    device_path = os.path.join(driver_dir, "device.h")
    
    with open(device_path, "w", encoding="utf-8") as f:
        f.write("""#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <string.h>

typedef enum {
    DEVICE_TYPE_GPIO,
    DEVICE_TYPE_UART,
    // 其他外设类型
} device_type_t;

typedef struct {
    const char* name;
    device_type_t type;
    void* dev;  // 指向HAL设备句柄
} device_t;

// 注册设备
static inline void device_register(const device_t* dev) {
    static device_t devices[32];
    static uint8_t count = 0;
    if (count < 32) {
        memcpy(&devices[count], dev, sizeof(device_t));
        count++;
    }
}

// 查找设备
static inline device_t* device_find(const char* name) {
    static device_t devices[32];
    for (uint8_t i = 0; i < 32; i++) {
        if (strcmp(devices[i].name, name) == 0) {
            return &devices[i];
        }
    }
    return NULL;
}

#endif // DEVICE_H
""")
    print("✅ 驱动管理层文件生成完成")

def modify_cmakelists(root_dir, chip_name):
    """修改CMakeLists.txt以集成框架"""
    cmake_path = os.path.join(root_dir, "CMakeLists.txt")
    if not os.path.exists(cmake_path):
        print(f"❌ 未找到CMakeLists.txt: {cmake_path}")
        return
    
    with open(cmake_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    # 添加框架目录配置
    framework_config = f"""# 框架核心目录（driver）
set(FRAMEWORK_CORE_DIRS
    "drivers"
)

# 芯片驱动目录（自动根据目标芯片选择）
if(TARGET_NAME MATCHES "{chip_name}")
    set(CHIP_DRIVER_DIR "chip/{chip_name}/drivers")
elseif(TARGET_NAME MATCHES "TL721X")
    set(CHIP_DRIVER_DIR "chip/TL721X/drivers")
elseif(TARGET_NAME MATCHES "TL321X")
    set(CHIP_DRIVER_DIR "chip/TL321X/drivers")
endif()

# 板级配置目录
set(BOARD_DIR "boards")
"""
    
    # 在include_sources函数前插入配置
    if "FRAMEWORK_CORE_DIRS" not in content:
        new_content = content.replace("function(include_sources)", 
                                     f"{framework_config}\nfunction(include_sources)")
        with open(cmake_path, "w", encoding="utf-8") as f:
            f.write(new_content)
        print("✅ CMakeLists.txt添加框架目录完成")
    else:
        print("⚠️ CMakeLists.txt已包含框架目录，跳过修改")
    
    # 合并框架目录到源文件列表
    if "FRAMEWORK_CORE_DIRS" in content and "FRAMEWORK_CORE_DIRS" not in content:
        new_content = content.replace("list(APPEND SOURCE_DIR_LIST", 
                                     f"list(APPEND SOURCE_DIR_LIST ${FRAMEWORK_CORE_DIRS} ${CHIP_DRIVER_DIR} ${BOARD_DIR})\n    list(APPEND SOURCE_DIR_LIST")
        with open(cmake_path, "w", encoding="utf-8") as f:
            f.write(new_content)

def modify_cmake_config(root_dir, chip_name):
    """修改cmake配置JSON文件"""
    # config_path = os.path.join(root_dir, "cmake_configs", f"{chip_name}_Driver_Demo_cmake.json")
    config_path = os.path.join(root_dir, "cmake_configs", f"TC_PLATFORM_SDK_{chip_name}_cmake.json")
    if not os.path.exists(config_path):
        print(f"❌ 未找到配置文件: {config_path}")
        return
    
    with open(config_path, "r", encoding="utf-8") as f:
        config = json.load(f)
    
    # 添加框架目录到编译路径
    framework_dirs = [
        "drivers", 
        f"chip/{chip_name}/drivers", 
        "boards"
    ]
    
    # 更新每个target的directories
    if "targets" in config:
        for target in config["targets"]:
            if "directories" in target:
                for dir in framework_dirs:
                    if dir not in target["directories"]:
                        target["directories"].append(dir)
    
    with open(config_path, "w", encoding="utf-8") as f:
        json.dump(config, f, indent=4)
    print(f"✅ {chip_name}_Driver_Demo_cmake.json更新完成")

def main():
    parser = argparse.ArgumentParser(description="添加无device tree依赖的驱动框架")
    parser.add_argument("--chip", required=True, help="目标芯片型号（如B80、TL721X）", dest="chip_name")
    parser.add_argument("--board", required=True, help="开发板名称（如B80_EVK）", dest="board_name")
    parser.add_argument("--root", help="项目根目录路径", default="..")
    
    global args
    args = parser.parse_args()
    
    # 确认项目根目录
    root_dir = os.path.abspath(args.root)
    print(f"🔧 项目根目录: {root_dir}")
    
    # 执行框架集成步骤
    create_directories(root_dir)
    generate_hal_files(root_dir)
    generate_chip_driver(root_dir, args.chip_name)
    generate_board_config(root_dir, args.board_name)
    generate_driver_manager(root_dir)
    modify_cmakelists(root_dir, args.chip_name)
    modify_cmake_config(root_dir, args.chip_name)
    
    print("\n🎉 驱动框架集成完成！")
    print("提示：可在demo中包含以下头文件使用框架：")
    print(f"  #include \"drivers/hal_gpio.h\"")
    print(f"  #include \"drivers/device.h\"")
    print(f"  #include \"board_{args.board_name.lower()}.h\"")

if __name__ == "__main__":
    main()
