#ifndef __HB_CORE_H__
#define __HB_CORE_H__

#include "hb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__C51__) || defined(VSCODE)
#pragma pack(1)
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
    XX(0, ARM_AWAY, "ARM_AWAY")   \
    XX(1, ARM_STAY, "ARM_STAY")   \
    XX(2, DISARM, "DISARM")       \
    XX(3, SETTING, "SETTING")

// 主机状态与安定宝事件码对照表
#define HMS_EVENT_MAP(XX)            \
    XX(HMS_ARM_AWAY, EVENT_ARM_AWAY) \
    XX(HMS_ARM_STAY, EVENT_ARM_STAY) \
    XX(HMS_DISARM, EVENT_DISARM)     \
    XX(HMS_SETTING, EVENT_ENTER_SETTING_MODE)

// 主机状态
typedef enum hb_machine_status_t {
#define XX(value, name, str) HMS_##name = value,
    HB_MACHINE_STATUS_MAP(XX)
#undef XX
    // valid count
    HMS_COUNT = 4,
    // invalid
    HMS_INVALID = 0xFF,
} hb_machine_status_t;

// 主机类型表
#define HB_MACHINE_TYPE_MAP(XX)          \
    XX(0, WIFI, "WiFi主机")              \
    XX(1, CAMERA, "摄像头主机")          \
    XX(2, GPRS_IOT, "简化物联卡主机")    \
    XX(3, NETMOD, "网络模块主机")        \
    XX(4, GPRS, "GPRS主机")              \
    XX(5, LCD, "液晶主机")               \
    XX(6, WIRED, "网线主机")             \
    XX(7, TRUE_COLOR, "真彩主机")        \
    XX(8, THREE_SECTION, "三区段主机")   \
    XX(9, IOT, "物联卡主机")             \
    XX(10, GPRS_PHONE, "GPRS主机可电话") \
    XX(11, NB, "NB报警接收主机")         \
    XX(12, WIFI2, "WiFi主机新版")

// 主机类型
typedef enum hb_machine_type_t {
#define XX(value, name, str) HMT_##name = value,
    HB_MACHINE_TYPE_MAP(XX)
#undef XX
    // valid count
    HMT_COUNT = 13,
    // invalid
    HMT_INVALID = 0xFF,
} hb_machine_type_t;

// 防区属性表
// 2024年8月2日21:37:14 删除 0x0F 旁路防区，修改 9 屏蔽防区为旁路防区：旁路即屏蔽
#define HB_ZONE_PROPERTY_MAP(XX)    \
    XX(0, BURGLAR, "匪警全局")      \
    XX(1, EMERGENCY, "匪警紧急")    \
    XX(2, FIRE, "火警防区")         \
    XX(3, DURESS, "胁迫防区")       \
    XX(4, GAS, "燃气防区")          \
    XX(5, WATER, "淹水防区")        \
    XX(6, SUB_MACHINE, "分机")      \
    XX(7, REMOTE_CONTROL, "遥控器") \
    XX(8, BURGLAR_HALF, "匪警半局") \
    XX(9, BYPASS, "旁路防区")       \
    XX(10, DOOR_RING, "门铃防区")

// 防区属性
typedef enum hb_zone_property_t {
#define XX(value, name, str) HZP_##name = value,
    HB_ZONE_PROPERTY_MAP(XX)
#undef XX

    // valid count
    HZP_COUNT = 11,
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
#if HB_ENABLE_TIMER_ARM_STAY
    uint8_t arm_what;  // 0: arm away, 1: arm stay
#define HB_TIMER_ARM_WHAT_ARM_AWAY 0
#define HB_TIMER_ARM_WHAT_ARM_STAY 1
#endif
    hb_machine_time_point_t arm_at;
    hb_machine_time_point_t disarm_at;
} hb_machine_timer_one_t;

