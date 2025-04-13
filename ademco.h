#ifndef __ADEMCO_H__
#define __ADEMCO_H__

#include "ademco_config.h"
#include "ademco_version.h"

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes
typedef uint32_t ademco_id_t;
typedef uint8_t ademco_gg_t;
typedef uint16_t ademco_zone_t;

#define ADEMCO_PACKET_ACCT_MIN_LEN 6                // #acct之acct最小长度
#define ADEMCO_PACKET_ACCT_MAX_LEN 18               // #acct之acct最大长度
#define ADEMCO_PACKET_ACCT_MAC_LEN 12               // WiFi主机账号使用MAC地址
#define ADEMCO_PACKET_PWD_MIN_LEN 6                 // 主机密码最小长度
#define ADEMCO_PACKET_PWD_MAX_LEN 8                 // 主机密码最大长度
#define ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN 2      // 空data段[]长度
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN 21      // 非空data段长度，acct长度6
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX 32  // 非空data段长度，acct长度未知
#define ADEMCO_PACKET_MAX_LEN 256                   // 安定宝协议最大长度，包括前缀、后缀、data段、xdata段
#define CONGWIN_FE100_PACKET_LEN 31                 // 丛文FE100协议长度
#define ADEMCO_PACKET_TIMESTAMP_LEN 20              // 时间戳长度
#define ADEMCO_PACKET_XDATA_MAX_LEN 128             // xdata段最大长度

// 安定宝ID范围
#define ADEMCO_ID_INVALID 0
#define ADEMCO_ID_MIN 1
#define ADEMCO_ID_MAX 999999  // 兼容性考虑，最大安定宝 ID 为 0x0F423F
#define ADEMCO_ID_SENTINEL (ADEMCO_ID_MAX + 1)
#define ADEMCO_ID_MASK 0xFFFFFF
#define ademco_is_valid_ademco_id(ademco_id) \
    ((ADEMCO_ID_MIN <= (ademco_id)) && ((ademco_id) <= ADEMCO_ID_MAX))

// 防区号为0时表示主机自身
#define ADEMCO_ZONE_FOR_MACHINE_SELF 0
// 主机防区号范围
#define ADEMCO_ZONE_MIN 1
// 对任何主机类型，最大的防区号
#define ADEMCO_ZONE_MAX 999
#define ADEMCO_ZONE_SENTINEL (ADEMCO_ZONE_MAX + 1)
#define ADEMCO_ZONE_MASK 0xFFFF

// 对任何主机类型，防区号是否合法（可以包含0防区）
#define ademco_is_valid_zone(zone) \
    ((ADEMCO_ZONE_FOR_MACHINE_SELF <= (zone)) && ((zone) <= ADEMCO_ZONE_MAX))

// 对任何主机类型，防区号是否合法（不可以包含0防区）
#define ademco_is_valid_zone_strict(zone) \
    ((ADEMCO_ZONE_MIN <= (zone)) && ((zone) <= ADEMCO_ZONE_MAX))

#define ADEMCO_GG_MIN 0
#define ADEMCO_GG_MAX 99
#define ADEMCO_GG_SENTINEL 100
#define ADEMCO_GG_MASK 0xFF

// 信号强度范围 [0, 31]
#define ADEMCO_SIGNAL_STRENGTH_MIN 0
#define ADEMCO_SIGNAL_STRENGTH_MAX 31

// 主机状态
#define ADEMCO_STATUS_EVENTS_MAP(XX) \
    XX(3400, ARM_AWAY, "离家布防")   \
    XX(1400, DISARM, "撤防")         \
    XX(3456, ARM_STAY, "留守布防")   \
    XX(1456, ARM_STAY_1456, "留守布防")

// 防区报警
#define ADEMCO_ALARM_EVENTS_MAP(XX)   \
    XX(1120, EMERGENCY, "紧急报警")   \
    XX(1130, BURGLAR, "盗警")         \
    XX(1134, DOOR_RING, "门铃")       \
    XX(1110, FIRE, "火警")            \
    XX(1121, DURESS, "胁迫")          \
    XX(1151, GAS, "煤气")             \
    XX(1113, WATER, "水警")           \
    XX(1137, TAMPER, "主机防拆")      \
    XX(1383, ZONE_TAMPER, "防区防拆") \
    XX(1570, BYPASS, "旁路")

