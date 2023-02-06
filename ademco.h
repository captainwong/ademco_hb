/*
* Ademco library
* captainwong
* 2022-6-13 rewrited this C version
*/

#ifndef __ADEMCO_H__
#define __ADEMCO_H__

#pragma once


#ifdef ADEMCO_EXPORTS

# if defined __GNUC__
#  define ADEMCO_EXPORT_SYMBOL __attribute__ ((visibility("default")))
# elif defined(_MSC_VER)
#  define ADEMCO_EXPORT_SYMBOL __declspec(dllexport)
# else
#  define ADEMCO_EXPORT_SYMBOL 
# endif

#else // ADEMCO_EXPORTS

# define ADEMCO_EXPORT_SYMBOL 

#endif


#include <stdint.h>
#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif

#define ADEMCO_OK 0
#define ADEMCO_ERR -1

#define ADEMCO_PACKET_ACCT_MAX_LEN 64 // #acct之acct最大长度
#define ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN 2 // 空data段[]长度
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN 21 // 非空data段长度，acct长度6
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX 64 // 非空data段长度，acct长度未知
#define ADEMCO_PACKET_MAX_LEN 512
#define CONGWIN_FE100_PACKET_LEN 31 // 丛文FE100协议长度
#define ADEMCO_PACKET_TIMESTAMP_LEN 20
#define ADEMCO_PACKET_XDATA_MAX_LEN 128

// Prototypes
typedef uint32_t AdemcoId;
typedef uint32_t AdemcoGG;
typedef uint32_t AdemcoZone;

//! 安定宝ID范围
#define ADEMCO_ID_MIN 1
#define ADEMCO_ID_MAX 999999 // 兼容性考虑，最大安定宝 ID 为 0x0F423F
#define ADEMCO_ID_SENTINEL (ADEMCO_ID_MAX + 1)
static inline int ademcoIsValidAdemcoId(AdemcoId ademcoId) {
	return ADEMCO_ID_MIN <= ademcoId && ademcoId <= ADEMCO_ID_MAX; 
}

//! 防区号为0时表示主机自身
#define ADEMCO_ZONE_FOR_MACHINE_SELF 0
//! 主机防区号范围
#define ADEMCO_ZONE_MIN 1
//! 对任何主机类型，最大的防区号
#define ADEMCO_ZONE_MAX 999
#define ADEMCO_ZONE_SENTINEL (ADEMCO_ZONE_MAX + 1)

//! 对任何主机类型，防区号是否合法（可以包含0防区）
static inline int ademcoIsValidZone(AdemcoZone zone) {
	return ADEMCO_ZONE_FOR_MACHINE_SELF <= zone && zone <= ADEMCO_ZONE_MAX;
}

//! 对任何主机类型，防区号是否合法（不可以包含0防区）
static inline int ademcoIsValidZoneStrict(AdemcoZone zone) {
	return ADEMCO_ZONE_MIN <= zone && zone <= ADEMCO_ZONE_MAX;
}

#define ADEMCO_GG_MIN 0
#define ADEMCO_GG_MAX 99
#define ADEMCO_GG_SENTINEL 100

