/*
* Ademco library
* captainwong
* 2022-6-13 rewrited this C version
*/



#include "ademco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

#define dline dprintf("%d\n", __LINE__);
#define dmsg dline; dprintf

#ifdef DEBUG
void ademcoPrint(const uint8_t* buff, int len)
{
	for (int i = 0; i < len; i++) {
		if (isprint(buff[i])) {
			printf("%c", buff[i]);
		} else {
			printf("\\x%02X", buff[i]);
		}
	}
	printf("\n");
}
#endif

int ademcoIsMachineStatusEvent(AdemcoEvent ademcoEvent) {
	return ademcoEvent == EVENT_ARM
		|| ademcoEvent == EVENT_HALFARM
		|| ademcoEvent == EVENT_HALFARM_1456
		|| ademcoEvent == EVENT_DISARM;
}

int ademcoIsMachineTypeEvent(AdemcoEvent ademcoEvent) {
	return ademcoEvent == EVENT_I_AM_NET_MODULE
		|| ademcoEvent == EVENT_I_AM_GPRS
		|| ademcoEvent == EVENT_I_AM_LCD_MACHINE
		|| ademcoEvent == EVENT_I_AM_WIRE_MACHINE
		|| ademcoEvent == EVENT_I_AM_WIFI_MACHINE
		|| ademcoEvent == EVENT_I_AM_3_SECTION_MACHINE
		|| ademcoEvent == EVENT_I_AM_IOT_MACHINE
		|| ademcoEvent == EVENT_I_AM_TRUE_COLOR
		|| ademcoEvent == EVENT_I_AM_GPRS_IOT
		|| ademcoEvent == EVENT_I_AM_GPRS_PHONE
		|| ademcoEvent == EVENT_I_AM_NB_MACHINE;
}

int ademcoIsEventNeedControlSource(AdemcoEvent ademcoEvent)
{
	return ademcoEvent == EVENT_ARM
		|| ademcoEvent == EVENT_HALFARM
		|| ademcoEvent == EVENT_HALFARM_1456
		|| ademcoEvent == EVENT_DISARM
		|| ademcoEvent == EVENT_EMERGENCY
		|| ademcoEvent == EVENT_DURESS
		|| ademcoEvent == EVENT_PHONE_USER_CANCLE_ALARM;
}

AdemcoEvent ademcoGetExceptionEventByResumeEvent(AdemcoEvent resumeEvent)
{
	switch (resumeEvent) {
	case EVENT_RECONNECT:					return EVENT_DISCONNECT;
	case EVENT_LOST_RECOVER:				return EVENT_LOST;
	case EVENT_SERIAL485CONN:				return EVENT_SERIAL485DIS;
	case EVENT_SUB_MACHINE_SENSOR_RESUME:	return EVENT_SUB_MACHINE_SENSOR_EXCEPTION;
	case EVENT_SUB_MACHINE_POWER_RESUME:	return EVENT_SUB_MACHINE_POWER_EXCEPTION;
	case EVENT_BATTERY_RECOVER:				return EVENT_LOWBATTERY;
	case EVENT_BATTERY_EXCEPTION_RECOVER:	return EVENT_BATTERY_EXCEPTION;
	case EVENT_OTHER_EXCEPTION_RECOVER:		return EVENT_OTHER_EXCEPTION;
	case EVENT_AC_RECOVER:					return EVENT_AC_BROKE;
	case EVENT_SOLARDISTURB_RECOVER:		return EVENT_SOLARDISTURB;
	case EVENT_BADBATTERY_RECOVER:			return EVENT_BADBATTERY;
	default:								return EVENT_INVALID_EVENT;
	}
}

AdemcoEventLevel ademcoGetEventLevel(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_ARM:
	case EVENT_DISARM:
	case EVENT_HALFARM:
	case EVENT_HALFARM_1456:
		return EVENT_LEVEL_STATUS;

	case EVENT_BADBATTERY_RECOVER:
	case EVENT_AC_RECOVER:
	case EVENT_RECONNECT:
	case EVENT_LOST_RECOVER:
	case EVENT_SERIAL485CONN:
	case EVENT_SUB_MACHINE_SENSOR_RESUME:
	case EVENT_SUB_MACHINE_POWER_RESUME:
	case EVENT_BATTERY_RECOVER:
	case EVENT_BATTERY_EXCEPTION_RECOVER:
	case EVENT_OTHER_EXCEPTION_RECOVER:
	case EVENT_SOLARDISTURB_RECOVER:
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

const char* ademcoEventToString(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_ARM:									return "ARM";
	case EVENT_BURGLAR:								return "BURGLAR";
	case EVENT_DISARM:								return "DISARM";
	case EVENT_DURESS:								return "DURESS";
	case EVENT_EMERGENCY:							return "EMERGENCY";
	case EVENT_FIRE:								return "FIRE";
	case EVENT_GAS:									return "GAS";
	case EVENT_HALFARM: case EVENT_HALFARM_1456:	return "HALFARM";
	case EVENT_TAMPER:								return "TAMPER";
	case EVENT_ZONE_TAMPER:							return "ZONE_TAMPER";
	case EVENT_BYPASS:								return "BYPASS";
	case EVENT_BYPASS_RESUME:						return "BYPASS_RESUME";

	case EVENT_WATER:								return "WATER";
	case EVENT_AC_BROKE:							return "AC BROKE";
	case EVENT_AC_RECOVER:							return "AC RECOVER";
	case EVENT_LOWBATTERY:							return "LOWBATTERY";
	case EVENT_BATTERY_RECOVER:						return "BATATTERY_RECOVER";
	case EVENT_BADBATTERY:							return "BADBATTERY";
	case EVENT_BADBATTERY_RECOVER:					return "BADBATTERY_RECOVER";
	case EVENT_SOLARDISTURB:						return "SOLARDISTURB";
	case EVENT_SOLARDISTURB_RECOVER:				return "SOLARDISTURB_RECOVER";
	case EVENT_DISCONNECT:							return "DISCONNECT";
	case EVENT_RECONNECT:							return "RECONNECT";
	case EVENT_LOST:								return "LOST";
	case EVENT_LOST_RECOVER:						return "LOST_RECOVER";
	case EVENT_3100:								return "LCD CLEAR EXCEPTION";
	case EVENT_BATTERY_EXCEPTION:					return "BATTERY_EXCEPTION";
	case EVENT_BATTERY_EXCEPTION_RECOVER:			return "BATTERY_EXCEPTION_RECOVER";
	case EVENT_OTHER_EXCEPTION:						return "OTHER_EXCEPTION";
	case EVENT_OTHER_EXCEPTION_RECOVER:				return "OTHER_EXCEPTION_RECOVER";

	case EVENT_SERIAL485DIS:						return "485DIS";
	case EVENT_SERIAL485CONN:						return "485CONN";
	case EVENT_DOORRINGING:							return "DOORRINGING";
	case EVENT_CONN_HANGUP: 						return "CONN_HANGUP";
	case EVENT_CONN_RESUME: 						return "CONN_RESUME";
	case EVENT_DISARM_PWD_ERR: 						return "DISARM_PWD_ERR";
	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:		return "SUB_SENSOR_EXCEPTION";
	case EVENT_SUB_MACHINE_SENSOR_RESUME:			return "SUB_SENSOR_RESUME";
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:			return "SUB_POWER_EXCEPTION";
	case EVENT_SUB_MACHINE_POWER_RESUME:			return "SUB_POWER_RESUME";
	case EVENT_COM_PASSTHROUGH:						return "COM_PASSTHROUGH";
	case EVENT_ENTER_SET_MODE:						return "ENTER_SET_MODE";
	case EVENT_EXIT_SET_MODE:						return "EXIT_SET_MODE";
	case EVENT_QUERY_SUB_MACHINE:					return "QUERY";
	case EVENT_WRITE_TO_MACHINE:					return "WRITE_TO_MACHINE";
	case EVENT_I_AM_NET_MODULE:						return "I_AM_NET_MODULE";
	case EVENT_PHONE_USER_SOS:						return "SOS";
	case EVENT_PHONE_USER_CANCLE_ALARM:				return "PHONE_USER_CANCLE_ALARM";
	case EVENT_I_AM_GPRS:							return "I_AM_GPRS_MACHINE";
	case EVENT_I_AM_LCD_MACHINE:					return "I_AM_LCD_MACHINE";
	case EVENT_I_AM_WIRE_MACHINE:					return "I_AM_WIRE_MACHINE";
	case EVENT_I_AM_WIFI_MACHINE:					return "I_AM_WIFI_MACHINE";
	case EVENT_I_AM_3_SECTION_MACHINE:				return "I_AM_3_SECTION_MACHINE";
	case EVENT_I_AM_IOT_MACHINE:					return "I_AM_IOT_MACHINE";
	case EVENT_I_AM_TRUE_COLOR:						return "I_AM_TRUE_COLOR";
	case EVENT_I_AM_GPRS_IOT:						return "I_AM_GPRS_IOT";
	case EVENT_I_AM_GPRS_PHONE:						return "I_AM_GPRS_PHONE";
	case EVENT_I_AM_NB_MACHINE:						return "I_AM_NB_MACHINE";
	case EVENT_SIM_IS_IOT_CARD:						return "SIM card is IOT";
	case EVENT_SIM_IS_IOT_PLATFORM_CARD:			return "SIM card is Platform IOT";
	case EVENT_SIM_IS_NOT_IOT_CARD:					return "SIM card is not IOT";
	case EVENT_ENTER_SETTING_MODE:					return "ENTER_SETTING_MODE";
	case EVENT_EXIT_SETTING_MODE:					return "EXIT_SETTING_MODE";
	case EVENT_RESTORE_FACTORY_SETTINGS_710:
	case EVENT_RESTORE_FACTORY_SETTINGS:			return "RESTORE_FACTORY_SETTINGS";
	case EVENT_WHAT_IS_YOUR_TYPE:					return "WHAT_IS_YOUR_TYPE";
	case EVENT_SIGNAL_STRENGTH_CHANGED:				return "SIGNAL_STRENGTH_CHANGED";
	case EVENT_OFFLINE:								return "EVENT_OFFLINE";
	case EVENT_ONLINE:								return "EVENT_ONLINE";
	default:										return "Undefined AdemcoEvent";
	}
}

