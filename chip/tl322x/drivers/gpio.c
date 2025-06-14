/********************************************************************************************************
 * @file    gpio.c
 *
 * @brief   This is the source file for tl322x
 *
 * @author  Driver Group
 * @date    2024
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "gpio.h"
/**********************************************************************************************************************
 *                                            local constants                                                       *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                              local macro                                                        *
 *********************************************************************************************************************/
#define GPIO_BIT_POSITION(byte) (byte ? __builtin_ctz(byte) : -1)

/**********************************************************************************************************************
 *                                             local data type                                                     *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                              global variable                                                       *
 *********************************************************************************************************************/

/**
 * @brief      This setting serves to set the configuration of stimer PEM event.
 */
pem_event_config_t gpio_pem_event_config = {
    .module      = PEM_EVENT_GPIO,
    .sig_sel     = 0,
    .clk_sel     = ASYNC_CLK,
    .lvl         = LEVEL,
    .edge_detect = RISING_EDGE,
    .inv         = 0,
};

/**
 * @brief      This setting serves to set the configuration of stimer PEM task.
 */
pem_task_config_t gpio_pem_task_config = {
    .module  = PEM_TASK_GPIO,
    .sig_sel = 0,
    .clk_sel = PCLK,
    .lvl     = PULSE,
};

/**********************************************************************************************************************
 *                                              local variable                                                     *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                          local function prototype                                               *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                         global function implementation                                             *
 *********************************************************************************************************************/

/**
 * @brief      This function serves to set the gpio-mux function.
 * @param[in]  pin      - the pin needs to set.
 * @param[in]  function - the function need to set.
 * @return     none.
 */
void gpio_set_mux_function(gpio_func_pin_e pin, gpio_func_e function)
{
    reg_gpio_func_mux(pin) = function;
}

/**
 * @brief      This function enable the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_en(gpio_pin_e pin)
{
    unsigned char  bit   = pin & 0xff;
    unsigned short group = pin & 0xf00;
    if ((pin == GPIO_PB4) || (pin == GPIO_PB5) || (pin == GPIO_PB6) || (pin == GPIO_PB7)) {
        analog_write_reg8(areg_gpio_pb_ie, analog_read_reg8(areg_gpio_pb_ie) | bit);
    } else if (group == GPIO_GROUPC) {
        analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie) | bit);
    } else {
        BM_SET(reg_gpio_ie(pin), bit);
    }
}

/**
 * @brief      This function set the pin's slew rate.
 * @param[in]  pin - the pin needs to set the driving strength
 * @param[in]  value - the level of slew rate
 * @return     none
 * @note       Logic HIGH selects slow slew rate, logic LOW selects fast slew rate.
 */
void gpio_set_slew_rate(gpio_pin_e pin, gpio_drv_slew_rate_e value)
{
    unsigned char  bit   = pin & 0xff;
    unsigned short group = pin & 0xf00;

    if (pin == GPIO_PB4 || pin == GPIO_PB5 || pin == GPIO_PB6 || pin == GPIO_PB7) {
        if (DRV_Slew_Rate_Fast == value) {
            analog_write_reg8(areg_gpio_pb_sr, analog_read_reg8(areg_gpio_pb_sr) & (~bit));
        } else {
            analog_write_reg8(areg_gpio_pb_sr, analog_read_reg8(areg_gpio_pb_sr) | bit);
        }
    } else if (group == GPIO_GROUPC) {
        if (DRV_Slew_Rate_Fast == value) {
            analog_write_reg8(areg_gpio_pc_sr, analog_read_reg8(areg_gpio_pc_sr) & (~bit));
        } else {
            analog_write_reg8(areg_gpio_pc_sr, analog_read_reg8(areg_gpio_pc_sr) | bit);
        }
    } else {
        if (DRV_Slew_Rate_Fast == value) {
            BM_CLR(reg_gpio_sr(pin), bit);
        } else {
            BM_SET(reg_gpio_sr(pin), bit);
        }
    }
}

/**
 * @brief      This function set the pin's driving strength.
 * @param[in]  pin - the pin needs to set the driving strength
 * @param[in]  value - the level of driving strength
 * @return     none
 * @note    | DS0 | DS1 | Drv Strength |
            | --- | --- | ------------ |
            | 0   | 0   | 2mA          |
            | 0   | 1   | 4mA          |
            | 1   | 0   | 8mA          |
            | 1   | 1   | 12mA         |
 */
