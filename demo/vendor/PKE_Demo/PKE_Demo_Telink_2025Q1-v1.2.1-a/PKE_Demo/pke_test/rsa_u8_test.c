/*! @file rsa_u8_test.c */
#include <stdio.h>

#include "lib/include/crypto_common/utility.h"
#include "lib/include/pke/rsa_u8.h"

#ifdef SUPPORT_RSA

#if 1

/**
 * @brief 
 * 
 */
 * @return          0 if all tests pass, 1 if any test fails
 */
unsigned int rsa_modexp_1024_test_u8(void)
{
    // n:
    // 0xBEC7A9400CF439F0EADB452BA69A2B1C9D950126282C21EEB5704D4104D816F2A6A476460E2A4D2E66DAC542571117787119E5DBEF660075C453225E16D22D06
    //      C74A11EFB9EE5D48E430C55E26AF59F0E106BDD8591F158B69230B9855565D787DF45F14EFA2C3F61D7B833A9D0B699791BEB0D884E8E1651538B245FEA18675
    // d:
    // 0xBC5D2053D8366EDADAD7E244C5FAE3E41E06979AD8013303A06B202BB91FAE10A8CE2AE4E2698BC8EBCEC702C83A5CE5132E91D0045FBEC6882FD8F7F32A533E
    //      F0667BA836013850A5C64770A830AEE44A2A74BE09655DB71DEA94F02B15D590495F06294AC3AC9DA7B0D6E692F2B6D69C0BBB3628A3F778765A9FBE39CD7DE1
    unsigned char n[] = {
        0xBE,
        0xC7,
        0xA9,
        0x40,
        0x0C,
        0xF4,
        0x39,
        0xF0,
        0xEA,
        0xDB,
        0x45,
        0x2B,
        0xA6,
        0x9A,
        0x2B,
        0x1C,
        0x9D,
        0x95,
        0x01,
        0x26,
        0x28,
        0x2C,
        0x21,
        0xEE,
        0xB5,
        0x70,
        0x4D,
        0x41,
        0x04,
        0xD8,
        0x16,
        0xF2,
        0xA6,
        0xA4,
        0x76,
        0x46,
        0x0E,
        0x2A,
        0x4D,
        0x2E,
        0x66,
        0xDA,
        0xC5,
        0x42,
        0x57,
        0x11,
        0x17,
        0x78,
        0x71,
        0x19,
        0xE5,
        0xDB,
        0xEF,
        0x66,
        0x00,
        0x75,
        0xC4,
        0x53,
        0x22,
        0x5E,
        0x16,
        0xD2,
        0x2D,
        0x06,
        0xC7,
        0x4A,
        0x11,
        0xEF,
        0xB9,
        0xEE,
        0x5D,
        0x48,
        0xE4,
        0x30,
        0xC5,
        0x5E,
        0x26,
        0xAF,
        0x59,
        0xF0,
        0xE1,
        0x06,
        0xBD,
        0xD8,
        0x59,
        0x1F,
        0x15,
        0x8B,
        0x69,
        0x23,
        0x0B,
        0x98,
        0x55,
        0x56,
        0x5D,
        0x78,
        0x7D,
        0xF4,
        0x5F,
        0x14,
        0xEF,
        0xA2,
        0xC3,
        0xF6,
        0x1D,
        0x7B,
        0x83,
        0x3A,
        0x9D,
        0x0B,
        0x69,
        0x97,
        0x91,
        0xBE,
        0xB0,
        0xD8,
        0x84,
        0xE8,
        0xE1,
        0x65,
        0x15,
        0x38,
        0xB2,
        0x45,
        0xFE,
        0xA1,
        0x86,
        0x75,
    };
    unsigned char e[] = {0x01, 0x00, 0x01};
    unsigned char d[] = {
        0xBC,
        0x5D,
        0x20,
        0x53,
        0xD8,
        0x36,
        0x6E,
        0xDA,
        0xDA,
        0xD7,
        0xE2,
        0x44,
        0xC5,
        0xFA,
        0xE3,
        0xE4,
        0x1E,
        0x06,
        0x97,
        0x9A,
        0xD8,
        0x01,
        0x33,
        0x03,
        0xA0,
        0x6B,
        0x20,
        0x2B,
        0xB9,
        0x1F,
        0xAE,
        0x10,
        0xA8,
        0xCE,
        0x2A,
        0xE4,
        0xE2,
        0x69,
        0x8B,
        0xC8,
        0xEB,
        0xCE,
        0xC7,
        0x02,
        0xC8,
        0x3A,
        0x5C,
        0xE5,
        0x13,
        0x2E,
        0x91,
        0xD0,
        0x04,
        0x5F,
        0xBE,
        0xC6,
        0x88,
        0x2F,
        0xD8,
        0xF7,
        0xF3,
        0x2A,
        0x53,
        0x3E,
        0xF0,
        0x66,
        0x7B,
        0xA8,
        0x36,
        0x01,
        0x38,
        0x50,
        0xA5,
        0xC6,
        0x47,
        0x70,
        0xA8,
        0x30,
        0xAE,
        0xE4,
        0x4A,
        0x2A,
        0x74,
        0xBE,
        0x09,
        0x65,
        0x5D,
        0xB7,
        0x1D,
        0xEA,
        0x94,
        0xF0,
        0x2B,
        0x15,
        0xD5,
        0x90,
        0x49,
        0x5F,
        0x06,
        0x29,
        0x4A,
        0xC3,
        0xAC,
        0x9D,
        0xA7,
        0xB0,
        0xD6,
        0xE6,
        0x92,
        0xF2,
        0xB6,
        0xD6,
        0x9C,
        0x0B,
        0xBB,
        0x36,
        0x28,
        0xA3,
        0xF7,
        0x78,
        0x76,
        0x5A,
        0x9F,
        0xBE,
        0x39,
        0xCD,
        0x7D,
        0xE1,
    };
    unsigned char p[] = {
        0xE5,
        0x32,
        0x4D,
        0x00,
        0x2D,
        0x08,
        0xAE,
        0x4A,
        0x3B,
        0x21,
        0x16,
        0x7E,
        0xEC,
        0x55,
        0x68,
        0x13,
        0x6B,
        0x1B,
        0xA8,
        0x53,
        0x0F,
        0x45,
        0xB0,
        0x41,
        0xBD,
        0x3B,
        0x7A,
        0x16,
        0x84,
        0x21,
        0xD2,
        0x0C,
        0x18,
        0x4B,
        0x66,
        0x5F,
        0x0E,
        0x06,
        0x90,
        0x34,
        0x07,
        0x1A,
        0x59,
        0x4D,
        0x30,
        0x24,
        0x75,
        0x39,
        0xBC,
        0x27,
        0x4F,
        0x20,
        0x4C,
        0x43,
        0x66,
        0x6F,
        0x56,
        0x3A,
        0x7F,
        0x00,
        0x0C,
        0x3B,
        0x82,
        0xFD,
    };
    unsigned char q[] = {
        0xD5,
        0x17,
        0x3E,
        0x2C,
        0x75,
        0x79,
        0x67,
        0x07,
        0xC0,
        0x69,
        0x5F,
        0x17,
        0xF2,
        0x64,
        0x95,
        0x3B,
        0x84,
        0x71,
        0x5F,
        0x7A,
        0x4A,
        0x17,
        0xC8,
        0x33,
        0x7F,
        0x77,
        0xF9,
        0x47,
        0x1B,
        0x0E,
        0xB2,
        0x35,
        0xE1,
        0x2E,
        0xA9,
        0x19,
        0xC4,
        0x23,
        0xCD,
        0x7D,
        0x96,
        0x3F,
        0xBF,
        0x1F,
        0x25,
        0x0F,
        0x04,
        0x50,
        0x26,
        0x64,
        0xB3,
        0x57,
        0xA6,
        0x10,
        0xAF,
        0x00,
        0xD9,
        0x1E,
        0xB8,
        0x7C,
        0x7A,
        0x70,
        0xD6,
        0xD9,
    };
    unsigned char dp[] = {
        0x14,
        0xB8,
        0x90,
        0x77,
        0x94,
        0x9A,
        0xB5,
        0x4F,
        0x26,
        0xBB,
        0x5E,
        0x12,
        0x49,
        0x81,
        0x8B,
        0x30,
        0x3E,
        0x3D,
        0xB8,
        0xE1,
        0x11,
        0x7F,
        0x66,
        0x68,
        0xFB,
        0x17,
        0xB6,
        0x78,
        0xF4,
        0xA7,
        0x2E,
        0x1B,
        0x43,
        0xD3,
        0x05,
        0x45,
        0x50,
        0xBA,
        0x4D,
        0x2D,
        0x36,
        0xFB,
        0x2D,
        0xD9,
        0xA9,
        0xA1,
        0x92,
        0x2B,
        0x8F,
        0x14,
        0x2A,
        0xB6,
        0x2B,
        0xCE,
        0xE8,
        0x22,
        0xE6,
        0xB3,
        0xD1,
        0x29,
        0x8A,
        0xF1,
        0x92,
        0x6D,
    };
    unsigned char dq[] = {
        0x4A,
        0x16,
        0x74,
        0xB5,
        0x7C,
        0x7A,
        0xA3,
        0x49,
        0xDC,
        0xAA,
        0x46,
        0x37,
        0xCB,
        0x3D,
        0x11,
        0x6D,
        0xCA,
        0x14,
        0xA8,
        0xFF,
        0x99,
        0xB7,
        0x3F,
        0x12,
        0x38,
        0xA5,
        0x7B,
        0xF4,
        0x2F,
        0x02,
        0x23,
        0x0F,
        0xFC,
        0xA4,
        0xFE,
        0x82,
        0x48,
        0xE1,
        0xB1,
        0xD6,
        0x92,
        0x60,
        0xC3,
        0x90,
        0x8E,
        0x8F,
        0xF4,
        0x0D,
        0xF1,
        0xEB,
        0x5D,
        0x37,
        0x82,
        0x33,
        0x96,
        0xC5,
        0x12,
        0xB8,
        0x4D,
        0xA8,
        0x69,
        0xE1,
        0xC5,
        0xB1,
    };
    unsigned char u[] = {
        0x3F,
        0xB0,
        0xD1,
        0x77,
        0xBF,
        0xE4,
        0x4F,
        0x44,
        0x16,
        0x02,
        0xF8,
        0xE0,
        0x53,
        0xC4,
        0x72,
        0xE3,
        0xED,
        0xA1,
        0x55,
        0xDE,
        0xB3,
        0x43,
        0x30,
        0x0B,
        0x89,
        0x3B,
        0x7E,
        0x4D,
        0x22,
        0x34,
        0xD1,
        0xAA,
        0x48,
        0x89,
        0x9F,
        0x5B,
        0x73,
        0x85,
        0xC4,
        0x88,
        0xCD,
        0x27,
        0x51,
        0x3C,
        0xE3,
        0xB1,
        0x27,
        0x0B,
        0xCA,
        0xFA,
        0xAD,
        0x33,
        0x8B,
        0x15,
        0x25,
        0x4C,
        0xF3,
        0x7F,
        0xB2,
        0x6D,
        0xA2,
        0x96,
        0x4E,
        0x5B,
    };
    unsigned char std_out[] = {
        0x53,
        0x51,
        0xA8,
        0x2E,
        0x45,
        0xA4,
        0xB2,
        0x20,
        0x8C,
        0xD0,
        0x9D,
        0x47,
        0xC1,
        0x34,
        0x68,
        0xB2,
        0x9F,
        0x6D,
        0x92,
        0x85,
        0x95,
        0xF5,
        0x52,
        0x1A,
        0x35,
        0x70,
        0x44,
        0xF9,
        0x9C,
        0x96,
        0x0A,
        0x3B,
        0xDA,
        0xD4,
        0xC5,
        0x04,
        0x31,
        0x06,
        0x36,
        0x9F,
        0x9A,
        0x24,
        0x59,
        0x63,
        0xBC,
        0xF5,
        0x15,
        0x63,
        0x54,
        0x80,
        0xDD,
        0xC6,
        0xB3,
        0xC3,
        0xB2,
        0x59,
        0xDB,
        0x25,
        0x6A,
        0xAD,
        0xC4,
        0x24,
        0xF8,
        0x41,
        0x60,
        0xDA,
        0x6F,
        0x4F,
        0xDA,
        0x22,
        0x1E,
        0x31,
        0x0F,
        0x7B,
        0xD1,
        0xAC,
        0xFA,
        0x55,
        0x1E,
        0xC2,
        0x3A,
        0x0F,
        0xE3,
        0xFD,
        0xA1,
        0x93,
        0x37,
        0x33,
        0x9D,
        0x56,
        0x18,
        0x8D,
        0x20,
        0xF7,
        0x1F,
        0x91,
        0xE0,
        0x24,
        0x69,
        0xF7,
        0x10,
        0xBC,
        0x7D,
        0x78,
        0x08,
        0x2D,
        0xA9,
        0x94,
        0x90,
        0x6F,
        0xD6,
        0x49,
        0xBA,
        0xDE,
        0x3C,
        0x88,
        0x53,
        0x7C,
        0x93,
        0xF8,
        0x5A,
        0x78,
        0xA5,
        0xAE,
        0x69,
        0x18,
        0xBE,
        0xE1,
    };

    unsigned char in[128], out[128], out2[128];
    unsigned int eBitLen = 17;
    unsigned int nBitLen = 1024;
    unsigned int n_len = get_byte_len(nBitLen);
    unsigned int ret;

    // set input
    memset_(in, 0x5a, n_len);

    // sign(non-CRT mode)
    ret = RSA_ModExp_U8(in, d, n, out, nBitLen, nBitLen);
    if (RSA_SUCCESS != ret || memcmp_(out, std_out, n_len))
    {
        printf("\r\n RSA_ModExp_U8 1024 test failure, ret=%x", ret);
        fflush(stdout);
        print_buf_u8(in, n_len, "in");
        print_buf_u8(out, n_len, "out");
        return 1;
    }
    else
    {
        printf("\r\n RSA_ModExp_U8 1024 test success");
        fflush(stdout);
    }

    // sign(CRT mode)
    ret = RSA_CRTModExp_U8(in, p, q, dp, dq, u, out, nBitLen);
    if (RSA_SUCCESS != ret || memcmp_(out, std_out, n_len))
    {
        printf("\r\n RSA_CRTModExp_U8 1024 test failure, ret=%x", ret);
        fflush(stdout);
        print_buf_u8(p, n_len / 2, "p");
        print_buf_u8(q, n_len / 2, "q");
        print_buf_u8(dp, n_len / 2, "dp");
        print_buf_u8(dq, n_len / 2, "dq");
        print_buf_u8(u, n_len / 2, "u");
        print_buf_u8(in, n_len, "in");
        print_buf_u8(out, n_len, "out");
        return 1;
    }
    else
    {
        printf("\r\n RSA_CRTModExp_U8 1024 test success");
        fflush(stdout);
    }

    // verify
    ret = RSA_ModExp_U8(out, e, n, out2, eBitLen, nBitLen);
    if (RSA_SUCCESS != ret || memcmp_(out2, in, n_len))
    {
        printf("\r\n RSA_ModExp_U8 1024 test failure, ret=%x", ret);
        fflush(stdout);
        print_buf_u8(out, n_len, "out");
        print_buf_u8(out2, n_len, "out2");
        return 1;
    }
    else
    {
        printf("\r\n RSA_ModExp_U8 1024 test success");
        fflush(stdout);
    }

    return 0;
}

