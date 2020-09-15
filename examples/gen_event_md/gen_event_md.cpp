
#define ENABLE_ADEMCO_EVENT_TO_STRING
#include "../../include/ademco_event.h"
#define ENABLE_COMMON_MACHINE_TYPE_TO_STRING
#define ENABLE_COMMON_ZONE_PROPERTY_TO_STRING
#include "../../include/hb_detail.h"
#include "../../include/hb_helper.h"
#include <jlib/win32/UnicodeTool.h>
#include <jlib/win32/path_op.h>
#include <jlib/util/std_util.h>
#include <map>

using namespace ademco;
using namespace hb;
using namespace hb::common;

const char* print_bool(bool b) {
	return b ? "√" : " "; // "×";
}

ADEMCO_EVENT statusEvents[] = {

	// 主机或分机状态报告
	EVENT_ARM,
	EVENT_DISARM,
	EVENT_HALFARM_1456,
	EVENT_HALFARM,
	EVENT_EMERGENCY,
};

ADEMCO_EVENT alarmEvents[] = {

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

ADEMCO_EVENT excepEvents[] = {

	// 防区异常
	EVENT_AC_BROKE,
	EVENT_AC_RECOVER,
	EVENT_LOWBATTERY,
	EVENT_BATTERY_RECOVER,
	EVENT_BADBATTERY,
	EVENT_SOLARDISTURB,
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

ADEMCO_EVENT privateEvents[] = {

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
};

void printEvents(const ADEMCO_EVENT* events, size_t len)
{
	printf("|事件码|含义|en|\n|-----|----|--|\n");
	for (size_t i = 0; i < len; i++) {
		auto e = events[i];
		printf("|%04d|%s|%s|\n", e, 
			   jlib::win32::utf16_to_mbcs(ademcoEventToStringChinese(e, false)).data(), 
			   ademcoEventToStringEnglish(e, false).data());
	}
	printf("\n");
}


const char* get_core_author(MachineType t)
{
	switch (t) {
	case hb::common::WiFi:
	case hb::common::Gprs_IoT:
	case hb::common::Gprs:
	case hb::common::Wired:
		return "wzq";

	case hb::common::NetMod:
	case hb::common::Lcd:
	case hb::common::TrueColor:
	case hb::common::ThreeSection:
	case hb::common::IoT:
	case hb::common::Gprs_Phone:
		return "jjf";
	}

	return "";
}

const char* get_net_author(MachineType t)
{
	switch (t) {
	case hb::common::WiFi:
	case hb::common::Wired:
	case hb::common::NetMod:
		return "wzq";

	case hb::common::Gprs_IoT:
	case hb::common::Gprs:
	case hb::common::Lcd:
	case hb::common::TrueColor:
	case hb::common::ThreeSection:
	case hb::common::IoT:
	case hb::common::Gprs_Phone:
		return "qfm";
	}

	return "";
}

std::vector<std::string> get_machine_brands(MachineType t)
{
	switch (t) {
	case hb::common::WiFi:
	case hb::common::Camera: return {};

	case hb::common::Gprs_IoT: return { "HB-5050G-4GW", };

	case hb::common::NetMod: return { "HB-G250" };

	case hb::common::Gprs: return { "HB-4040G", "HB-5050G", "HB-5050G-4G",  };

	case hb::common::Lcd: return { "HB-BJQ560", "HB-BJQ560B" };

	case hb::common::Wired: return { "HB-4040R", "HB-5050R" };

	case hb::common::TrueColor:return { "HB-G1000", "HB-G1000-4G" };

	case hb::common::ThreeSection:return { "HB-G1000", "HB-G1000-4G" };

	case hb::common::IoT: return { "HB-2050-4GW" };

	case hb::common::Gprs_Phone: return { "HB-2050" };

	default: return {};
		break;
	}
}

std::string brand_to_path(const std::string& brand)
{
	std::vector<std::string> exts = { "png", "jpg" };
	std::string path = jlib::win32::utf16_to_mbcs(L"..\\..\\docs\\主机类型\\smartresize\\") + brand;
	for (auto ext : exts) {
		if (jlib::win32::fileExists(path + "-web." + ext)) {
			return jlib::win32::utf16_to_mbcs(L"./主机类型/smartresize/") + brand + "-web." + ext;
		}
	}
	return {};
}

void _print_machine_img(MachineType t)
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

	printf("|事件码类型|主机类型|半布防|信号强度|防区|有线防区|SMS|内核|网络|型号|\n"
		   "|---------|-------|------|-------|----|-------|---|---|---|----|\n");

	for (auto e : AdemcoEvents) {
		if (isMachineTypeEvent(e)) {
			auto t = hb::machineTypeFromAdemcoEvent(e);
			if(!machineIsSelling(t)) continue;

			printf("|%04d %s", (int)e, jlib::win32::utf16_to_mbcs(ademcoEventToStringChinese(e, false)).data());
			printf("|%s", jlib::win32::utf16_to_mbcs(machineTypeToWString(t)).data());
			printf("|%s", print_bool(machineCanHalfArm(t)));
			printf("|%s", print_bool(machineCanReportSignalStrength(t)));
			printf("|1~%d", zoneMax(t));
			if (machineHasWiredZone(t)) {
				printf("|%d~%d", wiredZoneMin(t), wiredZoneMax(t));
			} else {
				printf("| ");
			}
			printf("|%s", print_bool(machineCanReportBySMS(t)));
			printf("|%s", get_core_author(t));
			printf("|%s", get_net_author(t));
			_print_machine_img(t);
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
		if (isMachineTypeEvent(e)) {
			auto t = hb::machineTypeFromAdemcoEvent(e);
			if (!machineIsSelling(t)) continue;

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

void print_available_zone_props()
{
	printf("### *恒博主机类型与支持的防区属性对照表*\n\n");

	auto all_props = getAvailableZoneProperties();

	auto print_prop = [](ZoneProperty zp) {
		printf("%02X %s", (Char)zp, jlib::win32::utf16_to_mbcs(zonePropertyToStringChinese(zp)).data());
	};

	printf("* 防区属性是否支持防拆\n\n");
	printf("|");
	for (auto zp : all_props) {
		printf("|"); print_prop(zp);
	}
	printf("|\n");

	printf("|----");
	for (size_t i = 0; i < all_props.size(); i++) {
		printf("|----");
	}
	printf("|\n");

	printf("|防拆支持");
	for (auto zp : all_props) {
		printf("|%s", print_bool(zonePropCanReportTamper(zp)));
	}
	printf("|\n\n");



	printf("* 主机类型与支持的防区属性对照表\n\n");
	printf("|事件码类型|主机类型");
	for (auto zp : all_props) {
		printf("|"); print_prop(zp);
	}
	printf("|\n");

	printf("|---------|-------");
	for (size_t i = 0; i < all_props.size(); i++) {
		printf("|----");
	}
	printf("|\n");


	for (auto e : AdemcoEvents) {
		if (isMachineTypeEvent(e)) {
			auto t = hb::machineTypeFromAdemcoEvent(e);
			if (!machineIsSelling(t)) continue;
			printf("|%04d %s", (int)e, jlib::win32::utf16_to_mbcs(ademcoEventToStringChinese(e, false)).data());
			printf("|%s", jlib::win32::utf16_to_mbcs(machineTypeToWString(t)).data());
			auto avail_props = getAvailableZoneProperties(t);
			for (auto zp : all_props) {
				printf("|%s", print_bool(jlib::is_contain(avail_props, zp)));
			}
			printf("|\n");
		}
	}

}

int main()
{
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
}