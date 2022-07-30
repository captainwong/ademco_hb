#include "../../c/ademco.h"
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
	return b ? "��" : " "; // "��";
}

AdemcoEvent statusEvents[] = {

	// ������ֻ�״̬����
	EVENT_ARM,
	EVENT_DISARM,
	EVENT_HALFARM_1456,
	EVENT_HALFARM,
	EVENT_EMERGENCY,
};

AdemcoEvent alarmEvents[] = {

	// ��������
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

	// �����쳣
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

	// ��׼�¼�������������
	EVENT_3100,

};

AdemcoEvent privateEvents[] = {

	// ------------------˽���¼�-----------------------------------------
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
	printf("|�¼���|����|en|\n|-----|----|--|\n");
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
		return "wzq";

	case HMT_GPRS_IOT:
	case HMT_GPRS:
	case HMT_LCD:
	case HMT_TRUE_COLOR:
	case HMT_3_SECTION:
	case HMT_IOT:
	case HMT_GPRS_PHONE:
	//case hb::common::Nb:
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
	std::string path = jlib::win32::utf16_to_mbcs(L"..\\..\\docs\\��������\\smartresize\\HB-") + brand;
	for (auto ext : exts) {
		if (jlib::win32::fileExists(path + "-web." + ext)) {
			return jlib::win32::utf16_to_mbcs(L"./��������/smartresize/HB-") + brand + "-web." + ext;
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
	printf("### *�㲩�������Ͷ��ձ�*\n\n");

	printf("*SMSָ���������Ƿ���Բ���绰�����Ͷ��ţ�����ָͨ������������绰*\n");

	printf("|�¼�������|��������|����|����|�벼��|����|�ź�ǿ��|����|���߷���|SMS|�ں�|����|�ͺ�|\n"
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
	printf("\n\n### *�㲩�����ͺ�ʾ��ͼƬ*\n\n");

	printf("|�ͺ�|ʾ��ͼƬ|\n"
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
	printf("### *�㲩����������֧�ֵķ������Զ��ձ�*\n\n");

	
	//auto all_props = getAvailableZoneProperties();

	auto print_prop = [](HbZoneProperty zp) {
		printf("%02X %s", zp, hbZonePropertyToStringChinese(zp));
	};




	printf("* ���������Ƿ�֧��ʧ������\n\n");
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

	printf("|ʧ��֧��");
	for (auto zp : hbZoneProperties) {
		printf("|%s", print_bool(hbZonePropCanReportLost(zp)));
	}
	printf("|\n\n");


	printf("* ����������֧�ֵķ������Զ��ձ�\n\n");
	printf("|�¼���|����|�ͺ�");
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
	//printf("# ������������ͨ��Э��\n\n");
	//printf("9600, N, 8, 1\n\n");
	//printf("SUM = DATA(N) = DATA(0) + DATA(1) + ... + DATA(N-1)\n\n");

	//// ������
	//printf("\n\n## EB AB addr data sum\n");
	//printf("PC������������\n");
	//printf("* addr: PCģ����̵�ַ��1~4����ʱ�̶�Ϊ3\n");
	//printf("* data: ������\n\n");
	//printf("|������|����|\n");
	//printf("|-----|----|\n");
	//for (Key k = Key::Key_NULL; k <= Key::Key_STOP_ALARM; k = Key(k + 1)) {
	//	auto s = keyToString(k);
	//	if (s) {
	//		printf("|%02X|%s|\n", k, jlib::win32::utf16_to_mbcs(s).c_str());
	//	}
	//}


	//// EB B1 ������
	//printf("\n\n## EB B1 data0 data1 data2 code data4 sum\n");
	//printf("������PC�¼�����\n");
	//printf("* data0: ���������ֳ����̶�Ϊ8\n");
	//printf("* data1: �����Ÿ�λ\n");
	//printf("* data2: �����ŵ�λ\n");
	//printf("* data4: 00 ��ʾdata1��data2Ϊ����ֱ�������ţ�01~F0 ��ʾdata1��data2Ϊ�ֻ������ţ��ѷ�������EE ��ʾ�ֻ�����״̬���ѷ�����\n");
	//printf("* code: ������\n\n");
	//printf("|������|�������¼���|����|\n");
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
	if (0) {
		printf("### ����״̬\n\n");
		printEvents(statusEvents, _countof(statusEvents));

		printf("### ��������\n\n");
		printEvents(alarmEvents, _countof(alarmEvents));

		printf("### �����쳣\n\n");
		printEvents(excepEvents, _countof(excepEvents));

		printf("### *�㲩˽���¼���*\n\n");
		printEvents(privateEvents, _countof(privateEvents));

		print_machineTypes();
		print_imgs();

		print_available_zone_props();


	} else {
		print_g250_alarm_codes();

	}


}