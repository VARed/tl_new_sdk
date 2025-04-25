/********************************************************************************************************
 * @file    ecdh_test.c
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

#ifdef SUPPORT_ECDH

#include "lib/include/pke/ecdh.h"
#include "lib/include/crypto_common/eccp_curve.h"
#include "lib/include/crypto_common/utility.h"

// #define ECDH_SPEED_TEST_BY_TIMER

#ifdef ECDH_SPEED_TEST_BY_TIMER
extern unsigned int startp(void);
extern unsigned int endp(unsigned char mode, unsigned int once_bytes, unsigned int round);
#endif

#define ECDH_ROUND1 (2)
#define ECDH_ROUND2 (50)

/**
 * @brief           Perform ECDH test with standard data
 * @param[in]       curve                - Pointer to the elliptic curve parameters
 * @param[in]       curve_name           - Name of the elliptic curve
 * @param[in]       local_prikey         - Local private key
 * @param[in]       local_pubkey         - Local public key
 * @param[in]       peer_prikey          - Peer private key
 * @param[in]       peer_pubkey          - Peer public key
 * @param[in]       std_key              - Standard shared secret key
 * @param[in]       key_len              - Length of the key in bytes
 * @param[in]       kdf                  - Key Derivation Function (optional)
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_test_internal(const eccp_curve_t *curve, char *curve_name, unsigned char *local_prikey, unsigned char *local_pubkey, unsigned char *peer_prikey,
                                unsigned char *peer_pubkey, unsigned char *std_key, unsigned int key_len, KDF_FUNC kdf)
{
    unsigned char local_key[ECCP_MAX_BYTE_LEN];
    unsigned char peer_key[ECCP_MAX_BYTE_LEN];
    unsigned char tmp[2 * ECCP_MAX_BYTE_LEN];
    unsigned int ret;

    // printf("\r\n\r\n ------------- %s ECDH standard data test ------------- ",
    // curve_name);

    // standard inputs
    ret = ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key, key_len, kdf);
    if (ECDH_SUCCESS != ret)
    {
        printf("\r\n %s ECDH std test error 1, ret = %u", curve_name, ret);
        return 1;
    }

    ret = ecdh_compute_key(curve, peer_prikey, local_pubkey, peer_key, key_len, kdf);
    if (ECDH_SUCCESS != ret || memcmp_(local_key, std_key, key_len) || memcmp_(local_key, peer_key, key_len))
    {
        printf("\r\n %s ECDH std test error 2, ret = %u", curve_name, ret);
        return 1;
    }

    // corner case: peer pubkey all zero
    memset_(tmp, 0, sizeof(tmp));
    ret = ecdh_compute_key(curve, local_prikey, tmp, local_key, key_len, kdf);
    if (ECDH_SUCCESS == ret)
    {
        printf("\r\n %s ECDH corner case test error 1, ret = %u", curve_name, ret);
        return 1;
    }

    // corner case: peer pubkey corrupted
    memcpy_(tmp, peer_pubkey, 2 * get_byte_len(curve->eccp_p_bitLen));
    tmp[0] += 1;
    ret = ecdh_compute_key(curve, local_prikey, tmp, local_key, key_len, kdf);
    if (ECDH_SUCCESS == ret)
    {
        printf("\r\n %s ECDH corner case test error 2, ret = %u", curve_name, ret);
        return 1;
    }

    // corner case: local_prikey is 0
    memset_(tmp, 0, sizeof(tmp));
    ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key, key_len, kdf);
    if (ECDH_SUCCESS == ret)
    {
        printf("\r\n %s ECDH corner case test error 3, ret = %u", curve_name, ret);
        return 1;
    }

    // corner case: local_prikey is n
    reverse_byte_array((const unsigned char *)curve->eccp_n, (unsigned char *)tmp, get_byte_len(curve->eccp_n_bitLen));
    ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key, key_len, kdf);
    if (ECDH_SUCCESS == ret)
    {
        printf("\r\n %s ECDH corner case test error 4, ret = %u", curve_name, ret);
        return 1;
    }

    // corner case: local_prikey is n+1
    reverse_byte_array((const unsigned char *)curve->eccp_n, (unsigned char *)tmp, get_byte_len(curve->eccp_n_bitLen));
    tmp[get_byte_len(curve->eccp_n_bitLen) - 1] += 1;
    ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key, key_len, kdf);
    if (ECDH_SUCCESS == ret)
    {
        printf("\r\n %s ECDH corner case test error 5, ret = %u", curve_name, ret);
        return 1;
    }

    printf("\r\n %s ECDH std & corner_case test success", curve_name);

    return 0;
}

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160K1))
/**
 * @brief           Perform ECDH standard test for secp160k1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp160k1(void)
{
    unsigned char local_prikey[21] = {
        0x00,
        0xC7,
        0xE0,
        0x8A,
        0x1F,
        0x97,
        0x33,
        0x60,
        0xDB,
        0x2E,
        0x2A,
        0x82,
        0x20,
        0x3C,
        0xBB,
        0x99,
        0xCE,
        0xCA,
        0xD5,
        0x43,
        0xC7,
    };
    unsigned char local_pubkey[40] = {
        0xC6,
        0x01,
        0x69,
        0x5B,
        0x1A,
        0x83,
        0x75,
        0x23,
        0xD7,
        0x9B,
        0x4D,
        0x03,
        0xC0,
        0xCB,
        0xE7,
        0x64,
        0x1D,
        0x6A,
        0x23,
        0x15,
        0x1E,
        0x8D,
        0x0D,
        0xF1,
        0x9B,
        0xE0,
        0xA2,
        0xA8,
        0xBD,
        0xB8,
        0x54,
        0xDA,
        0xCF,
        0x1D,
        0x28,
        0x93,
        0x7D,
        0x5E,
        0x3F,
        0x15,
    };
    unsigned char peer_prikey[21] = {
        0x00,
        0xA2,
        0x8D,
        0x06,
        0xED,
        0x0E,
        0x6D,
        0x70,
        0x72,
        0xAA,
        0xB4,
        0x27,
        0x4F,
        0x3E,
        0x91,
        0x9B,
        0x52,
        0xDF,
        0x9C,
        0x8A,
        0x98,
    };
    unsigned char peer_pubkey[40] = {
        0xBE,
        0x02,
        0x27,
        0xB1,
        0x84,
        0xB5,
        0x85,
        0x5E,
        0xB3,
        0xEA,
        0x4B,
        0xB0,
        0x0B,
        0x37,
        0x17,
        0x87,
        0x63,
        0x67,
        0x4F,
        0xC4,
        0x2F,
        0xFB,
        0xC5,
        0x9F,
        0xA8,
        0x01,
        0x23,
        0x10,
        0x45,
        0xAE,
        0x82,
        0x1F,
        0x34,
        0x28,
        0xC7,
        0x5B,
        0x3C,
        0x52,
        0x7C,
        0xA9,
    };
    unsigned char std_key[20] = {0xFA, 0x16, 0x2E, 0x93, 0x75, 0xAF, 0x6F, 0xFF, 0x08, 0x6E, 0x4C, 0x01, 0x07, 0xCC, 0xCF, 0xA0, 0xBE, 0xC8, 0x5B, 0xC1};
    unsigned int key_len = 20;
    unsigned int ret;

    ret = ecdh_test_internal(secp160k1, "secp160k1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192K1))
/**
 * @brief           Perform ECDH standard test for secp192k1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp192k1(void)
{
    unsigned char local_prikey[24] = {
        0xFB,
        0xAD,
        0xDB,
        0xB2,
        0x5D,
        0x72,
        0x1F,
        0xDB,
        0xD7,
        0x65,
        0x14,
        0xD8,
        0x41,
        0x7C,
        0xFF,
        0xB2,
        0x83,
        0xC8,
        0x92,
        0x5E,
        0x4C,
        0x02,
        0x04,
        0x00,
    };
    unsigned char local_pubkey[48] = {
        0x13,
        0xBC,
        0x3F,
        0xE2,
        0x22,
        0xBB,
        0x16,
        0xBF,
        0x78,
        0xC6,
        0xC7,
        0xDF,
        0x71,
        0xAC,
        0x7A,
        0x3B,
        0x36,
        0xCC,
        0xF0,
        0xCE,
        0xEC,
        0xA6,
        0xFB,
        0x08,
        0xB8,
        0x01,
        0xA0,
        0x49,
        0x90,
        0x4E,
        0x8F,
        0x79,
        0x14,
        0xF7,
        0x85,
        0xFD,
        0x3C,
        0x6F,
        0xC6,
        0x88,
        0x7A,
        0x9C,
        0x4A,
        0x63,
        0x3D,
        0xA9,
        0x37,
        0x0A,
    };
    unsigned char peer_prikey[24] = {
        0xDA,
        0x21,
        0xBD,
        0x66,
        0x7E,
        0x48,
        0x30,
        0x95,
        0x25,
        0x7E,
        0x58,
        0x1B,
        0x10,
        0x0C,
        0x72,
        0x75,
        0x6B,
        0x6F,
        0xCD,
        0xCC,
        0x4B,
        0xC1,
        0x93,
        0x71,
    };
    unsigned char peer_pubkey[48] = {
        0xCB,
        0xC3,
        0x01,
        0xA9,
        0xB0,
        0x4A,
        0x8B,
        0xA5,
        0x95,
        0xC3,
        0x66,
        0xE2,
        0x4E,
        0xB8,
        0x76,
        0x0E,
        0xC8,
        0xEF,
        0xAF,
        0x3E,
        0x94,
        0x06,
        0x53,
        0x1D,
        0x49,
        0x2B,
        0xC0,
        0x24,
        0xFE,
        0x4A,
        0xA7,
        0x2C,
        0xB0,
        0x15,
        0x69,
        0x06,
        0x4A,
        0x61,
        0xD4,
        0x7B,
        0xD2,
        0x9D,
        0xB7,
        0x68,
        0x92,
        0x60,
        0x0E,
        0x2E,
    };
    unsigned char std_key[24] = {0xAF, 0xDD, 0x5C, 0xED, 0xB9, 0xE3, 0x2B, 0x9F, 0x58, 0xDA, 0xB2, 0x44, 0x48, 0xE1, 0x1E, 0x05, 0x52, 0x00, 0x7F, 0x61, 0x66, 0xA3, 0x03, 0xE7};
    unsigned int key_len = 24;
    unsigned int ret;

    ret = ecdh_test_internal(secp192k1, "secp192k1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224K1))
/**
 * @brief           Perform ECDH standard test for secp224k1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp224k1(void)
{
    unsigned char local_prikey[29] = {
        0x00,
        0x5C,
        0xE7,
        0xE7,
        0x32,
        0xDA,
        0xD5,
        0x7B,
        0x27,
        0x89,
        0xCC,
        0xD4,
        0x1A,
        0x0F,
        0x84,
        0x4A,
        0x46,
        0x8A,
        0x8D,
        0xBB,
        0x75,
        0x4F,
        0x24,
        0x73,
        0x68,
        0x44,
        0x98,
        0x7F,
        0xC2,
    };
    unsigned char local_pubkey[56] = {
        0x54,
        0x31,
        0xD0,
        0xFF,
        0xE4,
        0xD3,
        0xE2,
        0x94,
        0xFB,
        0x6D,
        0x54,
        0xD9,
        0xDD,
        0xDB,
        0x61,
        0x02,
        0xB2,
        0x79,
        0x92,
        0x49,
        0x23,
        0x2A,
        0x2C,
        0x09,
        0x74,
        0x15,
        0x92,
        0x85,
        0x04,
        0x82,
        0x40,
        0x9F,
        0xE8,
        0x34,
        0xB2,
        0xE9,
        0x4A,
        0xB7,
        0x7B,
        0x77,
        0xEC,
        0xEF,
        0x92,
        0x08,
        0x81,
        0x60,
        0x88,
        0xFA,
        0x94,
        0x78,
        0x82,
        0x29,
        0x24,
        0x96,
        0xFE,
        0x43,
    };
    unsigned char peer_prikey[29] = {
        0x00,
        0x6F,
        0x21,
        0x14,
        0xBE,
        0xF7,
        0x26,
        0xA1,
        0x9D,
        0x94,
        0x91,
        0x7D,
        0x2A,
        0xBB,
        0x07,
        0x01,
        0x6C,
        0xE2,
        0xD5,
        0x73,
        0xD1,
        0x25,
        0xC4,
        0x4D,
        0xAC,
        0x2D,
        0x9E,
        0xCD,
        0xC7,
    };
    unsigned char peer_pubkey[56] = {
        0xDA,
        0x00,
        0x50,
        0xFE,
        0x26,
        0x11,
        0x53,
        0x09,
        0x72,
        0x0D,
        0x1C,
        0xCC,
        0x42,
        0xF1,
        0x76,
        0xE9,
        0x01,
        0x8D,
        0xFA,
        0xCE,
        0xD3,
        0xA5,
        0xBE,
        0x0D,
        0x00,
        0x93,
        0x9E,
        0xD2,
        0x29,
        0xED,
        0x79,
        0xCF,
        0x65,
        0xB0,
        0xFB,
        0x3B,
        0x1F,
        0x4B,
        0x27,
        0x3C,
        0x75,
        0x25,
        0xA3,
        0xE2,
        0xCA,
        0xF4,
        0x22,
        0x79,
        0xB0,
        0x31,
        0x64,
        0xA2,
        0x34,
        0x08,
        0x14,
        0xE9,
    };
    unsigned char std_key[28] = {0x5F, 0x98, 0x6D, 0xD3, 0x13, 0xAF, 0x2D, 0xC1, 0xE8, 0xAB, 0x36, 0x9D, 0xD9, 0xF1,
                                 0x20, 0x07, 0x45, 0x73, 0xD2, 0xA7, 0x5F, 0xD9, 0xAD, 0xCB, 0x0D, 0xA7, 0xB7, 0x03};
    unsigned int key_len = 28;
    unsigned int ret;

    ret = ecdh_test_internal(secp224k1, "secp224k1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256K1))
/**
 * @brief           Perform ECDH standard test for secp256k1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp256k1(void)
{
    unsigned char local_prikey[32] = {
        0xDF,
        0xD4,
        0x3C,
        0xD8,
        0x25,
        0xC4,
        0xA6,
        0x13,
        0x4C,
        0x85,
        0x56,
        0x41,
        0x28,
        0xB1,
        0x2C,
        0x6C,
        0x27,
        0xB1,
        0xB4,
        0xE9,
        0x81,
        0x3A,
        0x1C,
        0x66,
        0xF0,
        0x39,
        0x27,
        0x97,
        0xB1,
        0xA2,
        0x5F,
        0xC7,
    };
    unsigned char local_pubkey[64] = {
        0x5F,
        0xA9,
        0xD0,
        0x36,
        0x15,
        0xC5,
        0xFE,
        0x54,
        0xCF,
        0x77,
        0xEF,
        0xB6,
        0x4E,
        0x1C,
        0xE9,
        0xD3,
        0x7A,
        0xE6,
        0x05,
        0xBB,
        0xD4,
        0x6C,
        0x8E,
        0xEB,
        0x35,
        0xBD,
        0x74,
        0x47,
        0x8F,
        0xCA,
        0xA2,
        0xA4,
        0x90,
        0xE3,
        0x93,
        0xB2,
        0x50,
        0x3F,
        0xDC,
        0x39,
        0xA8,
        0xDF,
        0xB6,
        0x0B,
        0x20,
        0xF1,
        0x44,
        0x7B,
        0x10,
        0xFF,
        0x5C,
        0xD5,
        0xBC,
        0x12,
        0x3C,
        0xEF,
        0xC1,
        0xBD,
        0xB1,
        0x38,
        0x34,
        0x2E,
        0x4B,
        0x9A,
    };
    unsigned char peer_prikey[32] = {
        0x1B,
        0xC1,
        0x74,
        0x68,
        0x42,
        0x0E,
        0x3E,
        0x5D,
        0xC9,
        0xE4,
        0x83,
        0xB1,
        0xF1,
        0x78,
        0x35,
        0xE4,
        0xB6,
        0xD0,
        0x8E,
        0x68,
        0x90,
        0x1E,
        0x1D,
        0x09,
        0xB6,
        0x17,
        0xBC,
        0xDF,
        0xB3,
        0x3C,
        0x50,
        0xD6,
    };
    unsigned char peer_pubkey[64] = {
        0x89,
        0x94,
        0xE8,
        0x5B,
        0x75,
        0x7C,
        0xF7,
        0xD0,
        0x87,
        0x3A,
        0x9F,
        0x09,
        0x42,
        0x09,
        0x4D,
        0x22,
        0x77,
        0x64,
        0x73,
        0x24,
        0x51,
        0x78,
        0x6D,
        0x2A,
        0x75,
        0x41,
        0x0E,
        0xD6,
        0xD0,
        0xF2,
        0x8D,
        0x74,
        0xB2,
        0xBB,
        0xD4,
        0xB0,
        0xF1,
        0x91,
        0x5C,
        0xEA,
        0x0D,
        0x02,
        0xFB,
        0xD9,
        0xBB,
        0x76,
        0xEA,
        0x00,
        0x0A,
        0x7D,
        0x51,
        0xC1,
        0x59,
        0xFD,
        0xF5,
        0xBB,
        0x4F,
        0xB7,
        0xA2,
        0x32,
        0xF7,
        0x7E,
        0x9E,
        0x3C,
    };
    unsigned char std_key[32] = {0x3A, 0xC4, 0x66, 0x00, 0xF7, 0xAF, 0x2D, 0x43, 0xE6, 0x70, 0xF3, 0x3A, 0x16, 0xA7, 0x46, 0xDE,
                                 0xE3, 0x91, 0x04, 0x56, 0x86, 0x85, 0x60, 0x7D, 0x37, 0xFC, 0xE3, 0x8A, 0xBD, 0x43, 0xE3, 0x01};
    unsigned int key_len = 32;
    unsigned int ret;

    ret = ecdh_test_internal(secp256k1, "secp256k1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_BRAINPOOLP160R1))
/**
 * @brief           Perform ECDH standard test for brainpoolp160r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_brainpoolp160r1(void)
{
    unsigned char local_prikey[20] = {
        0x62,
        0x31,
        0xB2,
        0xC9,
        0xF5,
        0x85,
        0x18,
        0x56,
        0x01,
        0xF5,
        0x9F,
        0x27,
        0xC0,
        0xC0,
        0x09,
        0x2C,
        0x18,
        0xA8,
        0xC5,
        0x86,
    };
    unsigned char local_pubkey[40] = {
        0x7E,
        0x47,
        0x71,
        0x0A,
        0x1D,
        0xF2,
        0xAD,
        0xCB,
        0xD9,
        0x87,
        0x05,
        0xD3,
        0xDD,
        0x8E,
        0x51,
        0x42,
        0x3F,
        0xA6,
        0xDC,
        0x77,
        0x5A,
        0x12,
        0x42,
        0x67,
        0xFC,
        0xC8,
        0x30,
        0x05,
        0xAF,
        0xB7,
        0x0B,
        0x9D,
        0x14,
        0x88,
        0xD6,
        0x1A,
        0x55,
        0xB2,
        0xB6,
        0xC1,
    };
    unsigned char peer_prikey[20] = {
        0xCA,
        0x2A,
        0x62,
        0x0F,
        0x4D,
        0x41,
        0xA6,
        0xC3,
        0x54,
        0x36,
        0xB5,
        0x01,
        0x53,
        0xA7,
        0x38,
        0x81,
        0xAB,
        0xB2,
        0xDD,
        0x56,
    };
    unsigned char peer_pubkey[40] = {
        0xA6,
        0xD4,
        0x50,
        0x1B,
        0xCF,
        0x85,
        0xED,
        0xA4,
        0xDA,
        0x3C,
        0xAD,
        0x55,
        0x52,
        0x18,
        0x26,
        0x58,
        0x57,
        0x0B,
        0xDA,
        0x09,
        0xAD,
        0x02,
        0xE3,
        0xE8,
        0xDF,
        0xC7,
        0x0F,
        0x1D,
        0xA3,
        0x50,
        0x2F,
        0x36,
        0x9D,
        0xD8,
        0xDB,
        0xC4,
        0x9E,
        0x3F,
        0xAE,
        0x2F,
    };
    unsigned char std_key[20] = {0xD6, 0x26, 0xA5, 0xC5, 0x30, 0xBA, 0x97, 0x27, 0xB5, 0xBB, 0x57, 0x19, 0x92, 0x04, 0xE0, 0x02, 0xE7, 0xA1, 0x33, 0xBD};
    unsigned int key_len = 20;
    unsigned int ret;

    ret = ecdh_test_internal(brainpoolp160r1, "brainpoolp160r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R1))
/**
 * @brief           Perform ECDH standard test for secp160r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp160r1(void)
{
    unsigned char local_prikey[21] = {
        0x00,
        0x72,
        0x32,
        0x34,
        0xD8,
        0x65,
        0x3F,
        0x8C,
        0xBD,
        0xE6,
        0x03,
        0x50,
        0x53,
        0xA9,
        0xE7,
        0x6C,
        0x01,
        0x8C,
        0x2A,
        0x0D,
        0xAE,
    };
    unsigned char local_pubkey[40] = {
        0x35,
        0x0F,
        0xD7,
        0x8C,
        0x56,
        0x7B,
        0x14,
        0x6E,
        0xB1,
        0x15,
        0xFB,
        0x05,
        0x2A,
        0x2E,
        0x1C,
        0xEF,
        0x13,
        0x58,
        0xCE,
        0x6E,
        0x2F,
        0x4F,
        0x0C,
        0x5C,
        0x9E,
        0xAD,
        0x16,
        0x9D,
        0x49,
        0x10,
        0xF1,
        0xC8,
        0xD3,
        0x9B,
        0xFA,
        0xE7,
        0x3F,
        0xF7,
        0xFB,
        0xCA,
    };
    unsigned char peer_prikey[21] = {
        0x00,
        0x1C,
        0x40,
        0x6F,
        0xDD,
        0x5C,
        0x9F,
        0xD6,
        0xFD,
        0xC1,
        0x16,
        0x98,
        0x9A,
        0x73,
        0x2B,
        0xE8,
        0x25,
        0x88,
        0x0B,
        0xA7,
        0xE4,
    };
    unsigned char peer_pubkey[40] = {
        0xAB,
        0xB2,
        0x40,
        0x52,
        0x85,
        0x46,
        0x19,
        0x85,
        0x5E,
        0xA8,
        0x4B,
        0x86,
        0x4B,
        0x09,
        0x21,
        0x8B,
        0xB7,
        0x46,
        0x1E,
        0x15,
        0x07,
        0x87,
        0xC1,
        0xB6,
        0x4A,
        0x3B,
        0xA1,
        0x80,
        0x30,
        0x88,
        0x00,
        0x3E,
        0xE1,
        0x24,
        0x0A,
        0x32,
        0x09,
        0x79,
        0x88,
        0x2D,
    };
    unsigned char std_key[20] = {
        0xFE,
        0x61,
        0xA4,
        0x02,
        0x4D,
        0x3D,
        0x00,
        0x74,
        0x67,
        0x1B,
        0x41,
        0x59,
        0xBF,
        0x4A,
        0xC7,
        0x3B,
        0xCE,
        0x65,
        0xEF,
        0xC0,
    };
    unsigned int key_len = 20;
    unsigned int ret;

    ret = ecdh_test_internal(secp160r1, "secp160r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R2))
/**
 * @brief           Perform ECDH standard test for secp160r2 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp160r2(void)
{
    unsigned char local_prikey[21] = {
        0x00,
        0x79,
        0x2E,
        0x23,
        0x36,
        0x89,
        0xF5,
        0x16,
        0x58,
        0xC1,
        0xC6,
        0x21,
        0xCF,
        0xFD,
        0x07,
        0x3F,
        0x08,
        0x17,
        0x79,
        0xDC,
        0x37,
    };
    unsigned char local_pubkey[40] = {
        0x90,
        0xF5,
        0x55,
        0x8C,
        0x9E,
        0xA6,
        0xBF,
        0x4E,
        0x4D,
        0x56,
        0xC8,
        0x2E,
        0x5B,
        0xB2,
        0x79,
        0x23,
        0x52,
        0x31,
        0xB1,
        0x44,
        0xD1,
        0x59,
        0x6C,
        0x53,
        0xFF,
        0x03,
        0x3C,
        0x65,
        0xD7,
        0x8B,
        0x5B,
        0xF0,
        0xF3,
        0x2F,
        0x05,
        0xA2,
        0xBF,
        0xED,
        0x47,
        0x8A,
    };
    unsigned char peer_prikey[21] = {
        0x00,
        0xE3,
        0x01,
        0x57,
        0x80,
        0x99,
        0xCA,
        0x20,
        0x87,
        0x8E,
        0xFA,
        0x9C,
        0xB7,
        0x50,
        0x17,
        0xFA,
        0x77,
        0x70,
        0xA1,
        0x6B,
        0x6E,
    };
    unsigned char peer_pubkey[40] = {
        0x79,
        0xC5,
        0xBF,
        0x97,
        0xE4,
        0xD6,
        0x22,
        0x84,
        0xF4,
        0x9B,
        0x68,
        0xBA,
        0x88,
        0xF8,
        0x9D,
        0x40,
        0x66,
        0x92,
        0xDB,
        0x12,
        0xA0,
        0x30,
        0xEF,
        0xE4,
        0x6A,
        0x0D,
        0x10,
        0xF3,
        0x95,
        0x4A,
        0xEC,
        0x04,
        0xEC,
        0x01,
        0x0B,
        0x72,
        0x05,
        0x9F,
        0xD9,
        0xBE,
    };
    unsigned char std_key[20] = {
        0x34,
        0xAC,
        0x8B,
        0xA2,
        0x28,
        0xB3,
        0x63,
        0x69,
        0x78,
        0x7C,
        0x0F,
        0x65,
        0x01,
        0x6E,
        0xF4,
        0x9C,
        0x64,
        0x71,
        0xC3,
        0x04,
    };
    unsigned int key_len = 20;
    unsigned int ret;

    ret = ecdh_test_internal(secp160r2, "secp160r2", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192R1))
/**
 * @brief           Perform ECDH standard test for secp192r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp192r1(void)
{
    unsigned char local_prikey[24] = {
        0x66,
        0x7F,
        0x88,
        0xD1,
        0xB6,
        0x78,
        0x47,
        0x6E,
        0x86,
        0xCA,
        0x5D,
        0x8F,
        0xDB,
        0x8C,
        0xD9,
        0xDB,
        0xDF,
        0x90,
        0xCF,
        0xF4,
        0x31,
        0x55,
        0x27,
        0x38,
    };
    unsigned char local_pubkey[48] = {
        0xBB,
        0xED,
        0xE4,
        0xD0,
        0x9B,
        0x6D,
        0xBD,
        0xB3,
        0x43,
        0xCE,
        0x04,
        0x0F,
        0x5D,
        0x83,
        0x1C,
        0xE3,
        0xC4,
        0x1F,
        0x1E,
        0x76,
        0xD7,
        0xE9,
        0x32,
        0xB8,
        0x90,
        0xBC,
        0x04,
        0x29,
        0x3E,
        0xF5,
        0x48,
        0x4E,
        0xC0,
        0x12,
        0x74,
        0xBA,
        0xD8,
        0xD3,
        0x34,
        0x56,
        0x36,
        0x1A,
        0xD1,
        0x37,
        0xCF,
        0xA8,
        0xD3,
        0x21,
    };
    unsigned char peer_prikey[24] = {
        0x9E,
        0x44,
        0x8E,
        0xB5,
        0x88,
        0x86,
        0xA0,
        0x23,
        0x93,
        0x7E,
        0x25,
        0x6A,
        0x84,
        0x65,
        0x10,
        0x82,
        0x6F,
        0xE4,
        0x29,
        0x12,
        0xD9,
        0x2B,
        0x4C,
        0xB3,
    };
    unsigned char peer_pubkey[48] = {
        0xF7,
        0x98,
        0x0A,
        0xF3,
        0x75,
        0x26,
        0xD5,
        0x96,
        0x88,
        0xE1,
        0x0E,
        0xF7,
        0x20,
        0xEB,
        0x1A,
        0xAF,
        0xBD,
        0x9C,
        0xAE,
        0x6E,
        0x0A,
        0xD8,
        0x30,
        0x6E,
        0xC7,
        0xCB,
        0x0A,
        0x51,
        0xF5,
        0xB9,
        0x4D,
        0x7E,
        0x78,
        0x53,
        0x71,
        0x1F,
        0x62,
        0xC3,
        0x3A,
        0x2B,
        0xCB,
        0xD7,
        0x96,
        0x77,
        0x1F,
        0x56,
        0x88,
        0x3F,
    };
    unsigned char std_key[24] = {
        0x63,
        0x78,
        0x19,
        0xC8,
        0xCE,
        0x76,
        0x0E,
        0xC3,
        0x73,
        0x6E,
        0xC5,
        0x53,
        0x46,
        0xF8,
        0x02,
        0x30,
        0x07,
        0x64,
        0xFA,
        0x15,
        0x68,
        0x9B,
        0xA2,
        0x2F,
    };
    unsigned int key_len = 24;
    unsigned int ret;

    ret = ecdh_test_internal(secp192r1, "secp192r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224R1))
/**
 * @brief           Perform ECDH standard test for secp224r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp224r1(void)
{
    unsigned char local_prikey[28] = {
        0x01,
        0x6B,
        0xBE,
        0x37,
        0xA1,
        0x54,
        0xC6,
        0xB8,
        0x65,
        0x66,
        0x86,
        0x80,
        0xDA,
        0xAF,
        0x6D,
        0x7F,
        0x33,
        0x89,
        0xFC,
        0x85,
        0xB2,
        0x6B,
        0x5D,
        0x62,
        0x2C,
        0x8F,
        0x5D,
        0x8B,
    };
    unsigned char local_pubkey[56] = {
        0xD2,
        0xCB,
        0x42,
        0xE9,
        0xE4,
        0xD4,
        0x5D,
        0x7A,
        0x66,
        0xCC,
        0x3B,
        0xA2,
        0x06,
        0x2A,
        0x0E,
        0x71,
        0x62,
        0xA7,
        0x6D,
        0x7F,
        0x54,
        0x68,
        0x18,
        0xB4,
        0x78,
        0xEF,
        0xEE,
        0xA4,
        0x34,
        0x23,
        0x82,
        0x73,
        0xEB,
        0xD7,
        0x01,
        0x43,
        0x02,
        0xED,
        0xF8,
        0x69,
        0x37,
        0x61,
        0xB5,
        0x58,
        0x0A,
        0xBE,
        0xD7,
        0x26,
        0x1B,
        0x42,
        0x5A,
        0xEA,
        0x54,
        0x92,
        0x6A,
        0xFF,
    };
    unsigned char peer_prikey[28] = {
        0xBE,
        0xD9,
        0x7E,
        0x69,
        0x72,
        0xD7,
        0xD0,
        0x71,
        0xF8,
        0x6C,
        0xC8,
        0x06,
        0x25,
        0x8E,
        0x2A,
        0x03,
        0x5C,
        0xAC,
        0x82,
        0x44,
        0x3B,
        0x46,
        0xCE,
        0x6D,
        0xA5,
        0x2D,
        0xB0,
        0xA6,
    };
    unsigned char peer_pubkey[56] = {
        0x97,
        0x3C,
        0xDD,
        0x74,
        0x51,
        0x9C,
        0xD6,
        0x9E,
        0x12,
        0x46,
        0x7C,
        0x32,
        0x11,
        0x71,
        0x95,
        0x33,
        0x16,
        0x10,
        0xFB,
        0x2D,
        0xE5,
        0x7B,
        0x42,
        0x5D,
        0x51,
        0x6A,
        0x88,
        0x1C,
        0x72,
        0xCD,
        0x47,
        0x52,
        0x2F,
        0x47,
        0x82,
        0xA2,
        0x03,
        0xCE,
        0x97,
        0xEA,
        0x27,
        0x6D,
        0x7F,
        0x83,
        0x2B,
        0x14,
        0x31,
        0xEE,
        0x38,
        0xAB,
        0xDC,
        0xC1,
        0xC8,
        0x91,
        0xD7,
        0xB9,
    };
    unsigned char std_key[28] = {
        0x9A,
        0x34,
        0x27,
        0x63,
        0x84,
        0xD7,
        0x88,
        0x9B,
        0x91,
        0xF6,
        0xC2,
        0x31,
        0x9B,
        0x35,
        0x42,
        0xD8,
        0x83,
        0xE5,
        0x4C,
        0x5A,
        0x7F,
        0xF3,
        0x87,
        0x81,
        0x15,
        0xEC,
        0x8D,
        0xBC,
    };
    unsigned int key_len = 28;
    unsigned int ret;

    ret = ecdh_test_internal(secp224r1, "secp224r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BRAINPOOLP256R1))
/**
 * @brief           Perform ECDH standard test for secp256r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_brainpoolp256r1(void)
{
    unsigned char local_prikey[32] = {
        0x7F,
        0x27,
        0x2C,
        0xE1,
        0x1F,
        0xED,
        0xB0,
        0x09,
        0x69,
        0xB5,
        0x6A,
        0xA2,
        0xAC,
        0x6A,
        0xF3,
        0xFA,
        0x53,
        0xF3,
        0x98,
        0x3F,
        0x6B,
        0x30,
        0x12,
        0x7D,
        0xB2,
        0x3F,
        0x3B,
        0x42,
        0xF4,
        0x5D,
        0x0E,
        0xEF,
    };
    unsigned char local_pubkey[64] = {
        0x42,
        0x5D,
        0x6E,
        0x27,
        0x39,
        0xC7,
        0x15,
        0x26,
        0x26,
        0x96,
        0xEB,
        0x6E,
        0xE1,
        0x53,
        0xD2,
        0xF9,
        0x72,
        0xA6,
        0xBF,
        0xC7,
        0x62,
        0x81,
        0x7A,
        0x97,
        0xD9,
        0x49,
        0xF8,
        0x0E,
        0x72,
        0xE9,
        0xB5,
        0x5B,
        0x8A,
        0xA4,
        0xC2,
        0x8E,
        0xBC,
        0x7D,
        0x13,
        0x70,
        0x67,
        0xA1,
        0x70,
        0x71,
        0x9B,
        0x72,
        0x27,
        0xF6,
        0x55,
        0xB6,
        0xA9,
        0x7C,
        0x22,
        0x3C,
        0x91,
        0x96,
        0x2D,
        0x5F,
        0x4D,
        0xB2,
        0xF4,
        0x8D,
        0xD7,
        0x78,
    };
    unsigned char peer_prikey[32] = {
        0x76,
        0xD2,
        0x4C,
        0x5F,
        0xCC,
        0x9E,
        0x9A,
        0x75,
        0xEF,
        0x32,
        0xFB,
        0x64,
        0xC0,
        0xE5,
        0x99,
        0x43,
        0x0B,
        0x1E,
        0x2E,
        0x78,
        0x32,
        0xF0,
        0xDD,
        0x3B,
        0x34,
        0x76,
        0xBE,
        0xD2,
        0x6F,
        0x0D,
        0xE8,
        0xE8,
    };
    unsigned char peer_pubkey[64] = {
        0x4D,
        0x8F,
        0xC0,
        0x9A,
        0xD2,
        0xEA,
        0x45,
        0xDF,
        0x88,
        0x36,
        0xD5,
        0xE0,
        0xCF,
        0xFC,
        0x89,
        0x14,
        0xE2,
        0xEC,
        0xB5,
        0x2A,
        0x01,
        0x99,
        0x0D,
        0xDC,
        0xB2,
        0x50,
        0xC0,
        0x92,
        0x00,
        0xB5,
        0x22,
        0x3F,
        0x62,
        0x6D,
        0xB9,
        0xD2,
        0x90,
        0x63,
        0xE8,
        0xFE,
        0xAC,
        0xC6,
        0x31,
        0x1B,
        0x4B,
        0xB3,
        0x19,
        0xDB,
        0x3E,
        0x64,
        0xBA,
        0x8E,
        0xBD,
        0xDE,
        0x61,
        0xE9,
        0xCA,
        0xAD,
        0x76,
        0x6F,
        0x48,
        0xB9,
        0xA4,
        0xB8,
    };

    unsigned char std_key[32] = {0xA0, 0xD4, 0x84, 0x93, 0x4D, 0x9D, 0xDA, 0xB2, 0x7F, 0xA9, 0xA3, 0xB8, 0xC9, 0x16, 0x9D, 0x76,
                                 0x64, 0x97, 0xF1, 0x43, 0x4B, 0x6C, 0x2B, 0x04, 0x61, 0x7F, 0x92, 0x16, 0x65, 0x81, 0x51, 0x0B};
    unsigned int key_len = 32;
    unsigned int ret;

    ret = ecdh_test_internal(brainpoolp256r1, "brainpoolp256r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256R1))
/**
 * @brief           Perform ECDH standard test for secp256r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp256r1(void)
{
    unsigned char local_prikey[32] = {
        0xF4,
        0x3B,
        0xAF,
        0xC3,
        0xC7,
        0xB2,
        0xFB,
        0x10,
        0xAF,
        0xEE,
        0xD6,
        0x56,
        0x68,
        0xD3,
        0x63,
        0x4E,
        0x32,
        0x39,
        0xC1,
        0x08,
        0xBB,
        0x6A,
        0xCF,
        0x34,
        0x33,
        0x31,
        0x49,
        0x02,
        0x55,
        0x84,
        0xB3,
        0x0E,
    };
    unsigned char local_pubkey[64] = {
        0xBD,
        0x2F,
        0xEF,
        0x24,
        0xAC,
        0x63,
        0x6F,
        0x55,
        0xDB,
        0xF1,
        0xF5,
        0x20,
        0x38,
        0x85,
        0x09,
        0x7E,
        0x18,
        0x62,
        0x9E,
        0xDF,
        0xFB,
        0x36,
        0xF4,
        0x03,
        0x89,
        0x61,
        0x27,
        0x44,
        0xA1,
        0x0F,
        0x02,
        0xD0,
        0x6B,
        0x51,
        0x9E,
        0x0F,
        0x82,
        0xBB,
        0x90,
        0xD1,
        0x63,
        0x3D,
        0x07,
        0xE8,
        0x70,
        0x8F,
        0x65,
        0x95,
        0xCE,
        0xBF,
        0xF3,
        0x86,
        0x6C,
        0xFE,
        0x33,
        0xF3,
        0xCB,
        0xE8,
        0xA3,
        0x68,
        0x1E,
        0xB8,
        0x20,
        0xEA,
    };
    unsigned char peer_prikey[32] = {
        0x45,
        0x97,
        0x46,
        0x63,
        0xBE,
        0x4A,
        0xEE,
        0x88,
        0xE1,
        0xFF,
        0xD5,
        0x8B,
        0xC9,
        0x28,
        0xB0,
        0xFE,
        0xF9,
        0x18,
        0x16,
        0x3A,
        0xD5,
        0x39,
        0x9C,
        0x99,
        0x99,
        0x7A,
        0x62,
        0x09,
        0xAB,
        0xB4,
        0xA6,
        0x82,
    };
    unsigned char peer_pubkey[64] = {
        0x03,
        0xE3,
        0x11,
        0x49,
        0xDA,
        0x81,
        0x57,
        0x00,
        0x19,
        0x19,
        0xC5,
        0xFB,
        0x64,
        0xEF,
        0xD5,
        0x2F,
        0x2A,
        0x62,
        0xEF,
        0xA9,
        0xB3,
        0x10,
        0xB8,
        0xB2,
        0xEA,
        0xA5,
        0x71,
        0x6B,
        0x2B,
        0x83,
        0xAD,
        0xA5,
        0x8B,
        0x73,
        0x43,
        0xCC,
        0x6E,
        0xF6,
        0x53,
        0xC2,
        0x91,
        0x03,
        0x57,
        0xE0,
        0xDC,
        0x7A,
        0x8C,
        0x0E,
        0xD5,
        0x64,
        0xCD,
        0x20,
        0x87,
        0x30,
        0x84,
        0x72,
        0xE5,
        0xC5,
        0x23,
        0x6C,
        0x4C,
        0xEB,
        0xF6,
        0x27,
    };

    unsigned char std_key[32] = {0x09, 0xB9, 0x29, 0x4B, 0xA4, 0x68, 0x6A, 0xDC, 0xE2, 0x72, 0x77, 0xE6, 0x3D, 0x87, 0x59, 0x8E,
                                 0x94, 0xBB, 0x1F, 0x9C, 0x44, 0xF7, 0xB9, 0xBB, 0x27, 0x96, 0x3D, 0x0F, 0x4E, 0xAC, 0x86, 0x1D};
    unsigned int key_len = 32;
    unsigned int ret;

    ret = ecdh_test_internal(secp256r1, "secp256r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_SECP384R1))
/**
 * @brief           Perform ECDH standard test for secp384r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp384r1(void)
{
    unsigned char local_prikey[48] = {
        0x29,
        0xAE,
        0x2E,
        0xE6,
        0xF9,
        0x83,
        0x86,
        0xD9,
        0x3E,
        0x9A,
        0x12,
        0x0D,
        0x40,
        0xD0,
        0x1D,
        0x57,
        0x4D,
        0x6A,
        0x0B,
        0x30,
        0x69,
        0xF3,
        0xF0,
        0xF1,
        0x1C,
        0xC1,
        0xF1,
        0x5A,
        0xE2,
        0xEB,
        0x54,
        0xD4,
        0x04,
        0x42,
        0x9E,
        0xAF,
        0xFF,
        0x6D,
        0xEA,
        0x18,
        0x62,
        0xC6,
        0x2D,
        0x6E,
        0x32,
        0x22,
        0x22,
        0x86,
    };
    unsigned char local_pubkey[96] = {
        0x96,
        0xE5,
        0x48,
        0x36,
        0x01,
        0x1F,
        0x13,
        0x1E,
        0xEA,
        0x26,
        0x7A,
        0xB4,
        0xD7,
        0x29,
        0x36,
        0x46,
        0xBC,
        0x09,
        0x0D,
        0x67,
        0x58,
        0x63,
        0x9D,
        0xB2,
        0x57,
        0xA7,
        0x70,
        0x5F,
        0x4F,
        0xB8,
        0xEC,
        0x18,
        0x37,
        0x11,
        0x4E,
        0x4F,
        0xED,
        0x5A,
        0xA6,
        0x01,
        0x1E,
        0xF2,
        0x6E,
        0x7F,
        0x96,
        0x74,
        0xE5,
        0x49,
        0x51,
        0xD2,
        0x41,
        0x43,
        0x0F,
        0xB7,
        0x92,
        0x0C,
        0x3A,
        0x7A,
        0x64,
        0x0E,
        0xB4,
        0xD1,
        0x84,
        0xC9,
        0xA4,
        0x7A,
        0xD3,
        0x9F,
        0x34,
        0xC8,
        0x68,
        0x62,
        0xF0,
        0x21,
        0xE1,
        0x4C,
        0xCF,
        0xB1,
        0x4D,
        0x82,
        0xD2,
        0xB9,
        0xD3,
        0x90,
        0x9B,
        0x91,
        0x84,
        0x0A,
        0x29,
        0xCB,
        0xD6,
        0xF1,
        0xFA,
        0xF9,
        0x73,
        0xC1,
    };
    unsigned char peer_prikey[48] = {
        0x39,
        0x96,
        0x4A,
        0x0A,
        0x71,
        0x4F,
        0x65,
        0xD8,
        0x47,
        0x1F,
        0xF6,
        0x0F,
        0x53,
        0xD6,
        0xE5,
        0xA7,
        0x75,
        0x36,
        0x92,
        0x41,
        0x38,
        0x83,
        0xEF,
        0x73,
        0xB9,
        0x78,
        0x19,
        0xAC,
        0xD6,
        0x46,
        0x48,
        0xFD,
        0xC6,
        0xE7,
        0x03,
        0x21,
        0xBC,
        0x47,
        0x9E,
        0x4E,
        0x43,
        0xEE,
        0x37,
        0x41,
        0x24,
        0x11,
        0x46,
        0x0C,
    };
    unsigned char peer_pubkey[96] = {
        0xF9,
        0x3F,
        0xA9,
        0x79,
        0x58,
        0xA4,
        0xC3,
        0x57,
        0x5A,
        0x2B,
        0x61,
        0x5E,
        0x39,
        0x74,
        0x11,
        0xBF,
        0x24,
        0xCB,
        0xC3,
        0x2D,
        0x44,
        0x44,
        0xDD,
        0x72,
        0x17,
        0x0A,
        0x2C,
        0x29,
        0x3B,
        0x14,
        0x25,
        0xB6,
        0x75,
        0xFA,
        0x59,
        0x8F,
        0x5B,
        0x24,
        0x5F,
        0xD8,
        0x8F,
        0x89,
        0xCA,
        0xF9,
        0x83,
        0xF6,
        0x2E,
        0x1F,
        0x69,
        0xE1,
        0x90,
        0x34,
        0xDC,
        0x9C,
        0x33,
        0x29,
        0x60,
        0x66,
        0x00,
        0x97,
        0x63,
        0x90,
        0xA5,
        0x55,
        0x18,
        0xF0,
        0x18,
        0x56,
        0x32,
        0x37,
        0x70,
        0xFB,
        0x0C,
        0xED,
        0x8D,
        0x4D,
        0x90,
        0x86,
        0x40,
        0x09,
        0x82,
        0x0A,
        0x1A,
        0xD1,
        0x9F,
        0x82,
        0x6D,
        0x47,
        0xC4,
        0xDC,
        0x19,
        0x30,
        0x8A,
        0xB1,
        0x0C,
        0xE8,
    };

    unsigned char std_key[48] = {
        0xD2,
        0x9D,
        0xB0,
        0x44,
        0x17,
        0xA5,
        0xA5,
        0x8C,
        0xA2,
        0x68,
        0x28,
        0xCA,
        0xE1,
        0x7C,
        0x00,
        0xF3,
        0xE2,
        0xE2,
        0x31,
        0x44,
        0x1E,
        0xF2,
        0xC4,
        0x53,
        0xDF,
        0x38,
        0xB4,
        0x33,
        0xAD,
        0x93,
        0x9C,
        0xF9,
        0x71,
        0x38,
        0xD2,
        0xAF,
        0xE5,
        0x3D,
        0x70,
        0x3D,
        0x3C,
        0x6E,
        0x64,
        0x1F,
        0xAA,
        0x6D,
        0xA7,
        0xD7,
    };
    unsigned int key_len = 48;
    unsigned int ret;

    ret = ecdh_test_internal(secp384r1, "secp384r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 512) && defined(SUPPORT_BRAINPOOLP512R1))
/**
 * @brief           Perform ECDH standard test for brainpoolp512r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_brainpoolp512r1(void)
{
    unsigned char local_prikey[64] = {
        0x15,
        0xFE,
        0xF6,
        0xF9,
        0xD6,
        0x9D,
        0xD2,
        0x6D,
        0xD3,
        0xF7,
        0xD9,
        0xD4,
        0xD8,
        0xBB,
        0x92,
        0x33,
        0x6B,
        0xF9,
        0x37,
        0x05,
        0xD0,
        0x23,
        0xB7,
        0xCB,
        0xA6,
        0xCB,
        0xE4,
        0x2A,
        0x09,
        0x56,
        0x84,
        0xBB,
        0x8C,
        0x05,
        0xD3,
        0x35,
        0xAF,
        0x58,
        0x70,
        0x07,
        0x27,
        0x3C,
        0x55,
        0x7B,
        0xC0,
        0x1B,
        0xA5,
        0x01,
        0x36,
        0xC0,
        0xB5,
        0x1B,
        0x4E,
        0xA5,
        0x40,
        0xFA,
        0xEC,
        0x8A,
        0xB1,
        0x8F,
        0xB0,
        0x05,
        0x1F,
        0x42,
    };
    unsigned char local_pubkey[128] = {
        0x10,
        0x27,
        0xAA,
        0x4B,
        0x6B,
        0x18,
        0x0D,
        0x7C,
        0xC6,
        0xA4,
        0x4D,
        0x76,
        0x95,
        0x52,
        0x1C,
        0x69,
        0x26,
        0xF9,
        0x47,
        0x66,
        0x48,
        0x35,
        0x3E,
        0x6C,
        0xEB,
        0x41,
        0x44,
        0xB8,
        0x0F,
        0x67,
        0x7D,
        0xFB,
        0xEF,
        0x54,
        0x92,
        0x01,
        0x34,
        0x77,
        0x0E,
        0xEF,
        0xCF,
        0x55,
        0x93,
        0x8D,
        0x78,
        0xB8,
        0xBA,
        0x09,
        0x8C,
        0x8C,
        0x4D,
        0x9F,
        0xC3,
        0x91,
        0xA5,
        0x85,
        0x51,
        0x62,
        0xD9,
        0x9D,
        0x1C,
        0x11,
        0x66,
        0x81,
        0x40,
        0xA3,
        0xD1,
        0x5D,
        0xDD,
        0xE0,
        0x12,
        0xF3,
        0x77,
        0xD0,
        0xE8,
        0x02,
        0xE2,
        0x6C,
        0x64,
        0x5F,
        0xB4,
        0xB6,
        0xE8,
        0x93,
        0x51,
        0xE3,
        0xC8,
        0xE1,
        0x29,
        0x7A,
        0x6C,
        0xB0,
        0x19,
        0x05,
        0x13,
        0x6F,
        0x9A,
        0x91,
        0x1A,
        0x9D,
        0x38,
        0x44,
        0xF5,
        0xC8,
        0x2D,
        0x8B,
        0x1E,
        0x2C,
        0x27,
        0x3C,
        0xD0,
        0x15,
        0xE7,
        0x29,
        0xC1,
        0x95,
        0xE0,
        0x77,
        0x3D,
        0xA8,
        0xF6,
        0x5F,
        0x88,
        0xBD,
        0xCF,
        0x49,
        0xA2,
        0xE8,
    };
    unsigned char peer_prikey[64] = {
        0x2F,
        0x84,
        0x11,
        0xED,
        0xC1,
        0xF6,
        0xEB,
        0xE3,
        0xCB,
        0x39,
        0xBB,
        0xCE,
        0xDC,
        0x95,
        0x8D,
        0x9E,
        0x58,
        0x73,
        0x42,
        0xF1,
        0xAE,
        0x49,
        0xFC,
        0xC7,
        0x14,
        0x87,
        0x94,
        0xB4,
        0x5E,
        0xED,
        0xAD,
        0xDA,
        0x9E,
        0x80,
        0xA1,
        0x4F,
        0xD3,
        0xAD,
        0x69,
        0xC3,
        0x4D,
        0x26,
        0xF9,
        0xE5,
        0xD3,
        0x85,
        0x6C,
        0xC5,
        0x7E,
        0x53,
        0x66,
        0x66,
        0xCE,
        0x05,
        0x47,
        0xD7,
        0x2E,
        0x37,
        0x1B,
        0xF3,
        0x0F,
        0xCD,
        0x3F,
        0xE2,
    };
    unsigned char peer_pubkey[128] = {
        0x8B,
        0x58,
        0xE5,
        0xBD,
        0x88,
        0xC4,
        0x1D,
        0x29,
        0xC8,
        0x5D,
        0x92,
        0x0B,
        0x64,
        0xD2,
        0x14,
        0xF0,
        0x45,
        0x26,
        0x25,
        0x2A,
        0x16,
        0x70,
        0x6C,
        0x41,
        0x23,
        0xD3,
        0xAD,
        0xD4,
        0x08,
        0xC4,
        0x62,
        0xFB,
        0xB4,
        0xCB,
        0xB9,
        0xE1,
        0x9F,
        0xF4,
        0x34,
        0x61,
        0x76,
        0x06,
        0x55,
        0x43,
        0x5C,
        0x2C,
        0x9E,
        0xD9,
        0x81,
        0x3A,
        0x98,
        0xCA,
        0x6E,
        0x06,
        0xAE,
        0x5A,
        0x08,
        0x6C,
        0x28,
        0xC7,
        0x55,
        0x0F,
        0xEF,
        0xDC,
        0x6C,
        0x10,
        0x29,
        0xC0,
        0x67,
        0xEB,
        0x73,
        0x27,
        0x36,
        0x69,
        0x7B,
        0x2E,
        0x27,
        0xA3,
        0x4D,
        0xE0,
        0xEC,
        0x46,
        0xA9,
        0x1E,
        0xDF,
        0xAC,
        0x4A,
        0x38,
        0xC2,
        0x9A,
        0x0A,
        0x19,
        0x09,
        0x57,
        0x46,
        0xD9,
        0xDD,
        0x20,
        0xE6,
        0x34,
        0xDB,
        0x89,
        0x41,
        0x64,
        0xC3,
        0x4E,
        0x69,
        0xED,
        0x97,
        0x23,
        0xE1,
        0x1A,
        0x2D,
        0x34,
        0x48,
        0x86,
        0x0D,
        0x98,
        0xDB,
        0x67,
        0xFF,
        0x37,
        0x7A,
        0xCD,
        0x55,
        0x9D,
        0x93,
        0x32,
    };

    unsigned char std_key[64] = {
        0x3B,
        0x87,
        0x3B,
        0xFB,
        0x4A,
        0xE0,
        0xCA,
        0x1C,
        0xB7,
        0xF0,
        0xDB,
        0xEF,
        0x86,
        0x9C,
        0x96,
        0xF3,
        0xB8,
        0x5C,
        0xD5,
        0x6D,
        0x29,
        0xCF,
        0x1F,
        0xBF,
        0xF9,
        0xF8,
        0xE9,
        0x83,
        0xE3,
        0x42,
        0x93,
        0x27,
        0xD2,
        0xB0,
        0x17,
        0x4E,
        0x9B,
        0x76,
        0x3B,
        0x48,
        0xAB,
        0x2A,
        0x6D,
        0x87,
        0x8A,
        0xA3,
        0xF8,
        0xE3,
        0x6E,
        0x5C,
        0xBD,
        0x29,
        0x4B,
        0xCA,
        0x36,
        0x5E,
        0x41,
        0x12,
        0x3A,
        0x6E,
        0x00,
        0xEF,
        0x7E,
        0x3A,
    };
    unsigned int key_len = 64;
    unsigned int ret;

    ret = ecdh_test_internal(brainpoolp512r1, "brainpoolp512r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 521) && defined(SUPPORT_SECP521R1))
/**
 * @brief           Perform ECDH standard test for secp521r1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_secp521r1(void)
{
    unsigned char local_prikey[66] = {
        0x00,
        0x3B,
        0x1A,
        0x32,
        0x0D,
        0xAA,
        0x7B,
        0xA5,
        0x77,
        0xA7,
        0x19,
        0x32,
        0x23,
        0xA4,
        0x9A,
        0x4A,
        0x1B,
        0xE4,
        0xC6,
        0x60,
        0x68,
        0x56,
        0xE5,
        0x87,
        0x7E,
        0x63,
        0x58,
        0x63,
        0xC0,
        0x1A,
        0xC8,
        0xBD,
        0x21,
        0x7C,
        0x51,
        0x0F,
        0xB1,
        0x86,
        0xC7,
        0x12,
        0xF5,
        0x2D,
        0x49,
        0x1B,
        0x01,
        0x16,
        0x93,
        0x50,
        0x81,
        0xEF,
        0x1F,
        0xF7,
        0xC4,
        0x36,
        0x5D,
        0xBC,
        0x5F,
        0x7C,
        0xFC,
        0x42,
        0xE0,
        0x65,
        0xE9,
        0x6B,
        0x0F,
        0x22,
    };
    unsigned char local_pubkey[132] = {
        0x01,
        0x84,
        0xB6,
        0xE1,
        0x7A,
        0x48,
        0x3B,
        0xCC,
        0x5E,
        0x3E,
        0x7B,
        0x41,
        0x74,
        0xFC,
        0xD8,
        0x17,
        0x43,
        0xF7,
        0x08,
        0xE1,
        0xB0,
        0xD9,
        0x43,
        0x4E,
        0x6D,
        0x99,
        0x31,
        0x68,
        0x94,
        0x33,
        0xBA,
        0x6A,
        0xE5,
        0xE6,
        0xD8,
        0xA8,
        0xE9,
        0x99,
        0x7F,
        0xE7,
        0x20,
        0x18,
        0xCA,
        0x80,
        0x34,
        0x19,
        0x00,
        0x4C,
        0xA4,
        0xF4,
        0x3A,
        0xD4,
        0x65,
        0xF9,
        0x2D,
        0x47,
        0x42,
        0x08,
        0x96,
        0xF8,
        0x3F,
        0x92,
        0xC6,
        0xDF,
        0xDD,
        0x01,
        0x01,
        0x7F,
        0x3E,
        0x09,
        0x6B,
        0x00,
        0xE0,
        0xA0,
        0x38,
        0xEF,
        0x7F,
        0x84,
        0xC1,
        0x92,
        0x90,
        0x91,
        0x2D,
        0x8C,
        0x68,
        0x1E,
        0x6A,
        0x24,
        0xBB,
        0xC1,
        0x37,
        0xBE,
        0x5A,
        0x6E,
        0x85,
        0x43,
        0xFB,
        0xC3,
        0x89,
        0x3E,
        0x6E,
        0xB0,
        0x4D,
        0x89,
        0x25,
        0xE7,
        0xA3,
        0xD4,
        0x09,
        0x4D,
        0xD1,
        0x41,
        0x81,
        0x61,
        0xD9,
        0x4B,
        0x67,
        0xB0,
        0x58,
        0xD4,
        0x36,
        0xEF,
        0xD1,
        0x3F,
        0xE5,
        0xFE,
        0x37,
        0xAF,
        0x58,
        0x1B,
        0x5F,
        0xCD,
    };
    unsigned char peer_prikey[66] = {
        0x00,
        0xB4,
        0x21,
        0xC4,
        0x6E,
        0xD1,
        0x32,
        0x45,
        0x3A,
        0xD1,
        0xCC,
        0xDA,
        0x00,
        0xB9,
        0xA1,
        0xCB,
        0x78,
        0x83,
        0xAF,
        0xDE,
        0x71,
        0x99,
        0xA0,
        0xE6,
        0x6D,
        0x5A,
        0xF9,
        0x31,
        0xB6,
        0x69,
        0xA5,
        0x61,
        0x92,
        0xAD,
        0x40,
        0xF6,
        0x99,
        0xA9,
        0x66,
        0xB4,
        0x62,
        0xDC,
        0x4E,
        0x7D,
        0xF5,
        0x3F,
        0x95,
        0x18,
        0xE1,
        0x1E,
        0xA5,
        0x39,
        0xCE,
        0x4B,
        0xD2,
        0x53,
        0xE6,
        0xAA,
        0xB3,
        0xD9,
        0xB4,
        0xDF,
        0xD8,
        0x58,
        0x2C,
        0xB3,
    };
    unsigned char peer_pubkey[132] = {
        0x01,
        0xE1,
        0x11,
        0x48,
        0x93,
        0xBE,
        0x8C,
        0x37,
        0xAC,
        0x29,
        0x26,
        0x9D,
        0x26,
        0x47,
        0x07,
        0xBE,
        0xFF,
        0x9E,
        0xD2,
        0x6F,
        0xBF,
        0x77,
        0x38,
        0xBE,
        0x46,
        0x34,
        0x45,
        0xCB,
        0x1A,
        0xAF,
        0x4A,
        0xDB,
        0xC8,
        0xE0,
        0xE2,
        0xEA,
        0xCA,
        0xAA,
        0x6C,
        0x00,
        0x80,
        0xC1,
        0x64,
        0xBE,
        0x67,
        0x9F,
        0x43,
        0x02,
        0x13,
        0x80,
        0x0F,
        0x50,
        0xA4,
        0x4F,
        0xFF,
        0x62,
        0x95,
        0xF1,
        0xDE,
        0xA9,
        0x9E,
        0x75,
        0xD0,
        0x0E,
        0x66,
        0xB0,
        0x01,
        0xC1,
        0x8F,
        0x72,
        0x62,
        0x8E,
        0x30,
        0x37,
        0xC8,
        0x74,
        0x48,
        0x4A,
        0xDF,
        0x66,
        0xDA,
        0x9C,
        0x22,
        0xFA,
        0xB3,
        0x6D,
        0x73,
        0x9D,
        0x86,
        0xF1,
        0x4F,
        0x58,
        0x31,
        0xA2,
        0x46,
        0xD1,
        0x9C,
        0x2A,
        0x21,
        0xEA,
        0xB3,
        0x3E,
        0x6D,
        0xF2,
        0x01,
        0xF3,
        0x58,
        0x84,
        0x35,
        0x22,
        0xAB,
        0x6F,
        0xAA,
        0xDA,
        0x3F,
        0xDF,
        0x7F,
        0x8E,
        0x82,
        0x0C,
        0x63,
        0x2C,
        0xA9,
        0x7C,
        0x53,
        0x1E,
        0x13,
        0x78,
        0x61,
        0x0F,
        0x09,
        0xBE,
    };

    unsigned char std_key[66] = {0x01, 0x50, 0x31, 0x02, 0x40, 0x64, 0xDD, 0x30, 0x36, 0xD2, 0x42, 0x03, 0x27, 0x95, 0x77, 0xE0, 0x17, 0x67, 0x66, 0xD7, 0x9E, 0x53,
                                 0xF1, 0x9F, 0x1C, 0x1C, 0x4A, 0x6B, 0xD0, 0x43, 0xD4, 0xE7, 0x51, 0x07, 0x96, 0x64, 0xF5, 0x18, 0xE9, 0xD6, 0x6C, 0x70, 0x10, 0xC2,
                                 0x75, 0x17, 0x52, 0xEE, 0x2E, 0xA8, 0xA7, 0x4A, 0xE9, 0x9E, 0xE5, 0x80, 0x7B, 0xD6, 0xC7, 0x42, 0xBA, 0xDE, 0xDC, 0x58, 0xD4, 0x65};
    unsigned int key_len = 66;
    unsigned int ret;

    ret = ecdh_test_internal(secp521r1, "secp521r1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

#if ((ECCP_MAX_BIT_LEN >= 1024) && defined(SUPPORT_ANDERS_1024_1))
/**
 * @brief           Perform ECDH standard test for anders_1024_1 curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_std_test_anders_1024_1(void)
{
    unsigned char local_prikey[128] = {
        0xAB,
        0x9C,
        0xAD,
        0xEA,
        0xAE,
        0x36,
        0x74,
        0xFC,
        0xE7,
        0x76,
        0x99,
        0x5B,
        0x80,
        0x91,
        0xA2,
        0x95,
        0x71,
        0xEC,
        0x54,
        0x3E,
        0x46,
        0x89,
        0x6E,
        0x25,
        0x6F,
        0x94,
        0x56,
        0xE1,
        0xC0,
        0xC7,
        0xA9,
        0x9E,
        0x7E,
        0xD6,
        0xDD,
        0x15,
        0x07,
        0x4E,
        0xDB,
        0xE6,
        0xA9,
        0x07,
        0xC9,
        0x30,
        0xBB,
        0xAB,
        0xD6,
        0x0E,
        0xD8,
        0x49,
        0x7B,
        0x15,
        0xD7,
        0x04,
        0x70,
        0x9B,
        0x1D,
        0xC3,
        0x54,
        0xC9,
        0x93,
        0xD2,
        0xF3,
        0x97,
        0xF0,
        0xA5,
        0x7A,
        0x4F,
        0x95,
        0x17,
        0xDC,
        0xFB,
        0x7E,
        0xBF,
        0xB8,
        0x2A,
        0x93,
        0xCC,
        0xD2,
        0x2A,
        0x4B,
        0xF8,
        0xA0,
        0x95,
        0x25,
        0x8F,
        0xCA,
        0x0D,
        0xD6,
        0x79,
        0x1C,
        0x96,
        0x3E,
        0x05,
        0x26,
        0x7C,
        0xE1,
        0xD3,
        0x22,
        0xDE,
        0x7F,
        0x34,
        0xC4,
        0x62,
        0x8B,
        0xFF,
        0x43,
        0xFB,
        0xE3,
        0x2D,
        0x43,
        0xA4,
        0x83,
        0x85,
        0x7F,
        0x7A,
        0x64,
        0x3C,
        0x27,
        0xFB,
        0xBE,
        0x7B,
        0xA3,
        0xA1,
        0xA7,
        0x25,
        0x3D,
        0x9B,
    };
    unsigned char local_pubkey[256] = {
        0xB4,
        0xD2,
        0xBC,
        0x27,
        0xAD,
        0xF5,
        0xA1,
        0x75,
        0x07,
        0x7F,
        0x65,
        0x78,
        0x04,
        0x29,
        0x74,
        0x2F,
        0xB2,
        0xE9,
        0x4A,
        0x4A,
        0x85,
        0xD0,
        0x28,
        0xD9,
        0x1F,
        0xE4,
        0xD4,
        0xFF,
        0x5D,
        0x9C,
        0xEF,
        0x3F,
        0xE9,
        0x7A,
        0x2B,
        0x9F,
        0xEB,
        0x97,
        0x22,
        0xFF,
        0x4F,
        0x19,
        0x18,
        0xB1,
        0xC9,
        0xFD,
        0x21,
        0xD9,
        0x3A,
        0xEA,
        0x04,
        0xDE,
        0xBC,
        0x8C,
        0xA7,
        0x88,
        0x55,
        0x46,
        0xC2,
        0xED,
        0xF9,
        0xDF,
        0x0E,
        0xA2,
        0x15,
        0xC8,
        0xC7,
        0x4E,
        0x83,
        0x9B,
        0xB3,
        0xE1,
        0x37,
        0xF4,
        0xD5,
        0x46,
        0x9A,
        0x92,
        0x97,
        0x94,
        0x04,
        0x8E,
        0x0C,
        0xC5,
        0xC8,
        0x03,
        0x17,
        0x8C,
        0x29,
        0x29,
        0x24,
        0xBA,
        0xC3,
        0xC2,
        0x16,
        0x0D,
        0x4C,
        0xC8,
        0x30,
        0x13,
        0x45,
        0xDF,
        0xE0,
        0x72,
        0xAA,
        0xA1,
        0xF8,
        0x4B,
        0x25,
        0x57,
        0xFC,
        0x7C,
        0x56,
        0x32,
        0x14,
        0xFD,
        0x50,
        0x46,
        0x11,
        0x37,
        0xBC,
        0x28,
        0x5E,
        0x8C,
        0xC4,
        0xF6,
        0xA7,
        0xCC,
        0xF3,
        0x95,
        0xA5,
        0x54,
        0xA9,
        0xC1,
        0x0B,
        0x22,
        0x47,
        0xD4,
        0x68,
        0xDE,
        0xD2,
        0x4C,
        0x1C,
        0x75,
        0x67,
        0x3A,
        0x5F,
        0x3A,
        0x22,
        0x1E,
        0x3D,
        0xED,
        0x3A,
        0xDB,
        0x41,
        0x37,
        0xB6,
        0x33,
        0x04,
        0xF9,
        0x0A,
        0x5A,
        0xC6,
        0x09,
        0x7D,
        0x61,
        0x0E,
        0x3D,
        0x7A,
        0x6A,
        0x9D,
        0xF9,
        0xCC,
        0xA3,
        0xCD,
        0x13,
        0xB1,
        0x2B,
        0xF8,
        0x3B,
        0xD9,
        0x83,
        0xDB,
        0x55,
        0xBF,
        0x59,
        0xEA,
        0x02,
        0x51,
        0x49,
        0x74,
        0xE7,
        0x41,
        0x1B,
        0x37,
        0x8F,
        0xF9,
        0x18,
        0x74,
        0x88,
        0x99,
        0x15,
        0x75,
        0x38,
        0xEF,
        0x33,
        0x95,
        0x4E,
        0x5E,
        0x61,
        0x46,
        0xA1,
        0x82,
        0x28,
        0x3B,
        0x5C,
        0xA5,
        0x2E,
        0xF8,
        0x8D,
        0x21,
        0xA2,
        0x1C,
        0x14,
        0x70,
        0xAF,
        0xA8,
        0x98,
        0x62,
        0xA0,
        0x9D,
        0x7F,
        0xFE,
        0x65,
        0xF5,
        0xEF,
        0xAD,
        0x24,
        0xF3,
        0xAB,
        0x5D,
        0x65,
        0xBE,
        0x0E,
        0x75,
        0x39,
        0xB4,
        0x04,
        0x5D,
        0x19,
        0xB6,
        0xDF,
        0x7F,
        0xE2,
        0xEC,
        0xB7,
    };
    unsigned char peer_prikey[128] = {
        0xD2,
        0x81,
        0xD9,
        0xF5,
        0x3A,
        0x40,
        0xCC,
        0x22,
        0xD7,
        0xCB,
        0xB0,
        0x3B,
        0x4A,
        0x8C,
        0x46,
        0x4B,
        0xF7,
        0x34,
        0xE1,
        0x57,
        0xAA,
        0x30,
        0x0E,
        0x10,
        0x91,
        0xE0,
        0xD9,
        0xB0,
        0x0C,
        0x55,
        0xB4,
        0x8C,
        0x8B,
        0xC1,
        0x84,
        0xF6,
        0x5A,
        0x3A,
        0x51,
        0x68,
        0x64,
        0x36,
        0x94,
        0x87,
        0xBC,
        0x8F,
        0x1D,
        0x2C,
        0x28,
        0x56,
        0x3B,
        0xDA,
        0x90,
        0x6D,
        0xD5,
        0x02,
        0x69,
        0x32,
        0xE4,
        0xAB,
        0x44,
        0x4D,
        0x7F,
        0x12,
        0x20,
        0xF8,
        0xDA,
        0x39,
        0xCF,
        0x5D,
        0xE0,
        0x9D,
        0xE6,
        0x36,
        0x2F,
        0xF5,
        0xD0,
        0x47,
        0xD7,
        0x22,
        0x15,
        0x03,
        0x74,
        0x2F,
        0xFF,
        0x0F,
        0x0F,
        0x56,
        0xD2,
        0x76,
        0x55,
        0x79,
        0x20,
        0x06,
        0x85,
        0x61,
        0x1F,
        0x31,
        0x7D,
        0x8C,
        0xAC,
        0x61,
        0x13,
        0x4D,
        0xB2,
        0x3D,
        0xC9,
        0xCD,
        0x8F,
        0x92,
        0x52,
        0xE9,
        0x2E,
        0x29,
        0xF6,
        0xBF,
        0x46,
        0xEA,
        0x82,
        0x21,
        0x8E,
        0xA8,
        0x08,
        0x55,
        0x03,
        0x92,
        0x29,
        0x0F,
    };
    unsigned char peer_pubkey[256] = {
        0xDD,
        0x09,
        0xD7,
        0x95,
        0x04,
        0xE0,
        0xFE,
        0x26,
        0x0C,
        0x6E,
        0x6F,
        0x03,
        0xC1,
        0x0F,
        0xF6,
        0x50,
        0xC5,
        0x32,
        0xC1,
        0xF4,
        0x09,
        0xA4,
        0x1D,
        0x6E,
        0x62,
        0x17,
        0xCE,
        0xF3,
        0x15,
        0xD1,
        0x09,
        0x31,
        0xD6,
        0xF0,
        0xD7,
        0x78,
        0xCC,
        0xCA,
        0x92,
        0xE9,
        0xCE,
        0xE0,
        0xA3,
        0xCD,
        0xC3,
        0x93,
        0x83,
        0xE6,
        0xCC,
        0xB8,
        0xE2,
        0xF5,
        0xE4,
        0x5C,
        0xC2,
        0x90,
        0xD4,
        0xAC,
        0x01,
        0x76,
        0x6D,
        0x0A,
        0x8E,
        0xF0,
        0x7A,
        0x59,
        0xB1,
        0x11,
        0x49,
        0xE2,
        0x01,
        0x86,
        0x62,
        0x97,
        0xC2,
        0x9E,
        0x2F,
        0x0E,
        0x61,
        0x3B,
        0xDC,
        0xD9,
        0xB6,
        0xD1,
        0x93,
        0x94,
        0xB6,
        0x8A,
        0xA9,
        0xF9,
        0xCE,
        0x67,
        0x8F,
        0x86,
        0x40,
        0xE4,
        0x58,
        0xF9,
        0xCA,
        0x32,
        0xB0,
        0x26,
        0x67,
        0xBD,
        0xF5,
        0x8E,
        0x88,
        0xFD,
        0x91,
        0x10,
        0xF1,
        0xF0,
        0x8B,
        0x8C,
        0xF1,
        0xFE,
        0x98,
        0x54,
        0x1D,
        0xA9,
        0x2F,
        0xD9,
        0xF3,
        0xC6,
        0x82,
        0x36,
        0x57,
        0x56,
        0xA9,
        0xD8,
        0x2F,
        0x71,
        0x03,
        0x48,
        0x3A,
        0x3C,
        0x27,
        0x77,
        0x08,
        0xD4,
        0x02,
        0xC2,
        0xCA,
        0xC9,
        0xD5,
        0xDC,
        0x97,
        0x19,
        0x3C,
        0x90,
        0xA0,
        0x19,
        0x17,
        0x04,
        0x67,
        0xFD,
        0x6D,
        0x72,
        0x94,
        0xD3,
        0x54,
        0x33,
        0x61,
        0x61,
        0xE2,
        0x78,
        0xAE,
        0x00,
        0x40,
        0x82,
        0x48,
        0xAA,
        0x7A,
        0x9E,
        0x1A,
        0x3F,
        0x80,
        0xE4,
        0x8A,
        0x0C,
        0xDA,
        0x2D,
        0xC3,
        0x2F,
        0xBF,
        0x57,
        0xC5,
        0xEE,
        0x8E,
        0xFB,
        0xAF,
        0x2B,
        0x3B,
        0x93,
        0x75,
        0x6B,
        0x05,
        0x7D,
        0x8D,
        0xC8,
        0xD4,
        0x5F,
        0x14,
        0x1A,
        0xD7,
        0x23,
        0xDB,
        0xD9,
        0x44,
        0x14,
        0xDE,
        0xF9,
        0x7A,
        0xC8,
        0x64,
        0xD9,
        0xC5,
        0x5A,
        0xB9,
        0x8D,
        0xEA,
        0x15,
        0x44,
        0x00,
        0x89,
        0x05,
        0x5C,
        0xD5,
        0x1E,
        0x89,
        0x90,
        0xF9,
        0x08,
        0x29,
        0x9F,
        0x81,
        0xA7,
        0xA8,
        0x82,
        0xEA,
        0xAB,
        0xEF,
        0xD7,
        0x2E,
        0xC9,
        0x07,
        0x59,
        0x23,
        0x78,
        0xFF,
        0x88,
        0xE3,
        0xDB,
        0xCD,
        0xED,
        0xCF,
    };

    unsigned char std_key[128] = {
        0xE8,
        0x89,
        0xD8,
        0xCF,
        0xAF,
        0xC5,
        0x53,
        0x36,
        0x10,
        0x33,
        0xAB,
        0x8E,
        0x47,
        0x19,
        0x63,
        0xBD,
        0x94,
        0xBB,
        0xED,
        0xD7,
        0x1E,
        0x39,
        0x03,
        0x2E,
        0xC0,
        0x26,
        0x46,
        0x05,
        0x2C,
        0x01,
        0x87,
        0x70,
        0xEE,
        0xC7,
        0xDE,
        0x77,
        0x13,
        0xBA,
        0x98,
        0x44,
        0xA3,
        0xD4,
        0x43,
        0xAD,
        0xA6,
        0x12,
        0x70,
        0xF3,
        0x8C,
        0x6A,
        0x31,
        0xC2,
        0x3D,
        0x01,
        0x19,
        0x05,
        0x09,
        0x4A,
        0xE9,
        0xDC,
        0x0A,
        0xF9,
        0x00,
        0xC0,
        0xF4,
        0xE2,
        0xEA,
        0xE8,
        0x73,
        0xE0,
        0x71,
        0x52,
        0xCE,
        0x47,
        0xE4,
        0x4F,
        0x21,
        0xFC,
        0xFF,
        0x80,
        0xDD,
        0x1D,
        0x4F,
        0x15,
        0x01,
        0xAF,
        0x13,
        0x6B,
        0x33,
        0x7C,
        0x64,
        0x23,
        0x1E,
        0x33,
        0x8C,
        0xFB,
        0xAA,
        0x1D,
        0x20,
        0xE7,
        0x3E,
        0x59,
        0x2F,
        0x21,
        0xEB,
        0x1C,
        0x41,
        0x23,
        0x5E,
        0xEA,
        0xE8,
        0xF3,
        0xAD,
        0xDE,
        0xC2,
        0x52,
        0x7E,
        0xDC,
        0x9C,
        0xBB,
        0x96,
        0x28,
        0xD5,
        0xB3,
        0x8C,
        0x07,
        0xC9,
        0x92,
    };
    unsigned int key_len = 128;
    unsigned int ret;

    ret = ecdh_test_internal(anders_1024_1, "anders_1024_1", local_prikey, local_pubkey, peer_prikey, peer_pubkey, std_key, key_len, NULL);
    if (ret)
    {
        return 1;
    }

    return 0;
}
#endif

/**
 * @brief           Perform ECDH test with random data
 * @param[in]       curve                - Pointer to the elliptic curve parameters
 * @param[in]       curve_name           - Name of the elliptic curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_rand_test(const eccp_curve_t *curve, char *curve_name)
{
    unsigned char local_prikey[ECCP_MAX_BYTE_LEN];
    unsigned char local_pubkey[2 * ECCP_MAX_BYTE_LEN];
    unsigned char peer_prikey[ECCP_MAX_BYTE_LEN];
    unsigned char peer_pubkey[2 * ECCP_MAX_BYTE_LEN];
    unsigned char local_key[ECCP_MAX_BYTE_LEN];
    unsigned char peer_key[ECCP_MAX_BYTE_LEN];
    unsigned int i, j;
    unsigned int key_len, truekey_len;
    KDF_FUNC kdf = NULL;
    unsigned int ret;

    printf("\r\n\r\n ------------- %s ECDH rand data test ------------- ", curve_name);

    for (i = 0; i < ECDH_ROUND1; i++)
    {
        ret = eccp_getkey(curve, local_prikey, local_pubkey);
        if (PKE_SUCCESS != ret)
        {
            printf("\r\n %s ECDH A get key error, ret=%u", curve_name, ret);
            return 1;
        }

        ret = eccp_getkey(curve, peer_prikey, peer_pubkey);
        if (PKE_SUCCESS != ret)
        {
            printf("\r\n %s ECDH B get key error, ret=%u", curve_name, ret);
            return 1;
        }

        for (j = 0; j < ECDH_ROUND2; j++)
        {
            printf("\r\n %s ECDH round1=%u, round2=%u", curve_name, i + 1, j + 1);

            key_len = local_prikey[0] + local_prikey[1];
            key_len %= 200;
            if (0 == key_len)
            {
                key_len += 1;
            }
            truekey_len = key_len;
            if (NULL == kdf)
            {
                if (key_len > get_byte_len(curve->eccp_p_bitLen))
                {
                    truekey_len = get_byte_len(curve->eccp_p_bitLen);
                }
            }

            ret = ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key, key_len, kdf);
            if (ECDH_SUCCESS != ret)
            {
                printf("\r\n %s ECDH test error 1, ret = %u", curve_name, ret);
                return 1;
            }

            ret = ecdh_compute_key(curve, peer_prikey, local_pubkey, peer_key, key_len, kdf);
            if (ECDH_SUCCESS != ret || memcmp_(local_key, peer_key, truekey_len))
            {
                printf("\r\n %s ECDH test error 2, ret = %u", curve_name, ret);
                return 1;
            }
        }
    }

    return 0;
}

/**
 * @brief           Perform ECDH speed test
 * @param[in]       curve                - Pointer to the elliptic curve parameters
 * @param[in]       curve_name           - Name of the elliptic curve
 * @return          0 if test passed, 1 if test failed
 */
