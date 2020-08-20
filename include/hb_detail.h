#pragma once

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <type_traits>
#include <iterator>
#include "ademco_detail.h"

namespace hb
{

typedef uint8_t Char;
typedef std::vector<Char> CharVector;


// checksum

//! only allow len<=255
inline Char sum(Char* arr, Char len) {
	auto& sum_ = arr[len - 1]; sum_ = 0;
	for (Char i = 0; i < len - 1; i++) { 
		sum_ = static_cast<Char>((sum_ + arr[i]) & 0xFFU);
	}
	return sum_;
}

inline Char sum(CharVector& vec) { return sum(vec.data(), static_cast<Char>(vec.size() & 0xFF)); }

template <typename T, typename = int> 
struct T_has_data_and_len : std::false_type
{};

template <typename T> 
struct T_has_data_and_len<T, decltype((void)T::data, (void)T::len, 0)> : std::true_type 
{};

template <class T> 
inline typename std::enable_if<T_has_data_and_len<T>::value, Char>::type
sum(T& t) { return sum(t.data, t.len); }

template <class T> 
inline typename std::enable_if<T_has_data_and_len<T>::value, Char>::type
sum(T* t) { return sum<T>(*t); }


//! now most machine use this proto
namespace common
{

//! 主机状态码
enum class MachineStatus : Char {
	//! 布防
	Arm		= 0,
	//! 半布防
	HalfArm = 1,
	//! 撤防
	Disarm	= 2,
	//! 设置
	Setting = 3,

	MachineStatusCount,
	InvalidMachineStatus = 0xFF,
};

static constexpr MachineStatus machineStatusFromChar(Char s) {
	if (static_cast<Char>(MachineStatus::Arm) <= s
		&& s < static_cast<Char>(MachineStatus::MachineStatusCount))
	{
		return static_cast<MachineStatus>(s);
	}
	return MachineStatus::InvalidMachineStatus;
}

#ifdef ENABLE_COMMON_MACHINE_STATUS_TO_STRING
static const char* machineStatusToString(MachineStatus status) {
	switch (status) {
	case MachineStatus::Arm:	return "Arm";
	case MachineStatus::HalfArm:return "HalfArm";
	case MachineStatus::Disarm:	return "Disarm";
	case MachineStatus::Setting:return "Setting";
	default:					return "Unknown MachineStatus";
	}
}

static const wchar_t* machineStatusToWString(MachineStatus status) {
	switch (status) {
	case MachineStatus::Arm:	return L"布防";
	case MachineStatus::HalfArm:return L"半布防";
	case MachineStatus::Disarm:	return L"撤防";
	case MachineStatus::Setting:return L"设置";
	default:					return L"未知主机状态";
	}
}
#endif // ENABLE_COMMON_MACHINE_STATUS_TO_STRING

//! 主机类型
enum MachineType : Char {
	//! WiFi主机 （已停用）
	WiFi		= 0x00,
	//! 网络摄像机 （未启用）
	Camera		= 0x01,
	//! 3G模块主机 （未启用）
	T_3G		= 0x02,
	//! 网络模块主机 [ HB-G250 ]
	NetMod		= 0x03,
	//! 改造的卧式主机 [ HB-4040G HB-5050G HB-5050-4G ]
	Gprs		= 0x04,
	//! 液晶主机 [ HB-BJQ-560B ]
	Lcd			= 0x05,
	//! 网线主机 [ HB-4040R HB-5050R ]
	Wired		= 0x06,
	//! 真彩主机 [ HB-2050G ]
	TrueColor	= 0x07,
	//! 三区段主机 [ HTZ-G1000 HTZ-G1000-4G ]
	ThreeSection = 0x08,
	//! 物联卡主机（基本上是GRPS主机砍掉了短信、电话功能）
	IoT			= 0x09,

	MachineTypeCount,
	InvalidMachineType = 0xFF,
};

static constexpr MachineType machineTypeFromChar(Char t) {
	if (static_cast<Char>(MachineType::WiFi) <= t
		&& t < static_cast<Char>(MachineType::MachineTypeCount))
	{
		return static_cast<MachineType>(t);
	}
	return MachineType::InvalidMachineType;
}

static constexpr MachineType AllMachineTypes[MachineTypeCount] = {
	WiFi,
	Camera,
	T_3G,
	NetMod,
	Gprs,
	Lcd,
	Wired,
	TrueColor,
	ThreeSection,
	IoT,
};

//! 防区号为0时表示主机自身
static constexpr uint16_t Zone4MachineSelf = 0;
//! 主机防区号范围
static constexpr uint16_t ZoneMin = 1;
//! 最大防区号根据型号不同而不同
static constexpr uint16_t zoneMax(MachineType type) {
	switch (type) {
	case MachineType::Gprs: // HB-4040G HB-5050G HB-5050-4G
	case MachineType::Wired: // HB-4040R HB-5050R
	case MachineType::IoT:
		return 99;

	case MachineType::NetMod: // HB-G250
		return 999;

	case MachineType::Lcd: // HB-BJQ-560B
		return 249;

	case MachineType::TrueColor: // HB-2050G
		return 68;

	case MachineType::ThreeSection: // HTZ-G1000 HTZ-G1000-4G
		return 191;

	case MachineType::WiFi:
	case MachineType::Camera:
	case MachineType::T_3G:
	default:
		return 0;
		break;
	}
}
//! 对任何主机类型，最大的防区数量
static constexpr uint16_t MaxZoneCount = 1000;

//! 对任何主机类型，防区号是否合法（可以包含0防区）
static constexpr bool isValidZone(uint16_t zone) {
	return Zone4MachineSelf <= zone && zone < MaxZoneCount;
}

//! 对任何主机类型，防区号是否合法（不可以包含0防区）
static constexpr bool isValidZoneStrict(uint16_t zone) { 
	return ZoneMin <= zone && zone < MaxZoneCount;
}

//! 防区号是否合法（可以包含0防区）
static constexpr bool isValidZone(MachineType type, uint16_t zone) {
	return Zone4MachineSelf <= zone && zone <= zoneMax(type);
}

//! 防区号是否合法（不可以可以包含0防区）
static constexpr bool isValidZoneStrict(MachineType type, uint16_t zone) {
	return ZoneMin <= zone && zone <= zoneMax(type);
}

//! 主机是否具有半布防功能
static constexpr bool machineCanHalfArm(MachineType type) {
	return type == MachineType::NetMod
		|| type == MachineType::Lcd
		;
}

//! 主机是否可以报告信号强度
static constexpr bool machineCanReportSignalStrength(MachineType type) {
	return type == MachineType::Gprs
		|| type == MachineType::IoT
		|| type == MachineType::Lcd
		|| type == MachineType::TrueColor
		|| type == MachineType::ThreeSection
		;
}

//! 主机本身是否可以短信报警（不算通过服务如阿里语音等）
static constexpr bool machineCanReportBySMS(MachineType type) {
	return type == MachineType::Gprs
		|| type == MachineType::Lcd
		|| type == MachineType::TrueColor
		|| type == MachineType::ThreeSection
		;
}

//! 主机是否已投产使用
static constexpr bool machineIsSolding(MachineType type) {
	return type == MachineType::NetMod
		|| type == MachineType::Gprs
		|| type == MachineType::IoT
		|| type == MachineType::Lcd
		|| type == MachineType::Wired
		|| type == MachineType::TrueColor
		|| type == MachineType::ThreeSection
		;
}

//! 主机是否可以直接写入防区数据（无需对码）
static constexpr bool machineCanDirectlyWriteZone(MachineType type) {
	return type == MachineType::NetMod;
}

//! 主机是否可以挂载分机
static constexpr bool machineCanLinkSubMachine(MachineType type) {
	return type == MachineType::NetMod;
}

#ifdef ENABLE_COMMON_MACHINE_TYPE_TO_STRING
static const char* machineTypeToString(MachineType type) {
	switch (type) {
	case MachineType::WiFi:		return "0 WiFi";
	case MachineType::Camera:	return "1 Camera";
	case MachineType::T_3G:		return "2 3G";
	case MachineType::NetMod:	return "3 NetMod";
	case MachineType::Gprs:		return "4 GPRS";
	case MachineType::Lcd:		return "5 LCD";
	case MachineType::Wired:	return "6 Wired";
	case MachineType::TrueColor:return "7 TrueColor";
	case MachineType::ThreeSection:return "8 ThreeSection";
	case MachineType::IoT:		return "9 IoT";
	default:					return "Unknown MachineType";
	}
}

static const wchar_t* machineTypeToWString(MachineType type) {
	switch (type) {
	case MachineType::WiFi:		return L"0 WiFi主机";
	case MachineType::Camera:	return L"1 摄像头主机";
	case MachineType::T_3G:		return L"2 3G主机";
	case MachineType::NetMod:	return L"3 网络模块+工程主机";
	case MachineType::Gprs:		return L"4 GPRS主机";
	case MachineType::Lcd:		return L"5 液晶主机";
	case MachineType::Wired:	return L"6 网线主机";
	case MachineType::TrueColor:return L"7 真彩主机";
	case MachineType::ThreeSection:return L"8 三区段主机";
	case MachineType::IoT:		return L"9 物联卡主机";
	default:					return L"未知主机";
	}
}
#endif // ENABLE_COMMON_MACHINE_TYPE_TO_STRING


//! 防区属性
enum ZoneProperty : Char {
	//! 匪警全局
	BuglarGlobal	= 0x00,
	//! 匪警紧急
	Emergency		= 0x01,
	//! 火警防区
	Fire			= 0x02,
	//! 胁迫防区
	Duress			= 0x03,
	//! 燃气防区
	Gas				= 0x04,
	//! 淹水防区
	Water			= 0x05,
	//! 分机
	SubMachine		= 0x06,
	//! 遥控器
	RemoteControl	= 0x07,
	//! 匪警半局
	BuglarHalf		= 0x08,
	//! 屏蔽防区
	Shield			= 0x09,
	//! 门铃防区
	DoorRing		= 0x0A,
	//! 保留
	Reserved0B		= 0x0B,
	//! 保留
	Reserved0C		= 0x0C,
	//! 保留
	Reserved0D		= 0x0D,
	//! 保留
	Reserved0E		= 0x0E,

