/********************************************************************************************************
 * @file    tl322x_usb.c
 *
 * @brief   This is the source file for tl322x
 *
 * @author  Driver Group
 * @date    2025
 *
 * @par     Copyright (c) 2025, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "driver.h"
#if defined(MCU_CORE_TL322X)
#include "tl_usb/core/usbd_core.h"

unsigned char ep0_out_data[64];

void usbd_ep_open(unsigned char bus, usb_endpoint_descriptor_t *endpoint_desc)
{
    (void)bus;

    unsigned char ep_num = usb_get_ep_number(endpoint_desc->bEndpointAddress);
    unsigned char ep_dir = usb_get_ep_dir(endpoint_desc->bEndpointAddress);

    usb0hw_ep_open(ep_num, ep_dir, (unsigned char)(endpoint_desc->bmAttributes & BIT_RNG(0, 1)), endpoint_desc->wMaxPacketSize);
}

void usbd_ep_write(unsigned char bus, const unsigned char ep_addr, unsigned char *buf, unsigned int len)
{
    (void)bus;

    unsigned char ep_num = usb_get_ep_number(ep_addr);

    usb0hw_write_ep_data(ep_num, buf, len);
}

void usbd_ep_read(unsigned char bus, const unsigned char ep_addr, unsigned char *buf, unsigned int len)
{
    (void)bus;

    unsigned char ep_num = usb_get_ep_number(ep_addr);

    usb0hw_read_ep_data(ep_num, buf, len);
}

void usbd_ep_stall(unsigned char bus, const unsigned char ep_addr)
{
    (void)bus;

    unsigned char const ep_dir   = usb_get_ep_dir(ep_addr);
    unsigned char const ep_num = usb_get_ep_number(ep_addr);

    if (ep_dir == USB_DIR_IN) {
        usb0hw_set_inep_stall(ep_num);
    } else {
        usb0hw_set_outep_stall(ep_num);
    }

    if (ep_num == 0) {
        /* receive next setup. */
        usbd_ep_read(0, 0, ep0_out_data, sizeof(ep0_out_data) / sizeof(ep0_out_data[0]));
    }
}

void usbd_ep_clear_stall(unsigned char bus, const unsigned char ep_addr)
{
    (void)bus;
    (void)ep_addr;

    unsigned char const ep_dir   = usb_get_ep_dir(ep_addr);
    unsigned char const ep_num = usb_get_ep_number(ep_addr);

    if (ep_dir == USB_DIR_IN) {
        usb0hw_clear_epin_stall(ep_num);
    } else {
        usb0hw_clear_epout_stall(ep_num);
    }
}

void usbd_set_address(unsigned char bus, unsigned char address)
{
    (void)bus;
    (void)address;

    usb0hw_set_address(address);
    usbd_ep_write(bus, 0, 0, 0); /* status. */
}

void usbd_test_mode(unsigned char bus, unsigned char test_mode)
{
    (void)bus;
    (void)test_mode;
    delay_ms(1); /* Waiting for the status phase to complete. */
    usb0hw_test_mode(test_mode);
}

static void usb_irq_handler_epout(void)
{
    usb_log("usb epout irq\r\n");

    for (unsigned char epnum = 0; epnum < 9; epnum++) {
        if ((usb0hw_get_daint() >> 16) & BIT(epnum)) {
            unsigned int doepint = usb0hw_get_doepint(epnum);
            if (doepint & FLD_USB_DOEPINT_XFERCOMPL) {
                usb0hw_clear_doepint(epnum, FLD_USB_DOEPINT_XFERCOMPL);
                unsigned short xfered_len = usb0hw_get_epout_len(epnum);
                if ((epnum == 0)) {
                    if ((xfered_len == 0)) {
                        usbd_ep_read(0, 0, ep0_out_data, sizeof(ep0_out_data) / sizeof(ep0_out_data[0]));
                        usbd_epout_complete_handler(0, 0, 0);
                    }
                    else {
                        usbd_epout_complete_handler(0, 0, xfered_len);
                    }
                } else {
                    usbd_epout_complete_handler(0, epnum, xfered_len);
                }
            }

            if (doepint & FLD_USB_DOEPINT_SETUP) {
                usb0hw_clear_doepint(epnum, FLD_USB_DOEPINT_SETUP);
                usb_log("  setup data: ");
                for (unsigned char i = 0; i < 8; i++) {
                    usb_log("0x%x ", ep0_out_data[i]);
                }
                usb_log("\r\n");

                if (!usbd_control_request_process(0, (usb_control_request_t *)ep0_out_data, 1)) {
                    usbd_ep_stall(0, 0);
                    usbd_ep_stall(0, 0 | USB_DIR_IN_MASK);
                }
            }

            if (doepint & FLD_USB_DOEPINT_STSPHSERCVD) {
                usb0hw_clear_doepint(epnum, FLD_USB_DOEPINT_STSPHSERCVD);
                usb_log("control write status phase\r\n");
                if (!usbd_control_request_process(0, (usb_control_request_t *)ep0_out_data, 0)) {
                    usbd_ep_stall(0, 0);
                    usbd_ep_stall(0, 0 | USB_DIR_IN_MASK);
                } else {
                    usbd_ep_write(0, 0, 0, 0);
                }
            }
        }
    }
}

