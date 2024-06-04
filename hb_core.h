#ifndef __HB_CORE_H__
#define __HB_CORE_H__

#include <stdint.h>

#include "hb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// 三防区主机状态GG范围 1~3
#define HB_3SECTION_MACHINE_GG_MIN 1
#define HB_3SECTION_MACHINE_GG_MAX 3

// 是否合法的三防区主机状态GG
#define hb_is_valid_3section_machine_gg(gg) \
    HB_3SECTION_MACHINE_GG_MIN <= (gg) &&   \
        (gg) <= HB_3SECTION_MACHINE_GG_MAX

// 主机状态表
#define HB_MACHINE_STATUS_MAP(XX) \
    XX(ARM_AWAY, 0, "离家布防")   \
    XX(ARM_STAY, 1, "留守布防")   \
    XX(DISARM, 2, "撤防")         \
    XX(SETTING, 3, "设置")

// 主机状态与安定宝事件码对照表
#define HMS_EVENT_MAP(XX)            \
    XX(HMS_ARM_AWAY, EVENT_ARM_AWAY) \
    XX(HMS_ARM_STAY, EVENT_ARM_STAY) \
    XX(HMS_DISARM, EVENT_DISARM)     \
    XX(HMS_SETTING, EVENT_ENTER_SETTING_MODE)

// 主机状态
typedef enum hb_machine_status_t {
#define XX(name, value, str) HMS_##name = value,
    HB_MACHINE_STATUS_MAP(XX)
#undef XX
    // valid count
    HMS_COUNT = 4,
    // invalid
    HMS_INVALID = 0xFF,
} hb_machine_status_t;

// 主机类型表
#define HB_MACHINE_TYPE_MAP(XX)          \
    XX(WIFI, 0, "WiFi主机")              \
    XX(CAMERA, 1, "网络摄像机")          \
    XX(GPRS_IOT, 2, "简化物联卡主机")    \
    XX(NETMOD, 3, "网络模块主机")        \
    XX(GPRS, 4, "GPRS主机")              \
    XX(LCD, 5, "液晶主机")               \
    XX(WIRED, 6, "网线主机")             \
    XX(TRUE_COLOR, 7, "真彩主机")        \
    XX(THREE_SECTION, 8, "三区段主机")   \
    XX(IOT, 9, "物联卡主机")             \
    XX(GPRS_PHONE, 10, "GPRS主机可电话") \
    XX(NB, 11, "NB报警接收主机")         \
    XX(WIFI2, 12, "WiFi主机新版")

// 主机类型与安定宝事件码对照表
#define HMT_EVENT_MAP(XX)                       \
    XX(HMT_WIFI, EVENT_I_AM_WIFI)               \
    XX(HMT_GPRS_IOT, EVENT_I_AM_GPRS_IOT)       \
    XX(HMT_NETMOD, EVENT_I_AM_NET_MODULE)       \
    XX(HMT_GPRS, EVENT_I_AM_GPRS)               \
    XX(HMT_LCD, EVENT_I_AM_LCD)                 \
    XX(HMT_WIRED, EVENT_I_AM_WIRE)              \
    XX(HMT_TRUE_COLOR, EVENT_I_AM_TRUE_COLOR)   \
    XX(HMT_THREE_SECTION, EVENT_I_AM_3_SECTION) \
    XX(HMT_IOT, EVENT_I_AM_IOT)                 \
    XX(HMT_GPRS_PHONE, EVENT_I_AM_GPRS_PHONE)   \
    XX(HMT_NB, EVENT_I_AM_NB)                   \
    XX(HMT_WIFI2, EVENT_I_AM_WIFI2)

// 主机类型
typedef enum hb_machine_type_t {
#define XX(name, value, str) HMT_##name = value,
    HB_MACHINE_TYPE_MAP(XX)
#undef XX
    // valid count
    HMT_COUNT = 13,
    // invalid
    HMT_INVALID = 0xFF,
} hb_machine_type_t;

// 防区属性表
#define HB_ZONE_PROPERTY_MAP(XX)    \
    XX(BURGLAR, 0, "匪警全局")      \
    XX(EMERGENCY, 1, "匪警紧急")    \
    XX(FIRE, 2, "火警防区")         \
    XX(DURESS, 3, "胁迫防区")       \
    XX(GAS, 4, "燃气防区")          \
    XX(WATER, 5, "淹水防区")        \
    XX(SUB_MACHINE, 6, "分机")      \
    XX(REMOTE_CONTROL, 7, "遥控器") \
    XX(BURGLAR_HALF, 8, "匪警半局") \
    XX(SHIELD, 9, "屏蔽防区")       \
    XX(DOOR_RING, 0x0A, "门铃防区") \
    XX(BYPASS, 0x0F, "旁路防区")

