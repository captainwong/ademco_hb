﻿#include "../../ademco.h"
#include "../../hb_com.h"
#include <jlib/win32/UnicodeTool.h>
#include <jlib/win32/path_op.h>
#include <jlib/util/std_util.h>
#include <map>


#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/ademco.lib")
#else
#pragma comment(lib, "../x64/Release/ademco.lib")
#endif
#else // WIN32
#ifdef _DEBUG
#pragma comment(lib, "../Debug/ademco.lib")
#else
#pragma comment(lib, "../Release/ademco.lib")
#endif
#endif

const char* print_bool(bool b) {
	return b ? "√" : " "; // "×";
}

AdemcoEvent statusEvents[] = {

	// 主机或分机状态报告
	EVENT_ARM,
	EVENT_DISARM,
	EVENT_HALFARM_1456,
	EVENT_HALFARM,
	EVENT_EMERGENCY,
};

AdemcoEvent alarmEvents[] = {

	// 防区报警
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

};

AdemcoEvent excepEvents[] = {

	// 防区异常
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
	EVENT_BATTERY_EXCEPTION,
	EVENT_BATTERY_EXCEPTION_RECOVER,
	EVENT_OTHER_EXCEPTION,
	EVENT_OTHER_EXCEPTION_RECOVER,
	EVENT_LOST,
	EVENT_LOST_RECOVER,

	// 标准事件，但不做处理
	EVENT_3100,

};

AdemcoEvent privateEvents[] = {

	// ------------------私有事件-----------------------------------------
	EVENT_SERIAL485DIS,
	EVENT_SERIAL485CONN,

	EVENT_CONN_HANGUP,
	EVENT_CONN_RESUME,

	EVENT_DISARM_PWD_ERR,

	EVENT_SUB_MACHINE_SENSOR_EXCEPTION ,
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
	EVENT_I_AM_WIFI2_MACHINE,

	EVENT_PHONE_USER_SOS,
	EVENT_PHONE_USER_CANCLE_ALARM,

	EVENT_ENTER_SETTING_MODE,
	EVENT_EXIT_SETTING_MODE,
	EVENT_RESTORE_FACTORY_SETTINGS_710,
	EVENT_RESTORE_FACTORY_SETTINGS,

	EVENT_SIM_IS_IOT_CARD,
	EVENT_SIM_IS_IOT_PLATFORM_CARD,
	EVENT_SIM_IS_NOT_IOT_CARD,

	EVENT_WHAT_IS_YOUR_TYPE,
	EVENT_SIGNAL_STRENGTH_CHANGED,

	EVENT_OFFLINE,
	EVENT_ONLINE,
};

void printEvents(const AdemcoEvent* events, size_t len)
{
	printf("|事件码|含义|en|\n|-----|----|--|\n");
	for (size_t i = 0; i < len; i++) {
		auto e = events[i];
		printf("|%04d|%s|%s|\n", e, 
			   ademcoEventToStringChinese(e), 
			   ademcoEventToString(e));
	}
	printf("\n");
}


const char* get_core_author(HbMachineType t)
{
	switch (t) {
	case HMT_WIFI:
	case HMT_GPRS_IOT:
	case HMT_GPRS:
	case HMT_WIRED:
	case HMT_WIFI2:
		return "wzq";

	case HMT_NETMOD:
	case HMT_LCD:
	case HMT_TRUE_COLOR:
	case HMT_3_SECTION:
	case HMT_IOT:
	case HMT_GPRS_PHONE:
		return "jjf";
	}

	return "";
}

const char* get_net_author(HbMachineType t)
{
	switch (t) {
	case HMT_WIFI:
	case HMT_WIRED:
	case HMT_NETMOD:
	case HMT_GPRS_IOT:
	case HMT_GPRS:
	case HMT_LCD:
	case HMT_TRUE_COLOR:
	case HMT_3_SECTION:
	case HMT_IOT:
	case HMT_GPRS_PHONE:
	case HMT_WIFI2:
		return "qfm";
	}

	return "";
}

std::vector<std::string> get_machine_brands(HbMachineType t)
{
	switch (t) {
	case HMT_WIFI:
	case HMT_CAMERA: return {};
	case HMT_GPRS_IOT: return { "5050G-4GW", };
	case HMT_NETMOD: return { "G250" };
	case HMT_GPRS: return { "4040G", "5050G", "5050G-4G",  };
	case HMT_LCD: return { "BJQ560", "BJQ560B" };
	case HMT_WIRED: return { "4040R", "5050R" };
	case HMT_TRUE_COLOR:return { "G1000", "G1000-4G" };
	case HMT_3_SECTION:return { "G1000", "G1000-4G" };
	case HMT_IOT: return { "2050-4GW" };
	case HMT_GPRS_PHONE: return { "2050" };
	//case hb::common::Nb: return { "" };
	default: return {};
		break;
	}
}

