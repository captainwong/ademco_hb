/*
* Hengbo COM library
* captainwong
* 2022-8-12 rewrited this C version
*/

#ifndef __HB_COM_H__
#define __HB_COM_H__

#pragma once

#include "ademco.h"


#ifdef __cplusplus
extern "C" {
#endif

//! 三防区主机状态GG范围 1~3
#define HB_3SECTION_MACHINE_GG_MIN 1
#define HB_3SECTION_MACHINE_GG_MAX 3

//! 是否合法的三防区主机状态GG
static inline int hbIsValid3SectionMachineGG(AdemcoGG gg) {
	return HB_3SECTION_MACHINE_GG_MIN <= gg && gg <= HB_3SECTION_MACHINE_GG_MAX;
}

//! 主机状态
typedef enum HbMachineStatus {
	HMS_ARM,		// 布防
	HMS_HALF_ARM,	// 半布防
	HMS_DISARM,		// 撤防
	HMS_SETTING,	// 设置
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

typedef struct HbZoneAndProperty {
	AdemcoZone zone;
	HbZoneProperty prop;
}HbZoneAndProperty;

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

// 串口透传数据最大长度
#define HB_COM_DATA_MAX_LEN 64

// 串口透传数据
typedef struct HbComData {
	uint8_t data[HB_COM_DATA_MAX_LEN];
	uint8_t len;
}HbComData;

// 发给主机
typedef enum HbComRequestType {
	HbComReq_A0, // 索要主机状态
	HbComReq_A1, // 索要防区
	HbComReq_A2, // 索要更多防区
	HbComReq_A3, // 修改防区
	HbComReq_A5, // 获取定时器
	HbComReq_A7, // 设置定时器
	HbComReq_A9, // TODO
	HbComReq_AA, // 修改防区探头遗失/失联
	HbComReq_AC, // 索要防区探头遗失/失联--第一次索要
	HbComReq_AD, // 索要防区探头遗失/失联--继续索要
	HbComReq_AE, // 三区段主机布撤防命令
	HbComReq_B0, // 索要三区段主机状态
	HbComReq_RD_acct, // 读取主机账号
	HbComReq_WR_acct, // 写入主机账号
	HbComReq_Invalid = -1,
}HbComRequestType;

// 主机回应
typedef enum HbComResponseType {
	HbComResp_A0, // 索要主机状态回应
	HbComResp_A2, // 索要防区回应
	HbComResp_A3, // 学码开始回应
	HbComResp_A4, // 修改防区回应
	HbComResp_A6, // 获取主机定时器回应
	HbComResp_A7, // 设置主机定时器回应
	HbComResp_A8, // 拒绝设置或设置失败回应
	HbComResp_A9, // TODO
	HbComResp_AB, // 修改防区探头遗失/失联回应
	HbComResp_AD, // 索要防区探头遗失/失联回应
	HbComResp_AF, // 三区段主机布撤防命令回应
	HbComResp_B1, // 三区段主机状态回应
	HbComResp_Invalid = -1,
}HbComResponseType;

//! 索要主机状态 EB AB 3F A0 75
#define HbComReq_A0_len 5
#define HbComReq_A0_data "\xEB\xAB\x3F\xA0\x75"

//! 回应主机状态 EB BA 3F 07 P0 A0 P1 P2 P3 SUM
#define HbComResp_A0_len 9
#define HbComResp_A0_head "\xEB\xBA\x3F\x07\xCC\xA0"

//! 索要主机防区 EB AB 3F A1 76
#define HbComReq_A1_len 5
#define HbComReq_A1_data "\xEB\xAB\x3F\xA1\x76"

//! 回应主机防区 EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
#define HbComResp_A2_len_min 8 // 无防区数据时长度最小为8
#define HbComResp_A2_max_zone 20 // 最多可以包含 20 个防区
#define HbComResp_A2_len_max (HbComResp_A2_len_min + HbComResp_A2_max_zone * 2) // 一包数据最多有8+20*2=48个字节
typedef enum HbComResp_A2_p1 {
	HbComResp_A2_p1_nomore = 0xFF, // 无更多防区
	HbComResp_A2_p1_more = 0x00, // 还有更多防区
}HbComResp_A2_p1;
#define HbComResp_A2_head "\xEB\xBA\x3F\x08\xCC\xA2"
typedef struct HbComResp_A2_Iter {
	HbComData com;
	uint8_t i, total;
	HbComResp_A2_p1 p1;
}HbComResp_A2_Iter;

//! 索要更多主机防区 EB AB 3F A2 77
//! 仅应在收到ZoneResponse的param非0xFF时发送，以索要全部防区
#define HbComReq_A2_len 5
#define HbComReq_A2_data "\xEB\xAB\x3F\xA2\x77"

//! 修改主机防区 EB CB 3F 09 A3 P1 P2 P3 SUM
#define HbComReq_A3_len 9
#define HbComReq_A3_head "\xEB\xCB\x3F\x09\xA3"

typedef enum HbComReq_A3_p3 {
	HbComReq_A3_p3_del = 0x00, //! 删除防区
	HbComReq_A3_p3_learn = 0x01, //! 学码
	HbComReq_A3_p3_stop = 0x02, //! 停止学码
	HbComReq_A3_p3_modify = 0x04, //! 修改防区属性
}HbComReq_A3_op;

//! 学码开始回应 EB BA 3F 07 P0 A3 5A
//! 因为学码时主机要等待外部无线信号（用户触发探测器），因此先回应A3表示已经开始学码，学码成功时回 ZoneOpResponse A4
#define HbComResp_A3_len 7
#define HbComResp_A3_data "\xEB\xBA\x3F\x07\xCC\xA3\x5A"

//! 修改防区回应 EB BA 3F 0A P0 A4 P1 P2 P3 SUM
#define HbComResp_A4_len 10
#define HbComResp_A4_head "\xEB\xBA\x3F\x0A\xCC\xA4"
typedef enum HbComResp_A4_p3 {
	HbComResp_A4_p3_fail = 0x00, //! 失败
	HbComResp_A4_p3_ok   = 0x01, //! 成功
	HbComResp_A4_p3_dup  = 0x02, //! 失败--重码
	HbComResp_A4_p3_ne   = 0x03, //! 失败--防区未对码 not exists
}HbComResp_A4_p3;

//! 获取主机定时器 EB AB 3F A5 7A
#define HbComReq_A5_len 5
#define HbComReq_A5_data "\xEB\xAB\x3F\xA5\x7A"

//! 获取主机定时器回应 EB BA 3F 0F P0 A6 H1 M1 H2 M2 H3 M3 H4 M4 SUM
#define HbComResp_A6_len 15
#define HbComResp_A6_head "\xEB\xBA\x3F\x0F\xCC\xA6"

//! 设置主机定时器 EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
#define HbComReq_A7_len 14
#define HbComReq_A7_head "\xEB\xCB\x3F\x0E\xA7"
#define hbComResq_A7_to_timer(a7, timer) memcpy((timer)->data, (a7) + 5, 8);

//! 设置主机定时器回应 EB BA 3F 07 P0 A7 SUM
#define HbComResp_A7_len 7
#define HbComResp_A7_data "\xEB\xBA\x3F\x07\xCC\xA7\x5E"

//! 拒绝设置回应 EB BA 3F 07 P0 A8 SUM
//! 任何操作，主机如果认为非法，都可以用A8直接回复
#define HbComResp_A8_len 7
#define HbComResp_A8_data "\xEB\xBA\x3F\x07\xCC\xA8\x5F"

//! 修改防区探头遗失/失联 EB CB 3F 08 AA P1 P2 SUM
#define HbComReq_AA_len 8
#define HbComReq_AA_head "\xEB\xAB\x3F\x08\xAA"

//! 修改防区探头遗失/失联回应 EB BA 3F 09 P0 AB P1 P2 SUM
#define HbComResp_AB_len 9
#define HbComResp_AB_head "\xEB\xBA\x3F\x09\xCC\xAB"
typedef enum HbComResp_AB_p2 {
	HbComResp_AB_p2_close = 0, // 防区失联关
	HbComResp_AB_p2_open = 1, // 防区失联开
	HbComResp_AB_p2_reject = 2, // 拒绝操作
}HbComResp_AB_p2;

//! 索要防区探头遗失/失联  第一次索要 EB AB 3F AC 81
#define HbComReq_AC_len 5
#define HbComReq_AC_data "\xEB\xAB\x3F\xAC\x81"

//! 索要防区探头遗失/失联  索要更多 EB AB 3F AD 82
#define HbComReq_AD_len 5
#define HbComReq_AD_data "\xEB\xAB\x3F\xAD\x82"

//! 索要防区探头遗失/失联回应 EB BA 3F PN P0 AD P1 DATA P2 SUM
#define HbComResp_AD_len_min 9 // 无防区失联数据时一包长度最小为9
#define HbComResp_AD_max_zone 20 // 一包最多包含20个防区
#define HbComResp_AD_len_max (HbComResp_AD_len_min + HbComResp_AD_max_zone * 2) // 一包最多有 9 + 20 *2 = 49 个字节
typedef enum HbComResp_AD_p1 {
	HbComResp_AD_p1_single = 0xF0, // 防区号以单字节表示
	HbComResp_AD_p1_double = 0xF1, // 防区号以双字节表示
}HbComResp_AD_p1;
typedef enum HbComResp_AD_p2 {
	HbComResp_AD_p2_nomore = 0xFF, // 无更多防区
	HbComResp_AD_p2_more = 0x00, // 还有更多防区
}HbComResp_AD_p2;
#define HbComResp_AD_head "\xEB\xBA\x3F\x09\xCC\xAD"

typedef enum HbCom_3sec_status {
	HbCom_3sec_arm = 0x00, // 布防（成功）
	HbCom_3sec_disarm = 0x01, // 撤防（成功）
}HbCom_3sec_status;

typedef enum HbCom_3sec_which {
	HbCom_3sec_machine,  // 主机
	HbCom_3sec_section1, // 区段1
	HbCom_3sec_section2, // 区段2
	HbCom_3sec_section3, // 区段3
}HbCom_3sec_which;

//! 三区段主机布撤防命令 EB CB 3F 08 AE P1 P2 SUM
#define HbComReq_AE_len 8
#define HbComReq_AE_head "\xEB\xCB\x3F\x08\xAE"

//! 三区段主机布撤防命令回应 EB BA 3F 08 P0 AF P1 P2 SUM
#define HbComResp_AF_len 9
#define HbComResp_AF_head "\xEB\xBA\x3F\x08\xCC\xAF"

//! 三区段主机索要主机状态 EB CB 3F 06 B0 AB
#define HbComReq_B0_len 6
#define HbComReq_B0_data "\xEB\xCB\x3F\x06\xB0\xAB"

//! 三区段主机状态回应 EB BA 3F 08 P0 B1 P1 SUM
#define HbComResp_B1_len 8
#define HbComResp_B1_head "\xEB\xBA\x3F\x08\xCC\xB1"
#define hbComResp_B1_status(d)  ((((d)[6] >> 6) & 0x03) == HbCom_3sec_arm ? HMS_ARM : HMS_DISARM)
#define hbComResp_B1_status1(d) ((((d)[6] >> 4) & 0x03) == HbCom_3sec_arm ? HMS_ARM : HMS_DISARM)
#define hbComResp_B1_status2(d) ((((d)[6] >> 2) & 0x03) == HbCom_3sec_arm ? HMS_ARM : HMS_DISARM)
#define hbComResp_B1_status3(d) ((((d)[6]) & 0x03) == HbCom_3sec_arm ? HMS_ARM : HMS_DISARM)


//! 读取主机账号 
#define HbComReq_RD_acct_len 7
#define HbComReq_RD_acct_data "\xEB\xBA\x3F\x07\x00\x4C\x37"

//! 读取主机账号回应
#define HbComResp_RD_acct_len 15
#define HbComResp_RD_acct_head "\xEB\xCB\x3F\x0F\x4C"

//! 写入主机账号
#define HbComReq_WR_acct_len 15
#define HbComReq_WR_acct_head "\xEB\xCB\x3F\x0F\x4D"

//! 写入主机账号回应（与读取主机账号命令相同）
#define HbComResp_WR_acct_len HbComReq_RD_acct_len
#define HbComResp_WR_acct_head HbComReq_WR_acct_head

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
ADEMCO_EXPORT_SYMBOL int hbIsValidTimePoint(HbMachineTimePoint* tp);
ADEMCO_EXPORT_SYMBOL void hbTimePointToGreenwich(HbMachineTimePoint* tp);
ADEMCO_EXPORT_SYMBOL void hbTimePointFromGreenwich(HbMachineTimePoint* tp);
ADEMCO_EXPORT_SYMBOL int hbIsValidTimer(HbTimer* timer);
ADEMCO_EXPORT_SYMBOL int hbIsValidMachineTimer(HbMachineTimer* timer);
//! 传输的是格林威治时间
ADEMCO_EXPORT_SYMBOL void hbMachineTimerToGreenwich(HbMachineTimer* timer);
ADEMCO_EXPORT_SYMBOL void hbMachineTimerFromGreenwich(HbMachineTimer* timer);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbMachineStatusToAdemcoEvent(HbMachineStatus status);
ADEMCO_EXPORT_SYMBOL HbMachineStatus hbMachineStatusFromAdemcoEvent(AdemcoEvent ademcoEvent);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbMachineTypeToAdemcoEvent(HbMachineType type);
ADEMCO_EXPORT_SYMBOL HbMachineType hbMachineTypeFromAdemcoEvent(AdemcoEvent ademcoEvent);
ADEMCO_EXPORT_SYMBOL AdemcoEvent hbZonePropertyToAdemcoEvent(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const char* hbMachineStatusToString(HbMachineStatus status);
ADEMCO_EXPORT_SYMBOL const char* hbMachineStatusToStringChinese(HbMachineStatus status);
ADEMCO_EXPORT_SYMBOL const char* hbMachineTypeToString(HbMachineType type);
ADEMCO_EXPORT_SYMBOL const char* hbMachineTypeToStringChinese(HbMachineType type);
ADEMCO_EXPORT_SYMBOL const char* hbZonePropertyToString(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const char* hbZonePropertyToStringChinese(HbZoneProperty zp);
ADEMCO_EXPORT_SYMBOL const char* hbGetZoneFormatString(HbMachineType type);

// 累加校验，计算data[0] ~ data[len-2]，校验和放在data[len-1]
ADEMCO_EXPORT_SYMBOL void hbSum(uint8_t* data, size_t len);
// 校验和是否正确, return 0 for incorrect, otherwise correct
ADEMCO_EXPORT_SYMBOL int hbCheckSum(const uint8_t* data, size_t len);

ADEMCO_EXPORT_SYMBOL HbComRequestType hbComParseRequest(const uint8_t* buff, size_t len, HbComData* cd);
ADEMCO_EXPORT_SYMBOL HbComRequestType hbComParseXDataRequest(const AdemcoXDataSegment* xdata, HbComData* cd);
ADEMCO_EXPORT_SYMBOL HbComResponseType hbComParseResponse(const uint8_t* buff, size_t len, HbComData* cd);
ADEMCO_EXPORT_SYMBOL HbComResponseType hbComParseXDataResponse(const AdemcoXDataSegment* xdata, HbComData* cd);

ADEMCO_EXPORT_SYMBOL void hbComMakeReqA0_getMachineStatus(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqA1_getMachineZones(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqA2_getMoreMachineZones(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqA3_modifyMachineZone(HbComData* data, uint8_t zone, HbZoneProperty prop, HbComReq_A3_op op);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqA5_getMachineTimer(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqA7_setMachineTimer(HbComData* data, HbMachineTimer* timer);
// TODO
// ADEMCO_EXPORT_SYMBOL void hbComMakeReqA9(HbComData* data, HbMachineStatus status, HbMachineType type);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqAA_modifyMachineZoneLostConfig(HbComData* data, uint8_t zone, int on);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqAC_getMachineZoneLostConfig(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqAD_getMoreMachineZoneLostConfig(HbComData* data);
// p2 must be HbCom_3sec_arm or HbCom_3sec_disarm
ADEMCO_EXPORT_SYMBOL void hbComMakeReqAE_set3SectionMachineStatus(HbComData* data, HbCom_3sec_which p1, HbCom_3sec_status status);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqB0_get3SectionMachineStatus(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqRD_acct(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeReqWR_acct(HbComData* data, const char* acct);

ADEMCO_EXPORT_SYMBOL void hbComResp_A2_Iter_init(HbComResp_A2_Iter* iter, const HbComData* com);
ADEMCO_EXPORT_SYMBOL HbComResp_A2_p1 hbComResp_A2_Iter_next(HbComResp_A2_Iter* iter, HbZoneAndProperty* zp);

ADEMCO_EXPORT_SYMBOL void hbComMakeRespA0_getMachineStatus(HbComData* data, HbMachineStatus status, HbMachineType type);
// zones and props length is count
// if count is 0, p1 wil be set to HbComResp_A2_p1_nomore
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA2_getMachineZones(HbComData* data, size_t count, AdemcoZone* zones, HbZoneProperty* props, HbComResp_A2_p1 p1);
// 学码开始，等待探测器无线信号中
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA3_waitingSignal(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA4_modifyMachineZone(HbComData* data, AdemcoZone zone, HbZoneProperty prop, HbComResp_A4_p3 p3);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA6_getMachineTimer(HbComData* data, HbMachineTimer* timer);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA7_setMachineTimer(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespA8_reject(HbComData* data);
// TODO 
// ADEMCO_EXPORT_SYMBOL void hbComMakeRespA9_(HbComData* data);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespAB_modifyMachineZoneLostConfig(HbComData* data, AdemcoZone zone, HbComResp_AB_p2 p2);
// 返回所有失联开的防区
ADEMCO_EXPORT_SYMBOL void hbComMakeRespAD_getMachineZoneLostConfig(HbComData* data, HbComResp_AD_p1 p1, size_t count, AdemcoZone* zones, HbComResp_AD_p2 p2);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespAF_set3SectionMachineStatus(HbComData* data, HbCom_3sec_which p1, HbCom_3sec_status status);
ADEMCO_EXPORT_SYMBOL void hbComMakeRespB1_get3SectionMachineStatus(HbComData* data, HbMachineStatus statusMachine,
																   HbMachineStatus statusSec1, HbMachineStatus statusSec2, HbMachineStatus statusSec3);
ADEMCO_EXPORT_SYMBOL void hbComDataToAdemcoXData(const HbComData* const data, AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr);

#ifdef __cplusplus
}
#endif

#endif