static void usb_irq_handler_epin(void)
{
    usb_log("usb epin irq\r\n");
    for (unsigned char epnum = 0; epnum < 9; epnum++) {
        if ((usb0hw_get_daint())&BIT(epnum)) {
            unsigned int diepint = usb0hw_get_diepint(epnum);
            if (diepint & FLD_USB_DIEPINT_XFERCOMPL) {
                usb0hw_clear_diepint(epnum, FLD_USB_DIEPINT_XFERCOMPL);
                unsigned short xfered_len = usb0hw_get_epin_len(epnum);
                usb_log("usb epin xfercompl\r\n");
                if (epnum == 0) {
                    usbd_ep_read(0, 0, ep0_out_data, sizeof(ep0_out_data) / sizeof(ep0_out_data[0]));
                    usbd_epin_complete_handler(0, 0, xfered_len);
                    BM_SET(reg_usb_diepctl(0), FLD_USB_DIEPCTL_STALL);
                } else {
                    usbd_epin_complete_handler(0, epnum, xfered_len);
                }
                usb_log("in xfered_len = %d\r\n", xfered_len);
            }
        }
    }
}

static void usb_irq_handler_reset(void)
{
    usb0hw_reset();
    usbd_ep_read(0, 0, ep0_out_data, sizeof(ep0_out_data) / sizeof(ep0_out_data[0]));
}

volatile unsigned int sof_cnt;
volatile unsigned int sof_buf[4];
volatile unsigned int stimer_cnt[4];

_attribute_ram_code_sec_ void usb0_irq_handler(void)
{
    unsigned int status = usb0hw_get_gintsts() & reg_usb_gintmsk;

    if (status & FLD_USB_GINTSTS_ENUMDONE) {
        usb_log("usb0 enum done irq handler\r\n");
        usb0hw_clear_gintsts(FLD_USB_GINTSTS_ENUMDONE);
    }

    if (status & FLD_USB_GINTSTS_OEPINT) {
        usb_irq_handler_epout();
    }

    if (status & FLD_USB_GINTSTS_IEPINT) {
        usb_irq_handler_epin();
    }

    if (status & FLD_USB_GINTSTS_SOF) {
        usb0hw_clear_gintsts(FLD_USB_GINTSTS_SOF);
        sof_cnt++;
        sof_buf[sof_cnt % 4]    = usb0hw_get_sof_fn();
        stimer_cnt[sof_cnt % 4] = usb0hw_get_timer_stamp();
    }

    if (status & FLD_USB_GINTSTS_USBSUSP) {
        usb_log("usb0 suspend irq handler\r\n");
        usb0hw_clear_gintsts(FLD_USB_GINTSTS_USBSUSP);
    }

    if (status & FLD_USB_GINTSTS_WKUPINT) {
        usb_log("usb0 wakeup irq handler\r\n");
        usb0hw_clear_gintsts(FLD_USB_GINTSTS_WKUPINT);
    }

    if (status & FLD_USB_GINTSTS_USBRST) {
        usb0hw_clear_gintsts(FLD_USB_GINTSTS_USBRST);
        usb_log("usb0 reset irq handler\r\n");
        usb_irq_handler_reset();
        usbd_bus_reset(0);
    }
}
PLIC_ISR_REGISTER(usb0_irq_handler, IRQ_USB0)

#endif
