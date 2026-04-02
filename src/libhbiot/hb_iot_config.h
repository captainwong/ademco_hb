#ifndef __HB_IOT_CONFIG_H__
#define __HB_IOT_CONFIG_H__

#include <libhbcheck/libhbcheck.h>
#include <libhbcrypto/libhbcrypto.h>

// 端序
#define HB_ENDIAN_LITTLE 0x00  // 小端序
#define HB_ENDIAN_BIG 0x01     // 大端序
#ifndef HB_ENDIAN              // 默认小端序
#define HB_ENDIAN HB_ENDIAN_LITTLE
#endif /* HB_ENDIAN */
#define hb_is_little_endian() (HB_ENDIAN == HB_ENDIAN_LITTLE)
#define hb_is_big_endian() (HB_ENDIAN == HB_ENDIAN_BIG)

#endif /* __HB_IOT_CONFIG_H__ */