// Ademco events
typedef enum AdemcoEvent {
	EVENT_INVALID_EVENT							= 0,

	// -------------------标准安定宝协议事件--------------------------------
	// 主机或分机状态报告
	EVENT_ARM									= 3400, // 布防
	EVENT_DISARM								= 1400, // 撤防
	EVENT_HALFARM_1456							= 1456, // 半布防
	EVENT_HALFARM								= 3456, // 半布防
	EVENT_EMERGENCY								= 1120, // 紧急报警

	// 防区报警
	EVENT_BURGLAR								= 1130, // 盗警
	EVENT_DOORRINGING							= 1134, // 门铃
	EVENT_FIRE									= 1110, // 火警
	EVENT_DURESS								= 1121, // 胁迫
	EVENT_GAS									= 1151, // 煤气
	EVENT_WATER									= 1113, // 水警
	EVENT_TAMPER								= 1137, // 主机防拆
	EVENT_ZONE_TAMPER							= 1383, // 防区防拆
	EVENT_BYPASS								= 1570, // 旁路
	EVENT_BYPASS_RESUME							= 3570, // 解除旁路

	// 防区异常
	EVENT_AC_BROKE								= 1301, // 主机AC掉电
	EVENT_AC_RECOVER							= 3301, // 主机AC复电
	EVENT_LOWBATTERY							= 1302, // 低电
	EVENT_BATTERY_RECOVER						= 3302, // 复电
	EVENT_BADBATTERY							= 1311, // 坏电
	EVENT_BADBATTERY_RECOVER					= 3311, // 坏电恢复
	EVENT_SOLARDISTURB							= 1387, // 光扰
	EVENT_SOLARDISTURB_RECOVER					= 3387, // 光扰恢复
	EVENT_DISCONNECT							= 1381, // 失效
	EVENT_RECONNECT								= 3381, // 恢复
	EVENT_BATTERY_EXCEPTION						= 1384, // 电源故障
	EVENT_BATTERY_EXCEPTION_RECOVER				= 3384, // 电源故障恢复
	EVENT_OTHER_EXCEPTION						= 1380, // 其他故障
	EVENT_OTHER_EXCEPTION_RECOVER				= 3380, // 其他故障恢复
	EVENT_LOST									= 1393, // 失联
	EVENT_LOST_RECOVER							= 3393, // 失联恢复

	// 标准事件，但不做处理
	EVENT_3100									= 3100, // 液晶主机清除异常指示
	
	// ------------------私有事件-----------------------------------------
	EVENT_SERIAL485DIS							= 1485, // 网络模块与主机485串口连接断开
	EVENT_SERIAL485CONN							= 3485, // 网络模块与主机485串口连接恢复

	EVENT_CONN_HANGUP							= 1700, // 链路挂起
	EVENT_CONN_RESUME							= 3700, // 链路恢复

	EVENT_DISARM_PWD_ERR						= 1701, // 撤防密码错误

	EVENT_SUB_MACHINE_SENSOR_EXCEPTION			= 1702, // 分机探头异常
	EVENT_SUB_MACHINE_SENSOR_RESUME				= 3702, // 分机探头恢复
	EVENT_SUB_MACHINE_POWER_EXCEPTION			= 1703, // 分机电源异常
	EVENT_SUB_MACHINE_POWER_RESUME				= 3703, // 分机电源恢复

	EVENT_COM_PASSTHROUGH						= 1704, // 串口透传
	EVENT_ENTER_SET_MODE						= 2704, // 进入设置状态
	EVENT_EXIT_SET_MODE							= 3704,	// 接警中心发送，为停止索要；报警主机发送，为拒绝索要

	EVENT_QUERY_SUB_MACHINE						= 1705, // 查询分机信息
	EVENT_WRITE_TO_MACHINE						= 1706, // 写入主机信息

	EVENT_I_AM_NET_MODULE						= 1707, // 主机类型--网络模块
	EVENT_I_AM_GPRS								= 1717, // 主机类型--GPRS主机
	EVENT_I_AM_LCD_MACHINE						= 1727, // 主机类型--液晶主机
	EVENT_I_AM_WIRE_MACHINE						= 1737, // 主机类型--网线主机
	EVENT_I_AM_WIFI_MACHINE						= 1747, // 主机类型--WiFi主机
	EVENT_I_AM_3_SECTION_MACHINE				= 1757, // 主机类型--三区段主机
	EVENT_I_AM_IOT_MACHINE						= 1767, // 主机类型--物联卡主机
	EVENT_I_AM_TRUE_COLOR						= 1777, // 主机类型--真彩主机
	EVENT_I_AM_GPRS_IOT							= 1787, // 主机类型--物联卡主机
	EVENT_I_AM_GPRS_PHONE						= 1797, // 主机类型--GRPS主机带电话功能
	EVENT_I_AM_NB_MACHINE						= 1807, // 主机类型--NB报警接收主机

	EVENT_PHONE_USER_SOS						= 1709, // 手机用户SOS
	EVENT_PHONE_USER_CANCLE_ALARM				= 1711, // 手机用户消警

	EVENT_ENTER_SETTING_MODE					= 1712, // 主机进入设置状态
	EVENT_EXIT_SETTING_MODE						= 3712, // 主机退出设置状态
	EVENT_RESTORE_FACTORY_SETTINGS_710			= 1710, // 主机恢复出厂设置
	EVENT_RESTORE_FACTORY_SETTINGS				= 1713, // 主机恢复出厂设置

	// 此处电话设置仅为主机自身支持的电话设置，与阿里语音无关
	EVENT_SIM_IS_IOT_CARD						= 1756, // 主机SIM卡为物联卡，禁用主机侧电话设置
	EVENT_SIM_IS_IOT_PLATFORM_CARD				= 2756, // 主机SIM卡为平台物联卡，禁用主机侧电话设置
	EVENT_SIM_IS_NOT_IOT_CARD					= 3756, // 主机SIM卡为非物联卡，启用主机侧电话设置

	EVENT_WHAT_IS_YOUR_TYPE						= 1798, // 索要主机类型
	EVENT_SIGNAL_STRENGTH_CHANGED				= 1799, // 主机信号强度变化

	// 2021年1月24日17:06:55修改，对标丛文3B0 3B2
	EVENT_OFFLINE								= 1944, // 主机断线
	EVENT_ONLINE								= 1946, // 主机上线

#define ADEMCO_EVENT_SENTINEL					10000

}AdemcoEvent;

