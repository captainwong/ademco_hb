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


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

#define ADEMCO_OK 0
#define ADEMCO_ERR -1

#define ADEMCO_PACKET_ACCT_MAX_LEN 64 // #acct之acct最大长度
#define ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN 2 // 空data段[]长度
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN 21 // 非空data段长度，acct长度6
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX 64 // 非空data段长度，acct长度未知
#define ADEMCO_PACKET_MAX_LEN 1024
#define CONGWIN_FE100_PACKET_LEN 31 // 丛文FE100协议长度
#define ADEMCO_PACKET_TIMESTAMP_LEN 20
#define ADEMCO_PACKET_XDATA_MAX_LEN 256

// Prototypes
typedef uint32_t AdemcoId;
typedef uint8_t  AdemcoGG;
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
	uint8_t raw[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX];
	int raw_len;
	AdemcoId ademcoId;
	AdemcoEvent ademcoEvent;
	AdemcoGG gg;
	AdemcoZone zone;
}AdemcoDataSegment;

typedef enum AdemcoXDataLengthFormat {
	TWO_HEX, // 两个hex字节表示长度
	FOUR_DECIMAL, // 四个decimal char 表示长度
}AdemcoXDataLengthFormat;

typedef struct AdemcoXDataSegment {
	AdemcoXDataLengthFormat lenghFormat;
	uint8_t raw[ADEMCO_PACKET_XDATA_MAX_LEN];
	int raw_len;
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
	int len;
	AdemcoPacketId id;
	uint16_t seq;
	char acct[ADEMCO_PACKET_ACCT_MAX_LEN + 1];
	AdemcoDataSegment data;
	AdemcoXDataSegment xdata;
	time_t timestamp;
}AdemcoPacket;


// Functions

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
ADEMCO_EXPORT_SYMBOL const wchar_t* ademcoEventToStringChinese(AdemcoEvent ademcoEvent);

ADEMCO_EXPORT_SYMBOL int ademcoAppendDataSegment(uint8_t* packet, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone);
ADEMCO_EXPORT_SYMBOL int ademcoAppendDataSegment2(uint8_t* packet, const AdemcoDataSegment* dataSegment);
ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoParseDataSegment(const uint8_t* packet, int packet_len, AdemcoDataSegment* dataSegment);
// return 0 for empty packet, CONGWIN_FE100_PACKET_LEN for non-empty packet
ADEMCO_EXPORT_SYMBOL int ademcoDataSegmentToCongwinFe100(uint8_t* fe100, const AdemcoDataSegment* dataSegment);
ADEMCO_EXPORT_SYMBOL const uint8_t* ademcoXDataGetValidContentAddr(const AdemcoXDataSegment* xdata);
ADEMCO_EXPORT_SYMBOL int ademcoXDataGetValidContentLen(const AdemcoXDataSegment* xdata);
// return ADEMCO_OK for success, return ADEMCO_ERR for len is too long
ADEMCO_EXPORT_SYMBOL int ademcoMakeXData(AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, const uint8_t* content, int len);

ADEMCO_EXPORT_SYMBOL int isAdemcoId(const char* standard, const char* id, int len);
ADEMCO_EXPORT_SYMBOL AdemcoPacketId getAdemcoId(const char* id, int len);

/*
* ademcoMake*Packet functions
* call with buff = NULL or len < length needed, will return length needed.
* if buff != NULL and len >= length needed, return length needed and copy data to buff
* otherwise return 0 for fail
*/

ADEMCO_EXPORT_SYMBOL int ademcoMakeNullPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeAckPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeNakPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeHbPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId, 
											AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, AdemcoXDataSegment* xdata);

ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoPacketParse(const uint8_t* buff, int len, AdemcoPacket* pkt, int* cb_commited);

ADEMCO_EXPORT_SYMBOL uint16_t ademcoCRC8(char c, uint16_t crc);
ADEMCO_EXPORT_SYMBOL uint16_t ademcoCRC16(const uint8_t* buff, int len, uint16_t crc);


/* Hengbo */