// 防区属性
typedef enum hb_zone_property_t {
#define XX(name, value, str) HZP_##name = value,
    HB_ZONE_PROPERTY_MAP(XX)
#undef XX

    // valid count
    HZP_COUNT = 12,
    // invalid
    HZP_INVALID = 0xFF,
} hb_zone_property_t;

typedef struct {
    uint16_t zone;
    hb_zone_property_t prop;
} hb_zone_and_property_t;

// UTC time, its user's responsibility to convert to local time
typedef struct {
    uint8_t hour;
    uint8_t minute;
} hb_machine_time_point_t;

typedef struct {
    hb_machine_time_point_t arm_at;
    hb_machine_time_point_t disarm_at;
} hb_machine_timer_one_t;

typedef union {
    hb_machine_timer_one_t timer[2];  // 2 set of timer
    uint8_t dat[8];
} hb_machine_timer_t;

///////////////////////////// property functions //////////////////////////////

// return available count
// props must have at least HZP_COUNT items
HB_EXPORT_SYMBOL
uint8_t hb_get_available_zone_properties_by_type(hb_machine_type_t type,
                                                 hb_zone_property_t props[HZP_COUNT]);

// 最大防区号根据型号不同而不同
HB_EXPORT_SYMBOL
uint16_t hb_get_max_zone_by_type(hb_machine_type_t type);

// 防区号是否合法（可以包含0防区）
HB_EXPORT_SYMBOL
uint8_t hb_is_valid_zone_by_type(hb_machine_type_t type, uint16_t zone);

// 防区号是否合法（不可以可以包含0防区）
HB_EXPORT_SYMBOL
uint8_t hb_is_valid_zone_by_type_strict(hb_machine_type_t type, uint16_t zone);

// 主机是否已投产使用
HB_EXPORT_SYMBOL
uint8_t hb_is_machine_on_sale(hb_machine_type_t type);

// 主机是否具有离家布防功能
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_arm_away(hb_machine_type_t type);

// 主机是否具有撤防功能
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_disarm(hb_machine_type_t type);

// 主机是否可以进入设置状态
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_config(hb_machine_type_t type);

// 主机是否具有留守布防功能
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_arm_stay(hb_machine_type_t type);

// 主机是否可以报告信号强度
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_report_signal_strength(hb_machine_type_t type);

// 主机本身是否可以短信报警（不算通过服务如阿里语音等）
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_report_by_sms(hb_machine_type_t type);

// 主机是否支持有线防区
HB_EXPORT_SYMBOL
uint8_t hb_machine_has_wired_zones(hb_machine_type_t type);

// 主机最小有线防区号
HB_EXPORT_SYMBOL
uint16_t hb_wired_zone_min(hb_machine_type_t type);

// 主机最大有线防区号
HB_EXPORT_SYMBOL
uint16_t hb_wired_zone_max(hb_machine_type_t type);

// 主机是否可以直接写入防区数据（无需对码）
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_write_zone(hb_machine_type_t type);

// 主机是否可以挂载分机
HB_EXPORT_SYMBOL
uint8_t hb_machine_can_link_sub_machine(hb_machine_type_t type);

// 根据防区属性判断是否支持失联报告
// 失联报告是主机侧实现的跟防区属性没关系，但是人为限制了只有以下属性的才可以设置
HB_EXPORT_SYMBOL
uint8_t hb_zone_can_report_lost(hb_zone_property_t zp);

///////////////////////////// timer functions /////////////////////////////////

HB_EXPORT_SYMBOL
void hb_machine_timer_one_init(hb_machine_timer_one_t* timer);

HB_EXPORT_SYMBOL
void hb_machine_timer_init(hb_machine_timer_t* timer);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_time_point(hb_machine_time_point_t* tp);

HB_EXPORT_SYMBOL
uint8_t hb_time_point_equal(hb_machine_time_point_t* tp1, hb_machine_time_point_t* tp2);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_timer_one(hb_machine_timer_one_t* timer);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_machine_timer(hb_machine_timer_t* timer);

///////////////////////////// helper functions ////////////////////////////////

HB_EXPORT_SYMBOL
const char* hb_machine_status_to_string(hb_machine_status_t status);

HB_EXPORT_SYMBOL
const char* hb_machine_type_to_string(hb_machine_type_t type);

HB_EXPORT_SYMBOL
const char* hb_zone_property_to_string(hb_zone_property_t zp);

#if HB_ENABLE_CHINESE
HB_EXPORT_SYMBOL
const char* hb_machine_status_to_string_chinese(hb_machine_status_t status);

HB_EXPORT_SYMBOL
const char* hb_machine_type_to_string_chinese(hb_machine_type_t type);

HB_EXPORT_SYMBOL
const char* hb_zone_property_to_string_chinese(hb_zone_property_t zp);
#endif  // HB_ENABLE_CHINESE

HB_EXPORT_SYMBOL
const char* hb_get_zone_format_str(hb_machine_type_t type);

#ifdef __cplusplus
}
#endif

#endif