	//! 旁路防区
	//! 2019年9月2日16:01:58 适配金建峰真彩主机
	Bypass			= 0x0F,

	ZonePropertyCount,
	InvalidZoneProperty = 0xFF,
};

static ZoneProperty zonePropertyFromChar(Char zp) {
	if (ZoneProperty::BuglarGlobal <= zp && zp < ZoneProperty::ZonePropertyCount)
	{ return static_cast<ZoneProperty>(zp); }
	return ZoneProperty::InvalidZoneProperty;
}

static std::vector<ZoneProperty> getAvailableZoneProperties(MachineType type) {
	switch (type) {
	case NetMod:	return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, SubMachine, RemoteControl, BuglarHalf, Shield, DoorRing };
	case Gprs:		return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, };
	case Lcd:		return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, SubMachine, RemoteControl, BuglarHalf, Shield, DoorRing };
	case Wired:		return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, };
	case TrueColor:	return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, RemoteControl, Shield, DoorRing, Bypass };
	case ThreeSection:return { BuglarGlobal, Emergency, Fire, Duress, Gas, Water, RemoteControl, Shield, DoorRing, Bypass };
	default:		return {};
	}
}

#ifdef ENABLE_COMMON_ZONE_PROPERTY_TO_STRING
static const wchar_t* zonePropertyToString(ZoneProperty zp) {
	switch (zp) {
	case ZoneProperty::BuglarGlobal:	return L"匪警全局";
	case ZoneProperty::Emergency:		return L"匪警紧急";
	case ZoneProperty::Fire:			return L"火警防区";
	case ZoneProperty::Duress:			return L"胁迫防区";
	case ZoneProperty::Gas:				return L"燃气防区";
	case ZoneProperty::Water:			return L"淹水防区";
	case ZoneProperty::SubMachine:		return L"分机";
	case ZoneProperty::RemoteControl:	return L"遥控器";
	case ZoneProperty::BuglarHalf:		return L"匪警半局";
	case ZoneProperty::Shield:			return L"屏蔽防区";
	case ZoneProperty::DoorRing:		return L"门铃防区";
	case ZoneProperty::Reserved0B:		return L"保留";
	case ZoneProperty::Reserved0C:		return L"保留";
	case ZoneProperty::Reserved0D:		return L"保留";
	case ZoneProperty::Reserved0E:		return L"保留";
	case ZoneProperty::Bypass:			return L"旁路防区";
	case ZoneProperty::InvalidZoneProperty:
	default:							return L"无效属性";
	}
}
#endif // ENABLE_COMMON_ZONE_PROPERTY_TO_STRING


/**
* @brief 串口透传协议
* @note 网络传输模块与主机模块通信协议
* @note 客户端（手机APP、接警中心等）会用到一些透传数据
*/
namespace com
{

//! 读取主机账号 
struct ReadMachineAcctRequest {
	static constexpr Char len = 7;
	static constexpr Char data[len] = { 0xEB, 0xBA, 0x3F, 0x07, 0x00, 0x4C, 0x37 };
};

//! 读取主机账号回应
struct ReadMachineAcctResponse {
	static constexpr Char len = 15;
	static constexpr Char fixHeader[5] = { 0xEB, 0xCB, 0x3F, 0x0F, 0x4C };
	Char data[len] = { 0 };

	bool parseAcct(std::string& acct) {
		if (memcmp(data, fixHeader, sizeof(fixHeader))) { return false; }
		ReadMachineAcctResponse tmp; memcpy(tmp.data, data, len - 1); sum(tmp);
		if (tmp.data[len - 1] != data[len - 1]) { return false; }
		char buff[32] = { 0 };
		acct = ademco::detail::HexCharArrayToStr(buff, (const char*)data + 5, 9);
		return true;
	}
};

//! 写入主机账号
struct WriteMachineAcctRequest {
	static constexpr Char len = 15;
	Char data[len] = { 0xEB, 0xCB, 0x3F, 0x0F, 0x4D };
	//! maker, max acct len is 18
	void make(const std::string& acct) {
		ademco::detail::NumStr2HexCharArray_N(acct.data(), (char*)data + 5, 9);
		sum(this);
	}
};

//! 写入主机账号回应（与读取主机账号命令相同）
typedef ReadMachineAcctRequest WriteMachineAcctResponse;

//! 三区段主机索要主机状态 EB CB 3F 06 B0 AB
struct MachineStatusRequest3Section {
	static constexpr Char len = 6;
	static constexpr Char data[len] = { 0xEB, 0xCB, 0x3F, 0x06, 0xB0, 0xAB };
};
typedef MachineStatusRequest3Section B0;

//! 三区段主机状态回应 EB BA 3F 08 P0 B1 P1 SUM
struct MachineStatusResponse3Section {
	static constexpr Char len = 8;

	union {
		struct Cmd {
			Char head[6];
			// 0 撤防，1 布防
			Char status3  : 2;
			Char status2 : 2;
			Char status1 : 2;
			Char status : 2;
			Char sum;
		};

		Char data[len];
		Cmd cmd;
	}data = {};

	bool check() {
		Char head[4] = { 0xEB, 0xBA, 0x3F, 0x08 };
		Char s = data.data[len - 1];
		return memcmp(data.cmd.head, head, 4) == 0
			&& data.cmd.head[5] == 0xB1
			&& sum(data.data, len) == s;
	}
};
static_assert(sizeof(MachineStatusResponse3Section) == 8, "sizeof(MachineStatusResponse3Section) must be 8");
typedef MachineStatusResponse3Section B1R;

//! 索要主机状态 EB AB 3F A0 75
struct MachineStatusRequest { 
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xA0, 0x75 };
};
typedef MachineStatusRequest A0;

//! 回应主机状态 EB BA 3F 07 P0 A0 P1 P2 P3 SUM
struct MachineStatusResponse {
	static constexpr Char len = 9;
	//                  0     1     2     3    4 P0    5 6.status 7.type 8.sum
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x07, 0xCC, 0xA0, 0x00, 0x00, 0x00 };
	MachineStatus status = MachineStatus::Arm; // status;
	MachineType type = MachineType::WiFi; // type

	MachineStatusResponse& make(MachineStatus status_, MachineType type_) { status = status_; type = type_; return make(); }
	MachineStatusResponse& make() { data[6] = (Char)status; data[7] = type; sum(this); return *this; }
};
typedef MachineStatusResponse A0R;

//! 索要主机防区 EB AB 3F A1 76
struct ZoneRequest {
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xA1, 0x76 };
};
typedef ZoneRequest A1;