typedef union {
    hb_machine_timer_one_t timer[2];  // 2 set of timer
    uint8_t dat[sizeof(hb_machine_timer_one_t) * 2];
} hb_machine_timer_t;

///////////////////////////// property functions //////////////////////////////

#define HB_GPRS_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                             \
        HZP_BURGLAR,                              \
        HZP_EMERGENCY,                            \
        HZP_FIRE,                                 \
        HZP_DURESS,                               \
        HZP_GAS,                                  \
        HZP_WATER,                                \
        HZP_REMOTE_CONTROL,                       \
    }

#define HB_NETMOD_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                               \
        HZP_BURGLAR,                                \
        HZP_EMERGENCY,                              \
        HZP_FIRE,                                   \
        HZP_DURESS,                                 \
        HZP_GAS,                                    \
        HZP_WATER,                                  \
        HZP_SUB_MACHINE,                            \
        HZP_REMOTE_CONTROL,                         \
        HZP_BURGLAR_HALF,                           \
        HZP_BYPASS,                                 \
        HZP_DOOR_RING,                              \
    }

#define HB_LCD_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                            \
        HZP_BURGLAR,                             \
        HZP_EMERGENCY,                           \
        HZP_FIRE,                                \
        HZP_DURESS,                              \
        HZP_GAS,                                 \
        HZP_WATER,                               \
        HZP_SUB_MACHINE,                         \
        HZP_REMOTE_CONTROL,                      \
        HZP_BURGLAR_HALF,                        \
        HZP_DOOR_RING,                           \
        HZP_BYPASS,                              \
    }

#define HB_WIRED_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                              \
        HZP_BURGLAR,                               \
        HZP_EMERGENCY,                             \
        HZP_FIRE,                                  \
        HZP_DURESS,                                \
        HZP_GAS,                                   \
        HZP_WATER,                                 \
        HZP_REMOTE_CONTROL,                        \
    }

#define HB_TRUE_COLOR_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                                   \
        HZP_BURGLAR,                                    \
        HZP_EMERGENCY,                                  \
        HZP_FIRE,                                       \
        HZP_DURESS,                                     \
        HZP_GAS,                                        \
        HZP_WATER,                                      \
        HZP_REMOTE_CONTROL,                             \
        HZP_DOOR_RING,                                  \
        HZP_BYPASS,                                     \
    }

#define HB_THREE_SECTION_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                                      \
        HZP_BURGLAR,                                       \
        HZP_EMERGENCY,                                     \
        HZP_FIRE,                                          \
        HZP_DURESS,                                        \
        HZP_GAS,                                           \
        HZP_WATER,                                         \
        HZP_REMOTE_CONTROL,                                \
        HZP_DOOR_RING,                                     \
        HZP_BYPASS,                                        \
    }

#define HB_IOT_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                            \
        HZP_BURGLAR,                             \
        HZP_EMERGENCY,                           \
        HZP_FIRE,                                \
        HZP_DURESS,                              \
        HZP_GAS,                                 \
        HZP_WATER,                               \
        HZP_REMOTE_CONTROL,                      \
        HZP_DOOR_RING,                           \
        HZP_BYPASS,                              \
    }

#define HB_GPRS_PHONE_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                                   \
        HZP_BURGLAR,                                    \
        HZP_EMERGENCY,                                  \
        HZP_FIRE,                                       \
        HZP_DURESS,                                     \
        HZP_GAS,                                        \
        HZP_WATER,                                      \
        HZP_REMOTE_CONTROL,                             \
    }

#define HB_NB_MACHINE_SUPPORTED_ZONE_PROPERTIES \
    {                                           \
        HZP_BURGLAR,                            \
        HZP_EMERGENCY,                          \
        HZP_FIRE,                               \
        HZP_DURESS,                             \
        HZP_GAS,                                \
        HZP_WATER,                              \
        HZP_DOOR_RING,                          \
    }