const char* ademcoEventToStringChinese(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_ARM:									return "布防";
	case EVENT_BURGLAR:								return "盗警";
	case EVENT_DISARM:								return "撤防";
	case EVENT_DURESS:								return "胁迫";
	case EVENT_EMERGENCY:							return "紧急报警";

	case EVENT_FIRE:								return "火警";
	case EVENT_GAS:									return "煤气";
	case EVENT_HALFARM:	case EVENT_HALFARM_1456:	return "半布防";
	case EVENT_TAMPER:								return "防拆";
	case EVENT_ZONE_TAMPER:							return "防区防拆";
	case EVENT_WATER:								return "水警";
	case EVENT_BYPASS:								return "旁路";
	case EVENT_BYPASS_RESUME:						return "解除旁路";

	case EVENT_AC_BROKE:							return "主机AC掉电";
	case EVENT_AC_RECOVER:							return "主机AC恢复";
	case EVENT_LOWBATTERY:							return "低电";
	case EVENT_BATTERY_RECOVER: 					return "复电";
	case EVENT_BADBATTERY:							return "坏电";
	case EVENT_BADBATTERY_RECOVER:					return "坏电恢复";
	case EVENT_SOLARDISTURB:						return "光扰";
	case EVENT_SOLARDISTURB_RECOVER:				return "光扰恢复";
	case EVENT_DISCONNECT:							return "失效";
	case EVENT_RECONNECT:							return "恢复";
	case EVENT_LOST:								return "失联";
	case EVENT_LOST_RECOVER:						return "恢复";
	case EVENT_3100:								return "液晶主机清除异常指示";
	case EVENT_BATTERY_EXCEPTION:					return "防区电源故障";
	case EVENT_BATTERY_EXCEPTION_RECOVER:			return "防区电源故障恢复";
	case EVENT_OTHER_EXCEPTION:						return "防区其他故障";
	case EVENT_OTHER_EXCEPTION_RECOVER:				return "防区其他故障恢复";

	case EVENT_SERIAL485DIS:						return "485断开";
	case EVENT_SERIAL485CONN:						return "485恢复";
	case EVENT_DOORRINGING:							return "门铃";
	case EVENT_CONN_HANGUP:							return "链路挂起";
	case EVENT_CONN_RESUME:							return "链路恢复";
	case EVENT_DISARM_PWD_ERR: 						return "撤防密码错误";

	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:		return "分防区异常";
	case EVENT_SUB_MACHINE_SENSOR_RESUME:			return "分防区恢复";
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:			return "分防区电源异常";
	case EVENT_SUB_MACHINE_POWER_RESUME:			return "分防区电源恢复";
	case EVENT_COM_PASSTHROUGH:						return "串口透传";
	case EVENT_ENTER_SET_MODE:						return "进入设置状态";
	case EVENT_EXIT_SET_MODE:						return "拒绝/停止设置";
	case EVENT_QUERY_SUB_MACHINE:					return "查询";
	case EVENT_WRITE_TO_MACHINE:					return "写入主机信息";
	case EVENT_I_AM_NET_MODULE:						return "我是网络模块";
	case EVENT_PHONE_USER_SOS:						return "手机用户SOS";
	case EVENT_PHONE_USER_CANCLE_ALARM:				return "手机用户消警";
	case EVENT_I_AM_GPRS:							return "我是GPRS主机";
	case EVENT_I_AM_LCD_MACHINE:					return "我是液晶主机";
	case EVENT_I_AM_WIRE_MACHINE:					return "我是网线主机";
	case EVENT_I_AM_WIFI_MACHINE:					return "我是WiFi主机";
	case EVENT_I_AM_3_SECTION_MACHINE:				return "我是三区段主机";
	case EVENT_I_AM_IOT_MACHINE:					return "我是物联卡主机";
	case EVENT_I_AM_TRUE_COLOR:						return "我是真彩主机";
	case EVENT_I_AM_GPRS_IOT:						return "我是简化版物联卡主机";
	case EVENT_I_AM_GPRS_PHONE:						return "我是GPRS主机能打电话";
	case EVENT_I_AM_NB_MACHINE:						return "我是NB报警接收主机";
	case EVENT_SIM_IS_IOT_CARD:						return "SIM卡为物联卡";
	case EVENT_SIM_IS_IOT_PLATFORM_CARD:			return "SIM卡为平台物联卡";
	case EVENT_SIM_IS_NOT_IOT_CARD:					return "SIM卡为非物联卡";
	case EVENT_ENTER_SETTING_MODE:					return "主机进入设置状态";
	case EVENT_EXIT_SETTING_MODE:					return "主机退出设置状态";
	case EVENT_RESTORE_FACTORY_SETTINGS_710:
	case EVENT_RESTORE_FACTORY_SETTINGS:			return "主机恢复出厂设置";
	case EVENT_WHAT_IS_YOUR_TYPE:					return "索要主机类型";
	case EVENT_SIGNAL_STRENGTH_CHANGED:				return "信号强度变化";
	case EVENT_OFFLINE:								return "上线";
	case EVENT_ONLINE:								return "离线";
	default:										return "未定义的安定宝事件";
	}
}

static const uint16_t crc16Table[256] = {
	/* DEFINE THE FIRST ORDER POLYINOMIAL TABLE */
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,
};

uint16_t ademcoCRC16(const uint8_t* buff, int len)
{
	uint16_t crc = 0;
	while (len--) {
		crc = (crc >> 8) ^ crc16Table[(crc ^ *buff++) & 0xFF];
	}
	return crc;
}

int ademcoAppendDataSegment(uint8_t* buff, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone)
{
	char* p = (char*)buff;
	if (ademcoEvent == EVENT_INVALID_EVENT) {
		*p++ = '[';
		*p++ = ']';
		return ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
	} else {
		*p++ = '[';

		*p++ = '#';
		snprintf(p, 6 + 1, "%06X", ademcoId % ADEMCO_ID_SENTINEL);
		p += 6;

		*p++ = '|';

		snprintf(p, 4 + 1, "%04d", ademcoEvent % ADEMCO_EVENT_SENTINEL);
		p += 4;

		*p++ = ' ';

		if (gg == 0xEE) {
			*p++ = 'E';
			*p++ = 'E';
		} else if (gg == 0xCC) {
			*p++ = 'C';
			*p++ = 'C';
		} else {
			*p++ = ((gg / 10) & 0x0F) + '0';
			*p++ = ((gg % 10) & 0x0F) + '0';
		}

		*p++ = ' ';

		snprintf(p, 3 + 1, "%03d", zone % ADEMCO_ZONE_SENTINEL);
		p += 3;

		*p++ = ']';

		assert(p - (char*)buff == ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN);

		*p++ = '\0'; // for debug convenience

		return ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN;
	}
}

int ademcoAppendDataSegment2(AdemcoDataSegment* dataSegment, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone)
{
	dataSegment->ademcoId = ademcoId;
	dataSegment->ademcoEvent = ademcoEvent;
	dataSegment->gg = gg;
	dataSegment->zone = zone;
	return dataSegment->raw_len = ademcoAppendDataSegment(dataSegment->raw, ademcoId, ademcoEvent, gg, zone);
}

