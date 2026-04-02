#ifndef __HB_VERSION_H__
#define __HB_VERSION_H__

#include <libademco/ademco.h>
#include <libhb/hb_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t hb_vertion_t;  // 版本号

#define hb_version_hi(hex) (bcd_decode(((hex) & 0xFF00) >> 8))  // 高字节
#define hb_version_lo(hex) (bcd_decode((hex) & 0x00FF))         // 低字节
#define hb_make_version(hi, lo) (hb_vertion_t)(bcd_encode((uint8_t)(hi)) << 8 | bcd_encode((uint8_t)(lo)))

// 主机是否可以报告电源状态
HB_EXPORT_SYMBOL
bool hb_machine_can_report_power_state(hb_machine_type_t type, hb_vertion_t version);

#ifdef __cplusplus
}
#endif

#endif /* __HB_VERSION_H__ */