// return available count
// props must have at least HZP_COUNT slots
HB_EXPORT_SYMBOL
uint8_t hb_get_available_zone_properties_by_type(hb_machine_type_t type,
                                                 hb_zone_property_t props[HZP_COUNT]);

// 根据主机类型判断是否支持该属性
HB_EXPORT_SYMBOL
bool hb_machine_supports_zone_property(hb_machine_type_t type, hb_zone_property_t prop);

// 最大防区号根据型号不同而不同
HB_EXPORT_SYMBOL
uint16_t hb_get_max_zone_by_type(hb_machine_type_t type);

// 防区号是否合法（可以包含0防区）
HB_EXPORT_SYMBOL
bool hb_is_valid_zone_by_type(hb_machine_type_t type, uint16_t zone);

// 防区号是否合法（不可以可以包含0防区）
HB_EXPORT_SYMBOL
bool hb_is_valid_zone_by_type_strict(hb_machine_type_t type, uint16_t zone);

// 主机是否已投产使用
HB_EXPORT_SYMBOL
bool hb_is_machine_on_sale(hb_machine_type_t type);

// 主机是否具有离家布防功能
HB_EXPORT_SYMBOL
bool hb_machine_can_arm_away(hb_machine_type_t type);

// 主机是否具有撤防功能
HB_EXPORT_SYMBOL
bool hb_machine_can_disarm(hb_machine_type_t type);

// 主机是否可以进入设置状态
HB_EXPORT_SYMBOL
bool hb_machine_can_config(hb_machine_type_t type);

// 主机是否具有留守布防功能
HB_EXPORT_SYMBOL
bool hb_machine_can_arm_stay(hb_machine_type_t type);

// 主机是否可以报告信号强度
HB_EXPORT_SYMBOL
bool hb_machine_can_report_signal_strength(hb_machine_type_t type);

// 主机本身是否可以短信报警（不算通过服务如阿里语音等）
HB_EXPORT_SYMBOL
bool hb_machine_can_report_by_sms(hb_machine_type_t type);

// 主机是否支持有线防区
HB_EXPORT_SYMBOL
bool hb_machine_has_wired_zones(hb_machine_type_t type);

// 主机最小有线防区号
HB_EXPORT_SYMBOL
uint16_t hb_wired_zone_min(hb_machine_type_t type);

// 主机最大有线防区号
HB_EXPORT_SYMBOL
uint16_t hb_wired_zone_max(hb_machine_type_t type);

// 主机是否可以直接写入防区数据（无需对码）
HB_EXPORT_SYMBOL
bool hb_machine_can_write_zone(hb_machine_type_t type);

// 主机是否可以挂载分机
HB_EXPORT_SYMBOL
bool hb_machine_can_link_sub_machine(hb_machine_type_t type);

// 根据防区属性判断是否支持失联报告
// 失联报告是主机侧实现的跟防区属性没关系，但是人为限制了只有以下属性的才可以设置
HB_EXPORT_SYMBOL
bool hb_zone_can_report_lost(hb_zone_property_t zp);

///////////////////////////// timer functions /////////////////////////////////

HB_EXPORT_SYMBOL
void hb_machine_timer_one_init(hb_machine_timer_one_t* timer);

HB_EXPORT_SYMBOL
void hb_machine_timer_init(hb_machine_timer_t* timer);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_time_point(const hb_machine_time_point_t* tp);

HB_EXPORT_SYMBOL
uint8_t hb_time_point_equal(const hb_machine_time_point_t* tp1, const hb_machine_time_point_t* tp2);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_timer_one(const hb_machine_timer_one_t* timer);

HB_EXPORT_SYMBOL
uint8_t hb_is_valid_machine_timer(const hb_machine_timer_t* timer);

///////////////////////////// helper functions ////////////////////////////////

#if HB_ENABLE_TO_STRING
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
#endif  // HB_ENABLE_TO_STRING

#if !defined(__C51__) || defined(VSCODE)
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif
