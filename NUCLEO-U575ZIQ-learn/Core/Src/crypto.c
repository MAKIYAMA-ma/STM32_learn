#include "mbedtls/pk.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/sha256.h"
#include "public_key.h"
#include "crypto.h"
#include <string.h>

int VerifySignature(uint8_t *firmware, size_t firmware_size,
                     uint8_t *signature, size_t sig_len)
{
    int ret;
    mbedtls_pk_context pk;
    uint8_t hash[32]; // SHA-256 hash

    mbedtls_pk_init(&pk);

    // 公開鍵の読み込み
    ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char *)rsa_public_key_pem, strlen(rsa_public_key_pem) + 1);
    if (ret != 0) return ret;

    // ハッシュ計算
    mbedtls_sha256(firmware, firmware_size, hash, 0); // 0 = SHA-256（非ハッシュ224）

    // DEBUG
    uart_printf(DBG_LVL_DBG, "HASH:\n");
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 8; j++) {
            uart_printf(DBG_LVL_DBG, "%02X ", hash[i*8+j]);
        }
        uart_printf(DBG_LVL_DBG, "\n");
    }

    uart_printf(DBG_LVL_DBG, "\nSIG:\n");
    uint8_t *sp = signature;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 8; j++) {
            uart_printf(DBG_LVL_DBG, "%02X ", sp[i*8+j]);
        }
        uart_printf(DBG_LVL_DBG, "\n");
    }

    // 署名の検証
    ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, sizeof(hash), signature, sig_len);

    mbedtls_pk_free(&pk);
    return ret; // 0: OK, その他: エラー
}

void TestVerifySignature(void)
{
    int ret = VerifySignature((uint8_t *)0x08000000, (size_t)(0x081FC000 - 0x08000000), (uint8_t *)0x081FC000, (size_t)256);
    if(ret == 0) {
        uart_printf(DBG_LVL_DBG, "Verify Signature Suceess!\n");
    } else {
        uart_printf(DBG_LVL_DBG, "Verify Signature Fail!\n");
    }
}
