#ifndef __HB_COM_H__
#define __HB_COM_H__

#pragma once

#include "ademco.h"

#ifdef __cplusplus
extern "C" {
#endif

// 串口透传数据最大长度
#define HB_COM_DATA_MAX_LEN 64

// 三防区主机状态GG范围 1~3
#define HB_3SECTION_MACHINE_GG_MIN 1
#define HB_3SECTION_MACHINE_GG_MAX 3

// 是否合法的三防区主机状态GG
#define hb_is_valid_3section_machine_gg(gg) \
    HB_3SECTION_MACHINE_GG_MIN <= (gg) &&   \
        (gg) <= HB_3SECTION_MACHINE_GG_MAX

// 主机状态表
#define HB_MACHINE_STATUS_MAP(XX) \
    XX(ARM, 0, "布防")            \
    XX(HALF_ARM, 1, "半布防")     \
    XX(DISARM, 2, "撤防")         \
    XX(SETTING, 3, "设置")

// 主机状态与安定宝事件码对照表
#define HMS_EVENT_MAP(XX)           \
    XX(HMS_ARM, EVENT_ARM)          \
    XX(HMS_HALF_ARM, EVENT_HALFARM) \
    XX(HMS_DISARM, EVENT_DISARM)    \
    XX(HMS_SETTING, EVENT_ENTER_SETTING_MODE)

// 主机状态
typedef enum hb_machine_status_t {
#define XX(name, value, str) HMS_##name = value,
    HB_MACHINE_STATUS_MAP(XX)
#undef XX
    // valid count
    HMS_COUNT = 4,
    // invalid
    HMS_INVALID = -1,
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
#define HMT_EVENT_MAP(XX)                               \
    XX(HMT_WIFI, EVENT_I_AM_WIFI_MACHINE)               \
    XX(HMT_GPRS_IOT, EVENT_I_AM_GPRS_IOT)               \
    XX(HMT_NETMOD, EVENT_I_AM_NET_MODULE)               \
    XX(HMT_GPRS, EVENT_I_AM_GPRS)                       \
    XX(HMT_LCD, EVENT_I_AM_LCD_MACHINE)                 \
    XX(HMT_WIRED, EVENT_I_AM_WIRE_MACHINE)              \
    XX(HMT_TRUE_COLOR, EVENT_I_AM_TRUE_COLOR)           \
    XX(HMT_THREE_SECTION, EVENT_I_AM_3_SECTION_MACHINE) \
    XX(HMT_IOT, EVENT_I_AM_IOT_MACHINE)                 \
    XX(HMT_GPRS_PHONE, EVENT_I_AM_GPRS_PHONE)           \
    XX(HMT_NB, EVENT_I_AM_NB_MACHINE)                   \
    XX(HMT_WIFI2, EVENT_I_AM_WIFI2_MACHINE)

// 主机类型
typedef enum hb_machine_type_t {
#define XX(name, value, str) HMT_##name = value,
    HB_MACHINE_TYPE_MAP(XX)
#undef XX
    // valid count
    HMT_COUNT = 13,
    // invalid
    HMT_INVALID = -1,
} hb_machine_type_t;

// 防区属性表
#define HB_ZONE_PROPERTY_MAP(XX)    \
    XX(BUGLAR, 0, "匪警全局")       \
    XX(EMERGENCY, 1, "匪警紧急")    \
    XX(FIRE, 2, "火警防区")         \
    XX(DURESS, 3, "胁迫防区")       \
    XX(GAS, 4, "燃气防区")          \
    XX(WATER, 5, "淹水防区")        \
    XX(SUB_MACHINE, 6, "分机")      \
    XX(REMOTE_CONTROL, 7, "遥控器") \
    XX(BUGLAR_HALF, 8, "匪警半局")  \
    XX(SHIELD, 9, "屏蔽防区")       \
    XX(DOOR_RING, 0x0A, "门铃防区") \
    XX(BYPASS, 0x0F, "旁路防区")

// 防区属性与安定宝事件码对照表
#define HZP_EVENT_MAP(XX)              \
    XX(HZP_BUGLAR, EVENT_BURGLAR)      \
    XX(HZP_BUGLAR_HALF, EVENT_BURGLAR) \
    XX(HZP_EMERGENCY, EVENT_EMERGENCY) \
    XX(HZP_FIRE, EVENT_FIRE)           \
    XX(HZP_DURESS, EVENT_DURESS)       \
    XX(HZP_GAS, EVENT_GAS)             \
    XX(HZP_WATER, EVENT_WATER)         \
    XX(HZP_DOOR_RING, EVENT_DOORRINGING)

// 防区属性
typedef enum hb_zone_property_t {
#define XX(name, value, str) HZP_##name = value,
    HB_ZONE_PROPERTY_MAP(XX)
#undef XX

    // valid count
    HZP_COUNT = 12,
    // invalid
    HZP_INVALID = -1,
} hb_zone_property_t;

typedef struct hb_zone_and_property_t {
    ademco_zone_t zone;
    hb_zone_property_t prop;
} hb_zone_and_property_t;

typedef struct hb_machine_time_point_t {
    uint8_t hour;
    uint8_t minute;
} hb_machine_time_point_t;

typedef struct hb_machine_timer_one_t {
    hb_machine_time_point_t arm_at;
    hb_machine_time_point_t disarm_at;
} hb_machine_timer_one_t;

typedef union hb_machine_timer_t {
    hb_machine_timer_one_t timer[2];  // 2 set of timer
    uint8_t data[8];
} hb_machine_timer_t;

// 串口透传数据
typedef struct hb_com_data_t {
    uint8_t data[HB_COM_DATA_MAX_LEN];
    uint8_t len;
} hb_com_data_t;

// COM request table
#define HB_COM_REQUESTS_MAP(XX)                 \
    XX(A0, "索要主机状态")                      \
    XX(A1, "索要防区")                          \
    XX(A2, "索要更多防区")                      \
    XX(A3, "修改防区")                          \
    XX(A5, "获取定时器")                        \
    XX(A7, "设置定时器")                        \
    XX(A9, "TODO")                              \
    XX(AA, "修改防区探头遗失/失联")             \
    XX(AC, "索要防区探头遗失/失联--第一次索要") \
    XX(AD, "索要防区探头遗失/失联--继续索要")   \
    XX(AE, "三区段主机布撤防命令")              \
    XX(B0, "索要三区段主机状态")                \
    XX(RA, "读取主机账号")                      \
    XX(WA, "写入主机账号")

// 发给主机
typedef enum hb_com_req_t {
#define XX(name, str) HB_COM_REQ_##name,
    HB_COM_REQUESTS_MAP(XX)
#undef XX

    // invalid
    HB_COM_REQ_INVALID = -1,
} hb_com_req_t;

// COM response table
#define HB_COM_RESPONSES_MAP(XX)        \
    XX(A0, "索要主机状态回应")          \
    XX(A2, "索要防区回应")              \
    XX(A3, "学码开始回应")              \
    XX(A4, "修改防区回应")              \
    XX(A6, "获取定时器回应")            \
    XX(A7, "设置定时器回应")            \
    XX(A8, "拒绝设置或设置失败回应")    \
    XX(A9, "TODO")                      \
    XX(AB, "修改防区探头遗失/失联回应") \
    XX(AD, "索要防区探头遗失/失联回应") \
    XX(AF, "三区段主机布撤防命令回应")  \
    XX(B1, "三区段主机状态回应")        \
    XX(RA, "读取主机账号回应")          \
    XX(WA, "写入主机账号回应")

// 主机回应
typedef enum hb_com_resp_t {
#define XX(name, str) HB_COM_RESP_##name,
    HB_COM_RESPONSES_MAP(XX)
#undef XX

    // invalid
    HB_COM_RESP_INVALID = -1,
} hb_com_resp_t;

// 索要主机状态 EB AB 3F A0 75
#define HB_COM_REQ_A0_LEN 5
#define HB_COM_REQ_A0_DATA "\xEB\xAB\x3F\xA0\x75"

// 回应主机状态 EB BA 3F 07 P0 A0 P1 P2 SUM
#define HB_COM_RESP_A0_LEN 9
#define HB_COM_RESP_A0_HEAD "\xEB\xBA\x3F\x07\xCC\xA0"

// 索要主机防区 EB AB 3F A1 76
#define HB_COM_REQ_A1_LEN 5
#define HB_COM_REQ_A1_DATA "\xEB\xAB\x3F\xA1\x76"

// 回应主机防区 EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
// 无防区数据时长度最小为8
#define HB_COM_RESP_A2_MIN_LEN 8
// 最多可以包含 20 个防区
#define HB_COM_RESP_A2_MAX_ZONE 20
// 一包数据最多有8+20*2=48个字节
#define HB_COM_RESP_A2_MAX_LEN (HB_COM_RESP_A2_MIN_LEN + HB_COM_RESP_A2_MAX_ZONE * 2)
typedef enum hb_com_resp_a2_p1_t {
    HB_COM_RESP_A2_P1_MORE = 0x00,    // 还有更多防区
    HB_COM_RESP_A2_P1_NOMORE = 0xFF,  // 无更多防区
} hb_com_resp_a2_p1_t;
#define HB_COM_RESP_A2_HEAD "\xEB\xBA\x3F\x08\xCC\xA2"
typedef struct hb_com_resp_a2_iter_t {
    const hb_com_data_t* com;
    uint8_t i, total;
    hb_com_resp_a2_p1_t p1;
} hb_com_resp_a2_iter_t;

// 索要更多主机防区 EB AB 3F A2 77
// 仅应在收到ZoneResponse的param非0xFF时发送，以索要全部防区
#define HB_COM_REQ_A2_LEN 5
#define HB_COM_REQ_A2_DATA "\xEB\xAB\x3F\xA2\x77"

// 修改主机防区 EB CB 3F 09 A3 P1 P2 P3 SUM
#define HB_COM_REQ_A3_LEN 9
#define HB_COM_REQ_A3_DATA "\xEB\xCB\x3F\x09\xA3"

typedef enum hb_com_req_a3_p3_t {
    HB_COM_REQ_A3_P3_DEL = 0x00,   // 删除防区
    HB_COM_REQ_A3_P3_PAIR = 0x01,  // 学码
    HB_COM_REQ_A3_P3_STOP = 0x02,  // 停止学码
    HB_COM_REQ_A3_P3_MOD = 0x03,   // 修改防区属性
} hb_com_req_a3_p3_t;

// 学码开始回应 EB BA 3F 07 P0 A3 5A
// 因为学码时主机要等待外部无线信号（用户触发探测器），
// 因此先回应A3表示已经开始学码，学码成功时回 ZoneOpResponse A4
#define HB_COM_RESP_A3_LEN 7
#define HB_COM_RESP_A3_DATA "\xEB\xBA\x3F\x07\xCC\xA3\x5A"

// 修改防区回应 EB BA 3F 0A P0 A4 P1 P2 P3 SUM
#define HB_COM_RESP_A4_LEN 10
#define HB_COM_RESP_A4_HEAD "\xEB\xBA\x3F\x0A\xCC\xA4"
typedef enum hb_com_resp_a4_p3_t {
    HB_COM_RESP_A4_P3_FAIL = 0x00,  // 失败
    HB_COM_RESP_A4_P3_OK = 0x01,    // 成功
    HB_COM_RESP_A4_P3_DUP = 0x02,   // 失败--重码
    HB_COM_RESP_A4_P3_NE = 0x03,    // 失败--防区未对码 not exists
} hb_com_resp_a4_p3_t;

// 获取主机定时器
// EB AB 3F A5 7A
#define HB_COM_REQ_A5_LEN 5
#define HB_COM_REQ_A5_DATA "\xEB\xAB\x3F\xA5\x7A"

// 获取主机定时器回应
// EB BA 3F 0F P0 A6 H1 M1 H2 M2 H3 M3 H4 M4 SUM
#define HB_COM_RESP_A6_LEN 15
#define HB_COM_RESP_A6_HEAD "\xEB\xBA\x3F\x0F\xCC\xA6"

// 设置主机定时器
// EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
#define HB_COM_REQ_A7_LEN 14
#define HB_COM_REQ_A7_HEAD "\xEB\xCB\x3F\x0E\xA7"

// 设置主机定时器回应
// EB BA 3F 07 P0 A7 SUM
#define HB_COM_RESP_A7_LEN 7
#define HB_COM_RESP_A7_DATA "\xEB\xBA\x3F\x07\xCC\xA7\x5E"

// 拒绝设置回应
// EB BA 3F 07 P0 A8 SUM
// 任何操作，主机如果认为非法，都可以用A8直接回复
#define HB_COM_RESP_A8_LEN 7
#define HB_COM_RESP_A8_DATA "\xEB\xBA\x3F\x07\xCC\xA8\x5F"

// 修改防区探头遗失/失联
// EB CB 3F 08 AA P1 P2 SUM
#define HB_COM_REQ_AA_LEN 8
#define HB_COM_REQ_AA_HEAD "\xEB\xAB\x3F\x08\xAA"

// 修改防区探头遗失/失联回应
// EB BA 3F 09 P0 AB P1 P2 SUM
#define HB_COM_RESP_AB_LEN 9
#define HB_COM_RESP_AB_HEAD "\xEB\xBA\x3F\x09\xCC\xAB"
typedef enum hb_com_resp_ab_p2_t {
    HB_COM_RESP_AB_P2_CLOSE = 0,   // 防区失联关
    HB_COM_RESP_AB_P2_OPEN = 1,    // 防区失联开
    HB_COM_RESP_AB_P2_REJECT = 2,  // 拒绝操作
} hb_com_resp_ab_p2_t;

// 索要防区探头遗失/失联，第一次索要
// EB AB 3F AC 81
#define HB_COM_REQ_AC_LEN 5
#define HB_COM_REQ_AC_DATA "\xEB\xAB\x3F\xAC\x81"

// 索要防区探头遗失/失联,索要更多
// EB AB 3F AD 82
#define HB_COM_REQ_AD_LEN 5
#define HB_COM_REQ_AD_DATA "\xEB\xAB\x3F\xAD\x82"

// 索要防区探头遗失/失联回应
// EB BA 3F PN P0 AD P1 DATA P2 SUM
// 无防区失联数据时一包长度最小为9
#define HB_COM_RESP_AD_MIN_LEN 9
// 一包最多包含20个防区
#define HB_COM_RESP_AD_MAX_ZONE 20
// 一包最多有 9 + 20 *2 = 49 个字节
#define HB_COM_RESP_AD_MAX_LEN \
    (HB_COM_RESP_AD_MIN_LEN + HB_COM_RESP_AD_MAX_ZONE * 2)

typedef enum hb_com_resp_ad_p1_t {
    HB_COM_RESP_AD_P1_SINGLE = 0xF0,  // 防区号以单字节表示
    HB_COM_RESP_AD_P1_DOUBLE = 0xF1,  // 防区号以双字节表示
} hb_com_resp_ad_p1_t;

typedef enum hb_com_resp_ad_p2_t {
    HB_COM_RESP_AD_P2_MORE = 0x00,    // 还有更多防区
    HB_COM_RESP_AD_P2_NOMORE = 0xFF,  // 无更多防区
} hb_com_resp_ad_p2_t;

#define HB_COM_RESP_AD_HEAD "\xEB\xBA\x3F\x09\xCC\xAD"

typedef struct hb_com_resp_ad_iter_t {
    const hb_com_data_t* com;
    uint8_t i, total;
    hb_com_resp_ad_p1_t p1;
    hb_com_resp_ad_p2_t p2;
} hb_com_resp_ad_iter_t;

typedef enum hb_com_3sec_status_t {
    HB_COM_3SEC_ARM = 0x00,     // 布防（成功）
    HB_COM_3SEC_DISARM = 0x01,  // 撤防（成功）
} hb_com_3sec_status_t;

typedef enum hb_com_3sec_which_t {
    HB_COM_3SEC_MACHINE,   // 主机
    HB_COM_3SEC_SECTION1,  // 区段1
    HB_COM_3SEC_SECTION2,  // 区段2
    HB_COM_3SEC_SECTION3,  // 区段3
} hb_com_3sec_which_t;

// 三区段主机布撤防命令 EB CB 3F 08 AE P1 P2 SUM
#define HB_COM_REQ_AE_LEN 8
#define HB_COM_REQ_AE_HEAD "\xEB\xCB\x3F\x08\xAE"

// 三区段主机布撤防命令回应 EB BA 3F 08 P0 AF P1 P2 SUM
#define HB_COM_RESP_AF_LEN 9
#define HB_COM_RESP_AF_HEAD "\xEB\xBA\x3F\x08\xCC\xAF"

// 三区段主机索要主机状态 EB CB 3F 06 B0 AB
#define HB_COM_REQ_B0_LEN 6
#define HB_COM_REQ_B0_DATA "\xEB\xCB\x3F\x06\xB0\xAB"

// 三区段主机状态回应 EB BA 3F 08 P0 B1 P1 SUM
#define HB_COM_RESP_B1_LEN 8
#define HB_COM_RESP_B1_HEAD "\xEB\xBA\x3F\x08\xCC\xB1"
#define hb_com_resp_b1_get_status(d) \
    ((((d)[6] >> 6) & 0x03) == HB_COM_3SEC_ARM ? HMS_ARM : HMS_DISARM)

#define hb_com_resp_b1_get_status1(d) \
    ((((d)[6] >> 4) & 0x03) == HB_COM_3SEC_ARM ? HMS_ARM : HMS_DISARM)

#define hb_com_resp_b1_get_status2(d) \
    ((((d)[6] >> 2) & 0x03) == HB_COM_3SEC_ARM ? HMS_ARM : HMS_DISARM)

#define hb_com_resp_b1_get_status3(d) \
    ((((d)[6]) & 0x03) == HB_COM_3SEC_ARM ? HMS_ARM : HMS_DISARM)

// 读取主机账号
#define HB_COM_REQ_RA_LEN 7
#define HB_COM_REQ_RA_DATA "\xEB\xBA\x3F\x07\x00\x4C\x37"

// 读取主机账号回应
#define HB_COM_RESP_RA_LEN 15
#define HB_COM_RESP_RA_HEAD "\xEB\xCB\x3F\x0F\x4C"

// 写入主机账号
#define HB_COM_REQ_WA_LEN 15
#define HB_COM_REQ_WA_HEAD "\xEB\xCB\x3F\x0F\x4D"

// 写入主机账号回应（与读取命令相同）
#define HB_COM_RESP_WA_LEN HB_COM_REQ_RA_LEN
#define HB_COM_RESP_WA_DATA HB_COM_REQ_RA_DATA

///////////////////////////// property functions //////////////////////////////

// return available count
// props must have at least HZP_COUNT items
ADEMCO_EXPORT_SYMBOL
size_t hb_get_available_zone_properties_by_type(hb_machine_type_t type, hb_zone_property_t props[HZP_COUNT]);

// 最大防区号根据型号不同而不同
ADEMCO_EXPORT_SYMBOL
ademco_zone_t hb_get_max_zone_by_type(hb_machine_type_t type);

// 防区号是否合法（可以包含0防区）
ADEMCO_EXPORT_SYMBOL
int hb_is_valid_zone_by_type(hb_machine_type_t type, ademco_zone_t zone);

// 防区号是否合法（不可以可以包含0防区）
ADEMCO_EXPORT_SYMBOL
int hb_is_valid_zone_by_type_strict(hb_machine_type_t type, ademco_zone_t zone);

// 主机是否已投产使用
ADEMCO_EXPORT_SYMBOL
int hb_is_machine_on_sale(hb_machine_type_t type);

// 主机是否具有布防功能
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_arm(hb_machine_type_t type);

// 主机是否具有撤防功能
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_disarm(hb_machine_type_t type);

// 主机是否可以进入设置状态
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_config(hb_machine_type_t type);

// 主机是否具有半布防功能
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_half_arm(hb_machine_type_t type);

// 主机是否可以报告信号强度
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_report_signal_strength(hb_machine_type_t type);

// 主机本身是否可以短信报警（不算通过服务如阿里语音等）
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_report_by_sms(hb_machine_type_t type);

// 主机是否支持有线防区
ADEMCO_EXPORT_SYMBOL
int hb_machine_has_wired_zones(hb_machine_type_t type);

// 主机最小有线防区号
ADEMCO_EXPORT_SYMBOL
ademco_zone_t hb_wired_zone_min(hb_machine_type_t type);

// 主机最大有线防区号
ADEMCO_EXPORT_SYMBOL
ademco_zone_t hb_wired_zone_max(hb_machine_type_t type);

// 主机是否可以直接写入防区数据（无需对码）
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_write_zone(hb_machine_type_t type);
// 主机是否可以挂载分机
ADEMCO_EXPORT_SYMBOL
int hb_machine_can_link_sub_machine(hb_machine_type_t type);

// 根据防区属性判断是否支持失联报告
// 失联报告是主机侧实现的跟防区属性没关系，但是人为限制了只有以下属性的才可以设置
ADEMCO_EXPORT_SYMBOL
int hb_zone_can_report_lost(hb_zone_property_t zp);

///////////////////////////// timer functions /////////////////////////////////

ADEMCO_EXPORT_SYMBOL
void hb_machine_timer_init(hb_machine_timer_t* timer);

ADEMCO_EXPORT_SYMBOL
int hb_is_valid_time_point(hb_machine_time_point_t* tp);

ADEMCO_EXPORT_SYMBOL
void hb_time_point_to_greenwich(hb_machine_time_point_t* tp);

ADEMCO_EXPORT_SYMBOL
void hb_time_point_from_greenwich(hb_machine_time_point_t* tp);

ADEMCO_EXPORT_SYMBOL
int hb_is_valid_timer_one(hb_machine_timer_one_t* timer);

ADEMCO_EXPORT_SYMBOL
int hb_is_valid_machine_timer(hb_machine_timer_t* timer);

// 传输的是格林威治时间
ADEMCO_EXPORT_SYMBOL
void hb_machine_timer_to_greenwich(hb_machine_timer_t* timer);

ADEMCO_EXPORT_SYMBOL
void hb_machine_timer_from_greenwich(hb_machine_timer_t* timer);

///////////////////////////// helper functions ////////////////////////////////

ADEMCO_EXPORT_SYMBOL
ademco_event_t hb_machine_status_to_ademco_event(hb_machine_status_t status);

ADEMCO_EXPORT_SYMBOL
hb_machine_status_t hb_machine_status_from_ademco_event(ademco_event_t ademco_event);

ADEMCO_EXPORT_SYMBOL
ademco_event_t hb_machine_type_to_ademco_event(hb_machine_type_t type);

ADEMCO_EXPORT_SYMBOL
hb_machine_type_t hb_machine_type_from_ademco_event(ademco_event_t ademco_event);

ADEMCO_EXPORT_SYMBOL
ademco_event_t hb_zone_property_to_ademco_event(hb_zone_property_t zp);

ADEMCO_EXPORT_SYMBOL
const char* hb_machine_status_to_string(hb_machine_status_t status);

ADEMCO_EXPORT_SYMBOL
const char* hb_machine_type_to_string(hb_machine_type_t type);

ADEMCO_EXPORT_SYMBOL
const char* hb_zone_property_to_string(hb_zone_property_t zp);

ADEMCO_EXPORT_SYMBOL
const char* hb_com_req_to_string(hb_com_req_t type);

ADEMCO_EXPORT_SYMBOL
const char* hb_com_resp_to_string(hb_com_resp_t type);

#if ADEMCO_ENABLE_CHINESE
ADEMCO_EXPORT_SYMBOL
const char* hb_machine_status_to_string_chinese(hb_machine_status_t status);

ADEMCO_EXPORT_SYMBOL
const char* hb_machine_type_to_string_chinese(hb_machine_type_t type);

ADEMCO_EXPORT_SYMBOL
const char* hb_zone_property_to_string_chinese(hb_zone_property_t zp);

ADEMCO_EXPORT_SYMBOL
const char* hb_com_req_to_string_chinese(hb_com_req_t type);

ADEMCO_EXPORT_SYMBOL
const char* hb_com_resp_to_string_chinese(hb_com_resp_t type);
#endif  // ADEMCO_ENABLE_CHINESE

ADEMCO_EXPORT_SYMBOL
const char* hb_get_zone_format_str(hb_machine_type_t type);

///////////////////////////// COM functions ///////////////////////////////////

// 累加校验，计算data[0] ~ data[len-2]，校验和放在data[len-1]
ADEMCO_EXPORT_SYMBOL
void hb_sum(uint8_t* data, size_t len);

// 校验和是否正确, return 0 for incorrect, otherwise correct
ADEMCO_EXPORT_SYMBOL
int hb_chksum(const uint8_t* data, size_t len);

ADEMCO_EXPORT_SYMBOL
hb_com_req_t hb_com_parse_req(const uint8_t* buff, size_t len, hb_com_data_t* cd);

ADEMCO_EXPORT_SYMBOL
hb_com_req_t hb_com_parse_xdata_req(const ademco_xdata_t* xdata, hb_com_data_t* cd);

// parse COM response and return response type.
// If cd is non-NULL, valid data will be copied to cd
ADEMCO_EXPORT_SYMBOL
hb_com_resp_t hb_com_parse_resp(const uint8_t* buff, size_t len, hb_com_data_t* cd);

// parse COM response and return response type.
// If cd is non-NULL, valid data will be copied to cd
ADEMCO_EXPORT_SYMBOL
hb_com_resp_t hb_com_parse_xdata_resp(const ademco_xdata_t* xdata, hb_com_data_t* cd);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a0_get_machine_status(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a1_get_machine_zones(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a2_get_more_machine_zones(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a3_modify_machine_zone(hb_com_data_t* data,
                                            uint8_t zone,
                                            hb_zone_property_t prop,
                                            hb_com_req_a3_p3_t op);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a5_get_machine_timer(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_a7_set_machine_timer(hb_com_data_t* data, hb_machine_timer_t* timer);

// TODO
// ADEMCO_EXPORT_SYMBOL
// void hbComMakeReqA9(hb_com_data_t* data, hb_machine_status_t status, hb_machine_type_t type);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_aa_modify_machine_zone_lost_config(hb_com_data_t* data, uint8_t zone, int on);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_ac_get_machine_zone_lost_config(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_ad_get_more_machine_zone_lost_config(hb_com_data_t* data);

// p2 must be HB_COM_3SEC_ARM or HB_COM_3SEC_DISARM
ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_ae_set_3sec_machine_status(hb_com_data_t* data,
                                                hb_com_3sec_which_t p1,
                                                hb_com_3sec_status_t status);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_b0_get_3sec_machine_status(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_ra_read_acct(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_req_wa_write_acct(hb_com_data_t* data, const char* acct);

ADEMCO_EXPORT_SYMBOL
void hb_com_resp_a2_iter_init(hb_com_resp_a2_iter_t* iter, const hb_com_data_t* com);

ADEMCO_EXPORT_SYMBOL
hb_com_resp_a2_p1_t hb_com_resp_a2_iter_next(hb_com_resp_a2_iter_t* iter,
                                             hb_zone_and_property_t* zp);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a0_machine_status(hb_com_data_t* data,
                                        hb_machine_status_t status,
                                        hb_machine_type_t type);

// zones and props length is count
// if count is 0, p1 wil be set to HB_COM_RESP_A2_P1_NOMORE
ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a2_machine_zones(hb_com_data_t* data,
                                       size_t count,
                                       ademco_zone_t* zones,
                                       hb_zone_property_t* props,
                                       hb_com_resp_a2_p1_t p1);

// 学码开始，等待探测器无线信号中
ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a3_waiting_sensor_signal(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a4_modify_zone_result(hb_com_data_t* data,
                                            ademco_zone_t zone,
                                            hb_zone_property_t prop,
                                            hb_com_resp_a4_p3_t p3);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a6_machine_timer(hb_com_data_t* data, hb_machine_timer_t* timer);

ADEMCO_EXPORT_SYMBOL
void hb_com_resp_a7_to_timer(hb_com_data_t* data, hb_machine_timer_t* timer);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a7_set_machine_timer_result(hb_com_data_t* data);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_a8_reject(hb_com_data_t* data);

// TODO
// ADEMCO_EXPORT_SYMBOL void hbComMakeRespA9_(hb_com_data_t* data);
ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_ab_modify_machine_zone_lost_config_result(hb_com_data_t* data,
                                                                ademco_zone_t zone,
                                                                hb_com_resp_ab_p2_t p2);

// 返回所有失联开的防区
ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_ad_machine_zone_lost_config(hb_com_data_t* data,
                                                  hb_com_resp_ad_p1_t p1,
                                                  size_t count,
                                                  ademco_zone_t* zones,
                                                  hb_com_resp_ad_p2_t p2);

ADEMCO_EXPORT_SYMBOL
void hb_com_resp_ad_iter_init(hb_com_resp_ad_iter_t* iter, const hb_com_data_t* com);

ADEMCO_EXPORT_SYMBOL
hb_com_resp_ad_p2_t hb_com_resp_ad_iter_next(hb_com_resp_ad_iter_t* iter, ademco_zone_t* zone);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_af_set_3sec_machine_status_result(hb_com_data_t* data,
                                                        hb_com_3sec_which_t p1,
                                                        hb_com_3sec_status_t status);

ADEMCO_EXPORT_SYMBOL
void hb_com_make_resp_b1_3sec_machine_status(hb_com_data_t* data,
                                             hb_machine_status_t statusMachine,
                                             hb_machine_status_t statusSec1,
                                             hb_machine_status_t statusSec2,
                                             hb_machine_status_t statusSec3);

ADEMCO_EXPORT_SYMBOL
void hb_com_data_to_ademco_xdata(const hb_com_data_t* const data,
                                 ademco_xdata_t* xdata,
                                 ademco_xdata_length_format_t xlf,
                                 ademco_xdata_transform_t xtr);

#ifdef __cplusplus
}
#endif

#endif