static const AdemcoEvent AdemcoEvents[] = {
	EVENT_ARM,
	EVENT_DISARM,
	EVENT_HALFARM,
	EVENT_EMERGENCY,

	EVENT_BURGLAR,
	EVENT_DOORRINGING,
	EVENT_FIRE,
	EVENT_DURESS,
	EVENT_GAS,
	EVENT_WATER,
	EVENT_TAMPER,
	EVENT_ZONE_TAMPER,
	EVENT_BYPASS,
	EVENT_BYPASS_RESUME,

	EVENT_AC_BROKE,
	EVENT_AC_RECOVER,
	EVENT_LOWBATTERY,
	EVENT_BATTERY_RECOVER,
	EVENT_BADBATTERY,
	EVENT_BADBATTERY_RECOVER,
	EVENT_SOLARDISTURB,
	EVENT_SOLARDISTURB_RECOVER,
	EVENT_DISCONNECT,
	EVENT_RECONNECT,
	EVENT_LOST,
	EVENT_LOST_RECOVER,
	EVENT_3100,
	EVENT_BATTERY_EXCEPTION,
	EVENT_BATTERY_EXCEPTION_RECOVER,
	EVENT_OTHER_EXCEPTION,
	EVENT_OTHER_EXCEPTION_RECOVER,

	EVENT_SERIAL485DIS,
	EVENT_SERIAL485CONN,
	EVENT_CONN_HANGUP,
	EVENT_CONN_RESUME,
	EVENT_DISARM_PWD_ERR,

	EVENT_SUB_MACHINE_SENSOR_EXCEPTION,
	EVENT_SUB_MACHINE_SENSOR_RESUME,
	EVENT_SUB_MACHINE_POWER_EXCEPTION,
	EVENT_SUB_MACHINE_POWER_RESUME,
	EVENT_COM_PASSTHROUGH,

	EVENT_ENTER_SET_MODE,
	EVENT_EXIT_SET_MODE,
	EVENT_QUERY_SUB_MACHINE,
	EVENT_WRITE_TO_MACHINE,

	EVENT_I_AM_NET_MODULE,
	EVENT_I_AM_GPRS,
	EVENT_I_AM_LCD_MACHINE,
	EVENT_I_AM_WIRE_MACHINE,
	EVENT_I_AM_WIFI_MACHINE,
	EVENT_I_AM_3_SECTION_MACHINE,
	EVENT_I_AM_IOT_MACHINE,
	EVENT_I_AM_TRUE_COLOR,
	EVENT_I_AM_GPRS_IOT,
	EVENT_I_AM_GPRS_PHONE,
	EVENT_I_AM_NB_MACHINE,

	EVENT_SIM_IS_IOT_CARD,
	EVENT_SIM_IS_IOT_PLATFORM_CARD,
	EVENT_SIM_IS_NOT_IOT_CARD,

	EVENT_PHONE_USER_SOS,
	EVENT_PHONE_USER_CANCLE_ALARM,
	EVENT_ENTER_SETTING_MODE,
	EVENT_EXIT_SETTING_MODE,
	EVENT_RESTORE_FACTORY_SETTINGS,

	EVENT_WHAT_IS_YOUR_TYPE,
	EVENT_SIGNAL_STRENGTH_CHANGED,
};

