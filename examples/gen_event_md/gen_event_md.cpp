#include "../../ademco.h"
#include "../../hb_core.h"
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

ademco_event_t statusEvents[] = {
#define XX(name, code, zh) EVENT_##name,
	ADEMCO_STATUS_EVENTS_MAP(XX)
#undef XX
};

ademco_event_t alarmEvents[] = {
#define XX(name, code, zh) EVENT_##name,
	ADEMCO_ALARM_EVENTS_MAP(XX)
#undef XX
};

ademco_event_t excepEvents[] = {
#define XX(name, code, zh) EVENT_##name,
	ADEMCO_EXEPTION_EVENTS_MAP(XX)
	ADEMCO_RESUME_EVENTS_MAP(XX)
#undef XX
};

ademco_event_t privateEvents[] = {
#define XX(name, code, zh) EVENT_##name,
	ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
};

ademco_event_t allEvents[] = {
#define XX(name, code, zh) EVENT_##name,
	ADEMCO_STATUS_EVENTS_MAP(XX)
	ADEMCO_ALARM_EVENTS_MAP(XX)
	ADEMCO_EXEPTION_EVENTS_MAP(XX)
	ADEMCO_RESUME_EVENTS_MAP(XX)
	ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
};

hb_zone_property_t allZoneProperties[] = {
#define XX(name, value, str) HZP_##name,
	HB_ZONE_PROPERTY_MAP(XX)
#undef XX
};

void printEvents(const ademco_event_t* events, size_t len)
{
	printf("|事件码|含义|en|\n|-----|----|--|\n");
	for (size_t i = 0; i < len; i++) {
		auto e = events[i];
		printf("|%04d|%s|%s|\n", e, 
			   ademco_event_to_string_chinese(e), 
			   ademco_event_to_string(e));
	}
	printf("\n");
}


const char* get_core_author(hb_machine_type_t t)
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
	case HMT_THREE_SECTION:
	case HMT_IOT:
	case HMT_GPRS_PHONE:
		return "jjf";
	}

	return "";
}

const char* get_net_author(hb_machine_type_t t)
{
	switch (t) {
	case HMT_WIFI:
	case HMT_WIRED:
	case HMT_NETMOD:
	case HMT_GPRS_IOT:
	case HMT_GPRS:
	case HMT_LCD:
	case HMT_TRUE_COLOR:
	case HMT_THREE_SECTION:
	case HMT_IOT:
	case HMT_GPRS_PHONE:
	case HMT_WIFI2:
		return "qfm";
	}

	return "";
}

std::vector<std::string> get_machine_brands(hb_machine_type_t t)
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
	case HMT_THREE_SECTION:return { "G1000", "G1000-4G" };
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