//! 回应主机防区 EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
struct ZoneResponse {
	//! 一包数据最多有20个防区
	static constexpr Char max_zone = 20;
	static constexpr Char max_len = 8 + 2 * max_zone;
	/*
	 * when param is not 0xFF, means there's more zone coming; vice versa
	 * zone&prop can be placed as much as 20 times
	 *                      0     1     2   3.len   4     5  6.zone 7.prop 8.param 9.sum
	 */
	Char data[max_len] = { 0xEB, 0xBA, 0x3F, 0x0A, 0xCC, 0xA2, 0x00, 0x00, 0xFF, 0x00 };
	Char len = 10; // init as minimum len

	// default ctor, no zone exists
	ZoneResponse() {
		len = 8;
		Char nodata[8] = { 0xEB, 0xBA, 0x3F, 0x08, 0xCC, 0xA2, 0xFF, 0x00 };
		memcpy(data, nodata, 8);
	}

	/*
	* @brief maker, add zone to data and (if this packet isOver) calc sum
	* @note first time zone&prop will be placed in 6&7, then second zone&prop will be in 8&9, and so on
	* @note but the max number of zone&prop you can add is 20
	* @note len is automatically added (with 2) everytime except for first time
	* @note if hasMoreZone, param will be set to 0, else 0xFF
	*/
	ZoneResponse& addZone(Char zone, ZoneProperty prop, bool isOver = true, bool hasMoreZone = false) {
		if (len == max_len) { return *this; } 
		Char pos = static_cast<Char>(len - 2); 
		data[pos] = zone; data[pos + 1] = prop;
		len = static_cast<Char>(len + 2); data[3] = len;
		data[len - 2] = hasMoreZone ? 0x00 : 0xFF; 
		if (isOver) { sum(this); }
		return *this;
	}

	struct ZoneAndProperty {
		Char zone = 0;
		ZoneProperty prop = ZoneProperty::InvalidZoneProperty;
	};
	typedef std::vector<ZoneAndProperty> ZoneAndProperties;

	//! parser, make sure to call ResponseParser::parse first and return is A2_response to check data valid, then copy data/len to member.
	bool parse(ZoneAndProperties& zps, bool& hasMore) {
		if (len < 8 || data[3] != len) { return false; } // check valid again
		Char sum_ = data[len - 1]; sum(data, len); if (sum_ != data[len - 1]) { return false; } // check sum again
		Char count = len - 8 / 2; // zone/prop pairs
		if (count == 0) { zps.clear(); hasMore = false; return true; }
		for (Char i = 0; i < count; i++) {
			Char zone = data[5 + i * 2];
			ZoneProperty prop = zonePropertyFromChar(data[6 + i * 2]);
			if (prop != ZoneProperty::InvalidZoneProperty) { zps.emplace_back(ZoneAndProperty{ zone, prop }); }
		}
		hasMore = data[len - 2] == 0xFF;
		return true;
	}
};
typedef ZoneResponse A2R;

//! 索要更多主机防区 EB AB 3F A2 77
//! 仅应在收到ZoneResponse的param非0xFF时发送，以索要全部防区
struct ZoneRequestMore {
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xA2, 0x77 };
};
typedef ZoneRequestMore A2;

//! 修改主机防区 EB CB 3F 09 A3 P1 P2 P3 SUM
struct ZoneOpRequest {
	static constexpr Char len = 9;

	//! 修改主机的操作码
	enum ZoneOpCode : Char {
		//! 删除防区
		Delete = 0x00,
		//! 学码
		Learn = 0x01,
		//! 停止学码
		Stop = 0x03,
		//! 修改属性
		Change = 0x04,

		ZoneOpCodeCount,
	};

	//                  0     1     2     3     4  5.zone 6.prop 7.op  8.sum
	Char data[len] = { 0xEB, 0xCB, 0x3F, 0x09, 0xA3, 0x00, 0x00, 0x00, 0x00 };
	ZoneOpRequest& make(Char zone, ZoneProperty prop, ZoneOpCode op) { 
		data[5] = zone; data[6] = prop; data[7] = op; sum(this); return *this;
	}
};
typedef ZoneOpRequest A3;

//! 学码开始回应 EB BA 3F 07 P0 A3 5A
//! 因为学码时主机要等待外部无线信号（用户触发探测器），因此先回应A3表示已经开始学码，学码成功时回 ZoneOpResponse A4
struct ZoneOpLearningResponse {
	static constexpr Char len = 7;
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x07, 0xCC, 0xA3, 0x5A };
};
typedef ZoneOpLearningResponse A3R;

//! 修改防区回应 EB BA 3F 0A P0 A4 P1 P2 P3 SUM
struct ZoneOpResponse {
	static constexpr Char len = 10;

	//! 防区操作回应码
	enum ZoneOpRespCode : Char {
		//! 失败
		Failed = 0x00,
		//! 成功
		Succeed = 0x01,
		//! 失败--重码
		Duplicate = 0x02,
		//! 失败--空防区
		NoSuchZone = 0x03,

		ZoneOpRespCodeCount,
	};

	//                  0     1     2     3     4     5    zone  prop  code  sum
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x0A, 0xCC, 0xA4, 0x00, 0x00, 0x00, 0x00 };

	ZoneOpResponse& make(Char zone, ZoneProperty prop, ZoneOpRespCode code) {
		data[6] = zone; data[7] = prop; data[8] = code; sum(this); return *this;
	}
};
typedef ZoneOpResponse A4R;

//! 主机定时器，一共2组
struct MachineTimer {
	struct Timepoint { Char hour = 0; Char minute = 0; };
	struct Timer { Timepoint armAt = {}; Timepoint disarmAt = {}; };
	Timer data[2] = {}; // 8byte
};
static_assert(sizeof(MachineTimer) == 8, "sizeof(MachineTimer) must be 8");

//! 获取主机定时器 EB AB 3F A5 7A
struct MachineTimerRequest {
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xA5, 0x7A };
};
typedef MachineTimerRequest A5;

//! 主机定时器回应 EB BA 3F 0F P0 A6 H1 M1 H2 M2 H3 M3 H4 M4 SUM
struct MachineTimerResponse {
	static constexpr Char len = 7 + sizeof(MachineTimer); // 15
	//                  0     1     2     3     4     5    a1ah  a1am  a1dh  a1dm  a2ah  a2am  a2dh  a2dm  sum  
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x0F, 0xCC, 0xA6, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00 };
	MachineTimer getTimer() const { MachineTimer t; memcpy(&t, &data[6], sizeof(MachineTimer)); return t; }
	void setTimer(const MachineTimer& t) { memcpy(&data[6], &t, sizeof(MachineTimer)); sum(this); }
};
typedef MachineTimerResponse A6R;

//! 设置主机定时器 EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
struct SetMachineTimerRequest {
	static constexpr Char len = 6 + sizeof(MachineTimer); // 14
	//                  0     1     2     3     4    a1ah  a1am  a1dh  a1dm  a2ah  a2am  a2dh  a2dm  sum  
	Char data[len] = { 0xEB, 0xCB, 0x3F, 0x0E, 0xA7, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00 };
	MachineTimer getTimer() const { MachineTimer t; memcpy(&t, &data[5], sizeof(MachineTimer)); return t; }
	void setTimer(const MachineTimer& t) { memcpy(&data[5], &t, sizeof(MachineTimer)); sum(this); }
};
typedef SetMachineTimerRequest A7;

//! 设置主机定时器回应 EB BA 3F 07 P0 A7 SUM
struct SetMachineTimerResponse {
	static constexpr Char len = 7;
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x07, 0xCC, 0xA7, 0x5E };
};
typedef SetMachineTimerResponse A7R;

//! 拒绝设置回应 EB BA 3F 07 P0 A8 SUM
//! 任何操作，主机如果认为非法，都可以用A8直接回复
struct RejectResponse {
	static constexpr Char len = 7;
	Char data[len] = { 0xEB, 0xBA, 0x3F, 0x07, 0xCC, 0xA8, 0x5F };
};
typedef RejectResponse A8R;

//! 测试地址 
// TODO

//! 索要防区探头遗失/失联  第一次索要 EB AB 3F AC 81
struct QuerySensorLostSettingsRequest {
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xAC, 0x81 };
};
typedef QuerySensorLostSettingsRequest AC;

