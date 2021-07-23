#pragma once

#include "hb_detail.h"

namespace hb
{

/******** 结合 hb 与 ademco ******/

//! 从事件码推出主机类型
static common::MachineType machineTypeFromAdemcoEvent(ademco::ADEMCO_EVENT e) {
	switch (e) {
	case ademco::EVENT_I_AM_NET_MODULE:		    return common::MachineType::NetMod;
	case ademco::EVENT_I_AM_GPRS: 			    return common::MachineType::Gprs;
	case ademco::EVENT_I_AM_LCD_MACHINE: 	    return common::MachineType::Lcd;
	case ademco::EVENT_I_AM_WIRE_MACHINE: 	    return common::MachineType::Wired;
	case ademco::EVENT_I_AM_WIFI_MACHINE: 	    return common::MachineType::WiFi;
	case ademco::EVENT_I_AM_3_SECTION_MACHINE: 	return common::MachineType::ThreeSection;
	case ademco::EVENT_I_AM_IOT_MACHINE: 		return common::MachineType::IoT;
	case ademco::EVENT_I_AM_TRUE_COLOR:			return common::MachineType::TrueColor;
	case ademco::EVENT_I_AM_GPRS_IOT:			return common::MachineType::Gprs_IoT;
	case ademco::EVENT_I_AM_GPRS_PHONE:			return common::MachineType::Gprs_Phone;
	case ademco::EVENT_I_AM_NB_MACHINE:			return common::MachineType::Nb;
	default: 								    return common::MachineType::InvalidMachineType;
	}
}

//! 从主机类型推出事件码
static ademco::ADEMCO_EVENT ademcoEventFromMachineType(common::MachineType type)
{
	switch (type) {
	case common::MachineType::WiFi: 	        return ademco::EVENT_I_AM_WIFI_MACHINE;
	case common::MachineType::Gprs_IoT:			return ademco::EVENT_I_AM_GPRS_IOT;
	case common::MachineType::NetMod:	        return ademco::EVENT_I_AM_NET_MODULE;
	case common::MachineType::Gprs:	            return ademco::EVENT_I_AM_GPRS;
	case common::MachineType::Lcd:		        return ademco::EVENT_I_AM_LCD_MACHINE;
	case common::MachineType::Wired: 	        return ademco::EVENT_I_AM_WIRE_MACHINE;
	case common::MachineType::TrueColor:		return ademco::EVENT_I_AM_TRUE_COLOR;
	case common::MachineType::ThreeSection: 	return ademco::EVENT_I_AM_3_SECTION_MACHINE;
	case common::MachineType::IoT:				return ademco::EVENT_I_AM_IOT_MACHINE;
	case common::MachineType::Gprs_Phone:		return ademco::EVENT_I_AM_GPRS_PHONE;
	case common::MachineType::Nb:				return ademco::EVENT_I_AM_NB_MACHINE;
	default: 						            return ademco::EVENT_INVALID_EVENT;
	}
}

//! 从事件码推出主机状态
static common::MachineStatus machineStatusFromAdemcoEvent(ademco::ADEMCO_EVENT e)
{
	switch (e) {
	case ademco::EVENT_ARM:		return common::MachineStatus::Arm;
	case ademco::EVENT_DISARM:	return common::MachineStatus::Disarm;
    
	case ademco::EVENT_HALFARM:  
	case ademco::EVENT_HALFARM_1456:
								return common::MachineStatus::HalfArm;

	case ademco::EVENT_ENTER_SETTING_MODE:  
	case ademco::EVENT_ENTER_SET_MODE:
								return common::MachineStatus::Setting;

	default:					return common::MachineStatus::InvalidMachineStatus;
	}
}

//! 从主机状态推出事件码
static ademco::ADEMCO_EVENT ademcoEventFromMachineStatus(common::MachineStatus status)
{
	switch (status) {
	case common::MachineStatus::Arm:		return ademco::EVENT_ARM;
	case common::MachineStatus::Disarm:	    return ademco::EVENT_DISARM;
	case common::MachineStatus::HalfArm:	return ademco::EVENT_HALFARM;				// ademco::EVENT_HALFARM_1456
	case common::MachineStatus::Setting:	return ademco::EVENT_ENTER_SETTING_MODE;	// ademco::EVENT_ENTER_SET_MODE
	default:							    return ademco::EVENT_INVALID_EVENT;
	}
}

//! 从防区属性推出防区应该触发什么警情事件码
static ademco::ADEMCO_EVENT ademcoEventFromZoneProperty(common::ZoneProperty zp)
{
	switch (zp) {
	case hb::common::Buglar:		return ademco::EVENT_BURGLAR;
	case hb::common::BuglarHalf:	return ademco::EVENT_BURGLAR;
	case hb::common::Emergency:	    return ademco::EVENT_EMERGENCY;
	case hb::common::Fire:			return ademco::EVENT_FIRE;
	case hb::common::Duress:		return ademco::EVENT_DURESS;
	case hb::common::Gas:			return ademco::EVENT_GAS;
	case hb::common::Water:		    return ademco::EVENT_WATER;
	case hb::common::DoorRing:		return ademco::EVENT_DOORRINGING;

	case hb::common::SubMachine:
	case hb::common::RemoteControl:
	case hb::common::Shield:
	case hb::common::Reserved0B:
	case hb::common::Reserved0C:
	case hb::common::Reserved0D:
	case hb::common::Reserved0E:
	case hb::common::Bypass:
	case hb::common::ZonePropertyCount:
	case hb::common::InvalidZoneProperty:
	default:					return ademco::EVENT_INVALID_EVENT;
	}
}

//! 从防区属性推出应该发送什么警情事件码
static ademco::ADEMCO_EVENT ademcoEventFromZonePropertyOld(g250::ZoneProperty zp)
{
	switch (zp) {
	case hb::g250::ZP_GLOBAL:		return ademco::EVENT_BURGLAR;
	case hb::g250::ZP_HALF:			return ademco::EVENT_BURGLAR;
	case hb::g250::ZP_EMERGENCY:	return ademco::EVENT_EMERGENCY;
	case hb::g250::ZP_DOORRING:		return ademco::EVENT_DOORRINGING;
	case hb::g250::ZP_FIRE:			return ademco::EVENT_FIRE;
	case hb::g250::ZP_DURESS:		return ademco::EVENT_DURESS;
	case hb::g250::ZP_GAS:			return ademco::EVENT_GAS;
	case hb::g250::ZP_WATER:		return ademco::EVENT_WATER;
	case hb::g250::ZP_SHIELD:
	default:						return ademco::EVENT_INVALID_EVENT;
	}
}

//! 获取主机防区数量的格式化字符串
static const char* getZoneFormatString(common::MachineType type)
{
	auto zone = common::zoneMax(type);
	if (zone < 10) { return "%d"; } 
	else if (zone < 100) { return "%02d"; } 
	else if (zone < 1000) { return "%03d"; } 
	else { return "%d"; }
}

//! 获取主机防区数量的格式化字符串
static const wchar_t* getZoneFormatStringW(common::MachineType type)
{
	auto zone = common::zoneMax(type);
	if (zone < 10) { return L"%d"; } 
	else if (zone < 100) { return L"%02d"; } 
	else if (zone < 1000) { return L"%03d"; } 
	else { return L"%d"; }
}
    
} // namespace hb