AdemcoParseResult ademcoParseDataSegment(const uint8_t* packet, int packet_len, AdemcoDataSegment* dataSegment)
{
	if (packet_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN && packet[0] == '[' && packet[1] == ']') {
		memset(dataSegment, 0, sizeof(*dataSegment));
		strcpy((char*)dataSegment->raw, "[]");
		dataSegment->raw_len = 2;
		return RESULT_OK;
	} else if (packet_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN && packet[0] == '[' && packet[packet_len - 1] == ']') { // [#000000|1400 00 000]		
		do {
			const uint8_t* p = packet + 2;
			int acct_len = packet_len - 15;
			if (acct_len < 6) {
				break;
			}

			char temp[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX] = {0};
			strncpy(temp, (const char*)p, acct_len);
			temp[acct_len] = '\0';
			dataSegment->ademcoId = (AdemcoId)strtoul(temp, NULL, 16);
			p += acct_len;

			if (*p++ != '|') { break; }
			strncpy(temp, (const char*)p, 4);
			temp[4] = '\0';
			dataSegment->ademcoEvent = (AdemcoEvent)(atoi(temp));
			p += 4;

			if (*p++ != ' ') { break; }
			if (*p == 'E' && *(p + 1) == 'E') {
				dataSegment->gg = 0xEE;
			} else if (*p == 'C' && *(p + 1) == 'C') {
				dataSegment->gg = 0xCC;
			} else {
				dataSegment->gg = (AdemcoGG)((*p - '0') * 10 + (*(p + 1) - '0'));
			}
			p += 2;

			if (*p++ != ' ') { break; }
			strncpy(temp, (const char*)p, 3);
			temp[3] = '\0';
			dataSegment->zone = atoi(temp);

			if (dataSegment->raw != packet) {
				memcpy(dataSegment->raw, packet, packet_len);
			}
			dataSegment->raw_len = packet_len;
			return RESULT_OK;
		} while (0);
	}

	return RESULT_ERROR;
}

int ademcoDataSegmentToCongwinFe100(uint8_t* fe100, const AdemcoDataSegment* dataSegment)
{
	if (dataSegment->raw_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN) {
		return 0;
	} else if (dataSegment->raw_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN) {
		int ndx = 0;
		char* p = (char*)fe100;
		*p++ = '\n';
		*p++ = ' ';

		*p++ = '0'; // RG
		*p++ = '0';
		*p++ = ' ';

		sprintf(p, "%08d", dataSegment->ademcoId);
		p += 8;

		*p++ = ' '; // 18
		*p++ = '1';
		*p++ = '8';
		*p++ = ' ';

		// E for open, R for close
		if (dataSegment->ademcoEvent / 1000 == 1 || dataSegment->ademcoEvent == EVENT_HALFARM) {
			*p++ = 'E';
		} else {
			*p++ = 'R';
		}

		if (dataSegment->ademcoEvent == EVENT_OFFLINE) {
			*p++ = '3';
			*p++ = 'B';
			*p++ = '2';
		} else if (dataSegment->ademcoEvent == EVENT_ONLINE) {
			*p++ = '3';
			*p++ = 'B';
			*p++ = '0';
		} else {
			sprintf(p, "%03d", dataSegment->ademcoEvent % 1000); // ȡ��3λ
			p += 3;
		}

		*p++ = ' '; // FE100, gg === 0
		*p++ = '0';
		*p++ = '0';
		*p++ = ' ';

		if (ademcoIsMachineStatusEvent(dataSegment->ademcoEvent)) {
			*p++ = 'U'; // user event
		} else {
			*p++ = 'C'; // zone event
		}

		sprintf(p, "%03d", dataSegment->zone);
		p += 3;

		*p++ = ' ';
		*p++ = '\r';
		return p - (char*)fe100;
	}
	return 0;
}

static const uint8_t* ademcoXDataGetValidContentAddr(const AdemcoXDataSegment* xdata)
{
	if (xdata->lenghFormat == TWO_HEX && xdata->raw_len > 4) {
		return xdata->raw + 3;
	} else if (xdata->lenghFormat == FOUR_DECIMAL && xdata->raw_len > 6) {
		return xdata->raw + 5;
	} else {
		return NULL;
	}
}

ADEMCO_EXPORT_SYMBOL int ademcoXDataMemcmp(const AdemcoXDataSegment* xdata, const void* buf, int buf_len)
{
	if (ademcoXDataGetValidContentLen(xdata) != buf_len) return 0;
	const uint8_t* p = ademcoXDataGetValidContentAddr(xdata);
	if (!p) return 0;
	return memcmp(p, buf, buf_len);
}

int ademcoXDataGetValidContentLen(const AdemcoXDataSegment* xdata)
{
	if (xdata->lenghFormat == TWO_HEX && xdata->raw_len > 4) {
		return xdata->raw_len - 4;
	} else if (xdata->lenghFormat == FOUR_DECIMAL && xdata->raw_len > 6) {
		return xdata->raw_len - 6;
	} else {
		return 0;
	}
}

int ademcoMakeXData(AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr, const uint8_t* content, int len)
{
	uint8_t transformed[ADEMCO_PACKET_XDATA_MAX_LEN];
	uint16_t translen = len;
	if (xtr == AdemcoXDataTransform_as_is) {
		memcpy(transformed, content, len);
	} else {
		if (len * 2 < ADEMCO_PACKET_XDATA_MAX_LEN) {
			translen = hbHexArrayToStr((char*)transformed, content, len);
		} else {
			return ADEMCO_ERR;
		}
	}

	xdata->lenghFormat = xlf;
	if (xlf == TWO_HEX && translen + 4 < ADEMCO_PACKET_XDATA_MAX_LEN) {
		uint8_t* p = xdata->raw;
		*p++ = '[';
		*p++ = (translen & 0xFFFF) >> 8;
		*p++ = (translen & 0xFFFF) & 0xFF;
		memcpy(p, transformed, translen);
		p += translen;
		*p++ = ']';
		xdata->raw_len = p - xdata->raw;
		return ADEMCO_OK;
	} else if (xlf == FOUR_DECIMAL && translen + 6 < ADEMCO_PACKET_XDATA_MAX_LEN) {
		uint8_t* p = xdata->raw;
		*p++ = '[';
		char temp[5];
		snprintf(temp, sizeof(temp), "%04d", translen & 0xFFFF);
		memcpy(p, temp, 4);
		p += 4;
		memcpy(p, transformed, translen);
		p += translen;
		*p++ = ']';
		xdata->raw_len = p - xdata->raw;
		return ADEMCO_OK;
	} else {
		return ADEMCO_ERR;
	}
}

int isAdemcoPacketId(const char* standard, const char* id, int len)
{
	return strncmp(standard, id, strlen(standard)) == 0;
}

static const char* const ids[AID_COUNT] = {
		ADEMCO_PACKET_ID_NULL,
		ADEMCO_PACKET_ID_ACK,
		ADEMCO_PACKET_ID_NAK,
		ADEMCO_PACKET_ID_DUH,
		ADEMCO_PACKET_ID_HB,
		ADEMCO_PACKET_ID_ADM_CID,
		ADEMCO_PACKET_ID_MOD_REG,
		ADEMCO_PACKET_ID_REG_RSP,
};

AdemcoPacketId getAdemcoPacketId(const char* id, int len)
{
	for (int i = 0; i < AID_COUNT; i++) {
		if (isAdemcoPacketId(ids[i], id, len)) {
			return (AdemcoPacketId)(i);
		}
	}

	return AID_INVALID;
}

const char* admecoPacketIdToString(AdemcoPacketId id)
{
	if (0 <= id && id < AID_COUNT) {
		return ids[id];
	} else {
		return "Unkown AdemcoPacketId";
	}
}

static void getNowTimestamp(char* buff)
{
	time_t now = time(NULL);
	struct tm* tm = localtime(&now);
	snprintf(buff, ADEMCO_PACKET_TIMESTAMP_LEN + 1, "_%02d:%02d:%02d,%02d-%02d-%04d",
			 tm->tm_hour, tm->tm_min, tm->tm_sec,
			 tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);
}