// 防区报警恢复
#define ADEMCO_ALARM_RECOVER_EVENTS_MAP(XX)       \
    XX(3120, EMERGENCY_RECOVER, "紧急恢复")       \
    XX(3130, BURGLAR_RECOVER, "盗警恢复")         \
    XX(3134, DOOR_RING_RECOVER, "门铃恢复")       \
    XX(3110, FIRE_RECOVER, "火警恢复")            \
    XX(3121, DURESS_RECOVER, "胁迫恢复")          \
    XX(3151, GAS_RECOVER, "煤气恢复")             \
    XX(3113, WATER_RECOVER, "水警恢复")           \
    XX(3137, TAMPER_RECOVER, "主机防拆恢复")      \
    XX(3383, ZONE_TAMPER_RECOVER, "防区防拆恢复") \
    XX(3570, BYPASS_RECOVER, "解除旁路")

// 防区异常
#define ADEMCO_EXEPTION_EVENTS_MAP(XX)    \
    XX(1301, AC_BROKEN, "主机AC掉电")     \
    XX(1302, LOW_BATTERY, "低电")         \
    XX(1311, BAD_BATTERY, "坏电")         \
    XX(1387, SOLAR_DISTURB, "光扰")       \
    XX(1381, DISCONNECT, "失效")          \
    XX(1393, LOST, "失联")                \
    XX(1384, POWER_EXCEPTION, "电源故障") \
    XX(1380, OTHER_EXCEPTION, "其他故障")

// 防区异常恢复
#define ADEMCO_EXEPTION_RECOVER_EVENTS_MAP(XX)        \
    XX(3301, AC_RECOVER, "主机AC复电")                \
    XX(3302, LOW_BATTERY_RECOVER, "低电恢复")         \
    XX(3311, BAD_BATTERY_RECOVER, "坏电恢复")         \
    XX(3387, SOLAR_DISTURB_RECOVER, "光扰恢复")       \
    XX(3381, DISCONNECT_RECOVER, "失效恢复")          \
    XX(3393, LOST_RECOVER, "失联恢复")                \
    XX(3384, POWER_EXCEPTION_RECOVER, "电源故障恢复") \
    XX(3380, OTHER_EXCEPTION_RECOVER, "其他故障恢复") \
    XX(3100, CLEAR_EXCPTION, "清除异常指示")

// 恒博自定义安定宝事件码
#define ADEMCO_HB_EVENTS_MAP(XX)                               \
    XX(1485, SERIAL_485_DIS, "485断开")                        \
    XX(3485, SERIAL_485_RECOVER, "485连接")                    \
    XX(1700, CONN_HANGUP, "链路挂起")                          \
    XX(3700, CONN_RECOVER, "链路恢复")                         \
    XX(1701, DISARM_PWD_ERR, "撤防密码错误")                   \
    XX(1702, SUB_MACHINE_SENSOR_EXCEPTION, "分机探头异常")     \
    XX(3702, SUB_MACHINE_SENSOR_RECOVER, "分机探头恢复")       \
    XX(1703, SUB_MACHINE_POWER_EXCEPTION, "分机电源异常")      \
    XX(3703, SUB_MACHINE_POWER_RECOVER, "分机电源恢复")        \
    XX(1704, COM_PASSTHROUGH, "串口透传")                      \
    XX(2704, ENTER_SET_MODE, "进入设置状态")                   \
    XX(3704, EXIT_SET_MODE, "退出设置状态")                    \
    XX(1705, QUERY_SUB_MACHINE, "查询分机信息")                \
    XX(1706, WRITE_TO_MACHINE, "写入主机信息")                 \
    XX(1707, I_AM_NET_MODULE, "主机类型--网络模块")            \
    XX(1717, I_AM_GPRS, "主机类型--GPRS主机")                  \
    XX(1727, I_AM_LCD, "主机类型--液晶主机")                   \
    XX(1737, I_AM_WIRE, "主机类型--网线主机")                  \
    XX(1747, I_AM_WIFI, "主机类型--WiFi主机(停用)")            \
    XX(1757, I_AM_3_SECTION, "主机类型--三区段主机")           \
    XX(1767, I_AM_IOT, "主机类型--物联卡主机")                 \
    XX(1777, I_AM_TRUE_COLOR, "主机类型--真彩主机")            \
    XX(1787, I_AM_GPRS_IOT, "主机类型--物联卡主机")            \
    XX(1797, I_AM_GPRS_PHONE, "主机类型--GRPS主机带电话功能")  \
    XX(1807, I_AM_NB, "主机类型--NB报警接收主机")              \
    XX(1817, I_AM_WIFI2, "主机类型--WiFi主机(新版)")           \
    XX(1709, PHONE_USER_SOS, "手机用户SOS")                    \
    XX(1711, PHONE_USER_CANCLE_ALARM, "手机用户消警")          \
    XX(1712, ENTER_SETTING_MODE, "主机进入设置状态")           \
    XX(3712, EXIT_SETTING_MODE, "主机退出设置状态")            \
    XX(1710, RESTORE_FACTORY_SETTINGS_710, "主机恢复出厂设置") \
    XX(1713, RESTORE_FACTORY_SETTINGS, "主机恢复出厂设置")     \
    XX(1756, SIM_IS_IOT_CARD, "物联卡")                        \
    XX(2756, SIM_IS_IOT_PLATFORM_CARD, "平台物联卡")           \
    XX(3756, SIM_IS_NOT_IOT_CARD, "非物联卡")                  \
    XX(1796, SYS_READY, "主机稳定")                            \
    XX(1798, WHAT_IS_YOUR_TYPE, "索要主机类型")                \
    XX(1799, SIGNAL_STRENGTH_CHANGED, "信号强度")              \
    XX(1944, OFFLINE, "主机断线")                              \
    XX(1946, ONLINE, "主机上线")