std::string brand_to_path(const std::string& brand)
{
	std::vector<std::string> exts = { "png", "jpg" };
	std::string path = jlib::win32::utf16_to_mbcs(L"..\\..\\docs\\主机类型\\smartresize\\HB-") + brand;
	for (auto ext : exts) {
		if (jlib::win32::fileExists(path + "-web." + ext)) {
			return jlib::win32::utf16_to_mbcs(L"./主机类型/smartresize/HB-") + brand + "-web." + ext;
		}
	}
	return {};
}

void print_machine_brands(HbMachineType t)
{
	printf("|<ul>");
	for (auto brand : get_machine_brands(t)) {		
		auto path = brand_to_path(brand);
		if (!path.empty()) {
			printf(jlib::win32::utf16_to_mbcs(LR"(<li>%s</li>)").data(), brand.data());// , brand.data(), path.data());
		}
		
	}
	printf("</ul>");
}

void print_machineTypes()
{
	printf("### *恒博主机类型对照表*\n\n");

	printf("*SMS指主机自身是否可以拨打电话、发送短信，不是指通过阿里语音打电话*\n");

	printf("|事件码类型|主机类型|布防|撤防|半布防|设置|信号强度|防区|有线防区|SMS|内核|网络|型号|\n"
		   "|---------|-------|----|----|-----|----|-------|----|-------|---|----|---|----|\n");

	for (auto e : AdemcoEvents) {
		if (ademcoIsMachineTypeEvent(e)) { 
			auto t = hbMachineTypeFromAdemcoEvent(e); 
			if(!hbMachineIsSelling(t)) continue; 

			printf("|%04d %s", (int)e, ademcoEventToStringChinese(e));
			printf("|%s", hbMachineTypeToStringChinese(t));
			printf("|%s", print_bool(hbMachineCanArm(t)));
			printf("|%s", print_bool(hbMachineCanDisarm(t)));
			printf("|%s", print_bool(hbMachineCanHalfArm(t)));
			printf("|%s", print_bool(hbMachineCanEnterSettings(t)));
			printf("|%s", print_bool(hbMachineCanReportSignalStrength(t)));
			printf("|1~%d", hbZoneMax(t));
			if (hbMachineHasWiredZone(t)) {
				printf("|%d~%d", hbWiredZoneMin(t), hbWiredZoneMax(t));
			} else {
				printf("| ");
			}
			printf("|%s", print_bool(hbMachineCanReportBySMS(t)));
			printf("|%s", get_core_author(t));
			printf("|%s", get_net_author(t));
			print_machine_brands(t);
			printf("|\n");
		}
	}

}

void print_imgs()
{
	printf("\n\n### *恒博主机型号示例图片*\n\n");

	printf("|型号|示例图片|\n"
		   "|---|--------|\n");

	std::map<std::string, std::string> imgs;
	for (auto e : AdemcoEvents) {
		if (ademcoIsMachineTypeEvent(e)) {
			auto t = hbMachineTypeFromAdemcoEvent(e);
			if (!hbMachineIsSelling(t)) continue;

			for (auto brand : get_machine_brands(t)) {
				auto path = brand_to_path(brand);
				if (!path.empty()) {
					imgs[brand] = path;
				}

			}
		}
	}
	for (auto i : imgs) {
		printf(R"(|%s|<img alt="%s" src="%s" />|)" "\n", i.first.data(), i.first.data(), i.second.data());
	}
	printf("\n\n");
}

bool zprop_is_contain(HbZoneProperty prop) {
	for (const auto& zp : hbZoneProperties) {
		if (zp == prop)return true;
	}
	return false;
}

void print_available_zone_props()
{
	printf("### *恒博主机类型与支持的防区属性对照表*\n\n");

	
	//auto all_props = getAvailableZoneProperties();

	auto print_prop = [](HbZoneProperty zp) {
		printf("%02X %s", zp, hbZonePropertyToStringChinese(zp));
	};




	printf("* 防区属性是否支持失联报告\n\n");
	printf("|");
	for (auto zp : hbZoneProperties) {
		printf("|"); print_prop(zp);
	}
	printf("|\n");

	printf("|----");
	for (size_t i = 0; i < sizeof(hbZoneProperties) / sizeof(hbZoneProperties[0]); i++) {
		printf("|----");
	}
	printf("|\n");

	printf("|失联支持");
	for (auto zp : hbZoneProperties) {
		printf("|%s", print_bool(hbZonePropCanReportLost(zp)));
	}
	printf("|\n\n");


	printf("* 主机类型与支持的防区属性对照表\n\n");
	printf("|事件码|类型|型号");
	for (auto zp : hbZoneProperties) {
		//printf("|"); print_prop(zp);
		printf("|%02X", (int)zp);
	}
	printf("|\n");

	printf("|----|----|----");
	for (size_t i = 0; i < sizeof(hbZoneProperties) / sizeof(hbZoneProperties[0]); i++) {
		printf("|----");
	}
	printf("|\n");


	for (auto e : AdemcoEvents) {
		if (ademcoIsMachineTypeEvent(e)) {
			auto t = hbMachineTypeFromAdemcoEvent(e);
			if (!hbMachineIsSelling(t)) continue;
			printf("|%04d", (int)e);
			printf("|%d", (int)t);
			print_machine_brands(t);
			HbZoneProperty avail_props[12];
			int count = hbGetAvailableZoneProperties(t, avail_props);
			for (auto zp : hbZoneProperties) {
				printf("|%s", print_bool(zprop_is_contain(zp)));
			}
			printf("|\n");
		}
	}


}