int ademcoMakeEmptyDataPacket(uint8_t* dst_buff, int len, const char* id, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	uint8_t buff[ADEMCO_PACKET_MAX_LEN];
	uint8_t* p = buff;
	uint8_t* pcrc = buff + 1;
	uint8_t* plen = buff + 5;
	uint8_t* pid = buff + 9;

	buff[0] = ADEMCO_PACKET_PREFIX;
	memcpy(pid, id, strlen(id));
	p = pid + strlen(id);
	snprintf((char*)p, 5, "%04d", seq);
	p += 4;
	memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
	p += strlen(ADEMCO_RRCVR_DEFAULT);
	memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
	p += strlen(ADEMCO_LPREF_DEFAULT);
	*p++ = '#';
	if (acct != NULL && strlen(acct) > 0) {
		memcpy(p, acct, strlen(acct));
		p += strlen(acct);
	} else {
		snprintf((char*)p, 7, "%06X", ademcoId);
		p += 6;
	}
	*p++ = '[';
	*p++ = ']';
	getNowTimestamp((char*)p);
	p += ADEMCO_PACKET_TIMESTAMP_LEN;
	*p++ = ADEMCO_PACKET_SUFIX;

	char temp[5];
	int packet_len = p - buff;
	//                           \n  crc len  \r
	int ademco_len = packet_len - 1 - 4 - 4 - 1;
	snprintf(temp, 5, "%04X", ademco_len);
	memcpy(plen, temp, 4);
	uint16_t crc = ademcoCRC16(pid, ademco_len);
	snprintf(temp, 5, "%04X", crc);
	memcpy(pcrc, temp, 4);

	if (dst_buff != NULL && len > packet_len) {
		memcpy(dst_buff, buff, packet_len);
		dst_buff[packet_len] = '\0'; // for debug convenience
		return packet_len;
	} else {
		return 0;
	}
}

int ademcoMakeNullPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	return ademcoMakeEmptyDataPacket(buff, len, ADEMCO_PACKET_ID_NULL, seq, acct, ademcoId);
}

int ademcoMakeAckPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	return ademcoMakeEmptyDataPacket(buff, len, ADEMCO_PACKET_ID_ACK, seq, acct, ademcoId);
}

int ademcoMakeNakPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	return ademcoMakeEmptyDataPacket(buff, len, ADEMCO_PACKET_ID_NAK, seq, acct, ademcoId);
}

int ademcoMakeHbPacket(uint8_t* dst_buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId, 
					   AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, AdemcoXDataSegment* xdata)
{
	char buff[ADEMCO_PACKET_MAX_LEN];
	char* p = buff;
	char* pcrc = buff + 1;
	char* plen = buff + 5;
	char* pid = buff + 9;

	buff[0] = ADEMCO_PACKET_PREFIX;
	memcpy(pid, ADEMCO_PACKET_ID_HB, strlen(ADEMCO_PACKET_ID_HB));
	p = pid + strlen(ADEMCO_PACKET_ID_HB);
	snprintf((char*)p, 5, "%04d", seq);
	p += 4;
	memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
	p += strlen(ADEMCO_RRCVR_DEFAULT);
	memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
	p += strlen(ADEMCO_LPREF_DEFAULT);
	*p++ = '#';
	if (acct != NULL) {
		memcpy(p, acct, strlen(acct));
		p += strlen(acct);
	} else {
		snprintf(p, 7, "%06X", ademcoId);
		p += 6;
	}
	p += ademcoAppendDataSegment((uint8_t*)p, ademcoId, ademcoEvent, gg, zone);
	if (xdata && xdata->raw_len > 0) {
		memcpy(p, xdata->raw, xdata->raw_len);
		p += xdata->raw_len;
	}
	getNowTimestamp(p);
	p += ADEMCO_PACKET_TIMESTAMP_LEN;
	*p++ = ADEMCO_PACKET_SUFIX;

	char temp[5];
	int packet_len = p - buff;
	//                           \n  crc len  \r
	int ademco_len = packet_len - 1 - 4 - 4 - 1;
	snprintf(temp, 5, "%04X", ademco_len);
	memcpy(plen, temp, 4);
	uint16_t crc = ademcoCRC16((const uint8_t*)pid, ademco_len);
	snprintf(temp, 5, "%04X", crc);
	memcpy(pcrc, temp, 4);

	if (dst_buff != NULL && len > packet_len) {
		memcpy(dst_buff, buff, packet_len);
		dst_buff[packet_len] = '\0'; // for debug convenience
		return packet_len;
	}
	return 0;
}

static void copyAcct2AdemcoPacket(AdemcoPacket* pkt, const char* acct)
{
	int len = strlen(acct);
	if (len > ADEMCO_PACKET_ACCT_MAX_LEN) {
		len = ADEMCO_PACKET_ACCT_MAX_LEN;
	}
	memcpy(pkt->acct, acct, len);
	pkt->acct[len] = '\0';
}

int ademcoMakeNullPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId)
{	
	pkt->seq = seq;
	copyAcct2AdemcoPacket(pkt, acct);
	pkt->data.ademcoId = ademcoId;
	return pkt->raw_len = ademcoMakeNullPacket(pkt->raw, sizeof(pkt->raw), seq, acct, ademcoId);
}

int ademcoMakeAckPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	pkt->seq = seq;
	copyAcct2AdemcoPacket(pkt, acct);
	pkt->data.ademcoId = ademcoId;
	return pkt->raw_len = ademcoMakeAckPacket(pkt->raw, sizeof(pkt->raw), seq, acct, ademcoId);
}

int ademcoMakeNakPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	pkt->seq = seq;
	copyAcct2AdemcoPacket(pkt, acct);
	pkt->data.ademcoId = ademcoId;
	return pkt->raw_len = ademcoMakeNakPacket(pkt->raw, sizeof(pkt->raw), seq, acct, ademcoId);
}

int ademcoMakeHbPacket2(AdemcoPacket* pkt, uint16_t seq, const char* acct, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, AdemcoXDataSegment* xdata)
{
	pkt->seq = seq;
	copyAcct2AdemcoPacket(pkt, acct);
	ademcoAppendDataSegment2(&pkt->data, ademcoId, ademcoEvent, gg, zone);
	if (xdata && xdata->raw_len > 0) {
		pkt->xdata.lenghFormat = xdata->lenghFormat;
		memcpy(pkt->xdata.raw, xdata->raw, xdata->raw_len);
		pkt->xdata.raw_len = xdata->raw_len;
	} else {
		memset(&pkt->xdata, 0, sizeof(*xdata));
	}
	return pkt->raw_len = ademcoMakeHbPacket(pkt->raw, sizeof(pkt->raw), seq, acct, ademcoId, ademcoEvent, gg, zone, xdata);
}