// Ademco events
typedef enum ademco_event_t {
    EVENT_INVALID_EVENT = 0,

#define XX(code, name, zh) EVENT_##name = code,

    // ----标准安定宝协议事件----
    // 主机状态
    ADEMCO_STATUS_EVENTS_MAP(XX)
    // 防区报警
    ADEMCO_ALARM_EVENTS_MAP(XX)
    // 防区报警恢复
    ADEMCO_ALARM_RECOVER_EVENTS_MAP(XX)
    // 防区异常
    ADEMCO_EXEPTION_EVENTS_MAP(XX)
    // 防区异常恢复
    ADEMCO_EXEPTION_RECOVER_EVENTS_MAP(XX)

    // ---私有事件---------
    ADEMCO_HB_EVENTS_MAP(XX)
#undef XX

#define ADEMCO_EVENT_SENTINEL 10000

} ademco_event_t;

// 安定宝事件级别
typedef enum ademco_event_level_t {
    EVENT_LEVEL_NULL,
    EVENT_LEVEL_STATUS,             // 主机状态
    EVENT_LEVEL_EXCEPTION_RECOVER,  // 黄色报警
    EVENT_LEVEL_EXCEPTION,          // 橙色报警
    EVENT_LEVEL_ALARM,              // 红色报警
} ademco_event_level_t;

// 安定宝协议解析结果
typedef enum ademco_parse_result_t {
    ADEMCO_PARSE_RESULT_OK,          // 解析到正确、完整的一包
    ADEMCO_PARSE_RESULT_NOT_ENOUGH,  // 解析到正确、不完整的一包，需要缓存数据，下次继续处理
    ADEMCO_PARSE_RESULT_ERROR,       // 解析到错误的数据，应当丢弃该数据包，丢弃长度为 *ate
} ademco_parse_result_t;

// 安定宝协议解析错误信息
typedef struct ademco_parse_error_t {
    const char* file;  // user do not free
    int line;          // line number in ademco.c
    size_t offset;     // error offset
    const char* msg;   // user do not free
} ademco_parse_error_t;

// 安定宝协议data段
typedef struct ademco_data_t {
    ademco_char_t raw[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX];
    size_t raw_len;
    ademco_id_t ademco_id;
    ademco_event_t ademco_event;
    ademco_gg_t gg;
    ademco_zone_t zone;
} ademco_data_t;

typedef enum ademco_xdata_length_format_t {
    ADEMCO_XDATA_LENGTH_FMT_TWO_HEX,       // 两个hex字节表示长度
    ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL,  // 四个decimal char 表示长度
} ademco_xdata_length_format_t;

typedef enum ademco_xdata_transform_t {
    ADEMCO_XDATA_TRANSFORM_AS_IS,     // 原样打包
    ADEMCO_XDATA_TRANSFORM_TO_ASCII,  // 转为ascii打包，如 0xEB 0xBA 打包为 "EBBA"
} ademco_xdata_transform_t;