// 安定宝事件级别
typedef enum AdemcoEventLevel
{
	EVENT_LEVEL_NULL,
	EVENT_LEVEL_STATUS,				// 主机状态
	EVENT_LEVEL_EXCEPTION_RESUME,	// 黄色报警
	EVENT_LEVEL_EXCEPTION,			// 橙色报警
	EVENT_LEVEL_ALARM,				// 红色报警
}AdemcoEventLevel;

// 安定宝协议解析结果
typedef enum AdemcoParseResult {
	RESULT_OK, //! 解析到正确、完整的一包
	RESULT_NOT_ENOUGH, //! 解析到正确、不完整的一包，需要缓存数据，下次继续处理
	RESULT_ERROR, //! 解析到错误的数据，应当丢弃该数据包
}AdemcoParseResult;

// 安定宝协议data段
typedef struct AdemcoDataSegment {
	char raw[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX];
	size_t raw_len;
	AdemcoId ademcoId;
	AdemcoEvent ademcoEvent;
	AdemcoGG gg;
	AdemcoZone zone;
}AdemcoDataSegment;

typedef enum AdemcoXDataLengthFormat {
	TWO_HEX, // 两个hex字节表示长度
	FOUR_DECIMAL, // 四个decimal char 表示长度
}AdemcoXDataLengthFormat;

typedef enum AdemcoXDataTransform {
	AdemcoXDataTransform_as_is,		// 原样打包
	AdemcoXDataTransform_to_ascii,	// 转为ascii打包，如 0xEB 0xBA 打包为 "EBBA"
}AdemcoXDataTransform;

typedef struct AdemcoXDataSegment {
	AdemcoXDataLengthFormat lenghFormat;
	char raw[ADEMCO_PACKET_XDATA_MAX_LEN];
	size_t raw_len;
}AdemcoXDataSegment;

#define ADEMCO_PACKET_ID_NULL	 "\"NULL\""
#define ADEMCO_PACKET_ID_ACK	 "\"ACK\""
#define ADEMCO_PACKET_ID_NAK     "\"NAK\""
#define ADEMCO_PACKET_ID_DUH     "\"DUH\""
#define ADEMCO_PACKET_ID_HB      "\"HENG-BO\""
#define ADEMCO_PACKET_ID_ADM_CID "\"ADM-CID\""
#define ADEMCO_PACKET_ID_MOD_REG "\"MODULE_REG\""
#define ADEMCO_PACKET_ID_REG_RSP "\"REG-RSP\""

typedef enum AdemcoPacketId {
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
}AdemcoPacketId;

#define ADEMCO_RRCVR_DEFAULT "R123ABC"
#define ADEMCO_LPREF_DEFAULT "L456DEF"

#define ADEMCO_PACKET_PREFIX '\n'
#define ADEMCO_PACKET_SUFIX  '\r'

typedef struct AdemcoPacket {
	uint16_t crc;
	size_t len;
	AdemcoPacketId id;
	uint16_t seq;
	char acct[ADEMCO_PACKET_ACCT_MAX_LEN + 1];
	AdemcoDataSegment data;
	AdemcoXDataSegment xdata;
	time_t timestamp;

	char raw[ADEMCO_PACKET_MAX_LEN];
	size_t raw_len;
}AdemcoPacket;


//////////////////////// Functions ////////////////////////

//! print as HEX
ADEMCO_EXPORT_SYMBOL void ademcoPrint(const char* buff, size_t len);


//////////////////////// AdemcoEvent functions ////////////////////////