AdemcoParseResult ademcoPacketParse(const uint8_t* buff, int len, AdemcoPacket* pkt, int* cb_commited)
{
	if (len < 9) { return RESULT_NOT_ENOUGH; }
	do{
		const uint8_t* p = buff;
		if (*p++ != ADEMCO_PACKET_PREFIX) {
			dline;
			break;
		}

		char temp[5];

		// crc
		memcpy(temp, p, 4);
		temp[4] = '\0';
		uint16_t crc = strtoul(temp, NULL, 16) & 0xFFFF;
		p += 4;

		// len
		memcpy(temp, p, 4);
		temp[4] = '\0';
		pkt->len = strtoul(temp, NULL, 16);
		p += 4;
		int len_needed = 9 + pkt->len + 1;
		if (len < len_needed) {
			return RESULT_NOT_ENOUGH;
		}
		if (len_needed >= ADEMCO_PACKET_MAX_LEN) {
			dline;
			return RESULT_ERROR;
		}

		const uint8_t* pid = p;
		const uint8_t* pcr = pid + pkt->len;

		if (pcr - buff > len || *pcr != ADEMCO_PACKET_SUFIX) {
			dline;
			break;
		}

		pkt->crc = ademcoCRC16(pid, pkt->len);
		if (pkt->crc != crc) {
			dline;
			break;
		}

		// id
		if (*pid != '\"') {
			dline;
			break;
		}
		p = pid + 1;
		while (p < pcr && *p != '\"') { p++; }
		if (p >= pcr || *p != '\"') { dline; break; }
		pkt->id = getAdemcoPacketId((const char*)pid, ++p - pid);
		if (pkt->id == AID_INVALID) { dline; break; }

		// seq
		const uint8_t* pseq = p;
		while (p < pcr && *p != 'R' && *p != 'L') { p++; }
		if (p - pseq != 4 || (*p != 'R' && *p != 'L')) { dline; break; }
		memcpy(temp, pseq, 4); temp[4] = '\0';
		pkt->seq = strtoul(temp, NULL, 10) & 0xFFFF;
		if (pkt->seq >= 10000) { pkt->seq = 0; }

		// *rrcvr
		if (*p == 'R') { // rrcvr exists
			const uint8_t* prcvr = p;
			while (p < pcr && *p != 'L' && *p != '#') { p++; }
			if (p >= pcr || (*p != 'L' && *p != '#')) { dline; break; }
			// only check if format is correct, ignore it's content
		} else if (*p == 'L') { // rrcvr not exits
			// pass
		} else {
			dline; break;
		}

		// lpref
		if (*p != 'L') { dline; break; }
		while (p < pcr && *p != '#') { p++; }
		if (p >= pcr || *p != '#') { break; }
		// only check if format is correct, ignore it's content
		// pass

		// #acct
		const uint8_t* pacct = ++p;
		while (p < pcr && *p != '[') { 
			if (!isalnum(*p)) { p = NULL; break; }
			p++; 
		}
		if (p == NULL || p >= pcr || *p != '[' || p - pacct >= ADEMCO_PACKET_ACCT_MAX_LEN) { dline; break; }
		strncpy(pkt->acct, (const char*)pacct, p - pacct);
		pkt->acct[p - pacct] = '\0';

		// data
		const uint8_t* pdata = p;
		while (p < pcr && *p != ']') { p++; }
		if (p >= pcr || *p != ']') { dline; break; }
		if (ademcoParseDataSegment(pdata, ++p - pdata, &pkt->data) != RESULT_OK) { dline; break; }

		// *xdata
		if (*p == '[') { // xdata exists
			const uint8_t* pxdata = p++;
			AdemcoXDataLengthFormat xlf = FOUR_DECIMAL;
			int valid_len;
			for (int i = 0; i < 4; i++) {
				if (!isxdigit(*(p + i))) {
					xlf = TWO_HEX;
				}
			}

			if (xlf == FOUR_DECIMAL) {
				memcpy(temp, pxdata + 1, 4); temp[4] = '\0';
				valid_len = strtoul(temp, NULL, 16);
				p += 4 + valid_len;
			} else {
				valid_len = pxdata[1] * 256 + pxdata[2];
				p += 2 + valid_len;
			}

			if (p >= pcr || *p != ']' || *(p + 1) != '_') { dline; break; }
			pkt->xdata.lenghFormat = xlf;
			pkt->xdata.raw_len = ++p - pxdata;			
			memcpy(pkt->xdata.raw, pxdata, pkt->xdata.raw_len);
		} else {
			pkt->xdata.raw_len = 0;
		}

		// timestamp
		if (pcr - p != ADEMCO_PACKET_TIMESTAMP_LEN) { dline; break; }
		{
			struct tm tm;
			int ret = sscanf((const char*)p, "_%02d:%02d:%02d,%02d-%02d-%04d",
							 &tm.tm_hour, &tm.tm_min, &tm.tm_sec,
							 &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
			if (ret != 6) { 
				// wont break
			}

			tm.tm_year -= 1900;
			tm.tm_mon--;
			tm.tm_isdst = -1;
			pkt->timestamp = mktime(&tm);
			if (pkt->timestamp < 0) { // use local time instead
				pkt->timestamp = time(NULL);
			}
			p += ADEMCO_PACKET_TIMESTAMP_LEN;
		}

		if (p++ != pcr) { dline; break; }
		pkt->raw_len = *cb_commited = p - buff;
		if (pkt->raw != buff) {
			memcpy(pkt->raw, buff, pkt->raw_len);
		}
		return RESULT_OK;
	} while (0);

	return RESULT_ERROR;
}

int hbGetAvailableZoneProperties(HbMachineType type, HbZoneProperty props[12])
{
	switch (type) {
	case HMT_WIFI:
		break;
	case HMT_CAMERA:
		break;
	case HMT_GPRS_IOT:
	case HMT_GPRS:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_NETMOD:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE, HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD, HZP_DOOR_RING, };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_LCD:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE, HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_WIRED:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_TRUE_COLOR:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_3_SECTION:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_IOT:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_GPRS_PHONE:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_NB:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_DOOR_RING, };
		memcpy(props, hzps, sizeof(hzps));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	default: return 0;
	}
	return 0;
}


uint16_t hbZoneMax(HbMachineType type)
{
	switch (type) {
	case HMT_GPRS_IOT:
	case HMT_GPRS:
	case HMT_WIRED:
		return 99;

	case HMT_NETMOD:
	case HMT_NB:
		return 999;

	case HMT_LCD:
		return 249;

	case HMT_TRUE_COLOR:
		return 68;

	case HMT_3_SECTION:
		return 191;

	case HMT_IOT:
	case HMT_GPRS_PHONE:
		return 60;

	case HMT_WIFI:
	case HMT_CAMERA:
	default:
		return 0;
		break;
	}
}

int hbIsValidZone(HbMachineType type, uint16_t zone)
{
	return ADEMCO_ZONE_FOR_MACHINE_SELF <= zone && zone <= hbZoneMax(type);
}

int hbIsValidZoneStrict(HbMachineType type, uint16_t zone)
{
	return ADEMCO_ZONE_MIN <= zone && zone <= hbZoneMax(type);
}

int hbMachineIsSelling(HbMachineType type)
{
	return type == HMT_NETMOD
		|| type == HMT_GPRS
		|| type == HMT_GPRS_IOT
		|| type == HMT_IOT
		|| type == HMT_LCD
		|| type == HMT_WIRED
		|| type == HMT_TRUE_COLOR
		|| type == HMT_3_SECTION
		|| type == HMT_GPRS_PHONE
		|| type == HMT_NB
		;
}

int hbMachineCanArm(HbMachineType type)
{
	return hbMachineIsSelling(type) && (type != HMT_NB);
}

int hbMachineCanDisarm(HbMachineType type)
{
	return hbMachineIsSelling(type) && (type != HMT_NB);
}

int hbMachineCanEnterSettings(HbMachineType type)
{
	return hbMachineIsSelling(type);
}

int hbMachineCanHalfArm(HbMachineType type)
{
	return type == HMT_NETMOD
		|| type == HMT_LCD
		;
}

int hbMachineCanReportSignalStrength(HbMachineType type)
{
	return type == HMT_GPRS
		|| type == HMT_GPRS_IOT
		|| type == HMT_IOT
		|| type == HMT_LCD
		|| type == HMT_TRUE_COLOR
		|| type == HMT_3_SECTION
		|| type == HMT_GPRS_PHONE
		|| type == HMT_NB
		;
}

int hbMachineCanReportBySMS(HbMachineType type)
{
	return type == HMT_GPRS
		|| type == HMT_LCD
		|| type == HMT_TRUE_COLOR
		|| type == HMT_3_SECTION
		|| type == HMT_GPRS_PHONE
		;
}

int hbMachineHasWiredZone(HbMachineType type)
{
	return type == HMT_NETMOD
		|| type == HMT_TRUE_COLOR
		|| type == HMT_3_SECTION
		;
}

AdemcoZone hbWiredZoneMin(HbMachineType type)
{
	switch (type) {
	case HMT_NETMOD: return 1;
	case HMT_TRUE_COLOR: return 1;
	case HMT_3_SECTION: return 61;
	default: return 0;
	}
}

AdemcoZone hbWiredZoneMax(HbMachineType type)
{
	switch (type) {
	case HMT_NETMOD: return 8;
	case HMT_TRUE_COLOR: return 8;
	case HMT_3_SECTION: return 68;
	default: return 0;
	}
}

int hbMachineCanDirectlyWriteZone(HbMachineType type)
{
	return type == HMT_NETMOD;
}

int hbMachineCanLinkSubMachine(HbMachineType type)
{
	return type == HMT_NETMOD;
}

int hbZonePropCanReportLost(HbZoneProperty zp)
{
	switch (zp) {
	case HZP_BUGLAR:
	case HZP_EMERGENCY:
	case HZP_DURESS:
	case HZP_SUB_MACHINE:
	case HZP_BUGLAR_HALF:
	case HZP_BYPASS:
		return 1;
	default:
		return 0;
	}
}

void hbInitMachineTimer(HbMachineTimer* timer)
{
	memset(timer->data, 0xFF, sizeof(timer));
}

int hbIsValidTimer(HbTimer* timer)
{
	return 0 <= timer->armAt.hour && timer->armAt.hour < 24 &&
		0 <= timer->armAt.minute && timer->armAt.minute < 60 &&
		0 <= timer->disarmAt.hour && timer->disarmAt.hour < 24 &&
		0 <= timer->disarmAt.minute && timer->disarmAt.minute < 60;
}


AdemcoEvent hbMachineStatusToAdemcoEvent(HbMachineStatus status)
{
	switch (status) {
	case HMS_ARM:		return EVENT_ARM;
	case HMS_DISARM:	return EVENT_DISARM;
	case HMS_HALF_ARM:	return EVENT_HALFARM;				// EVENT_HALFARM_1456
	case HMS_SETTING:	return EVENT_ENTER_SETTING_MODE;	// EVENT_ENTER_SET_MODE
	default:			return EVENT_INVALID_EVENT;
	}
}