//! 三防区主机状态GG范围 1~3
#define HB_3SECTION_MACHINE_GG_MIN 1
#define HB_3SECTION_MACHINE_GG_MAX 3

//! 是否合法的三防区主机状态GG
static inline int hbIsValid3SectionMachineGG(AdemcoGG gg) {
	return HB_3SECTION_MACHINE_GG_MIN <= gg && gg <= HB_3SECTION_MACHINE_GG_MAX;
}

typedef enum HbMachineStatus {
	HMS_ARM,
	HMS_HALF_ARM,
	HMS_DISARM,
	HMS_SETTING,
	HMS_COUNT,
	HMS_INVALID = -1,
}HbMachineStatus;

typedef enum HbMachineType {
	//! 0 WiFi主机 （已停用）
	HMT_WIFI,
	//! 1 网络摄像机 （未启用）
	HMT_CAMERA,
	//! 2 简化的物联卡主机 [ HB-5050G-4GW ]
	HMT_GPRS_IOT,
	//! 3 网络模块主机 [ HB-G250 ]
	HMT_NETMOD,
	//! 4 改进型卧式主机 [ HB-4040G HB-5050G HB-5050-4G ]
	HMT_GPRS,
	//! 5 液晶主机 [ HB-BJQ560 HB-BJQ-560B ]
	HMT_LCD,
	//! 6 网线主机 [ HB-4040R HB-5050R ]
	HMT_WIRED,
	//! 7 真彩主机 [ HTZ-G1000 HTZ-G1000-4G ]
	HMT_TRUE_COLOR,
	//! 8 三区段主机 [ HTZ-G1000 HTZ-G1000-4G ]
	HMT_3_SECTION,
	//! 9 物联卡主机 [ HB-2050-4GW ]
	HMT_IOT,
	//! 10 GPRS主机可以打电话 [ HB-2050 ]
	HMT_GPRS_PHONE,
	//! 11 NB报警接收主机
	HMT_NB,

	HMT_COUNT,
	HMT_INVALID = -1,
}HbMachineType;

typedef enum HbZoneProperty {
	//! 匪警全局
	HZP_BUGLAR,
	//! 匪警紧急
	HZP_EMERGENCY,
	//! 火警防区
	HZP_FIRE,
	//! 胁迫防区
	HZP_DURESS,
	//! 燃气防区
	HZP_GAS,
	//! 淹水防区
	HZP_WATER,
	//! 分机
	HZP_SUB_MACHINE,
	//! 遥控器
	HZP_REMOTE_CONTROL,
	//! 匪警半局
	HZP_BUGLAR_HALF,
	//! 屏蔽防区
	HZP_SHIELD,
	//! 门铃防区
	HZP_DOOR_RING,
	//! 保留
	HZP_RESERVED_0B,
	//! 保留
	HZP_RESERVED_0C,
	//! 保留
	HZP_RESERVED_0D,
	//! 保留
	HZP_RESERVED_0E,
	//! 旁路防区
	//! 2019年9月2日16:01:58 适配金建峰真彩主机
	HZP_BYPASS,

	HZP_COUNT,
	HZP_INVALID = -1,
}HbZoneProperty;

typedef struct HbMachineTimePoint {
	uint8_t hour;
	uint8_t minute;
}HbMachineTimePoint;

typedef struct HbTimer {
	HbMachineTimePoint armAt;
	HbMachineTimePoint disarmAt;
}HbTimer;

typedef union HbMachineTimer {
	HbTimer timer[2]; // 2 set of timer
	uint8_t data[8];
}HbMachineTimer;


static const HbZoneProperty hbZoneProperties[12] = {
	HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE, 
	HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS
};

// return available count
ADEMCO_EXPORT_SYMBOL int hbGetAvailableZoneProperties(HbMachineType type, HbZoneProperty props[12]);