typedef struct ademco_xdata_t {
    ademco_xdata_length_format_t lenghFormat;
    ademco_char_t raw[ADEMCO_PACKET_XDATA_MAX_LEN];
    size_t raw_len;
} ademco_xdata_t;

#define ADEMCO_PACKET_ID_NULL "\"NULL\""
#define ADEMCO_PACKET_ID_ACK "\"ACK\""
#define ADEMCO_PACKET_ID_NAK "\"NAK\""
#define ADEMCO_PACKET_ID_DUH "\"DUH\""
#define ADEMCO_PACKET_ID_HB "\"HENG-BO\""
#define ADEMCO_PACKET_ID_ADM_CID "\"ADM-CID\""
#define ADEMCO_PACKET_ID_MOD_REG "\"MODULE_REG\""
#define ADEMCO_PACKET_ID_REG_RSP "\"REG-RSP\""

typedef enum ademco_packet_id_t {
    AID_NULL,
    AID_ACK,
    AID_NAK,
    AID_DUH,
    AID_HB,
    AID_ADM_CID,
    AID_MOD_REG,
    AID_REG_RSP,
    AID_COUNT,
    AID_INVALID = -1,
} ademco_packet_id_t;

#define ADEMCO_RRCVR_DEFAULT "R123ABC"
#define ADEMCO_LPREF_DEFAULT "L456DEF"

#define ADEMCO_PACKET_PREFIX '\n'
#define ADEMCO_PACKET_SUFIX '\r'

typedef struct ademco_packet_t {
    uint16_t crc;
    size_t len;
    ademco_packet_id_t id;
    uint16_t seq;
    // +1 for '\0'
    char acct[ADEMCO_PACKET_ACCT_MAX_LEN + 1];
    ademco_data_t dat;
    ademco_xdata_t xdat;
    time_t timestamp;

    ademco_char_t raw[ADEMCO_PACKET_MAX_LEN];
    size_t raw_len;
} ademco_packet_t;

/* control source defs
 * 范围 0-255
 * 0: 主机(布撤防来源目前只有键盘，紧急报警可以是键盘，也可以是遥控器)
 * 1-10: 遥控器
 * 11-50: 保留
 * 51-96: 智能家居
 * 97: 定时器自动控制
 * 98: 中转接警中心
 * 99: 直连接警中心
 * 100-199: 手机APP，末二位为手机尾号
 * 200: web用户，web端包括 网页，H5, 公众号，小程序等
 * 201-255: web分享用户
 * 特别注意：三区段主机，0 代表主机，1-255 都是遥控器
 */
#define ADEMCO_CONTROL_SOURCES_MAP(XX)                \
    XX(0, MACHINE, "主机")                            \
    XX(1, REMOTE, "遥控器")                           \
    XX(10, REMOTE_MAX, "遥控器MAX")                   \
    XX(51, SMART_HOME_ALEXA, "Amazon Alexa")          \
    XX(52, SMART_HOME_GOOGLE, "Google Home")          \
    XX(54, SMART_HOME_APPLE, "Apple Homekit")         \
    XX(55, SMART_HOME_SAMSUNG, "Samsung Smartthings") \
    XX(56, SMART_HOME_ALI_GENIE, "阿里天猫精灵")      \
    XX(57, SMART_HOME_MI_AI, "小米小爱同学")          \
    XX(58, SMART_HOME_BAIDU, "百度小度")              \
    XX(96, SMART_HOME_MAX, "智能家居MAX")             \
    XX(97, TIMER, "定时器")                           \
    XX(98, CENTER_TRANSMIT, "中转接警中心")           \
    XX(99, CENTER_DIRECT, "直连接警中心")             \
    XX(100, PHONE_APP, "手机APP")                     \
    XX(199, PHONE_APP_MAX, "手机APP_MAX")             \
    XX(200, OWNER, "web用户")                         \
    XX(201, SHAREE, "web分享用户")                    \
    XX(255, SHAREE_MAX, "web分享用户MAX")

typedef enum ademco_control_source_t {
#define XX(code, name, zh) ADEMCO_CONTROL_SOURCE_##name = code,
    ADEMCO_CONTROL_SOURCES_MAP(XX)
#undef XX
} ademco_control_source_t;