void gpio_set_data_strength(gpio_pin_e pin, gpio_drv_strength_e value)
{
    unsigned char  bit   = pin & 0xff;
    unsigned short group = pin & 0xf00;
    if (pin == GPIO_PB4 || pin == GPIO_PB5 || pin == GPIO_PB6 || pin == GPIO_PB7) {
        if (DRV_STRENGTH_2MA == value) {
            analog_write_reg8(areg_gpio_pb_ds0, analog_read_reg8(areg_gpio_pb_ds0) & (~bit));
            analog_write_reg8(areg_gpio_pb_ds1, analog_read_reg8(areg_gpio_pb_ds1) & (~bit));

        } else if (DRV_STRENGTH_4MA == value) {
            analog_write_reg8(areg_gpio_pb_ds0, analog_read_reg8(areg_gpio_pb_ds0) | bit);
            analog_write_reg8(areg_gpio_pb_ds1, analog_read_reg8(areg_gpio_pb_ds1) & (~bit));
        } else if (DRV_STRENGTH_8MA == value) {
            analog_write_reg8(areg_gpio_pb_ds0, analog_read_reg8(areg_gpio_pb_ds0) & (~bit));
            analog_write_reg8(areg_gpio_pb_ds1, analog_read_reg8(areg_gpio_pb_ds1 | bit));
        } else if (DRV_STRENGTH_12MA == value) {
            analog_write_reg8(areg_gpio_pb_ds0, analog_read_reg8(areg_gpio_pb_ds0) | bit);
            analog_write_reg8(areg_gpio_pb_ds1, analog_read_reg8(areg_gpio_pb_ds1) | bit);
        }

    } else if (group == GPIO_GROUPC) {
        if (DRV_STRENGTH_2MA == value) {
            analog_write_reg8(areg_gpio_pc_ds0, analog_read_reg8(areg_gpio_pc_ds0) & (~bit));
            analog_write_reg8(areg_gpio_pc_ds1, analog_read_reg8(areg_gpio_pc_ds1) & (~bit));

        } else if (DRV_STRENGTH_4MA == value) {
            analog_write_reg8(areg_gpio_pc_ds0, analog_read_reg8(areg_gpio_pc_ds0) | bit);
            analog_write_reg8(areg_gpio_pc_ds1, analog_read_reg8(areg_gpio_pc_ds1) & (~bit));
        } else if (DRV_STRENGTH_8MA == value) {
            analog_write_reg8(areg_gpio_pc_ds0, analog_read_reg8(areg_gpio_pc_ds0) & (~bit));
            analog_write_reg8(areg_gpio_pc_ds1, analog_read_reg8(areg_gpio_pc_ds1 | bit));
        } else if (DRV_STRENGTH_12MA == value) {
            analog_write_reg8(areg_gpio_pc_ds0, analog_read_reg8(areg_gpio_pc_ds0) | bit);
            analog_write_reg8(areg_gpio_pc_ds1, analog_read_reg8(areg_gpio_pc_ds1) | bit);
        }

    } else {
        if (DRV_STRENGTH_2MA == value) {
            BM_CLR(reg_gpio_ds0(pin), bit);
            BM_CLR(reg_gpio_ds1(pin), bit);
        } else if (DRV_STRENGTH_4MA == value) {
            BM_CLR(reg_gpio_ds0(pin), bit);
            BM_SET(reg_gpio_ds1(pin), bit);
        } else if (DRV_STRENGTH_8MA == value) {
            BM_SET(reg_gpio_ds0(pin), bit);
            BM_CLR(reg_gpio_ds1(pin), bit);
        } else if (DRV_STRENGTH_12MA == value) {
            BM_SET(reg_gpio_ds0(pin), bit);
            BM_SET(reg_gpio_ds1(pin), bit);
        }
    }
}

/**
 * @brief      This function disable the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_dis(gpio_pin_e pin)
{
    unsigned char  bit   = pin & 0xff;
    unsigned short group = pin & 0xf00;
    if ((pin == GPIO_PB4) || (pin == GPIO_PB5) || (pin == GPIO_PB6) || (pin == GPIO_PB7)) {
        analog_write_reg8(areg_gpio_pb_ie, analog_read_reg8(areg_gpio_pb_ie) & (~bit));
    } else if (group == GPIO_GROUPC) {
        analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie) & (~bit));
    } else {
        BM_CLR(reg_gpio_ie(pin), bit);
    }
}

/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function
 * @param[in]  value - enable or disable the pin's input function(1: enable,0: disable )
 * @return     none
 */
void gpio_set_input(gpio_pin_e pin, unsigned char value)
{
    if (value) {
        gpio_input_en(pin);
    } else {
        gpio_input_dis(pin);
    }
}

