#ifndef DEVICE_H
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