//! 索要防区探头遗失/失联  索要更多 EB AB 3F AD 82
struct QueryMoreSensorLostSettingsRequest {
	static constexpr Char len = 5;
	static constexpr Char data[len] = { 0xEB, 0xAB, 0x3F, 0xAD, 0x82 };
};
typedef QueryMoreSensorLostSettingsRequest AD;

//! 索要防区探头遗失/失联回应 EB BA 3F PN P0 AD P1 DATA P2 SUM
struct QuerySensorLostSettingsResponse {
	CharVector data{};
	const Char p1 = 0xF0; // F0 防区号单字节表示，F1防区号双字节表示

	QuerySensorLostSettingsResponse(Char p1 = 0xF0) :p1(p1) {
		data = { 0xEB, 0xBA, 0x3F, 0, 0xAD, p1 };
	}

	void addZone(size_t zone, bool isOver = true, bool hasMore = false) {
		data[3]++;
		if (p1 == 0xF0) { data.push_back(zone & 0xFF); }
		else {
			data.push_back((zone >> 8) & 0xFF);
			data.push_back(zone & 0xFF);
		}
		if (isOver) {
			data.push_back(hasMore ? 0x00 : 0xFF);
			data.push_back(0); // sum
			sum(data);
		}
	}
};
typedef QuerySensorLostSettingsResponse ADR;

//! 修改防区探头遗失/失联 EB CB 3F 08 AA P1 P2 SUM
struct ChangeSensorLostSettingsRequest {
	static constexpr Char len = 8;
	Char data[len] = { 0xEB, 0xAB, 0x3F, 0x08, 0xAA, 0x00, 0x00, 0x00 };

	ChangeSensorLostSettingsRequest() = default;
	ChangeSensorLostSettingsRequest(Char zone, Char on) {
		data[5] = zone; data[6] = on; sum(data, len);
	}
};
typedef ChangeSensorLostSettingsRequest AA;

//! 修改防区探头遗失/失联回应 EB BA 3F 09 P0 AB P1 P2 SUM
struct ChangeSensorLostSettingsResponse {
	static constexpr Char len = 9;
	Char data[len] = { 0xEB, 0xAB, 0x3F, 0x09, 0xCC, 0xAB, 0x00, 0x00, 0x00 };

	ChangeSensorLostSettingsResponse() = default;
	ChangeSensorLostSettingsResponse(Char zone, Char p2) {
		data[6] = zone; data[7] = p2; sum(data, len);
	}
};
typedef ChangeSensorLostSettingsResponse ABR;


//! 解析 Request
// 主要用来给模拟主机识别过来的串口命令类型
struct RequestParser {
	enum class RequestType {
		A0_request, // 索要主机状态
		A1_request, // 索要防区
		A2_request, // 索要更多防区
		A3_request, // 修改防区
		A5_request, // 获取定时器
		A7_request, // 设置定时器
		RD_acct_request, // 读取主机账号
		WR_acct_request, // 写入主机账号
		Invalid_request = -1, 
	};

	static RequestType parse(const Char* data, Char len) {
		do {
			if (len < 5) { break; }
			if (data[0] != 0xEB) { break; }
			switch (data[1]) {
			case 0xAB:
			{
				if (data[2] != 0x3F) { break; }
				switch (data[3]) {
				case 0xA0: // EB AB 3F A0 75
				{
					if (len != A0::len) { break; }
					if (memcmp(A0::data, data, len) != 0) { break; }
					return RequestType::A0_request;
				}
				case 0xA1: // EB AB 3F A1 76
				{
					if (len != A1::len) { break; }
					if (memcmp(A1::data, data, len) != 0) { break; }
					return RequestType::A1_request;
				}
				case 0xA2: // EB AB 3F A2 77
				{
					if (len != A2::len) { break; }
					if (memcmp(A2::data, data, len) != 0) { break; }
					return RequestType::A2_request;
				}
				case 0xA5: // EB AB 3F A5 7A
				{
					if (len != A5::len) { break; }
					if (memcmp(A5::data, data, len) != 0) { break; }
					return RequestType::A5_request;
				}
				default:
					if (data[3] == 0x09 && data[4] == 0xA3 && len == A3::len) {
						A3 a3; memcpy(a3.data, data, len); sum(a3);
						if (a3.data[len - 1] != data[len - 1]) { break; }
						return RequestType::A3_request;
					}
				}
				break;
			}
			case 0xBA:
			{
				if (data[2] != 0x3F) { break; }
				if (len == ReadMachineAcctRequest::len) {
					if (memcmp(data, ReadMachineAcctRequest::data, len) == 0) 
					{ return RequestType::RD_acct_request; }
				}
				break;
			}
			case 0xCB:
			{
				if (data[2] != 0x3F) { break; }

				// EB CB 3F 09 A3 P1 P2 P3 SUM
				if (data[3] == 0x09 && data[4] == 0xA3 && len == A3::len) {
					A3 req; memcpy(req.data, data, len); sum(req);
					if (data[len - 1] == req.data[len - 1]) { return RequestType::A3_request; }
				}

				if (data[3] == 0x0F && data[4] == 0x4D && len == WriteMachineAcctRequest::len) {
					WriteMachineAcctRequest req; memcpy(req.data, data, req.len); sum(req);
					if (data[len - 1] == req.data[len - 1]) { return RequestType::WR_acct_request; }
				}

				// EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
				if (data[3] == 0x0E && data[4] == 0xA7 && len == A7::len) {
					A7 req; memcpy(req.data, data, req.len); sum(req);
					if (data[len - 1] == req.data[len - 1]) { return RequestType::A7_request; }
				}
			}
			}
		} while (0);
		return RequestType::Invalid_request;
	}
};

//! 解析 Response 类型
// 用来解析主机发来的指令
struct ResponseParser {
	enum class ResponseType {
		A0_response,
		A2_response,
		A3_response,
		A4_response,
		A6_response,
		A7_response,
		A8_response,
		A9_response,
		AB_response,
		AD_response,
		AE_response,
		B1_response,
		Invalid_response = -1,
	};

	static ResponseType parse(const Char* data, Char len) {
		do {
			if (len < 7) { break; } // 所有的 response ，长度最小为 7
			if (data[0] != 0xEB || data[1] != 0xBA || data[2] != 0x3F) { break; }
			switch (data[5]) {
			case 0xA0: // EB BA 3F 07 P0 A0 P1 P2 P3 SUM
				{
					if (len != A0R::len) { break; }
					A0R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A0_response;
				}

			case 0xA2: // EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
				{
					if (len != 8 + data[3] * 2) { break; }
					A2R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A2_response;
				}

			case 0xA3: 
				{
					if (len != A3R::len) { break; }
					A3R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A3_response;
				}

			case 0xA4: 
				{
					if (len != A4R::len) { break; }
					A4R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A4_response;
				}

			case 0xA6: 
				{
					if (len != A6R::len) { break; }
					A6R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A6_response;
				}

			case 0xA7:
				{
					if (len != A7R::len) { break; }
					A7R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A7_response;
				}

			case 0xA8:
				{
					if (len != A8R::len) { break; }
					A8R resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::A8_response;
				}

			case 0xA9:
				{
					// TODO
				}

			case 0xAB:
				{
					if (len != ABR::len) { break; }
					ABR resp; memcpy(resp.data, data, len); sum(resp);
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::AB_response;
				}

			case 0xAD: // EB BA 3F PN P0 AD P1 DATA P2 SUM
				{
					Char lenExpected = 0;
					if (data[6] == 0xF0) { // 防区号1个字节
						lenExpected = 6 + data[3] + 2;
					} else if (data[6] == 0xF1) { // 防区号2个字节
						lenExpected = 6 + data[3] * 2 + 2;
					} else { break; }
					if (len != lenExpected) { break; }
					ADR resp; std::copy(data, data + len, std::back_inserter(resp.data)); sum(resp.data.data(), (Char)resp.data.size());
					if (resp.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::AD_response;
				}

			case 0xAF: // TODO
				{

					break;
				}

			case 0xB1: // EB BA 3F 08 P0 B1 P1 SUM
				{
					if (len != B1R::len) { break; }
					B1R resp; memcpy(resp.data.data, data, len); sum(resp.data.data, resp.len);
					if (resp.data.data[len - 1] != data[len - 1]) { break; }
					return ResponseType::B1_response;
				}

			}
		} while (0);
		return ResponseType::Invalid_response;
	}
};

} // namespace com
} // namespace common