HbMachineStatus hbMachineStatusFromAdemcoEvent(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_ARM:					
		return HMS_ARM;
	case EVENT_DISARM:				
		return HMS_DISARM;
	case EVENT_HALFARM:
	case EVENT_HALFARM_1456:		
		return HMS_HALF_ARM;
	case EVENT_ENTER_SETTING_MODE:
	case EVENT_ENTER_SET_MODE:		
		return HMS_SETTING;
	default:						
		return HMS_INVALID;
	}
}

AdemcoEvent hbMachineTypeToAdemcoEvent(HbMachineType type)
{
	switch (type) {
	case HMT_WIFI: 	        return EVENT_I_AM_WIFI_MACHINE;
	case HMT_GPRS_IOT:		return EVENT_I_AM_GPRS_IOT;
	case HMT_NETMOD:	    return EVENT_I_AM_NET_MODULE;
	case HMT_GPRS:	        return EVENT_I_AM_GPRS;
	case HMT_LCD:		    return EVENT_I_AM_LCD_MACHINE;
	case HMT_WIRED: 	    return EVENT_I_AM_WIRE_MACHINE;
	case HMT_TRUE_COLOR:	return EVENT_I_AM_TRUE_COLOR;
	case HMT_3_SECTION: 	return EVENT_I_AM_3_SECTION_MACHINE;
	case HMT_IOT:			return EVENT_I_AM_IOT_MACHINE;
	case HMT_GPRS_PHONE:	return EVENT_I_AM_GPRS_PHONE;
	case HMT_NB:			return EVENT_I_AM_NB_MACHINE;
	default: 				return EVENT_INVALID_EVENT;
	}
}

HbMachineType hbMachineTypeFromAdemcoEvent(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_I_AM_NET_MODULE:		    return HMT_NETMOD;
	case EVENT_I_AM_GPRS: 			    return HMT_GPRS;
	case EVENT_I_AM_LCD_MACHINE: 	    return HMT_LCD;
	case EVENT_I_AM_WIRE_MACHINE: 	    return HMT_WIRED;
	case EVENT_I_AM_WIFI_MACHINE: 	    return HMT_WIFI;
	case EVENT_I_AM_3_SECTION_MACHINE: 	return HMT_3_SECTION;
	case EVENT_I_AM_IOT_MACHINE: 		return HMT_IOT;
	case EVENT_I_AM_TRUE_COLOR:			return HMT_TRUE_COLOR;
	case EVENT_I_AM_GPRS_IOT:			return HMT_GPRS_IOT;
	case EVENT_I_AM_GPRS_PHONE:			return HMT_GPRS_PHONE;
	case EVENT_I_AM_NB_MACHINE:			return HMT_NB;
	default: 							return HMT_INVALID;
	}
}

AdemcoEvent hbZonePropertyToAdemcoEvent(HbZoneProperty zp)
{
	switch (zp) {
	case HZP_BUGLAR:		
	case HZP_BUGLAR_HALF:	return EVENT_BURGLAR;
	case HZP_EMERGENCY:	    return EVENT_EMERGENCY;
	case HZP_FIRE:			return EVENT_FIRE;
	case HZP_DURESS:		return EVENT_DURESS;
	case HZP_GAS:			return EVENT_GAS;
	case HZP_WATER:		    return EVENT_WATER;
	case HZP_DOOR_RING:		return EVENT_DOORRINGING;
	default:				return EVENT_INVALID_EVENT;
	}
}

const char* hbMachineStatusToString(HbMachineStatus status)
{
	switch (status) {
	case HMS_ARM:		return "Arm";
	case HMS_HALF_ARM:	return "Half-Arm";
	case HMS_DISARM:	return "Disarm";
	case HMS_SETTING:	return "Setting";
	default:			return "InvalidMachineStatus";
	}
}

const char* hbMachineStatusToStringChinese(HbMachineStatus status)
{
	switch (status) {
	case HMS_ARM:		return "布防";
	case HMS_HALF_ARM:	return "半布防";
	case HMS_DISARM:	return "撤防";
	case HMS_SETTING:	return "设置";
	default:			return "InvalidMachineStatus";
	}
}

const char* hbMachineTypeToString(HbMachineType type)
{
	switch (type) {
	case HMT_WIFI:			return "0 WiFi";
	case HMT_CAMERA:		return "1 Camera";
	case HMT_GPRS_IOT:		return "2 Gprs_IoT";
	case HMT_NETMOD:		return "3 NetMod";
	case HMT_GPRS:			return "4 GPRS";
	case HMT_LCD:			return "5 LCD";
	case HMT_WIRED:			return "6 Wired";
	case HMT_TRUE_COLOR:	return "7 TrueColor";
	case HMT_3_SECTION:		return "8 ThreeSection";
	case HMT_IOT:			return "9 IoT";
	case HMT_GPRS_PHONE:	return "10 Gprs_Phone";
	case HMT_NB:			return "11 Nb";
	default:				return "Unknown HbMachineType";
	}
}

const char* hbMachineTypeToStringChinese(HbMachineType type)
{
	switch (type) {
	case HMT_WIFI:			return "0 WiFi主机";
	case HMT_CAMERA:		return "1 摄像头主机";
	case HMT_GPRS_IOT:		return "2 简化的物联卡主机";
	case HMT_NETMOD:		return "3 网络模块+工程主机";
	case HMT_GPRS:			return "4 GPRS主机";
	case HMT_LCD:			return "5 液晶主机";
	case HMT_WIRED:			return "6 网线主机";
	case HMT_TRUE_COLOR:	return "7 真彩主机";
	case HMT_3_SECTION:		return "8 三区段主机";
	case HMT_IOT:			return "9 物联卡主机";
	case HMT_GPRS_PHONE:	return "10 GPRS主机能打电话";
	case HMT_NB:			return "11 NB报警接收主机";
	default:				return "未知主机";
	}
}

const char* hbZonePropertyToString(HbZoneProperty zp)
{
	switch (zp) {
	case HZP_BUGLAR:			return "HZP_BUGLAR";
	case HZP_EMERGENCY:			return "HZP_EMERGENCY";
	case HZP_FIRE:				return "HZP_FIRE";
	case HZP_DURESS:			return "HZP_DURESS";
	case HZP_GAS:				return "HZP_GAS";
	case HZP_WATER:				return "HZP_WATER";
	case HZP_SUB_MACHINE:		return "HZP_SUB_MACHINE";
	case HZP_REMOTE_CONTROL:	return "HZP_REMOTE_CONTRO";
	case HZP_BUGLAR_HALF:		return "HZP_BUGLAR_HALF";
	case HZP_SHIELD:			return "HZP_SHIELD";
	case HZP_DOOR_RING:			return "HZP_DOOR_RING";
	case HZP_RESERVED_0B:		return "Reserved0B";
	case HZP_RESERVED_0C:		return "Reserved0C";
	case HZP_RESERVED_0D:		return "Reserved0D";
	case HZP_RESERVED_0E:		return "Reserved0E";
	case HZP_BYPASS:			return "HZP_BYPASS";
	default:					return "InvalidZoneProperty";
	}
}

const char* hbZonePropertyToStringChinese(HbZoneProperty zp)
{
	switch (zp) {
	case HZP_BUGLAR:			return "匪警全局";
	case HZP_EMERGENCY:			return "匪警紧急";
	case HZP_FIRE:				return "火警防区";
	case HZP_DURESS:			return "胁迫防区";
	case HZP_GAS:				return "燃气防区";
	case HZP_WATER:				return "淹水防区";
	case HZP_SUB_MACHINE:		return "分机";
	case HZP_REMOTE_CONTROL:	return "遥控器";
	case HZP_BUGLAR_HALF:		return "匪警半局";
	case HZP_SHIELD:			return "屏蔽防区";
	case HZP_DOOR_RING:			return "门铃防区";
	case HZP_RESERVED_0B:		return "保留";
	case HZP_RESERVED_0C:		return "保留";
	case HZP_RESERVED_0D:		return "保留";
	case HZP_RESERVED_0E:		return "保留";
	case HZP_BYPASS:			return "旁路防区";
	default:					return "无效属性";
	}
}

const char* hbGetZoneFormatString(HbMachineType type)
{
	uint16_t zone = hbZoneMax(type);
	if (10 < zone && zone < 100) { return "%02d"; } 
	else if (zone < 1000) { return "%03d"; } 
	else { return "%d"; }
}

void hbSum(uint8_t* data, int len)
{
	uint8_t sum = 0;
	uint8_t* p = data;
	while (p != data + len - 1) {
		sum += *p++;
	}
	*p = sum;
}

int hbCheckSum(const uint8_t* data, int len)
{
	uint8_t sum = 0;
	const uint8_t* p = data;
	while (p != data + len - 1) {
		sum += *p;
	}
	return sum == *p;
}