//! 最大防区号根据型号不同而不同
ADEMCO_EXPORT_SYMBOL uint16_t hbZoneMax(HbMachineType type);
//! 防区号是否合法（可以包含0防区）
ADEMCO_EXPORT_SYMBOL int hbIsValidZone(HbMachineType type, uint16_t zone);
//! 防区号是否合法（不可以可以包含0防区）
ADEMCO_EXPORT_SYMBOL int hbIsValidZoneStrict(HbMachineType type, uint16_t zone);
//! 主机是否已投产使用
ADEMCO_EXPORT_SYMBOL int hbMachineIsSelling(HbMachineType type);
//! 主机是否具有布防功能
ADEMCO_EXPORT_SYMBOL int hbMachineCanArm(HbMachineType type);
//! 主机是否具有撤防功能
ADEMCO_EXPORT_SYMBOL int hbMachineCanDisarm(HbMachineType type);
//! 主机是否可以进入设置状态
ADEMCO_EXPORT_SYMBOL int hbMachineCanEnterSettings(HbMachineType type);
//! 主机是否具有半布防功能
ADEMCO_EXPORT_SYMBOL int hbMachineCanHalfArm(HbMachineType type);
//! 主机是否可以报告信号强度
ADEMCO_EXPORT_SYMBOL int hbMachineCanReportSignalStrength(HbMachineType type);
//! 主机本身是否可以短信报警（不算通过服务如阿里语音等）
ADEMCO_EXPORT_SYMBOL int hbMachineCanReportBySMS(HbMachineType type);
//! 主机是否支持有线防区
ADEMCO_EXPORT_SYMBOL int hbMachineHasWiredZone(HbMachineType type);
//! 主机最小有线防区号
ADEMCO_EXPORT_SYMBOL AdemcoZone hbWiredZoneMin(HbMachineType type);
//! 主机最大有线防区号
ADEMCO_EXPORT_SYMBOL AdemcoZone hbWiredZoneMax(HbMachineType type);
//! 主机是否可以直接写入防区数据（无需对码）
ADEMCO_EXPORT_SYMBOL int hbMachineCanDirectlyWriteZone(HbMachineType type);
//! 主机是否可以挂载分机
ADEMCO_EXPORT_SYMBOL int hbMachineCanLinkSubMachine(HbMachineType type);
//! 根据防区属性判断是否支持失联报告
//! 失联报告是主机侧实现的跟防区属性没关系，但是人为限制了只有以下属性的才可以设置
ADEMCO_EXPORT_SYMBOL int hbZonePropCanReportLost(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL void hbInitMachineTimer(HbMachineTimer* timer);
ADEMCO_EXPORT_SYMBOL int hbIsValidTimer(HbTimer* timer);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbMachineStatusToAdemcoEvent(HbMachineStatus status);
ADEMCO_EXPORT_SYMBOL HbMachineStatus hbMachineStatusFromAdemcoEvent(AdemcoEvent ademcoEvent);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbMachineTypeToAdemcoEvent(HbMachineType type);
ADEMCO_EXPORT_SYMBOL HbMachineType hbMachineTypeFromAdemcoEvent(AdemcoEvent ademcoEvent);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbZonePropertyToAdemcoEvent(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const char* hbMachineTypeToString(HbMachineType type);
ADEMCO_EXPORT_SYMBOL const wchar_t* hbMachineTypeToStringChinese(HbMachineType type);
ADEMCO_EXPORT_SYMBOL const char* hbZonePropertyToString(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const wchar_t* hbZonePropertyToStringChinese(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const char* hbGetZoneFormatString(HbMachineType type);
ADEMCO_EXPORT_SYMBOL const wchar_t* hbGetZoneFormatStringW(HbMachineType type);



// 将一串以高低字节表示的十六进制数组转换为10进制数字符串，遇0xf或非'0'~'9'字符停止，返回字符串长度
// 示例：输入数组：0x18 0x24 0x08 0x88 0x10 0x1f, 0xff，输出字符串"18240888101"
ADEMCO_EXPORT_SYMBOL int hbHiLoArrayToDecStr(char* str, const uint8_t* arr, int len);
// 将一个10进制数字符串转为高低字节表示的数组，节省空间，返回数组长度
ADEMCO_EXPORT_SYMBOL int hbDecStrToHiLoArray(uint8_t* arr, const char* str, int len);


#ifdef __cplusplus
}
#endif

#endif // __ADEMCO_H__
