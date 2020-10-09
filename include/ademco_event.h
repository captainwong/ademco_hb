#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <stdint.h>

#ifdef ENABLE_ADEMCO_EVENT_TO_STRING
#include <string>
#endif // ENABLE_ADEMCO_EVENT_TO_STRING


namespace ademco
{

//! 安定宝事件代码
enum ADEMCO_EVENT : uint32_t {

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
	EVENT_SOLARDISTURB							= 1387, // 光扰
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
	

	// ------------------------------------------------------------------

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

	AdemcoEventSentinel							= 10000,

	// -------------------接警中心内部使用事件------------------------------
	EVENT_PRIVATE_EVENT_BASE					= 0x00010000,
	EVENT_CLEARMSG,										// 清除报警信息
	EVENT_OFFLINE,										// 主机断线
	EVENT_ONLINE,										// 主机上线
	EVENT_SUBMACHINECNT,								// 分机数量变化
	EVENT_MACHINE_INFO_CHANGED,							// 主机信息改变，需要界面刷新
	EVENT_IM_GONNA_DIE,									// 主机类已析构，通知界面
	EVENT_LINK_TEST,									// 主机心跳
	EVENT_OP_FAILED,									// 布撤防失败
	EVENT_PRIVATE_EVENT_MAX,

};


#ifdef ENABLE_ADEMCO_EVENT_TO_STRING
static constexpr ADEMCO_EVENT AdemcoEvents[] = {
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
	EVENT_SOLARDISTURB,
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

inline std::string ademcoEventToStringEnglish(ADEMCO_EVENT ademco_event, bool with_event_number = true)
{
	auto n_to_s = [&with_event_number](uint32_t n) ->std::string {
		if (with_event_number) { return std::to_string(n) + "--"; }
		return std::string();
	};

	switch (ademco_event) {
	case EVENT_ARM:									return n_to_s(ademco_event) + "ARM";								
	case EVENT_BURGLAR:								return n_to_s(ademco_event) + "BURGLAR";							
	case EVENT_DISARM:								return n_to_s(ademco_event) + "DISARM";								
	case EVENT_DURESS:								return n_to_s(ademco_event) + "DURESS";								
	case EVENT_EMERGENCY:							return n_to_s(ademco_event) + "EMERGENCY";							
	case EVENT_FIRE:								return n_to_s(ademco_event) + "FIRE";								
	case EVENT_GAS:									return n_to_s(ademco_event) + "GAS";								
	case EVENT_HALFARM: case EVENT_HALFARM_1456:	return n_to_s(ademco_event) + "HALFARM";							
	case EVENT_TAMPER:								return n_to_s(ademco_event) + "TAMPER";								
	case EVENT_ZONE_TAMPER:							return n_to_s(ademco_event) + "ZONE_TAMPER";						
	case EVENT_BYPASS:								return n_to_s(ademco_event) + "BYPASS";						
	case EVENT_BYPASS_RESUME:						return n_to_s(ademco_event) + "BYPASS_RESUME";				

	case EVENT_WATER:								return n_to_s(ademco_event) + "WATER";								
	case EVENT_AC_BROKE:							return n_to_s(ademco_event) + "AC BROKE";							
	case EVENT_AC_RECOVER:							return n_to_s(ademco_event) + "AC RECOVER";							
	case EVENT_LOWBATTERY:							return n_to_s(ademco_event) + "LOWBATTERY";							
	case EVENT_BATTERY_RECOVER:						return n_to_s(ademco_event) + "BATATTERY_RECOVER"; 					
	case EVENT_BADBATTERY:							return n_to_s(ademco_event) + "BADBATTERY";							
	case EVENT_SOLARDISTURB:						return n_to_s(ademco_event) + "SOLARDISTURB";						
	case EVENT_DISCONNECT:							return n_to_s(ademco_event) + "DISCONNECT";							
	case EVENT_RECONNECT:							return n_to_s(ademco_event) + "RECONNECT";							
	case EVENT_LOST:								return n_to_s(ademco_event) + "LOST";								
	case EVENT_LOST_RECOVER:						return n_to_s(ademco_event) + "LOST_RECOVER";						
	case EVENT_3100:								return n_to_s(ademco_event) + "LCD CLEAR EXCEPTION";				
	case EVENT_BATTERY_EXCEPTION:					return n_to_s(ademco_event) + "BATTERY_EXCEPTION";					
	case EVENT_BATTERY_EXCEPTION_RECOVER:			return n_to_s(ademco_event) + "BATTERY_EXCEPTION_RECOVER";			
	case EVENT_OTHER_EXCEPTION:						return n_to_s(ademco_event) + "OTHER_EXCEPTION";					
	case EVENT_OTHER_EXCEPTION_RECOVER:				return n_to_s(ademco_event) + "OTHER_EXCEPTION_RECOVER";			

	case EVENT_SERIAL485DIS:						return n_to_s(ademco_event) + "485DIS";								
	case EVENT_SERIAL485CONN:						return n_to_s(ademco_event) + "485CONN";							
	case EVENT_DOORRINGING:							return n_to_s(ademco_event) + "DOORRINGING";						
	case EVENT_CONN_HANGUP: 						return n_to_s(ademco_event) + "CONN_HANGUP";						
	case EVENT_CONN_RESUME: 						return n_to_s(ademco_event) + "CONN_RESUME";						
	case EVENT_DISARM_PWD_ERR: 						return n_to_s(ademco_event) + "DISARM_PWD_ERR";						
	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:		return n_to_s(ademco_event) + "SUB_SENSOR_EXCEPTION"; 				
	case EVENT_SUB_MACHINE_SENSOR_RESUME:			return n_to_s(ademco_event) + "SUB_SENSOR_RESUME"; 					
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:			return n_to_s(ademco_event) + "SUB_POWER_EXCEPTION"; 				
	case EVENT_SUB_MACHINE_POWER_RESUME:			return n_to_s(ademco_event) + "SUB_POWER_RESUME"; 					
	case EVENT_COM_PASSTHROUGH:						return n_to_s(ademco_event) + "COM_PASSTHROUGH";
	case EVENT_ENTER_SET_MODE:						return n_to_s(ademco_event) + "ENTER_SET_MODE"; 					
	case EVENT_EXIT_SET_MODE:						return n_to_s(ademco_event) + "EXIT_SET_MODE";
	case EVENT_QUERY_SUB_MACHINE:					return n_to_s(ademco_event) + "QUERY"; 								
	case EVENT_WRITE_TO_MACHINE:					return n_to_s(ademco_event) + "WRITE_TO_MACHINE"; 					
	case EVENT_I_AM_NET_MODULE:						return n_to_s(ademco_event) + "I_AM_NET_MODULE"; 					
	case EVENT_PHONE_USER_SOS:						return n_to_s(ademco_event) + "SOS"; 								
	case EVENT_PHONE_USER_CANCLE_ALARM:				return n_to_s(ademco_event) + "PHONE_USER_CANCLE_ALARM"; 			
	case EVENT_I_AM_GPRS:							return n_to_s(ademco_event) + "I_AM_GPRS_MACHINE";					
	case EVENT_I_AM_LCD_MACHINE:					return n_to_s(ademco_event) + "I_AM_LCD_MACHINE";					
	case EVENT_I_AM_WIRE_MACHINE:					return n_to_s(ademco_event) + "I_AM_WIRE_MACHINE";					
	case EVENT_I_AM_WIFI_MACHINE:					return n_to_s(ademco_event) + "I_AM_WIFI_MACHINE";					
	case EVENT_I_AM_3_SECTION_MACHINE:				return n_to_s(ademco_event) + "I_AM_3_SECTION_MACHINE";				
	case EVENT_I_AM_IOT_MACHINE:					return n_to_s(ademco_event) + "I_AM_IOT_MACHINE";					
	case EVENT_I_AM_TRUE_COLOR:						return n_to_s(ademco_event) + "I_AM_TRUE_COLOR";
	case EVENT_I_AM_GPRS_IOT:						return n_to_s(ademco_event) + "I_AM_GPRS_IOT";
	case EVENT_I_AM_GPRS_PHONE:						return n_to_s(ademco_event) + "I_AM_GPRS_PHONE";
	case EVENT_SIM_IS_IOT_CARD:						return n_to_s(ademco_event) + "SIM card is IOT";			
	case EVENT_SIM_IS_IOT_PLATFORM_CARD:			return n_to_s(ademco_event) + "SIM card is Platform IOT";				
	case EVENT_SIM_IS_NOT_IOT_CARD:					return n_to_s(ademco_event) + "SIM card is not IOT";				
	case EVENT_ENTER_SETTING_MODE:					return n_to_s(ademco_event) + "ENTER_SETTING_MODE";					
	case EVENT_EXIT_SETTING_MODE:					return n_to_s(ademco_event) + "EXIT_SETTING_MODE";					
	case EVENT_RESTORE_FACTORY_SETTINGS_710:
	case EVENT_RESTORE_FACTORY_SETTINGS:			return n_to_s(ademco_event) + "RESTORE_FACTORY_SETTINGS";			
	case EVENT_WHAT_IS_YOUR_TYPE:					return n_to_s(ademco_event) + "WHAT_IS_YOUR_TYPE";			
	case EVENT_SIGNAL_STRENGTH_CHANGED:				return n_to_s(ademco_event) + "SIGNAL_STRENGTH_CHANGED";			
	default:			with_event_number = true;	return n_to_s(ademco_event) + "undefined";							
	}
}

inline const std::wstring ademcoEventToStringChinese(ADEMCO_EVENT ademco_event, bool with_event_number = true)
{
	auto n_to_s = [&with_event_number](uint32_t n) ->std::wstring {
		if (with_event_number) {
			return std::to_wstring(n) + L"--";
		}
		return std::wstring();
	};

	switch (ademco_event) {
	case EVENT_ARM:									return n_to_s(ademco_event) + L"布防";						
	case EVENT_BURGLAR:								return n_to_s(ademco_event) + L"盗警";						
	case EVENT_DISARM:								return n_to_s(ademco_event) + L"撤防";						
	case EVENT_DURESS:								return n_to_s(ademco_event) + L"胁迫";						
	case EVENT_EMERGENCY:							return n_to_s(ademco_event) + L"紧急报警";					

	case EVENT_FIRE:								return n_to_s(ademco_event) + L"火警";						
	case EVENT_GAS:									return n_to_s(ademco_event) + L"煤气";						
	case EVENT_HALFARM:	case EVENT_HALFARM_1456:	return n_to_s(ademco_event) + L"半布防";						
	case EVENT_TAMPER:								return n_to_s(ademco_event) + L"防拆";						
	case EVENT_ZONE_TAMPER:							return n_to_s(ademco_event) + L"防区防拆";					
	case EVENT_WATER:								return n_to_s(ademco_event) + L"水警";						
	case EVENT_BYPASS:								return n_to_s(ademco_event) + L"旁路";						
	case EVENT_BYPASS_RESUME:						return n_to_s(ademco_event) + L"解除旁路";					

	case EVENT_AC_BROKE:							return n_to_s(ademco_event) + L"主机AC掉电";					
	case EVENT_AC_RECOVER:							return n_to_s(ademco_event) + L"主机AC恢复";					
	case EVENT_LOWBATTERY:							return n_to_s(ademco_event) + L"低电";						
	case EVENT_BATTERY_RECOVER: 					return n_to_s(ademco_event) + L"复电";						
	case EVENT_BADBATTERY:							return n_to_s(ademco_event) + L"坏电";						
	case EVENT_SOLARDISTURB:						return n_to_s(ademco_event) + L"光扰";						
	case EVENT_DISCONNECT:							return n_to_s(ademco_event) + L"失效";						
	case EVENT_RECONNECT:							return n_to_s(ademco_event) + L"恢复";						
	case EVENT_LOST:								return n_to_s(ademco_event) + L"失联";						
	case EVENT_LOST_RECOVER:						return n_to_s(ademco_event) + L"恢复";						
	case EVENT_3100:								return n_to_s(ademco_event) + L"液晶主机清除异常指示";		
	case EVENT_BATTERY_EXCEPTION:					return n_to_s(ademco_event) + L"防区电源故障";				
	case EVENT_BATTERY_EXCEPTION_RECOVER:			return n_to_s(ademco_event) + L"防区电源故障恢复";			
	case EVENT_OTHER_EXCEPTION:						return n_to_s(ademco_event) + L"防区其他故障"; 				
	case EVENT_OTHER_EXCEPTION_RECOVER:				return n_to_s(ademco_event) + L"防区其他故障恢复"; 			

	case EVENT_SERIAL485DIS:						return n_to_s(ademco_event) + L"485断开";					
	case EVENT_SERIAL485CONN:						return n_to_s(ademco_event) + L"485恢复";					
	case EVENT_DOORRINGING:							return n_to_s(ademco_event) + L"门铃";						
	case EVENT_CONN_HANGUP:							return n_to_s(ademco_event) + L"链路挂起"; 					
	case EVENT_CONN_RESUME:							return n_to_s(ademco_event) + L"链路恢复"; 					
	case EVENT_DISARM_PWD_ERR: 						return n_to_s(ademco_event) + L"撤防密码错误";				

	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:		return n_to_s(ademco_event) + L"分防区异常"; 				
	case EVENT_SUB_MACHINE_SENSOR_RESUME:			return n_to_s(ademco_event) + L"分防区恢复"; 				
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:			return n_to_s(ademco_event) + L"分防区电源异常"; 				
	case EVENT_SUB_MACHINE_POWER_RESUME:			return n_to_s(ademco_event) + L"分防区电源恢复"; 				
	case EVENT_COM_PASSTHROUGH:						return n_to_s(ademco_event) + L"串口透传";
	case EVENT_ENTER_SET_MODE:						return n_to_s(ademco_event) + L"进入设置状态"; 				
	case EVENT_EXIT_SET_MODE:						return n_to_s(ademco_event) + L"拒绝/停止设置";
	case EVENT_QUERY_SUB_MACHINE:					return n_to_s(ademco_event) + L"查询"; 						
	case EVENT_WRITE_TO_MACHINE:					return n_to_s(ademco_event) + L"写入主机信息"; 				
	case EVENT_I_AM_NET_MODULE:						return n_to_s(ademco_event) + L"我是网络模块"; 				
	case EVENT_PHONE_USER_SOS:						return n_to_s(ademco_event) + L"手机用户SOS"; 				
	case EVENT_PHONE_USER_CANCLE_ALARM:				return n_to_s(ademco_event) + L"手机用户消警"; 				
	case EVENT_I_AM_GPRS:							return n_to_s(ademco_event) + L"我是GPRS主机";				
	case EVENT_I_AM_LCD_MACHINE:					return n_to_s(ademco_event) + L"我是液晶主机";				
	case EVENT_I_AM_WIRE_MACHINE:					return n_to_s(ademco_event) + L"我是网线主机";				
	case EVENT_I_AM_WIFI_MACHINE:					return n_to_s(ademco_event) + L"我是WiFi主机";				
	case EVENT_I_AM_3_SECTION_MACHINE:				return n_to_s(ademco_event) + L"我是三区段主机";				
	case EVENT_I_AM_IOT_MACHINE:					return n_to_s(ademco_event) + L"我是物联卡主机";	
	case EVENT_I_AM_TRUE_COLOR:						return n_to_s(ademco_event) + L"我是真彩主机";
	case EVENT_I_AM_GPRS_IOT:						return n_to_s(ademco_event) + L"我是简化版物联卡主机";
	case EVENT_I_AM_GPRS_PHONE:						return n_to_s(ademco_event) + L"我是GPRS主机能打电话";
	case EVENT_SIM_IS_IOT_CARD:						return n_to_s(ademco_event) + L"SIM卡为物联卡";	
	case EVENT_SIM_IS_IOT_PLATFORM_CARD:			return n_to_s(ademco_event) + L"SIM卡为平台物联卡";				
	case EVENT_SIM_IS_NOT_IOT_CARD:					return n_to_s(ademco_event) + L"SIM卡为非物联卡";				
	case EVENT_ENTER_SETTING_MODE:					return n_to_s(ademco_event) + L"主机进入设置状态";			
	case EVENT_EXIT_SETTING_MODE:					return n_to_s(ademco_event) + L"主机退出设置状态";			
	case EVENT_RESTORE_FACTORY_SETTINGS_710:
	case EVENT_RESTORE_FACTORY_SETTINGS:			return n_to_s(ademco_event) + L"主机恢复出厂设置";			
	case EVENT_WHAT_IS_YOUR_TYPE:					return n_to_s(ademco_event) + L"索要主机类型";				
	case EVENT_SIGNAL_STRENGTH_CHANGED:				return n_to_s(ademco_event) + L"信号强度变化";				
	default:			with_event_number = true;	return n_to_s(ademco_event) + L"未定义";						
	}
}
#endif

//! 是否主机状态事件
static inline bool isMachineStatusEvent(ADEMCO_EVENT ademco_event)
{
	return ademco_event == EVENT_ARM
		|| ademco_event == EVENT_HALFARM
		|| ademco_event == EVENT_HALFARM_1456
		|| ademco_event == EVENT_DISARM;
}

//! 是否主机类型事件
static inline bool isMachineTypeEvent(ADEMCO_EVENT ademco_event)
{
	return ademco_event == EVENT_I_AM_NET_MODULE
		|| ademco_event == EVENT_I_AM_GPRS
		|| ademco_event == EVENT_I_AM_LCD_MACHINE
		|| ademco_event == EVENT_I_AM_WIRE_MACHINE
		|| ademco_event == EVENT_I_AM_WIFI_MACHINE
		|| ademco_event == EVENT_I_AM_3_SECTION_MACHINE
		|| ademco_event == EVENT_I_AM_IOT_MACHINE
		|| ademco_event == EVENT_I_AM_TRUE_COLOR
		|| ademco_event == EVENT_I_AM_GPRS_IOT
		|| ademco_event == EVENT_I_AM_GPRS_PHONE;
}

//! 事件是否需要控制源
static inline bool isEventNeedControlSource(ADEMCO_EVENT ademco_event)
{
	return ademco_event == EVENT_ARM
		|| ademco_event == EVENT_HALFARM
		|| ademco_event == EVENT_HALFARM_1456
		|| ademco_event == EVENT_DISARM
		|| ademco_event == EVENT_EMERGENCY
		|| ademco_event == EVENT_DURESS
		|| ademco_event == EVENT_PHONE_USER_CANCLE_ALARM;
}

// 安定宝事件级别
enum EventLevel
{
	EVENT_LEVEL_NULL,
	EVENT_LEVEL_STATUS,				// 主机状态
	EVENT_LEVEL_EXCEPTION_RESUME,	// 黄色报警
	EVENT_LEVEL_EXCEPTION,			// 橙色报警
	EVENT_LEVEL_ALARM,				// 红色报警
};

//! 获取安定宝事件级别
inline EventLevel GetEventLevel(ADEMCO_EVENT ademco_event)
{
	switch (ademco_event) {
	case EVENT_ARM:
	case EVENT_DISARM:
	case EVENT_HALFARM:
	case EVENT_HALFARM_1456:
		return EVENT_LEVEL_STATUS;

	case EVENT_AC_RECOVER:
	case EVENT_RECONNECT:
	case EVENT_LOST_RECOVER:
	case EVENT_SERIAL485CONN:
	case EVENT_SUB_MACHINE_SENSOR_RESUME:
	case EVENT_SUB_MACHINE_POWER_RESUME:
	case EVENT_BATTERY_RECOVER:
	case EVENT_BATTERY_EXCEPTION_RECOVER:
	case EVENT_OTHER_EXCEPTION_RECOVER:
		return EVENT_LEVEL_EXCEPTION_RESUME;

	case EVENT_AC_BROKE:
	case EVENT_LOWBATTERY:
	case EVENT_BADBATTERY:
	case EVENT_SOLARDISTURB:
	case EVENT_DISCONNECT:
	case EVENT_LOST:
		//case EVENT_DOORRINGING: //case EVENT_CONN_HANGUP: //case EVENT_CONN_RESUME:
	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:
	case EVENT_BATTERY_EXCEPTION:
	case EVENT_OTHER_EXCEPTION:
		return EVENT_LEVEL_EXCEPTION;

	case EVENT_SERIAL485DIS:
	case EVENT_BURGLAR:
	case EVENT_DURESS:
	case EVENT_EMERGENCY:
	case EVENT_FIRE:
	case EVENT_GAS:
	case EVENT_TAMPER:
	case EVENT_ZONE_TAMPER:
	case EVENT_WATER:
		return EVENT_LEVEL_ALARM;

	default:
		return EVENT_LEVEL_NULL;
	}
}

//! 获取异常恢复事件所对应的异常事件
inline ADEMCO_EVENT getExceptionEventByResumeEvent(ADEMCO_EVENT resume_event)
{
	switch (resume_event) {
	case EVENT_RECONNECT:					return EVENT_DISCONNECT;	
	case EVENT_LOST_RECOVER:				return EVENT_LOST;						
	case EVENT_SERIAL485CONN:				return EVENT_SERIAL485DIS;				
	case EVENT_SUB_MACHINE_SENSOR_RESUME:	return EVENT_SUB_MACHINE_SENSOR_EXCEPTION;
	case EVENT_SUB_MACHINE_POWER_RESUME:	return EVENT_SUB_MACHINE_POWER_EXCEPTION;
	case EVENT_BATTERY_RECOVER:				return EVENT_LOWBATTERY;				
	case EVENT_BATTERY_EXCEPTION_RECOVER:	return EVENT_BATTERY_EXCEPTION;			
	case EVENT_OTHER_EXCEPTION_RECOVER:		return EVENT_OTHER_EXCEPTION;			
	case EVENT_AC_RECOVER:					return EVENT_AC_BROKE;					
	default:								return EVENT_INVALID_EVENT;				
	}
}


} // namespace ademco
