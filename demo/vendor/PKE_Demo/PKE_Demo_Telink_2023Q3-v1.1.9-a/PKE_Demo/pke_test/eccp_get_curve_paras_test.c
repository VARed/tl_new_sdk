/********************************************************************************************************
 * @file    eccp_get_curve_paras_test.c
 *
 * @brief   This is the source file for Telink RISC-V MCU
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
#include <stdio.h>

#include "lib/include/pke/pke_config.h"


#if (defined(SUPPORT_ECDH) || defined(SUPPORT_ECDSA) || defined(SUPPORT_ECIES) || defined(SUPPORT_SM2))

    #include "lib/include/crypto_common/utility.h"
    #include "lib/include/crypto_common/eccp_curve.h"
    #include "lib/include/pke/pke.h"

unsigned int brainpoolp256r1_get_curve_paras_test(void)
{
    unsigned int brainpoolp256r1_p[8] = {0x1F6E5377, 0x2013481D, 0xD5262028, 0x6E3BF623, 0x9D838D72, 0x3E660A90, 0xA1EEA9BC, 0xA9FB57DB};
    unsigned int brainpoolp256r1_p_h[8];
    unsigned int brainpoolp256r1_p_n0[1];
    //    unsigned int brainpoolp256r1_a[8]        = {0xF330B5D9,0xE94A4B44,0x26DC5C6C,0xFB8055C1,0x417AFFE7,0xEEF67530,0xFC2C3057,0x7D5A0975};
    //    unsigned int brainpoolp256r1_b[8]        = {0xFF8C07B6,0x6BCCDC18,0x5CF7E1CE,0x95841629,0xBBD77CBF,0xF330B5D9,0xE94A4B44,0x26DC5C6C};
    //    unsigned int brainpoolp256r1_Gx[8]       = {0x9ACE3262,0x3A4453BD,0xE3BD23C2,0xB9DE27E1,0xFC81B7AF,0x2C4B482F,0xCB7E57CB,0x8BD2AEB9};
    //    unsigned int brainpoolp256r1_Gy[8]       = {0x2F046997,0x5C1D54C7,0x2DED8E54,0xC2774513,0x14611DC9,0x97F8461A,0xC3DAC4FD,0x547EF835};
    unsigned int brainpoolp256r1_n[8] = {0x974856A7, 0x901E0E82, 0xB561A6F7, 0x8C397AA3, 0x9D838D71, 0x3E660A90, 0xA1EEA9BC, 0xA9FB57DB};
    unsigned int brainpoolp256r1_n_h[8];
    unsigned int brainpoolp256r1_n_n0[1];

    unsigned int std_brainpoolp256r1_p_h[8]  = {0xA6465B6C, 0x8CFEDF7B, 0x614D4F4D, 0x5CCE4C26, 0x6B1AC807, 0xA1ECDACD, 0xE5957FA8, 0x4717AA21};
    unsigned int std_brainpoolp256r1_p_n0[1] = {0xCEFD89B9};
    unsigned int std_brainpoolp256r1_n_h[8]  = {0x3312FCA6, 0xE1D8D8DE, 0x1134E4A0, 0xF35D176A, 0x6C815CB0, 0x9B7F25E7, 0xC3236762, 0x0B25F1B9};
    unsigned int std_brainpoolp256r1_n_n0[1] = {0xCBB40EE9};

    unsigned int ret;

    //    eccp_curve_t tmp_brainpoolp256r1[1];
    //
    //    tmp_brainpoolp256r1[0].eccp_p_bitLen = 256;
    //    tmp_brainpoolp256r1[0].eccp_n_bitLen = 256;
    //    tmp_brainpoolp256r1[0].eccp_p        = brainpoolp256r1_p;
    //    tmp_brainpoolp256r1[0].eccp_p_h      = brainpoolp256r1_p_h;
    //    tmp_brainpoolp256r1[0].eccp_p_n0     = brainpoolp256r1_p_n0;
    //    tmp_brainpoolp256r1[0].eccp_a        = brainpoolp256r1_a;
    //    tmp_brainpoolp256r1[0].eccp_b        = brainpoolp256r1_b;
    //    tmp_brainpoolp256r1[0].eccp_Gx       = brainpoolp256r1_Gx;
    //    tmp_brainpoolp256r1[0].eccp_Gy       = brainpoolp256r1_Gy;
    //    tmp_brainpoolp256r1[0].eccp_n        = brainpoolp256r1_n;
    //    tmp_brainpoolp256r1[0].eccp_n_h      = brainpoolp256r1_n_h;
    //    tmp_brainpoolp256r1[0].eccp_n_n0     = brainpoolp256r1_n_n0;

    ret = pke_pre_calc_mont((unsigned int *)brainpoolp256r1_p, 256, brainpoolp256r1_p_h, brainpoolp256r1_p_n0);
    if ((PKE_SUCCESS != ret) || (uint32_BigNumCmp(brainpoolp256r1_p_h, 8, std_brainpoolp256r1_p_h, 8) != 0) ||
        (brainpoolp256r1_p_n0[0] != std_brainpoolp256r1_p_n0[0])) {
        printf("\r\n get p_h error");
        print_BN_buf_U32(brainpoolp256r1_p_h, 8, "p_h");
        return 1;
    }

    ret = pke_pre_calc_mont((unsigned int *)brainpoolp256r1_n, 256, brainpoolp256r1_n_h, brainpoolp256r1_n_n0);
    if ((PKE_SUCCESS != ret) || (uint32_BigNumCmp(brainpoolp256r1_n_h, 8, std_brainpoolp256r1_n_h, 8) != 0) ||
        (brainpoolp256r1_n_n0[0] != std_brainpoolp256r1_n_n0[0])) {
        printf("\r\n get n_h error");
        print_BN_buf_U32((unsigned int *)brainpoolp256r1_n, 8, "n");
        print_BN_buf_U32(brainpoolp256r1_n_h, 8, "n_h");
        return 1;
    }

    printf("\r\n brainpoolp256r1_get_curve_paras_test success \r\n");

    return 0;
}

unsigned int ECCp_get_curve_paras_test(void)
{
    printf("\r\n\r\n =================== ECCp_get_key test =================== \r\n");


    if (brainpoolp256r1_get_curve_paras_test()) {
        return 1;
    }


    return 0;
}

#endif