/**
 * @brief      This function servers to set the specified GPIO into High-impedance state and also enable the pull-down resistor.
 *             To prevent power leakage, you need to call gpio_shutdown(GPIO_ALL) (set all gpio to high resistance, except SWS and MSPI)
 *             as front as possible in the program, and then initialize the corresponding GPIO according to the actual using situation.
 * @param[in]  pin  - select the specified GPIO.Only support GPIO_GROUPA ~GPIO_GROUPI,GPIO_GPOUPANA and GPIO_ALL.
 * @return     none.
 */
void gpio_shutdown(gpio_pin_e pin)
{
    unsigned short group = pin & 0xf00;
    unsigned char  bit   = pin & 0xff;
    switch (group) {
    case GPIO_GROUPA:
        reg_gpio_pa_oen |= bit;            //disable output
        reg_gpio_pa_gpio |= (bit & 0x7f);
        reg_gpio_pa_ie &= ((~bit) | 0x80); //disable input
        // set 100K pull down(except SWS)
        //A<3:0>
        analog_write_reg8(0x17, 0xaa);
        //A<7:4>
        analog_write_reg8(0x18, 0x6a);//SWS set 1M pull up
        break;
    case GPIO_GROUPB:
        reg_gpio_pb_oen |= bit;
        reg_gpio_pb_gpio |= bit;
        if ((pin == GPIO_PB4) || (pin == GPIO_PB5) || (pin == GPIO_PB6) || (pin == GPIO_PB7)) {
            analog_write_reg8(areg_gpio_pb_ie, analog_read_reg8(areg_gpio_pb_ie) & (~bit));
        } else {
            reg_gpio_pb_ie &= (~bit);
        }
        // set 100K pull down
        //B<3:0>
        analog_write_reg8(0x19, 0xaa);
       //B<7:4>
        analog_write_reg8(0x1a, 0xaa);
        break;
    case GPIO_GROUPC:
        reg_gpio_pc_oen |= bit;
        reg_gpio_pc_gpio |= bit;
        analog_write_reg8(areg_gpio_pc_ie, analog_read_reg8(areg_gpio_pc_ie) & (~bit));
        // set 100K pull down
        //C<3:0>
        analog_write_reg8(0x1b, 0xaa);
        //C<7:4>
        analog_write_reg8(0x1c, 0xaa);
        break;
    case GPIO_GROUPD:
        reg_gpio_pd_oen |= bit;
        reg_gpio_pd_gpio |= bit;
        reg_gpio_pd_ie &= (~bit);
        // set 100K pull down
        //D<3:0>
        analog_write_reg8(0x1d, 0xaa);
        //D<7:4>
        analog_write_reg8(0x1e, 0xaa);
        break;
    case GPIO_GROUPE:
        reg_gpio_pe_oen |= bit;
        reg_gpio_pe_gpio |= bit;
        reg_gpio_pe_ie &= (~bit);
        // set 100K pull down
        //E<3:0>
        analog_write_reg8(0x1f, 0xaa);
        //E<7:4>
        analog_write_reg8(0x20, 0xaa);
        break;
    case GPIO_GROUPF:
        reg_gpio_pf_oen |= bit;
        reg_gpio_pf_gpio |= bit;
        reg_gpio_pf_ie &= (~bit);
        // set 100K pull down
        //F<3:0>
        analog_write_reg8(0x21, 0xaa);
        //F<7:4>
        analog_write_reg8(0x22, 0xaa);
        break;
    case GPIO_GROUPG:
        reg_gpio_pg_oen |= bit;
        reg_gpio_pg_gpio |= bit;
        reg_gpio_pg_ie &= (~bit);
        // set 100K pull down
        //G<3:0>
        analog_write_reg8(0x23, 0xaa);
        //G<7:4>
        analog_write_reg8(0x24, 0xaa);
        break;
       case GPIO_GROUPH:
        reg_gpio_ph_oen |= bit;
        reg_gpio_ph_gpio |= bit;
        reg_gpio_ph_ie &= (~bit);
        // set 100K pull down
        //H<3:0>
        analog_write_reg8(0x25, 0xaa);
        //H<7:4>
        analog_write_reg8(0x26, 0xaa);
        break;
       case GPIO_GROUPI://GPIO_GROUPI has no pull down resistor
        reg_gpio_pi_oen |= bit;
        reg_gpio_pi_gpio |= bit;
        reg_gpio_pi_ie &= (~bit);
        break;
    case GPIO_ALL:
        //as gpio
        reg_gpio_pa_gpio = 0x7f;
        reg_gpio_pb_gpio = 0xff;
        reg_gpio_pc_gpio = 0xff;
        reg_gpio_pd_gpio = 0xff;
        reg_gpio_pe_gpio = 0xff;
        reg_gpio_pf_gpio = 0xff;
        reg_gpio_pg_gpio = 0xff;
        reg_gpio_ph_gpio = 0xff;
        reg_gpio_pi_gpio = 0x00;//PI0~PI5 for MSPI

        //output disable
        reg_gpio_pa_oen = 0xff;
        reg_gpio_pb_oen = 0xff;
        reg_gpio_pc_oen = 0xff;
        reg_gpio_pd_oen = 0xff;
        reg_gpio_pe_oen = 0xff;
        reg_gpio_pf_oen = 0xff;
        reg_gpio_pg_oen = 0xff;
        reg_gpio_ph_oen = 0xff;
        reg_gpio_pi_oen = 0x00;//PI0~PI5 for MSPI

        //disable input
        reg_gpio_pa_ie = 0x80; //SWS input is enabled
        reg_gpio_pb_ie &= 0xf0;
        analog_write_reg8(areg_gpio_pb_ie, (analog_read_reg8(areg_gpio_pb_ie) & 0x0f));
        analog_write_reg8(areg_gpio_pc_ie, 0);
        reg_gpio_pd_ie = 0x00;
        reg_gpio_pe_ie = 0x00;
        reg_gpio_pf_ie = 0x00;
        reg_gpio_pg_ie = 0x00;
        reg_gpio_ph_ie = 0x00;
        reg_gpio_pi_ie = 0x3f;//PI0~PI5 for MSPI

        // set 100K pull down(except SWS)
        //A<3:0>
        analog_write_reg8(0x17, 0xaa);
        //A<7:4>
        analog_write_reg8(0x18, 0x6a);//SWS set 1M pull up
        //B<3:0>
        analog_write_reg8(0x19, 0xaa);
        //B<7:4>
        analog_write_reg8(0x1a, 0xaa);
        //C<3:0>
        analog_write_reg8(0x1b, 0xaa);
        //C<7:4>
        analog_write_reg8(0x1c, 0xaa);
        //D<3:0>
        analog_write_reg8(0x1d, 0xaa);
        //D<7:4>
        analog_write_reg8(0x1e, 0xaa);
        //E<3:0>
        analog_write_reg8(0x1f, 0xaa);
        //E<7:4>
        analog_write_reg8(0x20, 0xaa);
        //F<3:0>
        analog_write_reg8(0x21, 0xaa);
        //F<7:4>
        analog_write_reg8(0x22, 0xaa);
        //G<3:0>
        analog_write_reg8(0x23, 0xaa);
        //G<7:4>
        analog_write_reg8(0x24, 0xaa);
        //H<3:0>
        analog_write_reg8(0x25, 0xaa);
        //H<7:4>
        analog_write_reg8(0x26, 0xaa);
        // excluding PI as there is no pull down resistor
        break;
    }
}