unsigned int ecdh_speed_test(const eccp_curve_t *curve, char *curve_name)
{
    unsigned char local_prikey[ECCP_MAX_BYTE_LEN];
    unsigned char local_pubkey[2 * ECCP_MAX_BYTE_LEN];
    unsigned char peer_prikey[ECCP_MAX_BYTE_LEN];
    unsigned char peer_pubkey[2 * ECCP_MAX_BYTE_LEN];
    unsigned char local_key[ECCP_MAX_BYTE_LEN];
    // unsigned char peer_key[ECCP_MAX_BYTE_LEN];
    unsigned int i, key_len = 16;
    unsigned int round;
    KDF_FUNC kdf = NULL;
    unsigned int ret;

    printf("\r\n\r\n *************************** %s *************************** "
           "\r\n",
           curve_name);

    if (curve->eccp_p_bitLen <= 160)
    {
        round = 600;
    }
    else if (curve->eccp_p_bitLen <= 192)
    {
        round = 500;
    }
    else if (curve->eccp_p_bitLen <= 224)
    {
        round = 400;
    }
    else if (curve->eccp_p_bitLen <= 256)
    {
        round = 300;
    }
    else if (curve->eccp_p_bitLen <= 384)
    {
        round = 80;
    }
    else if (curve->eccp_p_bitLen <= 521)
    {
        round = 40;
    }
    else if (curve->eccp_p_bitLen <= 638)
    {
        round = 20;
    }
    else if (curve->eccp_p_bitLen <= 1024)
    {
        round = 10;
    }

    // a small loop for sleeping
    for (i = 0; i <= round / 20; i++)
    {
        ret = eccp_getkey(curve, local_prikey, local_pubkey);
        if (PKE_SUCCESS != ret)
        {
            printf("\r\n ECDH %u A get key error, ret=%u", curve->eccp_p_bitLen, ret);
            return 1;
        }

        ret = eccp_getkey(curve, peer_prikey, peer_pubkey);
        if (PKE_SUCCESS != ret)
        {
            printf("\r\n ECDH %u B get key error, ret=%u", curve->eccp_p_bitLen, ret);
            return 1;
        }
    }

    // test ecdh_compute_key
    ret = 0;
    printf("\r\n ======== ecdh_compute_key begin ======== ");
    fflush(stdout);

#ifdef ECDH_SPEED_TEST_BY_TIMER
    round /= 10;
    startp();
#endif

    for (i = 0; i < round; i++)
    {
        ret |= ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key, key_len, kdf);
    }