//! barely used, everybody loves common machine
namespace old
{

//! 键盘按键码
enum Key : Char {
	//! 00没有按键动作
	Key_NULL,		
	//! 01键盘上 1 键按下
	Key_1,
	//! 02键盘上 2 键按下
	Key_2,
	//! 03键盘上 3 键按下
	Key_3,
	//! 04键盘上 4 键按下
	Key_4,
	//! 05键盘上 5 键按下
	Key_5,
	//! 06键盘上 6 键按下
	Key_6,
	//! 07键盘上 7 键按下
	Key_7,
	//! 08键盘上 8 键按下
	Key_8,
	//! 09键盘上 9 键按下
	Key_9,
	//! 0a键盘上 0 键按下
	Key_0,
	//! 0b键盘上 * 键按下
	Key_ASTERISK,
	//! 0c键盘上 # 键按下
	Key_SHARP,
	//! 0d键盘上 全布防 键按下
	Key_ARM,
	//! 0e键盘上 半布防 键按下
	Key_HALF_ARM,
	//! 0f键盘上 撤防 键按下
	Key_DISARM,
	//! 10键盘上 报警 键按下
	Key_EMERGENCY,
	//! 11键盘上 录音 键按下
	Key_RECORD_SOUND,
	//! 12键盘上 放音 键按下
	Key_PLAY_SOUND,
	//! 13键盘上 停止 键按下
	Key_STOP_SOUND,
	//! 0x16终止本次报警
	Key_STOP_ALARM = 0x16,
};
typedef std::vector<Key> Keys;

//! 用于将Key的值Char转为Key，不特殊处理Key_0
static constexpr Key keyFromChar(Char c) {
	if ((Key_1 <= c && c <= Key_STOP_SOUND) || (c == Key_STOP_ALARM)) {
		return static_cast<Key>(c);
	}return Key_NULL;
}

//! 将 0~9转换为Key，特殊处理Key_0
static constexpr Key keyFromNumber(int i) {
	if (i == 0) { return Key_0; }
	else if (1 <= i && i <= 9) { return static_cast<Key>(i); }
	else { return Key_NULL; }
}

static constexpr char keyToPrintableChar(Key key) {
	switch (key) {
	case Key_1: case Key_2: case Key_3:  
	case Key_4: case Key_5: 
	case Key_6: case Key_7: 
	case Key_8: case Key_9: return static_cast<char>(key + '0');
	case Key_0:				return '0';
	case Key_ASTERISK:		return '*';
	case Key_SHARP:			return '#';
	case Key_ARM:			return 'A';
	case Key_HALF_ARM:		return 'H';
	case Key_DISARM:		return 'D';
	case Key_EMERGENCY:		return 'E';
	case Key_RECORD_SOUND:	return 'R';
	case Key_PLAY_SOUND:	return 'P';
	case Key_STOP_SOUND:	return 'S';
	case Key_STOP_ALARM:	return 'M'; // m for mute
	case Key_NULL: default: return ' ';
	}
}

//! PC的模拟键盘序号为3
static constexpr Char PC_KEYBOARD_IDX = 3;

#ifdef ENABLE_SEND_MULTI_KEY_IN_ONE_COMMAND
//! 模拟键盘按下命令
struct KeyInput {
	std::vector<Char> data = {};

	KeyInput() { reset(); }
	KeyInput(Key key){ data = { 0xEB, 0xAB, PC_KEYBOARD_IDX, key, 0x00 }; sum(data); }

	void reset(){ data = { 0xEB, 0xAB, PC_KEYBOARD_IDX, Key_NULL, 0x00 }; sum(data); }
	void setKey(Key key) { data[3] = key; sum(data); }
	void appendKey(Key key, bool end = true) { data.insert(data.end() - 1, key); data[2] = size(); if (end) { sum(data); } }

	const char* buff() const { return reinterpret_cast<const char*>(data.data()); }
	Char size() const { return static_cast<Char>(data.size() & 0xFF); }

	static KeyInput null() { return KeyInput(); }
	static KeyInput back() { KeyInput ki; ki.setKey(Key_0); ki.appendKey(Key_SHARP); return ki; }

	static KeyInput quitConfig() {
		KeyInput ki(Key_0);  ki.appendKey(Key_SHARP, false);
		ki.appendKey(Key_0, false); ki.appendKey(Key_SHARP, false);
		ki.appendKey(Key_0, false); ki.appendKey(Key_SHARP); return ki;
	}

	// use default config password 112233
	static KeyInput enterConfig() {
		KeyInput ki(Key_SHARP);
		ki.appendKey(Key_SHARP, false); ki.appendKey(Key_SHARP, false);
		ki.appendKey(Key_1, false); ki.appendKey(Key_1, false);
		ki.appendKey(Key_2, false); ki.appendKey(Key_2, false);
		ki.appendKey(Key_3, false); ki.appendKey(Key_3, false); 
		ki.appendKey(Key_SHARP);
		return ki;
	}

	// use config password, password must be numeric like 112233, not "112233"
	static KeyInput enterConfig(const char pwd[6]) {
		KeyInput ki(Key_SHARP); 
		ki.appendKey(Key_SHARP); ki.appendKey(Key_SHARP);
		for (char i = 0; i < 6; i++) { ki.appendKey(keyFromChar(pwd[i]), false); }
		ki.appendKey(Key_SHARP);
		return ki;
	}

	// use default config password 112233 and code 12*48#
	static KeyInput enterDuimaMode() {
		KeyInput ki(Key_SHARP);
		ki.appendKey(Key_SHARP, false); ki.appendKey(Key_SHARP, false);
		ki.appendKey(Key_1, false); ki.appendKey(Key_1, false);
		ki.appendKey(Key_2, false); ki.appendKey(Key_2, false);
		ki.appendKey(Key_3, false); ki.appendKey(Key_3, false); 
		ki.appendKey(Key_SHARP, false);
		ki.appendKey(Key_1, false); ki.appendKey(Key_2, false); ki.appendKey(Key_ASTERISK, false); 
		ki.appendKey(Key_4, false); ki.appendKey(Key_8, false);
		ki.appendKey(Key_SHARP);
		return ki;
	}

	std::string toString() const {
		std::string str; 
		for (auto c : data) {
			str.push_back(ademco::detail::Dec2Hex((c >> 4) & 0x0F));
			str.push_back(ademco::detail::Dec2Hex(c & 0x0F));
			str.push_back(' ');
		}
		return str;
	}
};
#else // ENABLE_SEND_MULTI_KEY_IN_ONE_COMMAND
//! 模拟键盘按下命令
struct KeyInput {
	static constexpr Char OneKeyLen = 5;
	struct OneKey {
		std::vector<Char> data = {};
		OneKey(Key key = Key_NULL) { data = { 0xEB, 0xAB, PC_KEYBOARD_IDX, key, 0x00 }; sum(data); }
	};
	std::vector<char> buff_ = {};

	KeyInput() { reset(); }
	KeyInput(Key key) { 
		buff_.resize(OneKeyLen); OneKey data(key); 
		memcpy(buff_.data() + buff_.size() - OneKeyLen, data.data.data(), data.data.size());
	}
	KeyInput(Keys keys) {
		buff_.clear();
		for (auto key : keys) { 
			OneKey k(key); 
			std::copy(k.data.begin(), k.data.end(), std::back_inserter(buff_));
		}
	}

	void reset() { 
		buff_.resize(OneKeyLen); OneKey data;
		memcpy(buff_.data(), data.data.data(), data.data.size());
	}

	void appendKey(Key key) { 
		buff_.resize(buff_.size() + OneKeyLen); OneKey data(key);
		memcpy(buff_.data() + buff_.size() - OneKeyLen, data.data.data(), data.data.size());
	}

	const char* buff() const { return buff_.data(); }
	Char size() const { return static_cast<Char>(buff_.size() & 0xFF); }


	//////////////////////// helpers ///////////////////////////////////////////////////

	static KeyInput null() { return KeyInput(); }
	static KeyInput back() { KeyInput ki(Key_0); ki.appendKey(Key_SHARP); return ki; }

	static KeyInput quitConfig() {
		KeyInput ki(Key_0);  ki.appendKey(Key_SHARP);
		ki.appendKey(Key_0); ki.appendKey(Key_SHARP); return ki;
	}