//////////////////// bcd ////////////////////

#define bcd_encode(x) (((x) / 10) << 4 | ((x) % 10))
#define bcd_decode(x) (((x) >> 4) * 10 + ((x) & 0x0F))

/////////////////// reverse //////////////////

#define rev16(u) (uint16_t)((((uint16_t)(u) & 0xFF) << 8) | (((uint16_t)(u) & 0xFF00) >> 8))
#define rev32(u) (uint32_t)((((uint32_t)(u) & 0xFF) << 24) |    \
                            (((uint32_t)(u) & 0xFF00) << 8) |   \
                            (((uint32_t)(u) & 0xFF0000) >> 8) | \
                            (((uint32_t)(u) & 0xFF000000) >> 24))

//////////////////////// Functions ////////////////////////

ADEMCO_EXPORT_SYMBOL
uint32_t ademco_version(void);

#if ADEMCO_ENABLE_TO_STRING
ADEMCO_EXPORT_SYMBOL
const char* ademco_version_string(void);
#endif

ADEMCO_EXPORT_SYMBOL
void ademco_print(const ademco_char_t* buf, size_t len);

ADEMCO_EXPORT_SYMBOL
uint8_t ademco_encode_signal_strength(uint8_t strength);

ADEMCO_EXPORT_SYMBOL
uint8_t ademco_decode_signal_strength(uint8_t bcd_strength);

// 是否合法主机账号
ADEMCO_EXPORT_SYMBOL
bool ademco_is_valid_account(const char* acct);

// 是否合法主机密码
ADEMCO_EXPORT_SYMBOL
bool ademco_is_valid_password(const char* pwd);

//////////////////////// ademco_event_t functions ////////////////////////

// 是否主机状态事件
ADEMCO_EXPORT_SYMBOL
bool ademco_is_machine_status_event(ademco_event_t ademco_event);

// 是否主机类型事件
ADEMCO_EXPORT_SYMBOL
bool ademco_is_machine_type_event(ademco_event_t ademco_event);

// 事件是否需要控制源
ADEMCO_EXPORT_SYMBOL
bool ademco_is_event_need_control_source(ademco_event_t ademco_event);

// 获取安定宝事件级别
ADEMCO_EXPORT_SYMBOL
ademco_event_level_t ademco_get_event_level(ademco_event_t ademco_event);

// 获取异常恢复事件所对应的异常事件
ADEMCO_EXPORT_SYMBOL
ademco_event_t ademco_get_exception_event_by_recover_event(ademco_event_t recover_event);

#if ADEMCO_ENABLE_TO_STRING
ADEMCO_EXPORT_SYMBOL
const char* ademco_event_to_string(ademco_event_t ademco_event);
#endif

#if ADEMCO_ENABLE_CHINESE
ADEMCO_EXPORT_SYMBOL
const char* ademco_event_to_string_chinese(ademco_event_t ademco_event);
#endif

//////////////////////// ademco_data_t functions ////////////////////////

// make a `DATA` packet and store to `packet`, return length
ADEMCO_EXPORT_SYMBOL
size_t ademco_append_data(ademco_char_t* packet,
                          const char* acct,
                          ademco_id_t ademco_id,
                          ademco_event_t ademco_event,
                          ademco_gg_t gg,
                          ademco_zone_t zone);

// make a `DATA` packet and store to `ademco_data`, return length
ADEMCO_EXPORT_SYMBOL
size_t ademco_append_data2(ademco_data_t* ademco_data,
                           const char* acct,
                           ademco_id_t ademco_id,
                           ademco_event_t ademco_event,
                           ademco_gg_t gg,
                           ademco_zone_t zone);

// inplace make a `DATA` packet and store to `ademco_data`, return length
ADEMCO_EXPORT_SYMBOL
size_t ademco_append_data3(ademco_data_t* ademco_data, const char* acct);

// parse `DATA` packet, if ok, `ademco_data`'s members will be useful
// err can be NULL; caller should only check err on got ADEMCO_PARSE_RESULT_ERROR
ADEMCO_EXPORT_SYMBOL
ademco_parse_result_t ademco_parse_data(const ademco_char_t* packet,
                                        size_t packet_len,
                                        ademco_data_t* ademco_data,
                                        ademco_parse_error_t* err);