#ifdef ECDH_SPEED_TEST_BY_TIMER
    endp(1, 0, round);
#else
    printf("\r\n finished");
    fflush(stdout);
#endif

    if (ECDH_SUCCESS != ret)
    {
        printf("\r\n ecdh_compute_key test error 1, ret = %u", ret);
        return 1;
    }

    return 0;
}

/**
 * @brief           Run all ECDH tests
 * @return          0 if all tests passed, 1 if any test failed
 */
unsigned int ecdh_all_test(void)
{
    printf("\r\n\r\n =================== ECDH test =================== ");

#if 0

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160K1))
    if(ecdh_speed_test(secp160k1, "secp160k1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192K1))
    if(ecdh_speed_test(secp192k1, "secp192k1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224K1))
    if(ecdh_speed_test(secp224k1, "secp224k1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256K1))
    if(ecdh_speed_test(secp256k1, "secp256k1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R1))
    if(ecdh_speed_test(secp160r1, "secp160r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R2))
    if(ecdh_speed_test(secp160r2, "secp160r2"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192R1))
    if(ecdh_speed_test(secp192r1, "secp192r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224R1))
    if(ecdh_speed_test(secp224r1, "secp224r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256R1))
    if(ecdh_speed_test(secp256r1, "secp256r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_SECP384R1))
    if(ecdh_speed_test(secp384r1, "secp384r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 521) && defined(SUPPORT_SECP521R1))
    if(ecdh_speed_test(secp521r1, "secp521r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_BRAINPOOLP160R1))
    if(ecdh_speed_test(brainpoolp160r1, "brainpoolp160r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_BRAINPOOLP192R1))
    if(ecdh_speed_test(brainpoolp192r1, "brainpoolp192r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_BRAINPOOLP224R1))
    if(ecdh_speed_test(brainpoolp224r1, "brainpoolp224r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BRAINPOOLP256R1))
    if(ecdh_speed_test(brainpoolp256r1, "brainpoolp256r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 320) && defined(SUPPORT_BRAINPOOLP320R1))
    if(ecdh_speed_test(brainpoolp320r1, "brainpoolp320r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_BRAINPOOLP384R1))
    if(ecdh_speed_test(brainpoolp384r1, "brainpoolp384r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 512) && defined(SUPPORT_BRAINPOOLP512R1))
    if(ecdh_speed_test(brainpoolp512r1, "brainpoolp512r1"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BN256))
    if(ecdh_speed_test(bn256, "bn256"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 638) && defined(SUPPORT_BN638))
    if(ecdh_speed_test(bn638, "bn638"))
        return 1;
#endif

#if ((ECCP_MAX_BIT_LEN >= 1024) && defined(SUPPORT_ANDERS_1024_1))
    if(ecdh_speed_test(anders_1024_1, "anders_1024_1"))
        return 1;
#endif

#else

/********** standard data test **********/
#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160K1))
    if (ecdh_std_test_secp160k1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192K1))
    if (ecdh_std_test_secp192k1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224K1))
    if (ecdh_std_test_secp224k1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256K1))
    if (ecdh_std_test_secp256k1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R1))
    if (ecdh_std_test_secp160r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R2))
    if (ecdh_std_test_secp160r2())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192R1))
    if (ecdh_std_test_secp192r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224R1))
    if (ecdh_std_test_secp224r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256R1))
    if (ecdh_std_test_secp256r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_SECP384R1))
    if (ecdh_std_test_secp384r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 521) && defined(SUPPORT_SECP521R1))
    if (ecdh_std_test_secp521r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_BRAINPOOLP160R1))
    if (ecdh_std_test_brainpoolp160r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BRAINPOOLP256R1))
    if (ecdh_std_test_brainpoolp256r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 512) && defined(SUPPORT_BRAINPOOLP512R1))
    if (ecdh_std_test_brainpoolp512r1())
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 1024) && defined(SUPPORT_ANDERS_1024_1))
    if (ecdh_std_test_anders_1024_1())
    {
        return 1;
    }
#endif

/********** rand data test **********/
#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160K1))
    if (ecdh_rand_test(secp160k1, "secp160k1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192K1))
    if (ecdh_rand_test(secp192k1, "secp192k1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224K1))
    if (ecdh_rand_test(secp224k1, "secp224k1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256K1))
    if (ecdh_rand_test(secp256k1, "secp256k1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R1))
    if (ecdh_rand_test(secp160r1, "secp160r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_SECP160R2))
    if (ecdh_rand_test(secp160r2, "secp160r2"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_SECP192R1))
    if (ecdh_rand_test(secp192r1, "secp192r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_SECP224R1))
    if (ecdh_rand_test(secp224r1, "secp224r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_SECP256R1))
    if (ecdh_rand_test(secp256r1, "secp256r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_SECP384R1))
    if (ecdh_rand_test(secp384r1, "secp384r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 521) && defined(SUPPORT_SECP521R1))
    if (ecdh_rand_test(secp521r1, "secp521r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 160) && defined(SUPPORT_BRAINPOOLP160R1))
    if (ecdh_rand_test(brainpoolp160r1, "brainpoolp160r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 192) && defined(SUPPORT_BRAINPOOLP192R1))
    if (ecdh_rand_test(brainpoolp192r1, "brainpoolp192r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 224) && defined(SUPPORT_BRAINPOOLP224R1))
    if (ecdh_rand_test(brainpoolp224r1, "brainpoolp224r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BRAINPOOLP256R1))
    if (ecdh_rand_test(brainpoolp256r1, "brainpoolp256r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 320) && defined(SUPPORT_BRAINPOOLP320R1))
    if (ecdh_rand_test(brainpoolp320r1, "brainpoolp320r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 384) && defined(SUPPORT_BRAINPOOLP384R1))
    if (ecdh_rand_test(brainpoolp384r1, "brainpoolp384r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 512) && defined(SUPPORT_BRAINPOOLP512R1))
    if (ecdh_rand_test(brainpoolp512r1, "brainpoolp512r1"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 256) && defined(SUPPORT_BN256))
    if (ecdh_rand_test(bn256, "bn256"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 638) && defined(SUPPORT_BN638))
    if (ecdh_rand_test(bn638, "bn638"))
    {
        return 1;
    }
#endif

#if ((ECCP_MAX_BIT_LEN >= 1024) && defined(SUPPORT_ANDERS_1024_1))
    if (ecdh_rand_test(anders_1024_1, "anders_1024_1"))
    {
        return 1;
    }
#endif

#endif

    return 0;
}

#endif