void print_machine_brands(hb_machine_type_t t)
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

	printf("|事件码|主机类型|布防|撤防|半布防|设置|信号强度|防区|有线防区|SMS|内核|网络|型号|\n"
		   "|------|-------|----|----|-----|----|-------|----|-------|---|----|---|----|\n");

	for (auto e : allEvents) {
		if (ademco_is_machine_type_event(e)) { 
			auto t = hb_machine_type_from_ademco_event(e); 
			if(!hb_is_machine_on_sale(t)) continue; 

			printf("|%04d %s", (int)e, ademco_event_to_string_chinese(e));
			printf("|%d %s", (int)t, hb_machine_type_to_string_chinese(t));
			printf("|%s", print_bool(hb_machine_can_arm(t)));
			printf("|%s", print_bool(hb_machine_can_disarm(t)));
			printf("|%s", print_bool(hb_machine_can_half_arm(t)));
			printf("|%s", print_bool(hb_machine_can_config(t)));
			printf("|%s", print_bool(hb_machine_can_report_signal_strength(t)));
			printf("|1~%d", hb_get_max_zone_by_type(t));
			if (hb_machine_has_wired_zones(t)) {
				printf("|%d~%d", hb_wired_zone_min(t), hb_wired_zone_max(t));
			} else {
				printf("| ");
			}
			printf("|%s", print_bool(hb_machine_can_report_by_sms(t)));
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
	for (auto e : allEvents) {
		if (ademco_is_machine_type_event(e)) {
			auto t = hb_machine_type_from_ademco_event(e);
			if (!hb_is_machine_on_sale(t)) continue;

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

bool zprop_is_contain(hb_zone_property_t* props, int count, hb_zone_property_t prop) {
	for (int i = 0; i < count; i++) {
		if (props[i] == prop)return true;
	}
	return false;
}

void print_available_zone_props()
{
	printf("### *恒博主机类型与支持的防区属性对照表*\n\n");

	
	//auto all_props = getAvailableZoneProperties();

	auto print_prop = [](hb_zone_property_t zp) {
		printf("%02X %s", zp, hb_zone_property_to_string_chinese(zp));
	};




	printf("* 防区属性是否支持失联报告\n\n");
	printf("|");
	for (auto zp : allZoneProperties) {
		printf("|"); print_prop(zp);
	}
	printf("|\n");

	printf("|----");
	for (size_t i = 0; i < sizeof(allZoneProperties) / sizeof(allZoneProperties[0]); i++) {
		printf("|----");
	}
	printf("|\n");

	printf("|失联支持");
	for (auto zp : allZoneProperties) {
		printf("|%s", print_bool(hb_zone_can_report_lost(zp)));
	}
	printf("|\n\n");


	printf("* 主机类型与支持的防区属性对照表\n\n");
	printf("|事件码|类型|型号");
	for (auto zp : allZoneProperties) {
		//printf("|"); print_prop(zp);
		printf("|%02X", (int)zp);
	}
	printf("|\n");

	printf("|----|----|----");
	for (size_t i = 0; i < sizeof(allZoneProperties) / sizeof(allZoneProperties[0]); i++) {
		printf("|----");
	}
	printf("|\n");

	for (auto e : allEvents) {
		if (ademco_is_machine_type_event(e)) {
			auto t = hb_machine_type_from_ademco_event(e);
			if (!hb_is_machine_on_sale(t)) continue;
			printf("|%04d", (int)e);
			printf("|%d", (int)t);
			print_machine_brands(t);
			hb_zone_property_t avail_props[HZP_COUNT];
			int count = hb_get_available_zone_properties_by_type(t, avail_props);
			for (auto zp : allZoneProperties) {
				printf("|%s", print_bool(zprop_is_contain(avail_props, count, zp)));
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
	//	printf("|%02X|%04d|%s|\n", code, ademcoEventFromCode(code), jlib::win32::utf16_to_mbcs(ademco_event_to_string_chinese(ademcoEventFromCode(code), false)).c_str());
	//}

}

void gen_sources() {
	const char* comment = R"(
```c
/* control source defs
 * 范围 0~255
 * 0: 主机
 * 1~50: 遥控器
 * 51~97: 智能家居
 * 98: 中转接警中心
 * 99: 直连接警中心
 * 100~199: 手机APP，末二位为手机尾号
 * 200: web用户，web端包括 网页，H5, 公众号，小程序等
 * 201~255: web分享用户
 * 特别注意：三区段主机，0 代表主机，1~255 都是遥控器
 */
```
)";
	printf("%s\n", comment);
	printf("|zone|name|source|\n");
	printf("|----|----|------|\n");
#define XX(name, code, zh) \
	printf("|%d|%s|%s|\n", code, #name, zh);
	ADEMCO_CONTROL_SOURCES_MAP(XX)
#undef XX
}

int main()
{

#define GEN_EVENTS 1
#define GEN_G250_CODES 2
#define GEN_CONTROL_SOURCES 3

	const int what = GEN_EVENTS;

	if (what == GEN_EVENTS) {
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


	} else if (what == GEN_G250_CODES) {
		print_g250_alarm_codes();

	} else if (what == GEN_CONTROL_SOURCES) {
		gen_sources();
	}


}