	// use default config password 112233
	static KeyInput enterConfig() {
		KeyInput ki(Key_SHARP); ki.appendKey(Key_SHARP); ki.appendKey(Key_SHARP);
		ki.appendKey(Key_1); ki.appendKey(Key_1);
		ki.appendKey(Key_2); ki.appendKey(Key_2);
		ki.appendKey(Key_3); ki.appendKey(Key_3);
		ki.appendKey(Key_SHARP);
		return ki;
	}

	// use config password, password must be numeric like 112233, not "112233"
	static KeyInput enterConfig(const char pwd[6]) {
		KeyInput ki(Key_SHARP); ki.appendKey(Key_SHARP); ki.appendKey(Key_SHARP);
		for (size_t i = 0; i < 6; i++) { ki.appendKey(keyFromNumber(pwd[i])); }
		ki.appendKey(Key_SHARP);
		return ki;
	}

	// machine must be under config mode
	static KeyInput enterDuimaMode() {
		KeyInput ki(Key_1); ki.appendKey(Key_2); ki.appendKey(Key_ASTERISK);
		ki.appendKey(Key_4); ki.appendKey(Key_8); ki.appendKey(Key_SHARP);
		return ki;
	}

	// like ###112233# ...
	std::string toString() const {
		std::string str;
		for (size_t i = 0; i < buff_.size() / OneKeyLen; i++) {
			auto key = buff_[i * OneKeyLen + 3];
			str.push_back(keyToPrintableChar(keyFromChar(key)));
		}
		return str;
	}
};
#endif

//! 声音
enum Sound : Char {
	//! 按键音      0
	SOUND_BUTTON_PRESSED = 0,
	//! 成功
	SOUND_SUCCESS,
	//! 叮咚
	SOUND_DINGDONG,
	//! 非法操作
	SOUND_ILLEGAL_OPERATION,
	//! 读秒
	SOUND_SECOND,
	//! 异常
	SOUND_EXCEPTION,
	//! 退出设置
	SOUND_QUIT_CONFIG,
	//! 输入设置密码
	SOUND_INPUT_CONFIG_PSW,
	//! 输入新密码
	SOUND_INPUT_NEW_PSW,
	//! 输入用户密码
	SOUND_INPUT_USER_PSW,
	//! 再次输入            10
	SOUND_INPUT_AGAIN,
	//! "电话报警";
	SOUND_ALARM_PHONE,
	//! "现场报警";
	SOUND_ALARM_SOUNDTE,
	//! "半布防";
	SOUND_SECURE_HALF,
	//! "全局布防";
	SOUND_SECURE_ALL,
	//! "撤防";
	SOUND_SECURE_CANCEL,
	//! "请输入报警电话";
	SOUND_INPUT_PHONE_ALARM,
	//! "删除所有电话";
	SOUND_DEL_ALL_PHONE,
	//! "输入报警声响时间";
	SOUND_INPUT_ALARM_TIME,
	//! "输入离开时间";
	SOUND_INPUT_LEAVE_TIME,
	//! "请输入进入时间";   20
	SOUND_INPUT_ENTER_TIME,
	//! "请输入振铃次数";
	SOUND_INPUT_IN_DIAL_TIMES,
	//! "请输入拨号循环次数";
	SOUND_INPUT_OUT_DIAL_TIMES,
	//! "主机进入对码状态";
	SOUND_HOST_DUIMA,
	//! "请按遥控器布防键";
	SOUND_PRESS_RC_SECURE_ALL,
	//! "请触发探头";
	SOUND_TRIGGER_DETECTOR,
	//! "删除探头";
	SOUND_DEL_DETECTOR,
	//! "删除所有探头";
	SOUND_DEL_ALL_DETECTOR,
	//! "全局防区";
	SOUND_AREA_GLOBAL,
	//! "半局防区";
	SOUND_AREA_HALF,
	//! "紧急报警防区";             30
	SOUND_AREA_EMERGENCY,
	//! "屏蔽防区";
	SOUND_AREA_SHIELD,
	//! "门铃防区";
	SOUND_AREA_DOORRING,
	//! "下一个";
	SOUND_NEXT,
	//! "请注意，有非法进入";
	SOUND_SYSTEM_MALFUNTION,
	//! "主机进入调整状态";
	SOUND_HOST_MODULATION,
	//! "退回";
	SOUND_BACK,
	//! "错误请重新输入";
	SOUND_ERROR_INPUT_AGAIN,
	//! "请输入防区属性";
	SOUND_INPUT_AREA_PROPERTY,
	//! "该防区已存在";
	SOUND_AREA_ALREADY_EXISTS,
	//! "该遥控器已存在";   40
	SOUND_RC_ALREADY_EXISTS,
	//! "欢迎使用";
	SOUND_WELCOM,
	//! "请先删除该防区";
	SOUND_DEL_AREA_FIRST,
	//! "请注意有探头无效";
	SOUND_DETECTOR_INVALID,
	//! "电话线断了";
	SOUND_PHONE_LINE_DISCONNECT,
	//! "主机远程控制";
	SOUND_HOST_REMOTE_CONTROL,
	//! "开始监听";
	SOUND_START_LISTEN,
	//! "现场静音";
	SOUND_SOUNDTE_MUTE,
	//! "即将断线";
	SOUND_ON_THE_WAY_DISCONNECT,
	//! "警笛报警声";
	SOUND_ALARM_WISTLE,
	//! "录音报警声";               50
	SOUND_ALARM_RECORD,
	//! "清异常指示";
	SOUND_CLEAR_EXCEPTION,
	//! "报警";
	SOUND_ALARM,
	//! "输入接警中心电话";
	SOUND_INPUT_PHONE_CENTER,
	//! "请输入用户号";
	SOUND_INPUT_ADEMCO_ID,
	//! "请注意，你已非法进入"
	SOUND_VIALATION_ENTER,
	//! "请注意，报警了"
	SOUND_ATTENTION_ALARMING,
	//! "删除此电话"
	SOUND_DELETE_PHONE,
	//! "删除此遥控"
	SOUND_DELETE_REMOTE_RC,
	//! "删除所有遥控" 59
	SOUND_DELETE_ALL_RC,
	//! "提示"              60
	SOUND_WARNING,
	//! "火警防区"
	SOUND_AREA_FIRE,
	//! "胁迫防区"
	SOUND_AREA_DURESS,
	//! "煤气防区"
	SOUND_AREA_GAS,
	//! "水警防区"
	SOUND_AREA_WATER,
	//! "防区"
	SOUND_AREA,
	//! ”煤气泄漏"
	SOUND_GAS_LEAKAGE,
	//! "请救助"
	SOUND_PLEASE_HELP,
	//! "水泄漏"
	SOUND_WATER_LEAKAGE,
	//! "有火情"
	SOUND_FIRE,
	//! 0 ~ 9	70
	SOUND_0, SOUND_1, SOUND_2, SOUND_3, SOUND_4,	
	SOUND_5, SOUND_6, SOUND_7, SOUND_8, SOUND_9,
	//! "消防车音效"        80
	SOUND_FIRE_TRUCK_SOUND,
	//! "煤气音效"
	SOUND_GAS_SOUND,
	//! "水警音效"
	SOUND_WATER_SOUND,
	//! "全部报警"
	SOUND_ALL_ALARM,
	//! "部分报警"
	SOUND_PART_ALARM,
	//! "接警中心报警选择"  85
	SOUND_SEL_ALARM_CENTER,
	//! "网络主机"  86
	SOUND_NET_MACHINE,
	//! 已断开连接 87
	SOUND_DISCONNECT,
	//! 88
	SOUND_COUNT,
};

//! 主机到键盘显示 9字节
struct MachineInfoToKeyboard {
	static constexpr Char len = 9;
	//                             idx   LED1  LED2  data3 data4 data5 sum
	Char data[len] = { 0xEB, 0xAE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xFF };

	union LED1 {
		struct LED {
			char setting		: 1;
			char halfarm		: 1;
			char arm			: 1;
			char invalid		: 1;
			char solar_disturb	: 1;
			char low_battery	: 1;
			char shield			: 1;
			char alarming		: 1;
		} led;

		Char data;

		LED1() :data(0) {}
	};

	union LED2 {
		struct LED {
			char battery_broke	: 1;
			char door_ring		: 1;
			char emergency		: 1;
			char phone_disconn	: 1;
			char phone_alarm	: 1;
			char record			: 1;
			char signal			: 1;
			char run			: 1;
		} led;