// return 0 for empty packet, CONGWIN_FE100_PACKET_LEN for success
ADEMCO_EXPORT_SYMBOL
size_t ademco_data_to_congwin_fe100(ademco_char_t* fe100,
                                    size_t fe100_len,
                                    const ademco_data_t* ademco_data);

//////////////////////// ademco_xdata_t functions ////////////////////////

// init a empty xdat
ADEMCO_EXPORT_SYMBOL
void ademco_xdata_init(ademco_xdata_t* xdat);

ADEMCO_EXPORT_SYMBOL
bool ademco_xdata_convert(ademco_xdata_t* xdat, ademco_xdata_length_format_t xlf);

#ifndef SWIG
// get valid content address of xdat (except [len])
ADEMCO_EXPORT_SYMBOL
const char* ademco_xdata_get_valid_content_addr(const ademco_xdata_t* xdat);
#endif

// get valid content length of xdat (except [len])
ADEMCO_EXPORT_SYMBOL
size_t ademco_xdata_get_valid_content_len(const ademco_xdata_t* xdat);

// return 0 if xdat's valid content is exactly the same as [buf, buf+buf_len)
ADEMCO_EXPORT_SYMBOL
bool ademco_xdata_memeq(const ademco_xdata_t* xdat,
                        const void* buf,
                        size_t buf_len);

// copy xdat content from src to dst, return copied length = src.raw_len
ADEMCO_EXPORT_SYMBOL
size_t ademco_xdata_copy(ademco_xdata_t* dst,
                         const ademco_xdata_t* src);

ADEMCO_EXPORT_SYMBOL
bool ademco_make_xdata(ademco_xdata_t* xdat,
                       ademco_xdata_length_format_t xlf,
                       ademco_xdata_transform_t xtr,
                       const ademco_char_t* content,
                       size_t len);

//////////////////////// ademco_packet_id_t functions ////////////////////////

ADEMCO_EXPORT_SYMBOL
bool ademco_is_valid_packet_id(const char* standard,
                               const char* id,
                               size_t len);

ADEMCO_EXPORT_SYMBOL
ademco_packet_id_t ademco_packet_id_from_string(const char* id,
                                                size_t len);

ADEMCO_EXPORT_SYMBOL
const char* ademco_packet_id_to_string(ademco_packet_id_t id);