//! 是否主机状态事件
ADEMCO_EXPORT_SYMBOL int ademcoIsMachineStatusEvent(AdemcoEvent ademcoEvent);
//! 是否主机类型事件
ADEMCO_EXPORT_SYMBOL int ademcoIsMachineTypeEvent(AdemcoEvent ademcoEvent);
//! 事件是否需要控制源
ADEMCO_EXPORT_SYMBOL int ademcoIsEventNeedControlSource(AdemcoEvent ademcoEvent);
//! 获取安定宝事件级别
ADEMCO_EXPORT_SYMBOL AdemcoEventLevel ademcoGetEventLevel(AdemcoEvent ademcoEvent);
//! 获取异常恢复事件所对应的异常事件
ADEMCO_EXPORT_SYMBOL AdemcoEvent ademcoGetExceptionEventByResumeEvent(AdemcoEvent resumeEvent);
ADEMCO_EXPORT_SYMBOL const char* ademcoEventToString(AdemcoEvent ademcoEvent);
ADEMCO_EXPORT_SYMBOL const char* ademcoEventToStringChinese(AdemcoEvent ademcoEvent);


//////////////////////// AdemcoDataSegment functions ////////////////////////

//! make a `DATA` packet and store to `packet`, return length
ADEMCO_EXPORT_SYMBOL size_t ademcoAppendDataSegment(char* packet, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone);
//! make a `DATA` packet and store to `dataSegment`, return length
ADEMCO_EXPORT_SYMBOL size_t ademcoAppendDataSegment2(AdemcoDataSegment* dataSegment, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone);
//! parse `DATA` packet, if ok, `dataSegment`'s members will be useful
ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoParseDataSegment(const char* packet, size_t packet_len, AdemcoDataSegment* dataSegment);
// return 0 for empty packet, CONGWIN_FE100_PACKET_LEN for success
ADEMCO_EXPORT_SYMBOL size_t ademcoDataSegmentToCongwinFe100(char* fe100, const AdemcoDataSegment* dataSegment);


//////////////////////// AdemcoXDataSegment functions ////////////////////////

// init a empty xdata
ADEMCO_EXPORT_SYMBOL void ademcoXDataInit(AdemcoXDataSegment* xdata);
// return ADEMCO_OK for success, ADEMCO_ERR for failed
ADEMCO_EXPORT_SYMBOL int ademcoXDataConvert(AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf);
#ifndef SWIG
// get valid content address of xdata (except [len])
ADEMCO_EXPORT_SYMBOL const char* ademcoXDataGetValidContentAddr(const AdemcoXDataSegment* xdata);
#endif
// get valid content length of xdata (except [len])
ADEMCO_EXPORT_SYMBOL size_t ademcoXDataGetValidContentLen(const AdemcoXDataSegment* xdata);
// return 0 if xdata's valid content is exactly the same as [buf, buf+buf_len)
ADEMCO_EXPORT_SYMBOL int ademcoXDataMemcmp(const AdemcoXDataSegment* xdata, const void* buf, size_t buf_len);
// copy xdata content from src to dst, return copied length = src.raw_len
ADEMCO_EXPORT_SYMBOL size_t ademcoXDataCopy(AdemcoXDataSegment* dst, const AdemcoXDataSegment* src);
// return ADEMCO_OK for success, return ADEMCO_ERR for len is too long
ADEMCO_EXPORT_SYMBOL int ademcoMakeXData(AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr, const char* content, size_t len);


//////////////////////// AdemcoPacketId functions ////////////////////////

ADEMCO_EXPORT_SYMBOL int isAdemcoPacketId(const char* standard, const char* id, size_t len);
ADEMCO_EXPORT_SYMBOL AdemcoPacketId getAdemcoPacketId(const char* id, size_t len);
ADEMCO_EXPORT_SYMBOL const char* admecoPacketIdToString(AdemcoPacketId id);

/*
* ademcoMake*Packet functions
* if buff != NULL and len >= length needed, return length used and copy data to buff
* otherwise return 0
*/