HbComRequestType hbComParseRequest(const uint8_t* data, int len)
{
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
				if (len != HbComReq_A0_len) { break; }
				if (memcmp(HbComReq_A0_data, data, len) != 0) { break; }
				return HbComReq_A0;
			}
			case 0xA1: // EB AB 3F A1 76
			{
				if (len != HbComReq_A1_len) { break; }
				if (memcmp(HbComReq_A1_data, data, len) != 0) { break; }
				return HbComReq_A1;
			}
			case 0xA2: // EB AB 3F A2 77
			{
				if (len != HbComReq_A2_len) { break; }
				if (memcmp(HbComReq_A2_data, data, len) != 0) { break; }
				return HbComReq_A2;
			}
			case 0xA5: // EB AB 3F A5 7A
			{
				if (len != HbComReq_A5_len) { break; }
				if (memcmp(HbComReq_A5_data, data, len) != 0) { break; }
				return HbComReq_A5;
			}
			case 0xAC: // EB AB 3F AC 81
			{
				if (len != HbComReq_AC_len) { break; }
				if (memcmp(HbComReq_AC_data, data, len) != 0) { break; }
				return HbComReq_AC;
			}
			case 0xAD: // EB AB 3F AD 82
			{
				if (len != HbComReq_AD_len) { break; }
				if (memcmp(HbComReq_AD_data, data, len) != 0) { break; }
				return HbComReq_AD;
			}

			default:
				break;
			}
			break;
		}
		case 0xBA:
		{
			if (data[2] != 0x3F) { break; }
			if (len == HbComReq_RD_acct_len && memcmp(data, HbComReq_RD_acct_data, len) == 0) {
				return HbComReq_RD_acct;
			}
			break;
		}
		case 0xCB:
		{
			if (data[2] != 0x3F) { break; }
			
			if (data[3] == 0x09 && data[4] == 0xA3 && len == HbComReq_A3_len) { // EB CB 3F 09 A3 P1 P2 P3 SUM
				if (hbCheckSum(data, len)) { return HbComReq_A3; }
			} else if (data[3] == 0x0F && data[4] == 0x4D && len == HbComReq_WR_acct_len) {
				if (hbCheckSum(data, len)) { return HbComReq_WR_acct; }
			} else if (data[3] == 0x0E && data[4] == 0xA7 && len == HbComReq_A7_len) { // EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
				if (hbCheckSum(data, len)) { return HbComReq_A7; }
			} 

			/*else if (data[3] == 0x08 && data[4] == 0xA9 && len == A9_len) {
				A7 req; memcpy(req.data, data, req.len); sum(req);
				if (data[len - 1] == req.data[len - 1]) { return A7; }
			}*/
			
			else if (data[3] == 0x08 && data[4] == 0xAA && len == HbComReq_AA_len) { // EB CB 3F 08 AA P1 P2 SUM
				if (hbCheckSum(data, len)) { return HbComReq_AA; }
			}

			/*else if (data[3] == 0x08 && data[4] == 0xAE && len == AE_len) {
				AA req; memcpy(req.data, data, req.len); sum(req);
				if (data[len - 1] == req.data[len - 1]) { return AA; }
			}*/
			
			else if (data[3] == 0x06 && data[4] == 0xB0 && len == HbComReq_B0_len && memcmp(HbComReq_B0_data, data, len) == 0) { // EB CB 3F 06 B0 AB
				return HbComReq_B0;
			}
		}
		}
	} while (0);
	return HbComReq_Invalid;
}

ADEMCO_EXPORT_SYMBOL HbComRequestType hbComParseXDataRequest(const AdemcoXDataSegment* xdata)
{
	if (!xdata) { return HbComReq_Invalid; }
	return hbComParseRequest(ademcoXDataGetValidContentAddr(xdata), ademcoXDataGetValidContentLen(xdata));
}

HbComResponseType hbComParseResponse(const uint8_t* data, int len)
{
	do {
		if (len < 7) { break; } // ���е� response ��������СΪ 7
		if (data[0] != 0xEB || data[1] != 0xBA || data[2] != 0x3F) { break; }
		switch (data[5]) {
		case 0xA0: // EB BA 3F 07 P0 A0 P1 P2 P3 SUM
		{
			if (len != HbComResp_A0_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A0; }
		}

		case 0xA2: // EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
		{
			if (len != data[3]) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A2; }
		}

		case 0xA3:
		{
			if (len != HbComResp_A3_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A3; }
		}

		case 0xA4:
		{
			if (len != HbComResp_A4_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A4; }
			return HbComResp_A4;
		}

		case 0xA6:
		{
			if (len != HbComResp_A6_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A6; }
			return HbComResp_A6;
		}

		case 0xA7:
		{
			if (len != HbComResp_A7_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A7; }
			return HbComResp_A7;
		}

		case 0xA8:
		{
			if (len != HbComResp_A8_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_A8; }
			return HbComResp_A8;
		}

		case 0xA9:
		{
			// TODO
		}

		case 0xAB:
		{
			if (len != HbComResp_AB_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_AB; }
			return HbComResp_AB;
		}

		case 0xAD: // EB BA 3F PN P0 AD P1 DATA P2 SUM
		{
			if (len != data[3]) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_AD; }
			return HbComResp_AD;
		}

		case 0xAF: // TODO
		{

			break;
		}

		case 0xB1: // EB BA 3F 08 P0 B1 P1 SUM
		{
			if (len != HbComResp_B1_len) { break; }
			if (hbCheckSum(data, len)) { return HbComResp_B1; }
			return HbComResp_B1;
		}

		}
	} while (0);
	return HbComResp_Invalid;
}

void hbComMakeReqA0_getMachineStatus(HbComData* data)
{
	memcpy(data->data, HbComReq_A0_data, HbComReq_A0_len);
	data->len = HbComReq_A0_len;
}

void hbComMakeReqA1_getMachineZones(HbComData* data)
{
	memcpy(data->data, HbComReq_A1_data, HbComReq_A1_len);
	data->len = HbComReq_A1_len;
}

void hbComMakeReqA2_getMoreMachineZones(HbComData* data)
{
	memcpy(data->data, HbComReq_A2_data, HbComReq_A2_len);
	data->len = HbComReq_A2_len;
}

void hbComMakeReqA3_modifyMachineZone(HbComData* data, uint8_t zone, HbZoneProperty prop, HbComReq_A3_op op)
{
	memcpy(data->data, HbComReq_A3_head, 5);
	data->data[5] = zone;
	data->data[6] = prop;
	data->data[7] = op;
	data->len = HbComReq_A3_len;
	hbSum(data->data, data->len);
}

void hbComMakeReqA5_getMachineTimer(HbComData* data)
{
	memcpy(data->data, HbComReq_A5_data, HbComReq_A5_len);
	data->len = HbComReq_A5_len;
}

void hbComMakeReqA7_setMachineTimer(HbComData* data, HbMachineTimer* timer)
{
	memcpy(data->data, HbComReq_A7_head, 5);
	memcpy(data->data + 5, timer, sizeof(*timer));
	data->len = HbComReq_A7_len;
	hbSum(data->data, data->len);
}

void hbComMakeReqAA_modifyMachineZoneLostConfig(HbComData* data, uint8_t zone, int on)
{
	memcpy(data->data, HbComReq_AA_head, 5);
	data->data[5] = zone;
	data->data[6] = !!on;
	data->len = HbComReq_AA_len;
	hbSum(data->data, data->len);
}

void hbComMakeReqAC_getMachineZoneLostConfig(HbComData* data)
{
	memcpy(data->data, HbComReq_AC_data, HbComReq_AC_len);
	data->len = HbComReq_AC_len;
}

void hbComMakeReqAD_getMoreMachineZoneLostConfig(HbComData* data)
{
	memcpy(data->data, HbComReq_AD_data, HbComReq_AD_len);
	data->len = HbComReq_AD_len;
}

void hbComMakeReqAE_set3SectionMachineStatus(HbComData* data, HbCom_3sec_which p1, HbCom_3sec_status status)
{
	memcpy(data->data, HbComReq_AE_head, 5);
	data->data[5] = p1;
	data->data[6] = status;
	data->len = HbComReq_AE_len;
	hbSum(data->data, data->len);
}

void hbComMakeReqB0_get3SectionMachineStatus(HbComData* data)
{
	memcpy(data->data, HbComReq_B0_data, HbComReq_B0_len);
	data->len = HbComReq_B0_len;
}

void hbComMakeReqRD_acct(HbComData* data)
{
	memcpy(data->data, HbComReq_RD_acct_data, HbComReq_RD_acct_len);
	data->len = HbComReq_RD_acct_len;
}