/*
 * ademcoMake*Packet functions
 * if buf != NULL and len >= length needed, return length used and copy dat to buf
 * otherwise return 0
 */

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_empty_data_packet(ademco_char_t* dst_buff,
                                     size_t len,
                                     const char* id,
                                     uint16_t seq,
                                     const char* acct,
                                     ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_adm_empty_data_packet(ademco_char_t* dst_buff,
                                         size_t len,
                                         const char* id,
                                         uint16_t seq,
                                         const char* acct,
                                         ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_null_packet(ademco_char_t* buf,
                               size_t len,
                               uint16_t seq,
                               const char* acct,
                               ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_ack_packet(ademco_char_t* buf,
                              size_t len,
                              uint16_t seq,
                              const char* acct,
                              ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_nak_packet(ademco_char_t* buf,
                              size_t len,
                              uint16_t seq,
                              const char* acct,
                              ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_hb_packet(ademco_char_t* buf,
                             size_t len,
                             uint16_t seq,
                             const char* acct,
                             ademco_id_t ademco_id,
                             ademco_event_t ademco_event,
                             ademco_gg_t gg,
                             ademco_zone_t zone,
                             const ademco_xdata_t* xdat);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_adm_packet(ademco_char_t* buf,
                              size_t len,
                              uint16_t seq,
                              const char* acct,
                              ademco_id_t ademco_id,
                              ademco_event_t ademco_event,
                              ademco_gg_t gg,
                              ademco_zone_t zone,
                              const ademco_xdata_t* xdat);

// like upper funcs, store buf and len to pkt->raw, pkt->raw_len
ADEMCO_EXPORT_SYMBOL
size_t ademco_make_null_packet2(ademco_packet_t* pkt,
                                uint16_t seq,
                                const char* acct,
                                ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_ack_packet2(ademco_packet_t* pkt,
                               uint16_t seq,
                               const char* acct,
                               ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_nak_packet2(ademco_packet_t* pkt,
                               uint16_t seq,
                               const char* acct,
                               ademco_id_t ademco_id);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_hb_packet2(ademco_packet_t* pkt,
                              uint16_t seq,
                              const char* acct,
                              ademco_id_t ademco_id,
                              ademco_event_t ademco_event,
                              ademco_gg_t gg,
                              ademco_zone_t zone,
                              const ademco_xdata_t* xdat);

// inplace make from pkt and to pkt
ADEMCO_EXPORT_SYMBOL
size_t ademco_make_hb_packet3(ademco_packet_t* pkt);

ADEMCO_EXPORT_SYMBOL
size_t ademco_make_adm_packet2(ademco_packet_t* pkt,
                               uint16_t seq,
                               const char* acct,
                               ademco_id_t ademco_id,
                               ademco_event_t ademco_event,
                               ademco_gg_t gg,
                               ademco_zone_t zone,
                               const ademco_xdata_t* xdat);

// parse a ademco packet, if everything is OK, cb_commited is the packet length
// err can be NULL; caller should only check err on got ADEMCO_PARSE_RESULT_ERROR
ADEMCO_EXPORT_SYMBOL
ademco_parse_result_t ademco_parse_packet(const ademco_char_t* buf,
                                          size_t len,
                                          ademco_packet_t* pkt,
                                          size_t* cb_commited,
                                          ademco_parse_error_t* err);
#ifndef ADEMCO_USE_EXTERNAL_CRC16
/* CRC16 implementation according to ARC
 * Name                       : CRC-16/ARC
 * Alias					  : ARC, CRC-16, CRC-16/LHA, CRC-IBM
 * Width                      : 16 bit
 * Poly                       : 0x8005 (That is actually x^16 + x^15 + x^2 + 1)
 * Initialization             : 0x0000
 * Reflect Input byte         : True
 * Reflect Output CRC         : True
 * Xor constant to output CRC : 0x0000
 * Output for "123456789"     : 0xBB3D
 */
ADEMCO_EXPORT_SYMBOL
uint16_t ademco_crc16(const ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* buf, size_t len);
#endif

// 0-9, A-F to '0'-'9', 'A'-'F'
ADEMCO_EXPORT_SYMBOL uint8_t ademco_hex2char(uint8_t h);

// '0'-'9', 'A'-'F', 'a'-'f' to 0-9, A-F
ADEMCO_EXPORT_SYMBOL uint8_t ademco_char2hex(uint8_t c);

// like snprintf(str, hex_len * 2 + 1, "%0`hex_len`X", dec)
// caller should make sure `str` has enough space
// e.g. ademco_dec_to_hex_str(buf, 0x12345678, 3) => buf = "678"
ADEMCO_EXPORT_SYMBOL size_t ademco_dec_to_hex_str(char* str, uint32_t dec, uint8_t hex_len);

// 将一串以高低字节表示的十六进制数组转换为10进制数字符串
// 每个字节的高四位和低四位若不大于9，将该四位表示的数字以10进制ascii码填入str，否则停止
// 返回字符串长度
// 注意：函数不会在str末尾补 null terminator
// 调用者应确保str有足够空间，至少len的2倍，否则会崩溃
// 示例：输入数组：0x18 0x24 0x08 0x88 0x10 0x1f 0xff，输出字符串"18240888101"
ADEMCO_EXPORT_SYMBOL
size_t ademco_hilo_array_to_dec_str(ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* str,
                                    const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len);

// 将一串以高低字节表示的十六进制数组转换为字符串
// 每个字节的高四位和低四位若不大于9，将该四位表示的数字以10进制ascii码填入str，否则：
// 若定长6字节包含a-f，说明是WiFi主机
// 返回字符串长度
// 注意：函数不会在str末尾补 null terminator
// 调用者应确保str有足够空间，至少len的2倍，否则会崩溃
// 示例：输入数组：0x18 0x24 0x08 0x88 0x10 0x1f 0xff，输出字符串"18240888101"
// 示例：输入数组：0x24 0x94 0x94 0x4c 0x48 0x15 0xff，输出字符串"2494944C4815"
ADEMCO_EXPORT_SYMBOL
size_t ademco_hilo_array_to_hex_str(ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* str,
                                    const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len);

// 将一个10进制数字符串转为高低字节表示的数组，节省空间
// str应只包含'0'-'9'，否则失败返回0
// str长度若大于len * 2，str[len]及之后的内容将被舍弃以避免溢出
// 转换后的长度若不满len，则以0xF补满。
// 示例：输入字符串"123ABC"，返回0
// 示例：输入字符串 "18240888101", len=9, 则arr内容为
// 0x18 0x24 0x08 0x88 0x10 0x1f 0xff 0xff 0xff
// return 9
// 示例：输入字符串 "12345678901234567890", len=9, 则arr内容为
// 0x12 0x34 0x56 0x78 0x90 0x12 0x34 0x56 0x78
// return 9
// 示例：输入NULL，返回 9个 0xFF
ADEMCO_EXPORT_SYMBOL
size_t ademco_dec_str_to_hilo_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len,
                                    const char ADEMCO_BUF_MEMORY_MODIFIER* str);

// 将一个16进制数字符串转为高低字节表示的数组，节省空间
// str应只包含'0'-'9', 'A'-'F', 'a'-'f'，否则失败返回0
// str长度若大于len * 2，str[len]及之后的内容将被舍弃以避免溢出
// 转换后的长度若不满len，则以0xF补满。
// 示例：输入字符串"123ABC"，len=6, 则 arr 内容为 0x12 0x3a 0xbc 0xff 0xff 0xff
// 示例：输入字符串 "18240888101", len=9, 则arr内容为
// 0x18 0x24 0x08 0x88 0x10 0x1f 0xff 0xff 0xff
// return 9
// 示例：输入字符串 "12345678901234567890", len=9, 则arr内容为
// 0x12 0x34 0x56 0x78 0x90 0x12 0x34 0x56 0x78
// return 9
// 示例：输入NULL，返回 9个 0xFF
ADEMCO_EXPORT_SYMBOL
size_t ademco_hex_str_to_hilo_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len,
                                    const char ADEMCO_BUF_MEMORY_MODIFIER* str);

// 将一串字节流转换为可打印形式
// 返回str长度（len * 2)
// 调用者应确保str有足够的容量，至少len*2
// 注意：函数不会在str末尾补 null terminator
// 示例：输入 arr=[0xEB 0xBA 0x3F], len=3, 则str = "EBBA3F"，返回6
// 示例：输入 ary="123", len=3, 则str = "313233"，返回6
ADEMCO_EXPORT_SYMBOL
size_t ademco_hex_array_to_str(char ADEMCO_BUF_MEMORY_MODIFIER* str,
                               const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                               size_t len);

// 将一串字符串（内容为'0'-'9', 'A'-'F', 'a'-'f')转为字节流
// 若strlen(str)为奇数，以padding补足arr, padding 应 <= 0x0F
// 若str内包含非hex字符串，返回0
// 调用者应确保arr有足够的容量，至少strlen(str)*2
// 示例：输入字符串str="EBBA3F", 输出arr = [0xEB, 0xBA, 0x3F]，返回 3
// 示例：输入字符串str="ABC", padding = 0, 输出arr=[0xAB, 0xC0]，返回2
// 示例：输入字符串str="ABC", padding = 0x0F, 输出arr=[0xAB, 0xCF]，返回2
ADEMCO_EXPORT_SYMBOL
size_t ademco_hex_str_to_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                               const char ADEMCO_BUF_MEMORY_MODIFIER* str,
                               uint8_t padding);

// 功能同ademcoHexStrToArray
// 若strlen(str) > len, str[len]及之后的内容将被舍弃以避免溢出
// 示例：输入字符串str="ABCDE", len = 4 padding = 0x0F, 输出arr=[0xAB, 0xCD], return 2
ADEMCO_EXPORT_SYMBOL
size_t ademco_hex_str_to_array_n(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                 const char ADEMCO_BUF_MEMORY_MODIFIER* str,
                                 size_t len, uint8_t padding);

// 同ademcoHexStrToArrayN，但允许str包含非hex字符，
// 即'0'-'9','a'-'f','A'-'F'之外的内容，以 padding 替换
// 示例：str="AB\xFFD", len=4, padding=0x0F, 则 arr=[ 0xAB, 0xFD ], return 2
// 网线主机 0d 00 命令，接警中心账号部分，有可能是这种
ADEMCO_EXPORT_SYMBOL
size_t ademco_hex_str_to_array_n_allow_non_hex_str(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                                   const char ADEMCO_BUF_MEMORY_MODIFIER* str,
                                                   size_t len,
                                                   uint8_t padding);

#ifdef __cplusplus
}
#endif

#endif  // __ADEMCO_H__