		Char data;

		LED2() :data(0) {}
	};

	//! 数码管显示
	struct LCD {
		//! 百位
		Char h; 
		//! 十位
		Char d; 
		//! 个位
		Char s; 

		char data[4] = { 0 };
		wchar_t wdata[4] = { 0 };

		Char toChar(Char c) const {
			if (0 <= c && c <= 9)				{ return static_cast<Char>('0' + c); }
			else if (0x0A <= c && c <= 0x0D)	{ return static_cast<Char>('A' + c); }
			else if (c == 0x0E)					{ return '-'; }
			else								{ return ' '; }
		}

		wchar_t toWChar(Char c) const {
			if (0 <= c && c <= 9)				{ return L'0' + c; }
			else if (0x0A <= c && c <= 0x0D)	{ return L'A' + c; }
			else if (c == 0x0E)					{ return L'-'; }
			else								{ return L' '; }
		}

		const char* toString() {
			data[0] = toChar(h); data[1] = toChar(d); 
			data[2] = toChar(s); data[3] = '\0'; return data;
		}

		const wchar_t* toWString() {
			wdata[0] = toWChar(h); wdata[1] = toWChar(d);
			wdata[2] = toWChar(s); wdata[3] = L'\0'; return wdata;
		}
	};

	// 1~4
	Char getKeyboardAddr() const { return data[2]; }

	LED1 getLED1() const { LED1 led1; led1.data = data[3]; return led1; }
	void setLED1(LED1 led1) { data[3] = led1.data; }

	LED2 getLED2() const { LED2 led2; led2.data = data[4]; return led2; }
	void setLED2(LED2 led2) { data[4] = led2.data; }

	LCD getLCD() const {
		LCD lcd; lcd.h = data[5] & 0x0F; lcd.d = data[6] & 0x0F;
		lcd.s = data[7] & 0x0F; return lcd;
	}

	Sound getSound() const {
		Char s = static_cast<Char>((data[5] & 0xF0) | ((data[6] >> 4) & 0x0F));
		if (s < SOUND_COUNT) { return static_cast<Sound>(s); }
		return SOUND_COUNT;
	}

	// for windows message
#if defined(WIN32) && defined(_MINWINDEF_)
	inline void toWLParam(WPARAM& wParam, LPARAM& lParam) const {
		wParam = 0;
		wParam |= (data[2]) & 0xFF0000;
		wParam |= (data[3] << 8) & 0xFF00;
		wParam |= data[4] & 0xFF;

		lParam = 0;
		lParam |= (data[5] << 16) & 0xFF0000;
		lParam |= (data[6] << 8) & 0xFF00;
		lParam |= data[7] & 0xFF;
	}

	static MachineInfoToKeyboard fromWLParam(WPARAM wParam, LPARAM lParam) {
		MachineInfoToKeyboard mik;
		mik.data[2] = ((wParam >> 16) & 0xFF);
		mik.data[3] = (wParam >> 8) & 0xFF;
		mik.data[4] = wParam & 0xFF;
		mik.data[5] = (lParam >> 16) & 0xFF;
		mik.data[6] = (lParam >> 8) & 0xFF;
		mik.data[7] = lParam & 0xFF;
		return mik;
	}
#endif // WIN32
};

//! 主机模式
enum MahcineMode : Char {
	//! 普通模式
	ModeNormal,
	//! 设置模式
	ModeConfig,
	//! 对码模式
	ModeDuima,
};

//! 主机（或分主机）状态码
enum MachineStatus : Char {
	Arm						= 0x01,
	Disarm					= 0x02,
	HalfArm					= 0X04,
	MachineStatusInvalid	= 0xFF,
};

//! 报警码
enum AlarmCode : Char {
	//! 主机紧急报警
	MACHINE_EMERGENCY		= 0x08,

	//! 匪警
	ALARM_BURGLAR			= 0x00,		
	//! 火警
	ALARM_FIRE				= 0x20,		
	//! 呼救/挟持
	ALARM_DURESS			= 0x30,		
	//! 煤气
	ALARM_GAS				= 0x40,		
	//! 水警
	ALARM_WATER				= 0x50,		

	//! 开机
	ALARM_STARTUP			= 0xCA,		
	//! 关机
	ALARM_SHUTDOWN			= 0xED,		
	//! 防拆
	ALARM_TEMPER			= 0xBA,		
	//! 发低电
	ALARM_S_BATTERY_LOW		= 0xAB,		
	//! 收低电
	ALARM_R_BATTERY_LOW		= 0xAC,		
	//! 发电池损坏
	ALARM_S_BATTERY_BROKE	= 0xAE,		
	//! 收电池损坏
	ALARM_R_BATTERY_BROKE	= 0xA5,		
	//! 复电
	ALARM_BETTERY_RECOVER	= 0xAD,		
	//! 强光干扰
	ALARM_SOLAR_DISTURB		= 0xBD,		
	//! 强光恢复
	ALARM_SOLAR_RECOVER		= 0xCD,		
	//! 发送端重启
	ALARM_SENDER_REBOOT		= 0x69,		
	//! 长时间没有对上
	ALARM_LONGTIME_DISCONN	= 0xBC,		
	//! 长时间已经对上
	ALARM_LONGTIME_RECOVER	= 0xCB,		
	//! 门铃响了 2014年12月30日 14:39:43
	ALARM_DOOR_RING			= 0xDD,		

	//! SM for Sub Machine

	//! 分机产生异常信息
	ALARM_SM_EXCEPTION		= 0xCE,		
	//! 分机恢复异常信息
	ALARM_SM_EXCEPT_RESUME	= 0xCF,		
	//! 分机产生电源异常
	ALARM_SM_POWER_EXCEPT	= 0xBE,		
	//! 分机恢复电影异常
	ALARM_SM_POWER_RESUME	= 0xBF,		


	AlarmCodeInvalid		= 0xFF,
};

//! 防区类型
enum ZoneType : Char {
	//! 直属防区
	ZT_ZONE			= 0x00,
	//! 分主机防区
	ZT_SUBMACHINE	= 0xEE,
	//! 未对码防区
	ZT_NULL			= 0xCC,

	ZT_INVALID		= 0xFF,
};

static constexpr ZoneType zoneTypeFromChar(Char c) {
	switch (c) {
	case ZT_ZONE:				return ZT_ZONE;
	case ZT_SUBMACHINE:			return ZT_SUBMACHINE;
	case ZT_NULL:				return ZT_NULL;
	case ZT_INVALID: default:	return ZT_INVALID;
	}
}

//! 防区属性
enum ZoneProperty : Char {
	//! 全局防区
	ZP_GLOBAL		= 0xD0,		
	//! 半局防区
	ZP_HALF			= 0xD1,		
	//! 紧急防区
	ZP_EMERGENCY	= 0xD2,		
	//! 屏蔽防区	
	ZP_SHIELD		= 0xD3,		
	//! 门铃防区
	ZP_DOORRING		= 0xD4,		
	//! 火警防区
	ZP_FIRE			= 0xD5,		
	//! 胁迫防区
	ZP_DURESS		= 0xD6,		
	//! 煤气防区
	ZP_GAS			= 0xD7,		
	//! 水警防区
	ZP_WATER		= 0xD8,

	ZP_COUNT		= 0x09,
	ZP_INVALID		= 0xFF,
};

static constexpr ZoneProperty zonePropertyFromChar(Char c) {
	switch (c) {
	case ZP_GLOBAL:				return ZP_GLOBAL;
	case ZP_HALF:				return ZP_HALF;
	case ZP_EMERGENCY:			return ZP_EMERGENCY;
	case ZP_SHIELD:				return ZP_SHIELD;
	case ZP_DOORRING:			return ZP_DOORRING;
	case ZP_FIRE:				return ZP_FIRE;
	case ZP_DURESS:				return ZP_DURESS;
	case ZP_GAS:				return ZP_GAS;
	case ZP_WATER:				return ZP_WATER;
	case ZP_INVALID: default:	return ZP_INVALID;
	}
}

#ifdef ENABLE_OLD_ZONE_PROPERTY_TO_STRING
static constexpr wchar_t* zonePropertyToString(ZoneProperty zp) {
	switch (zp) {
	case ZP_GLOBAL:				return L"全局盗警";
	case ZP_HALF:				return L"半局盗警";
	case ZP_EMERGENCY:			return L"紧急防区";
	case ZP_SHIELD:				return L"屏蔽防区";
	case ZP_DOORRING:			return L"门铃防区";
	case ZP_FIRE:				return L"火警防区";
	case ZP_DURESS:				return L"胁迫防区";
	case ZP_GAS:				return L"煤气防区";
	case ZP_WATER:				return L"水警防区";
	case ZP_INVALID: default:	return L"未知属性";
	}
}
#endif

//! 防区无线地址
struct WirelessAddress {
	static constexpr uint16_t mask = 0xFFFF;
	Char hi = 0;
	Char lo = 0;