/**
 * @brief     This function set a pin's IRQ , here you can choose from 8 interrupts for flexible configuration, each interrupt is independent.
 * @param[in] irq           - there are 8 types of irq to choose.(irq0/irq1/irq2/irq3/irq4/irq5/irq6/irq7)
 * @param[in] pin           - the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type.
 *                            0: rising edge.
 *                            1: falling edge.
 *                            2: high level.
 *                            3: low level.
 * @return    none.
 */
void gpio_set_irq(gpio_irq_num_e irq, gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type)
{
    /*
        When selecting pull-up resistance and rising edge to trigger gpio interrupt, gpio_irq_en should be placed before setting gpio_set_irq,
        otherwise an interrupt will be triggered by mistake.
     */
    gpio_irq_en(pin, irq);
    gpio_clr_irq_status((gpio_irq_e)BIT(irq)); //must clear cause to unexpected interrupt.
    switch (trigger_type) {
    case INTR_RISING_EDGE:
        BM_CLR(reg_gpio_pol(pin), pin & 0xff);
        BM_CLR(reg_gpio_irq_level, BIT(irq));
        break;
    case INTR_FALLING_EDGE:
        BM_SET(reg_gpio_pol(pin), pin & 0xff);
        BM_CLR(reg_gpio_irq_level, BIT(irq));
        break;
    case INTR_HIGH_LEVEL:
        BM_CLR(reg_gpio_pol(pin), pin & 0xff);
        BM_SET(reg_gpio_irq_level, BIT(irq));
        break;
    case INTR_LOW_LEVEL:
        BM_SET(reg_gpio_pol(pin), pin & 0xff);
        BM_SET(reg_gpio_irq_level, BIT(irq));
        break;
    }
    if (irq == GPIO_IRQ0) {
        reg_gpio_irq_ctrl |= FLD_GPIO_CORE_INTERRUPT_EN; //Only GPIO_IRQ0 needs to enable FLD_GPIO_CORE_INTERRUPT_EN.
    }
}

