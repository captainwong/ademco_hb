#ifndef __HB_RF_H__
#define __HB_RF_H__

#include <libhbiot/hb_iot_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(push, 1)
#endif

#define HB_INVALID_RF_ADDR 0xFFFF
#define hb_is_valid_rf_addr(addr) ((0 < (addr)) && ((addr) < 0xFFFF))

#define HB_RF_ADDR_INVALID \
    {.hi = 0xFF, .mi = 0xFF, .lo = 0xFF}

typedef struct hb_rf_addr_t {
    uint8_t hi, mi, lo;  // 3字节地址
} hb_rf_addr_t;

typedef struct hb_rf_packet_t {
    uint8_t mi;   // 无线地址中间字节
    uint8_t lo;   // 无线地址低字节
    uint8_t cmd;  // 无线命令码
    uint8_t hi;   // 无线地址高字节
    uint8_t sum;  // 校验和
} hb_rf_packet_t;

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(pop)
#endif

// 无线命令码
// Note: 工程主机删除所有探头会报 E0，其他都是探头无线码
#define HB_RF_CODE_MAP(XX)                            \
    XX(0x00, BURGLAR, "报警")                         \
    XX(0x01, ARM, "布防")                             \
    XX(0x02, DISARM, "撤防")                          \
    XX(0x04, ARM_STAY_OR_CLEAR_ALARM, "半布防或消警") \
    XX(0x08, EMERGENCY, "紧急报警")                   \
    XX(0x20, FIRE, "火警")                            \
    XX(0x30, DURESS, "呼救/挟持")                     \
    XX(0x40, GAS, "煤气")                             \
    XX(0x50, WATER, "水警")                           \
    XX(0x57, NIGHT, "夜间")                           \
    XX(0x66, HEART_BEAT, "心跳")                      \
    XX(0x69, SENDER_REBOOT, "发送端重启")             \
    XX(0x75, DAY, "白天")                             \
    XX(0xA5, R_BATTERY_BROKEN, "收电池损坏")          \
    XX(0xA8, DOOR_RING, "门铃")                       \
    XX(0xAB, S_BATTERY_LOW, "发低电")                 \
    XX(0xAC, R_BATTERY_LOW, "收低电")                 \
    XX(0xAD, BATTERY_RECOVER, "复电")                 \
    XX(0xAE, S_BATTERY_BROKEN, "发电池损坏")          \
    XX(0xB0, AC_BROKEN, "AC掉电")                     \
    XX(0xB1, AC_RECOVER, "AC恢复")                    \
    XX(0xBA, TAMPER, "防拆")                          \
    XX(0xBB, SLEEP, "电池电压小于3V，终端进入休眠")   \
    XX(0xBC, LONGTIME_DISCONN, "长时间没有对上")      \
    XX(0xBD, SOLAR_DISTURB, "强光干扰")               \
    XX(0xBE, SM_POWER_EXCEPT, "分机产生电源异常")     \
    XX(0xBF, SM_POWER_RECOVER, "分机恢复电源异常")    \
    XX(0xCA, STARTUP, "开机")                         \
    XX(0xCB, LONGTIME_RECOVER, "长时间已经对上")      \
    XX(0xCD, SOLAR_RECOVER, "强光恢复")               \
    XX(0xCE, SM_EXCEPTION, "分机产生异常信息")        \
    XX(0xCF, SM_EXCEPT_RECOVER, "分机恢复异常信息")   \
    XX(0xDD, QUERY, "查询分机状态")                   \
    XX(0xE0, DELETED_ALL_SENSOR, "已删除所有传感器")  \
    XX(0xED, SHUTDOWN, "关机")

// 无线命令码
typedef enum hb_rf_code_t {
#define XX(value, name, str) HB_RF_CODE_##name = value,
    HB_RF_CODE_MAP(XX)
#undef XX

    // 哨兵
    HB_RF_CODE_MAX,
    // 无效命令码
    HB_RF_CODE_INVALID = 0xFF,
} hb_rf_code_t;

///////////////////////////// rf functions /////////////////////////////////

hb_rf_code_t hb_rf_code_from_u8(uint8_t rf_code);
const char* hb_rf_code_to_string(hb_rf_code_t rf_code);
const char* hb_rf_code_to_string_chinese(hb_rf_code_t rf_code);

// reset to all 0xFF address
void hb_rf_addr_reset(hb_rf_addr_t* addr);
bool hb_rf_addr_equal(const hb_rf_addr_t* addr1, const hb_rf_addr_t* addr2);
void hb_rf_addr_from_packet(hb_rf_addr_t* addr, const hb_rf_packet_t* pkt);
uint32_t hb_rf_addr_to_u32(const hb_rf_addr_t* addr);
void hb_rf_addr_from_u32(hb_rf_addr_t* addr, uint32_t u32_addr);

bool hb_rf_packet_eq(const hb_rf_packet_t* a, const hb_rf_packet_t* b);

#ifdef __cplusplus
}
#endif

#endif /* __HB_RF_H__ */