/**
 * @brief           Function to test RSA key generation and operations using unsigned char arrays
 * @param[in]       eBitLen              - Bit length of the public exponent
 * @param[in]       nBitLen              - Bit length of the modulus
 * @return          0 if all tests pass, 1 if any test fails
 */
unsigned int rsa_u8_get_key_test(unsigned int eBitLen, unsigned int nBitLen)
{
    unsigned char e[512];
    unsigned char d[512];
    unsigned char n[512];
    unsigned char in[512];
    unsigned char out[512];
    unsigned char out2[512];
    unsigned int e_len;
    unsigned int n_len;
    unsigned int ret;

    // RSA NON-CRT
    e_len = get_byte_len(eBitLen);
    n_len = get_byte_len(nBitLen);

    ret = RSA_GetKey_U8(e, d, n, eBitLen, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_GetKey_U8 failure, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    // test encrypt & decrypt
    memset_(in, 0x1, n_len);
    ret = RSA_ModExp_U8(in, d, n, out, nBitLen, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_ModExp_U8 error 1, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    ret = RSA_ModExp_U8(out, e, n, out2, eBitLen, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_ModExp_U8 error 2, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    if (memcmp_(out2, in, n_len))
    {
        printf("\r\n RSA encrypt & decrypt error");
        fflush(stdout);
        print_buf_u8(in, n_len, "in");
        print_buf_u8(out, n_len, "out");
        print_buf_u8(out2, n_len, "out2");
        print_buf_u8(e, e_len, "e");
        print_buf_u8(d, n_len, "d");
        print_buf_u8(n, n_len, "n");
        return 1;
    }
    else
    {
        printf("\r\n RSA unsigned char(eBitLen=%u, nBitLen=%u) get key & encrypt & "
               "decrypt success",
               eBitLen, nBitLen);
        fflush(stdout);
    }

    return 0;
}

/**
 * @brief           Function to test RSA CRT key generation and operations using unsigned char arrays
 * @param[in]       eBitLen              - Bit length of the public exponent
 * @param[in]       nBitLen              - Bit length of the modulus
 * @return          0 if all tests pass, 1 if any test fails
 */
unsigned int rsa_u8_crt_get_key_test(unsigned int eBitLen, unsigned int nBitLen)
{
    unsigned char e[512];
    unsigned char n[512];
    unsigned char p[256];
    unsigned char q[256];
    unsigned char dp[256];
    unsigned char dq[256];
    unsigned char u[256];
    unsigned char in[512];
    unsigned char out[512];
    unsigned char out2[512];
    unsigned char e_len;
    unsigned char n_len;
    unsigned char p_len;
    unsigned int ret;

    // RSA CRT
    e_len = get_byte_len(eBitLen);
    n_len = get_byte_len(nBitLen);
    p_len = get_byte_len(nBitLen / 2);

    ret = RSA_GetCRTKey_U8(e, p, q, dp, dq, u, n, eBitLen, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_GetCRTKey_U8 failure, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    // test encrypt & decrypt
    memset_(in, 0x1, n_len);
    ret = RSA_ModExp_U8(in, e, n, out, eBitLen, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_ModExp_U8 error 1, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    ret = RSA_CRTModExp_U8(out, p, q, dp, dq, u, out2, nBitLen);
    if (RSA_SUCCESS != ret)
    {
        printf("\r\n RSA_CRTModExp_U8 error 2, ret = %x", ret);
        fflush(stdout);
        return 1;
    }

    if (memcmp_(out2, in, n_len))
    {
        printf("\r\n RSA CRT encrypt & decrypt error");
        fflush(stdout);
        print_buf_u8(in, n_len, "in");
        print_buf_u8(out, n_len, "out");
        print_buf_u8(out2, n_len, "out2");
        print_buf_u8(e, e_len, "e");
        print_buf_u8(p, p_len, "p");
        print_buf_u8(q, p_len, "q");
        print_buf_u8(dp, p_len, "dp");
        print_buf_u8(dq, p_len, "dq");
        print_buf_u8(u, p_len, "u");
        print_buf_u8(n, n_len, "n");
        return 1;
    }
    else
    {
        printf("\r\n RSA CRT unsigned char(eBitLen=%u, nBitLen=%u) get key & "
               "encrypt & decrypt success",
               eBitLen, nBitLen);
        fflush(stdout);
    }

    return 0;
}

/**
 * @brief           Comprehensive function to run all RSA unsigned char array tests
 * @return          0 if all tests pass, 1 if any test fails
 */
unsigned int RSA_U8_simple_test(void)
{
    printf("\r\n\r\n\r\n =================== RSA unsigned char test "
           "=================== ");
    fflush(stdout);

#if 1
    if (rsa_modexp_1024_test_u8())
    {
        return 1;
    }
#endif

#if 1
    if (rsa_u8_get_key_test(17, 2048))
    {
        return 1;
    }
#endif

#if 1
    if (rsa_u8_crt_get_key_test(17, 2048))
    {
        return 1;
    }
#endif

    printf("\r\n\r\n RSA unsigned char test over");
    fflush(stdout);

    return 0;
}

#endif

#endif
