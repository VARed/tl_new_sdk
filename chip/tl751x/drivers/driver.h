/********************************************************************************************************
 * @file    driver.h
 *
 * @brief   This is the header file for tl751x
 *
 * @author  Driver Group
 * @date    2023
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#pragma once

#include "lib/include/plic.h"
#include "lib/include/pm/pm.h"
#include "lib/include/pm/pm_internal.h"
#include "lib/include/rf/rf_dma.h"
#include "lib/include/rf/rf_common.h"
#include "lib/include/rf/rf_ble.h"
#include "lib/include/rf/rf_private.h"
#include "lib/include/rf/rf_zigbee.h"
#include "lib/include/sys.h"
#include "lib/include/trng/trng.h"
#include "lib/include/trng/trng_portable.h"

#include "lib/include/pke/pke.h"
#include "lib/include/pke/pke_algorithm.h"

#include "lib/include/hash/hash.h"
#include "lib/include/hash/hash_algorithm.h"

#include "lib/include/ske/ske.h"
#include "lib/include/ske/ske_algorithm.h"

#if defined(MCU_CORE_TL751X_N22)
    #include "lib/include/clic.h"
#endif

#include "audio.h"
#include "lib/include/analog.h"
#include "dma.h"
#include "gpio.h"
#include "gpio_default.h"
#include "i2c.h"
#include "spi.h"
#include "pwm.h"
#include "timer.h"
#include "timer_bb.h"
#include "flash.h"
#include "usbhw.h"
#include "usb1hw.h"
#include "watchdog.h"
#include "s7816.h"
#include "lib/include/core.h"
#include "mdec.h"

#include "uart.h"
#include "lib/include/stimer.h"
#include "aes.h"

#include "lpc.h"
#include "lib/include/clock.h"
#include "lib/include/mspi.h"
#include "adc.h"
#include "qdec.h"
#include "mailbox.h"
#include "plic_sw.h"
#include "plmt.h"
#include "lib/include/otp.h"
#include "flash/flash_type.h"
