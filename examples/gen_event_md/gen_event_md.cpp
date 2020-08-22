
#define ENABLE_ADEMCO_EVENT_TO_STRING
#include "../../include/ademco_event.h"
#include <jlib/win32/UnicodeTool.h>

using namespace ademco;

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
	EVENT_BY_PASS,
	EVENT_BY_PASS_RESUME,

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

	EVENT_PHONE_USER_SOS,
	EVENT_PHONE_USER_CANCLE_ALARM,

	EVENT_ENTER_SETTING_MODE,
	EVENT_EXIT_SETTING_MODE,
	EVENT_RESTORE_FACTORY_SETTINGS_710,
	EVENT_RESTORE_FACTORY_SETTINGS,

	EVENT_SIM_IS_IOT_CARD,
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
}