/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] pin - the pin needs to set its pull-up/down resistor.
 * @param[in] up_down_res - the type of the pull-up/down resistor.
 * @return    none.
 */
void gpio_set_up_down_res(gpio_pin_e pin, gpio_pull_type_e up_down_res)
{
    unsigned char r_val = up_down_res & 0x03;

    unsigned char base_ana_reg = 0;
    if ((pin >> 8) < 8) //A-H
    {
        base_ana_reg = 0x17 + ((pin >> 8) << 1) + ((pin & 0xf0) ? 1 : 0);
    } else {
        return;
    }
    unsigned char shift_num, mask_not;

    if (pin & 0x11) {
        shift_num = 0;
        mask_not  = 0xfc;
    } else if (pin & 0x22) {
        shift_num = 2;
        mask_not  = 0xf3;
    } else if (pin & 0x44) {
        shift_num = 4;
        mask_not  = 0xcf;
    } else if (pin & 0x88) {
        shift_num = 6;
        mask_not  = 0x3f;
    } else {
        return;
    }
    analog_write_reg8(base_ana_reg, (analog_read_reg8(base_ana_reg) & mask_not) | (r_val << shift_num));
}

/**
 * @brief     This function set probe clk output.
 * @param[in] pin
 * @param[in] sel_clk
 * @return    none.
 */
void gpio_set_probe_clk_function(gpio_func_pin_e pin, probe_clk_sel_e sel_clk)
{
    reg_probe_clk_sel = (reg_probe_clk_sel & 0xe0) | sel_clk; //probe_clk_sel_e
    gpio_set_mux_function(pin, DBG_PROBE_CLK);                //sel probe_clk function
    gpio_function_dis((gpio_pin_e)pin);
}

/**********************************************************************************************************************
  *                                         local function implementation                                             *
  *********************************************************************************************************************/
/**
 * @brief      This function serves to configure the PEM task.
 * @param[in]  chn - to select the PEM channel.
 * @param[in]  pin - the pin needs to set the output function.
 * @return     none.
 */
void gpio_pem_task(pem_chn_e chn, gpio_pin_e pin)
{
    unsigned short group = (pin & 0xf00) >> 8;
    unsigned char  bit   = pin & 0xff;
    gpio_function_en(pin);
    gpio_output_en(pin);
    reg_gpio_pem_ctrl0 |= (1 << group);
    reg_gpio_pem_ctrl1 |= (group << 4);
    gpio_pem_task_config.sig_sel = GPIO_BIT_POSITION(bit);
    pem_task_config(chn, gpio_pem_task_config);
}

/**
 * @brief      This function serves to configure the PEM event.
 * @param[in]  chn - to select the PEM channel.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_pem_event(pem_chn_e chn, gpio_pin_e pin)
{
    unsigned char group = (pin & 0xf00) >> 8;
    unsigned char bit   = pin & 0xff;
    gpio_function_en(pin);
    gpio_output_dis(pin);
    gpio_input_en(pin);
    reg_gpio_pem_ctrl1 |= group;
    reg_gpio_irq_ctrl |= FLD_GPIO_PEM_EVENT_EN;
    gpio_pem_event_config.sig_sel = GPIO_BIT_POSITION(bit);
    pem_event_config(chn, gpio_pem_event_config);
}

/**
 * @brief     This function set jtag or sdp function.
 * @param[in] pin
 * @return    none.
 */
void jtag_sdp_set_pin(gpio_pin_e pin)
{
    gpio_input_en(pin);
    reg_gpio_func_mux(pin) = 0;
    gpio_function_dis(pin);
}

/**
 * @brief     This function serves to set jtag(4 wires) pin . Where, PD[4]; PD[5]; PD[6]; PD[7] correspond to TDI; TDO; TMS; TCK functions mux respectively.
 * @param[in] none
 * @return    none.
 */
void jtag_set_pin_en(void)
{
    jtag_sdp_set_pin(GPIO_PD4); //TDI
    gpio_set_up_down_res(GPIO_PD4, GPIO_PIN_PULLDOWN_100K);
    jtag_sdp_set_pin(GPIO_PD5); //TDO
    jtag_sdp_set_pin(GPIO_PD6); //TMS
    gpio_set_up_down_res(GPIO_PD6, GPIO_PIN_PULLUP_10K);
    jtag_sdp_set_pin(GPIO_PD7); //TCK
    gpio_set_up_down_res(GPIO_PD7, GPIO_PIN_PULLUP_10K);
}