void print_g250_alarm_codes()
{
	//printf("# 工程主机串口通信协议\n\n");
	//printf("9600, N, 8, 1\n\n");
	//printf("SUM = DATA(N) = DATA(0) + DATA(1) + ... + DATA(N-1)\n\n");

	//// 按键码
	//printf("\n\n## EB AB addr data sum\n");
	//printf("PC到主机，按键\n");
	//printf("* addr: PC模拟键盘地址，1~4，暂时固定为3\n");
	//printf("* data: 按键码\n\n");
	//printf("|按键码|按键|\n");
	//printf("|-----|----|\n");
	//for (Key k = Key::Key_NULL; k <= Key::Key_STOP_ALARM; k = Key(k + 1)) {
	//	auto s = keyToString(k);
	//	if (s) {
	//		printf("|%02X|%s|\n", k, jlib::win32::utf16_to_mbcs(s).c_str());
	//	}
	//}


	//// EB B1 报警码
	//printf("\n\n## EB B1 data0 data1 data2 code data4 sum\n");
	//printf("主机到PC事件报告\n");
	//printf("* data0: 命令字总字长，固定为8\n");
	//printf("* data1: 防区号高位\n");
	//printf("* data2: 防区号低位\n");
	//printf("* data4: 00 表示data1与data2为主机直属防区号，01~F0 表示data1与data2为分机防区号（已废弃），EE 表示分机自身状态（已废弃）\n");
	//printf("* code: 报警码\n\n");
	//printf("|报警码|安定宝事件码|含义|\n");
	//printf("|------|----------|----|\n");

	//static Char codes[] = {
	//	g250::MachineStatus::Arm,
	//	g250::MachineStatus::Disarm,
	//	g250::MachineStatus::HalfArm,
	//	AlarmCode::MACHINE_EMERGENCY,
	//	AlarmCode::ALARM_BURGLAR,
	//	AlarmCode::ALARM_FIRE,
	//	AlarmCode::ALARM_DURESS,
	//	AlarmCode::ALARM_GAS,
	//	AlarmCode::ALARM_WATER,
	//	AlarmCode::ALARM_TAMPER,
	//	AlarmCode::ALARM_S_BATTERY_LOW,
	//	AlarmCode::ALARM_R_BATTERY_LOW,
	//	AlarmCode::ALARM_S_BATTERY_BROKE,
	//	AlarmCode::ALARM_R_BATTERY_BROKE,
	//	AlarmCode::ALARM_BETTERY_RECOVER,
	//	AlarmCode::ALARM_SOLAR_DISTURB,
	//	AlarmCode::ALARM_SOLAR_RECOVER,
	//	AlarmCode::ALARM_LONGTIME_DISCONN,
	//	AlarmCode::ALARM_LONGTIME_RECOVER,
	//	AlarmCode::ALARM_DOOR_RING,
	//	AlarmCode::ALARM_SM_EXCEPTION,
	//	AlarmCode::ALARM_SM_EXCEPT_RESUME,
	//	AlarmCode::ALARM_SM_POWER_EXCEPT,
	//	AlarmCode::ALARM_SM_POWER_RESUME,
	//	AlarmCode::ALARM_AC_BROKE,
	//	AlarmCode::ALARM_AC_RECOVER,
	//};

	//for (auto code : codes) {
	//	printf("|%02X|%04d|%s|\n", code, ademcoEventFromCode(code), jlib::win32::utf16_to_mbcs(ademcoEventToStringChinese(ademcoEventFromCode(code), false)).c_str());
	//}

}

int main()
{
	if (1) {
		printf("### 主机状态\n\n");
		printEvents(statusEvents, _countof(statusEvents));

		printf("### 防区报警\n\n");
		printEvents(alarmEvents, _countof(alarmEvents));

		printf("### 防区异常\n\n");
		printEvents(excepEvents, _countof(excepEvents));

		printf("### *恒博私有事件码*\n\n");
		printEvents(privateEvents, _countof(privateEvents));

		print_machineTypes();
		print_imgs();

		print_available_zone_props();


	} else {
		print_g250_alarm_codes();

	}


}