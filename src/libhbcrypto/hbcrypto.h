#ifndef __HBCRYPTO_H__
#define __HBCRYPTO_H__

#include <libhbcrypto/hbcrypto_config.h>
#include <libhbcrypto/hbcrypto_version.h>

#ifdef __cplusplus
extern "C" {
#endif

HBCRYPTO_EXPORT_SYMBOL uint32_t libhbcrypto_version(void);
HBCRYPTO_EXPORT_SYMBOL const char* libhbcrypto_version_string(void);

/**
 * @brief ChaCha20加密函数
 * @param out 输出缓冲区，长度至少为len字节
 * @param input 输入数据缓冲区，长度为len字节
 * @param len 输入数据长度，单位为字节
 * @param key 32字节的ChaCha20密钥
 * @param nonce 12字节的ChaCha20随机数（nonce）
 */
HBCRYPTO_EXPORT_SYMBOL void hb_chacha20(uint8_t *out, const uint8_t *input, size_t len, const uint8_t key[32], const uint8_t nonce[12]);

#ifdef __cplusplus
}
#endif

#endif /* __HBCRYPTO_H__ */