	WirelessAddress(uint16_t addr = mask) { fromUInt16(addr); }
	WirelessAddress(const WirelessAddress&) = default;
	WirelessAddress& operator=(const WirelessAddress&) = default;
	WirelessAddress& operator=(uint16_t addr) { fromUInt16(addr); return *this; }
	bool operator==(const WirelessAddress& rhs) const { return hi == rhs.hi && lo == rhs.lo; }
	bool operator==(uint16_t addr) const { return toUInt16() == addr; }
	void fromUInt16(uint16_t addr){ hi = static_cast<Char>((addr >> 8) & 0xFF); lo = static_cast<Char>(addr & 0xFF); }
	uint16_t toUInt16() const { return static_cast<Char>(((hi << 8) | lo) & mask); }
	bool valid() const { return toUInt16() != mask; }
	void reset() { hi = lo = 0xFF; }

	static WirelessAddress randomAddress() {
		WirelessAddress addr = static_cast<uint16_t>(rand() % mask);
		return addr;
	}
};


//! 防区号为0时表示主机（或分主机）自身
static constexpr uint16_t Zone4MachineSelf	= 0;
//! 主机防区号范围1~999
static constexpr uint16_t ZoneMin			= 1;
static constexpr uint16_t ZoneMax			= 999;
//! 主机有线防区范围1~7
static constexpr uint16_t ZoneMinWired		= ZoneMin;
static constexpr uint16_t ZoneMaxWired		= 8;
//! 主机无线防区最小值
static constexpr uint16_t ZoneMinWireless	= ZoneMaxWired + 1;
static constexpr uint16_t ZoneMaxWireless	= ZoneMax;
//! 分主机防区号范围1~99
static constexpr uint16_t ZoneMinSubMachine = 1;
static constexpr uint16_t ZoneMaxSubMachine = 99;


//! 以2个字节表示的防区号
struct ZoneAsTwoChar {
	Char hi = 0;
	Char lo = 0;

	ZoneAsTwoChar(uint16_t zone = 0) { fromUInt16(zone); }
	ZoneAsTwoChar(const ZoneAsTwoChar&) = default;
	ZoneAsTwoChar& operator=(const ZoneAsTwoChar&) = default;
	ZoneAsTwoChar(Char hi, Char lo) : hi(hi), lo(lo) {}

	uint16_t toUInt16() const { return static_cast<Char>(((hi << 8) & 0xFF00) | lo); }
	void fromUInt16(uint16_t zone) { hi = static_cast<Char>((zone >> 8) & 0xFF); lo = static_cast<Char>(zone & 0xFF); }
	ZoneAsTwoChar& operator=(uint16_t zone) { fromUInt16(zone); return *this; }
	ZoneAsTwoChar& operator++() { if (++lo == 0xFF) { lo = 0; if (++hi == 0xFF) { assert(0); } } return *this; }
	bool operator==(const ZoneAsTwoChar& rhs) const { return hi == rhs.hi && lo == rhs.lo; }
	bool operator==(uint16_t zone) const { return toUInt16() == zone; }
};

//! 防区信息
struct ZoneInfo {
	union MachineStatusOrZoneProperty {
		MachineStatus status;
		ZoneProperty prop;
	};

	uint16_t zone = {};
	ZoneType type = {};
	MachineStatusOrZoneProperty prop;
	WirelessAddress addr = {};

	//! 是否有效的无线防区
	bool valid() const {
		return ZoneMinWireless <= zone && zone <= ZoneMaxWireless
			&& type != ZT_INVALID
			&& prop.prop != ZP_INVALID
			&& addr.valid();
	}

	void reset() {
		zone = UINT16_MAX;
		type = ZT_INVALID;
		prop.prop = ZP_INVALID;
		addr.reset();
	}
};

//! 所有防区信息
struct AllZoneInfo {
	//! use extra 1 space, for convenience to use zone as index
	ZoneInfo zones[ZoneMax + 1] = {};

	AllZoneInfo() { for (uint16_t i = 0; i <= ZoneMax; i++) { zones[i].zone = i; } }

	bool isUniqueAddr(WirelessAddress addr) const {
		for (const auto& z : zones) { if (z.addr == addr)return false; }
		return true;
	}

	WirelessAddress randomAddress() {
		auto addr = WirelessAddress::randomAddress();
		while (!isUniqueAddr(addr)) { addr = WirelessAddress::randomAddress(); }
		return addr;
	}
};

/**
* @brief 串口透传协议
* @note 网络传输模块与主机模块通信协议
* @note 客户端（手机APP、接警中心等）会用到一些透传数据
*/
namespace com
{

//! PC到主机，查询主机（自身或防区）或分机（自身或防区）状态
struct Query {
	static constexpr Char len = 8;
	//                  0     1     2    z-hi  z-lo   p1    p2   sum
	Char data[len] = { 0xEB, 0xB5, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//! 参数，可用来查询或设置
	union Param {
		//! 查询参数
		struct Get {
			enum P1 : Char {
				//! 查询分机（自身或防区）状态
				QuerySubMachine = 0xDD,
				//! 查询主机（自身或防区）状态
				QueryMachine = 0xDE,
			};

			P1 p1 = P1::QueryMachine;
			Char p2 = 0xEE;
		};

		//! 设置参数
		struct Set {
			enum P1 : Char {
				SetArm = 0x01,
				SetDisArm = 0x02,
				SetHalfArm = 0x04,
				SetEmergency = 0x08,
			};
			P1 p1 = P1::SetArm;
			Char p2 = 0xEE;
		};

		Get get;
		Set set;
	};

	ZoneAsTwoChar getZone() const { return ZoneAsTwoChar(data[3], data[4]); }
	void setZone(ZoneAsTwoChar zone) { data[3] = zone.hi; data[4] = zone.lo; }

	static Query queryMachineStatus() {
		return queryZoneStatus(Zone4MachineSelf);
	}

	static Query queryZoneStatus(uint16_t zone) {
		Query query;
		query.setZone(zone);
		query.data[5] = Param::Get::P1::QueryMachine;
		query.data[6] = 0xEE;
		sum(query); return query;
	}

	static Query querySubMachineStatus() {
		return queryZoneStatusOfSubMachine(Zone4MachineSelf);
	}

	static Query queryZoneStatusOfSubMachine(Char zone) {
		Query query;
		query.setZone(zone);
		query.data[5] = Param::Get::P1::QuerySubMachine;
		query.data[6] = 0xEE;
		sum(query); return query;
	}
};

//! 查询结果
struct Result {
	static constexpr Char len = 10;
	//                  0     1     2     hi    lo   type  zprop ad-hi ad-lo  sum
	Char data[len] = { 0xEB, 0xB1, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	ZoneAsTwoChar getZone() const { return ZoneAsTwoChar(data[3], data[4]); }
	ZoneType getZoneType() const { return zoneTypeFromChar(data[5]); }
	ZoneProperty getZoneProperty() const { return zonePropertyFromChar(data[6]); }
	WirelessAddress getWirelessAddress() const { WirelessAddress addr; addr.hi = data[7]; addr.lo = data[8]; return addr; }
};

//! 写入防区无线地址到主机
//! 用户恢复主机数据或已知探头（分主机）无线地址时直接写入进行对码，无需触发探头（分主机）
struct WriteToMachineRequest {
	static constexpr Char len = 10;
	//                  0     1     2    z-hi  z-lo        type  ad-hi ad-lo sum
	Char data[len] = { 0xEB, 0xB5, 0x0A, 0x00, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00 };

	WriteToMachineRequest(ZoneAsTwoChar zone, ZoneType type, WirelessAddress addr) {
		data[3] = zone.hi;
		data[4] = zone.lo;
		data[6] = type;
		data[7] = addr.hi;
		data[8] = addr.lo;
		sum(this);
	}
};

} // namespace com
} // namespace old


} // namespace hb
