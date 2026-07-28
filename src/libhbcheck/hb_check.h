#ifndef __HB_CHECK_H__
#define __HB_CHECK_H__

#include <libhbcheck/hbcheck_config.h>
#include <libhbcheck/hbcheck_version.h>

#ifdef __cplusplus
extern "C" {
#endif

HBCHECK_EXPORT_SYMBOL uint32_t libhbcheck_version(void);
HBCHECK_EXPORT_SYMBOL const char* libhbcheck_version_string(void);

// 累加校验，计算data[0] ~ dat[len-1]
HBCHECK_EXPORT_SYMBOL uint8_t hb_calc_sum(const uint8_t* dat, size_t len);

// 累加校验，计算data[0] ~ dat[len-2]，校验和放在data[len-1]
HBCHECK_EXPORT_SYMBOL void hb_sum(uint8_t* dat, size_t len);

// 校验和是否正确, return 0 for incorrect, otherwise correct
HBCHECK_EXPORT_SYMBOL bool hb_chksum(const uint8_t* dat, size_t len);

/**
 * @brief 计算CRC-8校验和
 * @param poly CRC多项式
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-8校验和
 */
HBCHECK_EXPORT_SYMBOL uint8_t hb_crc8_generic(uint8_t poly, uint8_t init,
                                              bool ref_in, bool ref_out, uint8_t xor_out,
                                              const uint8_t* dat, size_t len);

/**
 * @brief 生成CRC-8查找表
 * @param poly CRC多项式
 * @param ref_in 输入数据是否需要反转
 * @param table 输出的查找表缓冲区，大小应至少为256字节
 */
HBCHECK_EXPORT_SYMBOL void hb_crc8_generic_gen_table(uint8_t poly, bool ref_in, uint8_t* table);

/**
 * @brief 使用CRC-8查找表计算CRC-8校验和
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param table CRC-8查找表，大小应至少为256字节
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-8校验和
 */
HBCHECK_EXPORT_SYMBOL uint8_t hb_crc8_generic_table(uint8_t init, bool ref_in, bool ref_out,
                                                    uint8_t xor_out, const uint8_t* table,
                                                    const uint8_t* dat, size_t len);

/**
 * @brief 计算CRC-16校验和
 * @param poly CRC多项式
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-16校验和
 */
HBCHECK_EXPORT_SYMBOL uint16_t hb_crc16_generic(uint16_t poly, uint16_t init,
                                                bool ref_in, bool ref_out, uint16_t xor_out,
                                                const uint8_t* dat, size_t len);

/**
 * @brief 生成CRC-16查找表
 * @param poly CRC多项式
 * @param ref_in 输入数据是否需要反转
 * @param table 输出的查找表缓冲区，大小应至少为256字节
 */
HBCHECK_EXPORT_SYMBOL void hb_crc16_generic_gen_table(uint16_t poly, bool ref_in, uint16_t* table);

/**
 * @brief 使用CRC-16查找表计算CRC-16校验和
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param table CRC-16查找表，大小应至少为256字节
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-16校验和
 */
HBCHECK_EXPORT_SYMBOL uint16_t hb_crc16_generic_table(uint16_t init, bool ref_in, bool ref_out,
                                                      uint16_t xor_out, const uint16_t* table,
                                                      const uint8_t* dat, size_t len);

/**
 * @brief 计算CRC-32校验和
 * @param poly CRC多项式
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-32校验和
 */
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_generic(uint32_t poly, uint32_t init,
                                                bool ref_in, bool ref_out, uint32_t xor_out,
                                                const uint8_t* dat, size_t len);

/**
 * @brief 生成CRC-32查找表
 * @param poly CRC多项式
 * @param ref_in 输入数据是否需要反转
 * @param table 输出的查找表缓冲区，大小应至少为256字节
 */
HBCHECK_EXPORT_SYMBOL void hb_crc32_generic_gen_table(uint32_t poly, bool ref_in, uint32_t* table);

/**
 * @brief 使用CRC-32查找表计算CRC-32校验和
 * @param init 初始值
 * @param ref_in 输入数据是否需要反转
 * @param ref_out 输出数据是否需要反转
 * @param xor_out 最终输出的异或值
 * @param table CRC-32查找表，大小应至少为256字节
 * @param dat 输入数据缓冲区
 * @param len 输入数据长度
 * @return 计算得到的CRC-32校验和
 */
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_generic_table(uint32_t init, bool ref_in, bool ref_out,
                                                      uint32_t xor_out, const uint32_t* table,
                                                      const uint8_t* dat, size_t len);

/*
 * Name   : "CRC-8", CRC-8/SMBUS
 * Width  : 8
 * Poly   : 07
 * Init   : 00
 * RefIn  : False
 * RefOut : False
 * XorOut : 0000
 * Check  : F4 (for "123456789")
 */
HBCHECK_EXPORT_SYMBOL uint8_t hb_crc8(const uint8_t* dat, size_t len);
HBCHECK_EXPORT_SYMBOL uint8_t hb_crc8_byte(uint8_t crc, const uint8_t dat);

/*
 * Name   : "CRC-16", CRC-16/ARC, CRC-16/LHA, CRC-IBM
 * Width  : 16
 * Poly   : 8005
 * Init   : 0000
 * RefIn  : True
 * RefOut : True
 * XorOut : 0000
 * Check  : BB3D (for "123456789")
 */
HBCHECK_EXPORT_SYMBOL uint16_t hb_crc16(const uint8_t* dat, size_t len);
HBCHECK_EXPORT_SYMBOL uint16_t hb_crc16_byte(uint16_t crc, const uint16_t dat);

/* Most common CRC32 implementation, used in many applications
 * Name   : "CRC-32", CRC-32/ISO-HDLC, CRC-32/ADCCP, CRC-32/XZ, etc.
 * Width  :  32
 * Poly   :  0x04C11DB7
 * Init   :  0xFFFFFFFF
 * RefIn  :  True
 * RefOut :  True
 * XorOut :  0xFFFFFFFF
 * Check  :  CBF43926 (for "123456789")
 */
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32(const uint8_t* dat, size_t len);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_byte(uint32_t crc, const uint8_t dat);


//////////////////////////// fast version (with table) ////////////////////////////

HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_fast_init(void);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_fast_update1(uint32_t crc, uint8_t dat);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_fast_update(uint32_t crc, const uint8_t* dat, size_t len);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_fast_finalize(uint32_t crc);

//////////////////////////// slow version (without table) ////////////////////////////

// these functions are useful when ROM size is very limited but RAM size is sufficient
// these functions are not using the lookup table, so they are slower than the table version
// the result is the same as `hb_crc32`

HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_slow_init(void);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_slow_update1(uint32_t crc, uint8_t dat);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_slow_update(uint32_t crc, const uint8_t* dat, size_t len);
HBCHECK_EXPORT_SYMBOL uint32_t hb_crc32_slow_finalize(uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif /* __HB_CHECK_H__ */