void hbComMakeReqWR_acct(HbComData* data, const char* acct)
{
	memcpy(data->data, HbComReq_WR_acct_head, 5);
	hbDecStrToHiLoArray(data->data + 5, 9, acct);
	data->len = HbComReq_WR_acct_len;
	hbSum(data->data, data->len);
}

void hbComMakeRespA0_getMachineStatus(HbComData* data, HbMachineStatus status, HbMachineType type)
{
	memcpy(data->data, HbComResp_A0_head, 6);
	data->data[6] = status;
	data->data[7] = type;
	data->len = HbComResp_A0_len;
	hbSum(data->data, data->len);
}

void hbComMakeRespA2_getMachineZones(HbComData* data, int count, AdemcoZone* zones, HbZoneProperty* props, HbComResp_A2_p1 p1)
{
	if (count > HbComResp_A2_max_zone) { return; }
	memcpy(data->data, HbComResp_A2_head, 6);
	data->data[3] = HbComResp_A2_len_min + count * 2;
	uint8_t* p = data->data + 6;
	for (int i = 0; i < count; i++) {
		*p++ = zones[i] & 0xFF;
		*p++ = props[i];
	}
	*p++ = (count > 0) ? p1 : HbComResp_A2_p1_nomore;
	data->len = ++p - data->data;
	hbSum(data->data, data->len);
}

void hbComMakeRespA3_waitingSignal(HbComData* data)
{
	memcpy(data->data, HbComResp_A3_data, HbComResp_A3_len);
	data->len = HbComResp_A3_len;
}

void hbComMakeRespA4_modifyMachineZone(HbComData* data, AdemcoZone zone, HbZoneProperty prop, HbComResp_A4_p3 p3)
{
	memcpy(data->data, HbComResp_A4_head, 6);
	data->data[6] = zone & 0xFF;
	data->data[7] = prop;
	data->data[8] = p3;
	data->len = HbComResp_A4_len;
	hbSum(data->data, data->len);
}

void hbComMakeRespA6_getMachineTimer(HbComData* data, HbMachineTimer* timer)
{
	memcpy(data->data, HbComResp_A6_head, 6);
	memcpy(data->data + 6, timer, sizeof(*timer));
	data->len = HbComResp_A6_len;
	hbSum(data->data, data->len);
}

void hbComMakeRespA7_setMachineTimer(HbComData* data)
{
	memcpy(data->data, HbComResp_A7_data, HbComResp_A7_len);
	data->len = HbComResp_A7_len;
}

void hbComMakeRespA8_reject(HbComData* data)
{
	memcpy(data->data, HbComResp_A8_data, HbComResp_A8_len);
	data->len = HbComResp_A8_len;
}

void hbComMakeRespAB_modifyMachineZoneLostConfig(HbComData* data, AdemcoZone zone, HbComResp_AB_p2 p2)
{
	memcpy(data->data, HbComResp_AB_head, 6);
	data->data[6] = zone & 0xFF;
	data->data[7] = p2;
	data->len = HbComResp_AB_len;
	hbSum(data->data, data->len);	
}

void hbComMakeRespAD_getMachineZoneLostConfig(HbComData* data, HbComResp_AD_p1 p1, int count, AdemcoZone* zones, HbComResp_AD_p2 p2)
{
	if (count > HbComResp_AD_max_zone) { return; }
	memcpy(data->data, HbComResp_AD_head, 6);
	data->data[3] = HbComResp_AD_len_min + (p1 == HbComResp_AD_p1_single ? count : count * 2);
	data->data[6] = p1;
	uint8_t* p = data->data + 7;
	for (int i = 0; i < count; i++) {
		if (p1 == HbComResp_AD_p1_single) {
			*p++ = zones[i] & 0xFF;
		} else {
			*p++ = zones[i] >> 8 & 0xFF;
			*p++ = zones[i] & 0xFF;
		}		
	}
	*p++ = (count > 0) ? p2 : HbComResp_AD_p2_nomore;
	data->len = ++p - data->data;
	hbSum(data->data, data->len);
}

void hbComMakeRespAF_set3SectionMachineStatus(HbComData* data, HbCom_3sec_which p1, HbCom_3sec_status status)
{
	memcpy(data->data, HbComResp_AF_head, 6);
	data->data[6] = p1;
	data->data[7] = status;
	data->len = HbComResp_AF_len;
	hbSum(data->data, data->len);
}

void hbComMakeRespB1_get3SectionMachineStatus(HbComData* data, HbMachineStatus statusMachine, 
											  HbMachineStatus statusSec1, HbMachineStatus statusSec2, HbMachineStatus statusSec3)
{
	memcpy(data->data, HbComResp_B1_head, 6);
	data->data[6] = (statusMachine << 6) | (statusSec1 << 4) | (statusSec2 << 2) | (statusSec3);
	data->len = HbComResp_B1_len;
	hbSum(data->data, data->len);
}

void hbComDataToAdemcoXData(const HbComData* const data, AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr)
{
	ademcoMakeXData(xdata, xlf, xtr, data->data, data->len);
}

int hbHiLoArrayToDecStr(char* str, const uint8_t* arr, int len)
{
	char* p = str;
	for (int i = 0; i < len; i++) {
		char c = (arr[i] >> 4) & 0x0F;
		if (c > 9) { return p - str; }
		*p++ = c + '0';
		c = (arr[i] & 0x0F);
		if (c > 9) { return p - str; }
		*p++ = c + '0';
	}
	return p - str;
}

int hbDecStrToHiLoArray(uint8_t* arr, int len, const char* str)
{
	uint8_t* p = arr;
	int slen = strlen(str);
	if (slen > len * 2) {
		slen = len * 2;
	}
	for (int i = 0; i < slen; i += 2) {
		char hi = str[i];
		if ('0' <= hi && hi <= '9') {
			if (i + 1 < slen) {
				char lo = str[i + 1];
				if ('0' <= lo && lo <= '9') {
					*p++ = (hi << 4) | (lo & 0x0F);
				} else {
					*p++ = (hi << 4) | 0x0F;
					break;
				}
			} else {
				*p++ = (hi << 4) | 0x0F;
				break;
			}
		} else {
			break;
		}
	}
	while (p - arr < len) {
		*p++ = 0xFF;
	}
	return len;
}

static uint8_t hex2char(uint8_t h) {
	h &= 0x0F;
	if (h > 9) {
		return 'A' + h - 10;
	} else {
		return '0' + h;
	}
}

int hbHexArrayToStr(char* str, const uint8_t* arr, int len)
{
	uint8_t c = 0;
	char* p = str;
	for (int i = 0; i < len; i++) {
		*p++ = hex2char((arr[i] >> 4) & 0x0F);
		*p++ = hex2char(arr[i] & 0x0F);
	}
	return p - str;
}

static uint8_t char2hex(uint8_t c) {
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else {
		return 0xFF;
	}
}

int hbHexStrToArray(uint8_t* arr, const char* str, uint8_t padding)
{
	uint8_t* p = arr;
	uint8_t hi = 0, lo = 0;
	int slen = strlen(str);
	padding &= 0x0F;
	for (int i = 0; i < slen / 2; i++) {
		char c = str[i * 2];
		if ((hi = char2hex(c)) == 0xFF) {
			return 0;
		}
		if (i * 2 + 1 < slen) {
			c = str[i * 2 + 1];
			if ((lo = char2hex(c)) == 0xFF) {
				return 0;
			}
		} else {
			lo = padding;
		}
		*p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
	}
	return p - arr;
}

int hbHexStrToArrayN(uint8_t* arr, const char* str, int len, uint8_t padding)
{
	uint8_t* p = arr;
	uint8_t hi = 0, lo = 0;
	padding &= 0x0F;
	for (int i = 0; i < len / 2; i++) {
		char c = str[i * 2];
		if ((hi = char2hex(c)) == 0xFF) {
			return 0;
		}
		if (i * 2 + 1 < len) {
			c = str[i * 2 + 1];
			if ((lo = char2hex(c)) == 0xFF) {
				return 0;
			}
		} else {
			lo = padding;
		}
		*p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
	}
	return p - arr;
}

int hbHexStrToArrayN_allow_non_hex_str(uint8_t* arr, const char* str, int len, uint8_t padding)
{
	uint8_t* p = arr;
	uint8_t hi = 0, lo = 0;
	padding &= 0x0F;
	for (int i = 0; i < len / 2; i++) {
		char c = str[i * 2];
		if ((hi = char2hex(c)) == 0xFF) {
			hi = padding;
		}
		if (i * 2 + 1 < len) {
			c = str[i * 2 + 1];
			if ((lo = char2hex(c)) == 0xFF) {
				lo = padding;
			}
		} else {
			lo = padding;
		}
		*p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
	}
	return p - arr;
}