ADEMCO_EXPORT_SYMBOL size_t ademcoMakeEmptyDataPacket(char* dst_buff, size_t len, const char* id, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeNullPacket(char* buff, size_t len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeAckPacket(char* buff, size_t len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeNakPacket(char* buff, size_t len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeHbPacket(char* buff, size_t len, uint16_t seq, const char* acct, AdemcoId ademcoId,
											   AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, const AdemcoXDataSegment* xdata);

// like upper funcs, store buff and len to pkt->raw, pkt->raw_len
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeNullPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeAckPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeNakPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL size_t ademcoMakeHbPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId,
												AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, const AdemcoXDataSegment* xdata);

//! parse a ademco packet, if everything is OK, cb_commited is the packet length
ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoPacketParse(const char* buff, size_t len, AdemcoPacket* pkt, size_t* cb_commited);


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
ADEMCO_EXPORT_SYMBOL uint16_t ademcoCRC16(const char* buff, size_t len);

// 将一串以高低字节表示的十六进制数组转换为10进制数字符串
// 每个字节的高四位和低四位若不大于9，将该四位表示的数字以10进制ascii码填入str，否则停止
// 返回字符串长度
// 注意：函数不会在str末尾补 null terminator
// 调用者应确保str有足够空间，至少len的2倍，否则会崩溃
// 示例：输入数组：0x18 0x24 0x08 0x88 0x10 0x1f 0xff，输出字符串"18240888101"
ADEMCO_EXPORT_SYMBOL size_t ademcoHiLoArrayToDecStr(char* str, const uint8_t* arr, size_t len);

// 将一个10进制数字符串转为高低字节表示的数组，节省空间
// str应只包含'0'~'9'，否则失败返回0
// str长度若大于len * 2，str[len]及之后的内容将被舍弃以避免溢出
// 转换后的长度若不满len，则以0xF补满。
// 示例：输入字符串"123ABC"，返回0
// 示例：输入字符串 "18240888101", len=9, 则arr内容为 0x18 0x24 0x08 0x88 0x10 0x1f 0xff 0xff 0xff, return 9
// 示例：输入字符串 "12345678901234567890", len=9, 则arr内容为 0x12 0x34 0x56 0x78 0x90 0x12 0x34 0x56 0x78, return 9
// 示例：输入NULL，返回 9个 0xFF
ADEMCO_EXPORT_SYMBOL size_t ademcoDecStrToHiLoArray(uint8_t* arr, size_t len, const char* str);

// 将一串字节流转换为可打印形式
// 返回str长度（len * 2)
// 调用者应确保str有足够的容量，至少len*2
// 注意：函数不会在str末尾补 null terminator
// 示例：输入 arr=[0xEB 0xBA 0x3F], len=3, 则str = "EBBA3F"，返回6
// 示例：输入 ary="123", len=3, 则str = "313233"，返回6
ADEMCO_EXPORT_SYMBOL size_t ademcoHexArrayToStr(char* str, const uint8_t* arr, size_t len);

// 将一串字符串（内容为'0'~'9', 'A'~'F', 'a' ~'f')转为字节流
// 若strlen(str)为奇数，以padding补足arr, padding 应 <= 0x0F
// 若str内包含非hex字符串，返回0
// 调用者应确保arr有足够的容量，至少strlen(str)*2
// 示例：输入字符串str="EBBA3F", 输出arr = [0xEB, 0xBA, 0x3F]，返回 3
// 示例：输入字符串str="ABC", padding = 0, 输出arr=[0xAB, 0xC0]，返回2
// 示例：输入字符串str="ABC", padding = 0x0F, 输出arr=[0xAB, 0xCF]，返回2
ADEMCO_EXPORT_SYMBOL size_t ademcoHexStrToArray(uint8_t* arr, const char* str, uint8_t padding);

// 功能同ademcoHexStrToArray
// 若strlen(str) > len, str[len]及之后的内容将被舍弃以避免溢出
// 示例：输入字符串str="ABCDE", len = 4 padding = 0x0F, 输出arr=[0xAB, 0xCD], return 2
ADEMCO_EXPORT_SYMBOL size_t ademcoHexStrToArrayN(uint8_t* arr, const char* str, size_t len, uint8_t padding);

// 同ademcoHexStrToArrayN，但允许str包含非hex字符，即'0'~'9','a'~'f','A'~'F'之外的内容，以 padding 替换
// 示例：str="AB\xFFD", len=4, padding=0x0F, 则 arr=[ 0xAB, 0xFD ], return 2
// 网线主机 0d 00 命令，接警中心账号部分，有可能是这种
ADEMCO_EXPORT_SYMBOL size_t ademcoHexStrToArrayN_allow_non_hex_str(uint8_t* arr, const char* str, size_t len, uint8_t padding);

#ifdef __cplusplus
}
#endif

#endif // __ADEMCO_H__
