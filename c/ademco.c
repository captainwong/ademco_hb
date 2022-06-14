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

const wchar_t* ademcoEventToStringChinese(AdemcoEvent ademcoEvent)
{
	switch (ademcoEvent) {
	case EVENT_ARM:									return L"布防";
	case EVENT_BURGLAR:								return L"盗警";
	case EVENT_DISARM:								return L"撤防";
	case EVENT_DURESS:								return L"胁迫";
	case EVENT_EMERGENCY:							return L"紧急报警";

	case EVENT_FIRE:								return L"火警";
	case EVENT_GAS:									return L"煤气";
	case EVENT_HALFARM:	case EVENT_HALFARM_1456:	return L"半布防";
	case EVENT_TAMPER:								return L"防拆";
	case EVENT_ZONE_TAMPER:							return L"防区防拆";
	case EVENT_WATER:								return L"水警";
	case EVENT_BYPASS:								return L"旁路";
	case EVENT_BYPASS_RESUME:						return L"解除旁路";

	case EVENT_AC_BROKE:							return L"主机AC掉电";
	case EVENT_AC_RECOVER:							return L"主机AC恢复";
	case EVENT_LOWBATTERY:							return L"低电";
	case EVENT_BATTERY_RECOVER: 					return L"复电";
	case EVENT_BADBATTERY:							return L"坏电";
	case EVENT_BADBATTERY_RECOVER:					return L"坏电恢复";
	case EVENT_SOLARDISTURB:						return L"光扰";
	case EVENT_SOLARDISTURB_RECOVER:				return L"光扰恢复";
	case EVENT_DISCONNECT:							return L"失效";
	case EVENT_RECONNECT:							return L"恢复";
	case EVENT_LOST:								return L"失联";
	case EVENT_LOST_RECOVER:						return L"恢复";
	case EVENT_3100:								return L"液晶主机清除异常指示";
	case EVENT_BATTERY_EXCEPTION:					return L"防区电源故障";
	case EVENT_BATTERY_EXCEPTION_RECOVER:			return L"防区电源故障恢复";
	case EVENT_OTHER_EXCEPTION:						return L"防区其他故障";
	case EVENT_OTHER_EXCEPTION_RECOVER:				return L"防区其他故障恢复";

	case EVENT_SERIAL485DIS:						return L"485断开";
	case EVENT_SERIAL485CONN:						return L"485恢复";
	case EVENT_DOORRINGING:							return L"门铃";
	case EVENT_CONN_HANGUP:							return L"链路挂起";
	case EVENT_CONN_RESUME:							return L"链路恢复";
	case EVENT_DISARM_PWD_ERR: 						return L"撤防密码错误";

	case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:		return L"分防区异常";
	case EVENT_SUB_MACHINE_SENSOR_RESUME:			return L"分防区恢复";
	case EVENT_SUB_MACHINE_POWER_EXCEPTION:			return L"分防区电源异常";
	case EVENT_SUB_MACHINE_POWER_RESUME:			return L"分防区电源恢复";
	case EVENT_COM_PASSTHROUGH:						return L"串口透传";
	case EVENT_ENTER_SET_MODE:						return L"进入设置状态";
	case EVENT_EXIT_SET_MODE:						return L"拒绝/停止设置";
	case EVENT_QUERY_SUB_MACHINE:					return L"查询";
	case EVENT_WRITE_TO_MACHINE:					return L"写入主机信息";
	case EVENT_I_AM_NET_MODULE:						return L"我是网络模块";
	case EVENT_PHONE_USER_SOS:						return L"手机用户SOS";
	case EVENT_PHONE_USER_CANCLE_ALARM:				return L"手机用户消警";
	case EVENT_I_AM_GPRS:							return L"我是GPRS主机";
	case EVENT_I_AM_LCD_MACHINE:					return L"我是液晶主机";
	case EVENT_I_AM_WIRE_MACHINE:					return L"我是网线主机";
	case EVENT_I_AM_WIFI_MACHINE:					return L"我是WiFi主机";
	case EVENT_I_AM_3_SECTION_MACHINE:				return L"我是三区段主机";
	case EVENT_I_AM_IOT_MACHINE:					return L"我是物联卡主机";
	case EVENT_I_AM_TRUE_COLOR:						return L"我是真彩主机";
	case EVENT_I_AM_GPRS_IOT:						return L"我是简化版物联卡主机";
	case EVENT_I_AM_GPRS_PHONE:						return L"我是GPRS主机能打电话";
	case EVENT_I_AM_NB_MACHINE:						return L"我是NB报警接收主机";
	case EVENT_SIM_IS_IOT_CARD:						return L"SIM卡为物联卡";
	case EVENT_SIM_IS_IOT_PLATFORM_CARD:			return L"SIM卡为平台物联卡";
	case EVENT_SIM_IS_NOT_IOT_CARD:					return L"SIM卡为非物联卡";
	case EVENT_ENTER_SETTING_MODE:					return L"主机进入设置状态";
	case EVENT_EXIT_SETTING_MODE:					return L"主机退出设置状态";
	case EVENT_RESTORE_FACTORY_SETTINGS_710:
	case EVENT_RESTORE_FACTORY_SETTINGS:			return L"主机恢复出厂设置";
	case EVENT_WHAT_IS_YOUR_TYPE:					return L"索要主机类型";
	case EVENT_SIGNAL_STRENGTH_CHANGED:				return L"信号强度变化";
	case EVENT_OFFLINE:								return L"上线";
	case EVENT_ONLINE:								return L"离线";
	default:										return L"未定义的安定宝事件";
	}
}

uint16_t ademcoCRC8(char c, uint16_t crc)
{
	static const uint16_t crcTable[] = {
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

	uint16_t res = (uint16_t)(crc >> 8) ^ crcTable[(uint8_t)c ^ (uint8_t)(crc & 0xFF)];
	return res;
}

uint16_t ademcoCRC16(const uint8_t* buff, int len, uint16_t crc)
{
	uint16_t CRC = crc;
	for (int i = 0; i < len; i++) {
		CRC = ademcoCRC8(buff[i], CRC);
	}
	return CRC;
}

int ademcoAppendDataSegment(uint8_t* packet, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone)
{
	AdemcoDataSegment dataSegment;
	dataSegment.ademcoId = ademcoId;
	dataSegment.ademcoEvent = ademcoEvent;
	dataSegment.gg = gg;
	dataSegment.zone = zone;
	return ademcoAppendDataSegment2(packet, &dataSegment);
}

int ademcoAppendDataSegment2(uint8_t* buff, const AdemcoDataSegment* dataSegment)
{
	char* p = buff;
	if (dataSegment->ademcoEvent == EVENT_INVALID_EVENT) {
		*p++ = '[';
		*p++ = ']';
		return ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
	} else {
		*p++ = '[';

		*p++ = '#';
		snprintf(p, 6 + 1, "%06X", dataSegment->ademcoId % ADEMCO_ID_SENTINEL);
		p += 6;

		*p++ = '|';

		snprintf(p, 4 + 1, "%04d", dataSegment->ademcoEvent % ADEMCO_EVENT_SENTINEL);
		p += 4;

		*p++ = ' ';

		if (dataSegment->gg == 0xEE) {
			*p++ = 'E';
			*p++ = 'E';
		} else if (dataSegment->gg == 0xCC) {
			*p++ = 'C';
			*p++ = 'C';
		} else {
			*p++ = ((dataSegment->gg / 10) & 0x0F) + '0';
			*p++ = ((dataSegment->gg % 10) & 0x0F) + '0';
		}

		*p++ = ' ';

		snprintf(p, 3 + 1, "%03d", dataSegment->zone % ADEMCO_ZONE_SENTINEL);
		p += 3;

		*p++ = ']';

		assert(p - buff == ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN);

		return ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN;
	}
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

			memcpy(dataSegment->raw, packet, packet_len);
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
			sprintf(p, "%03d", dataSegment->ademcoEvent % 1000); // 取后3位
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
		*p++ = '\n';
		return p - (char*)fe100;
	}
	return 0;
}

const uint8_t* ademcoXDataGetValidContentAddr(const AdemcoXDataSegment* xdata)
{
	if (xdata->lenghFormat == TWO_HEX && xdata->raw_len > 4) {
		return xdata->raw + 3;
	} else if (xdata->lenghFormat == FOUR_DECIMAL && xdata->raw_len > 6) {
		return xdata->raw + 5;
	} else {
		return NULL;
	}
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

int ademcoMakeXData(AdemcoXDataSegment* xdata, AdemcoXDataLengthFormat xlf, const uint8_t* content, int len)
{
	xdata->lenghFormat = xlf;
	if (xlf == TWO_HEX && len + 4 < ADEMCO_PACKET_XDATA_MAX_LEN) {
		uint8_t* p = xdata->raw;
		*p++ = '[';
		*p++ = (len & 0xFFFF) >> 8;
		*p++ = (len & 0xFFFF) & 0xFF;
		memcpy(p, content, len);
		p += len;
		*p++ = ']';
		return ADEMCO_OK;
	} else if (xlf == FOUR_DECIMAL && len + 6 < ADEMCO_PACKET_XDATA_MAX_LEN) {
		uint8_t* p = xdata->raw;
		*p++ = '[';
		char temp[5];
		sprintf(temp, "%04d", len & 0xFFFF);
		memcpy(p, temp, 4);
		p += 4;
		memcpy(p, content, len);
		p += len;
		*p++ = ']';
		return ADEMCO_OK;
	} else {
		return ADEMCO_ERR;
	}
}

int isAdemcoId(const char* standard, const char* id, int len)
{
	return strncmp(standard, id, strlen(standard)) == 0;
}

AdemcoPacketId getAdemcoId(const char* id, int len)
{
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

	for (int i = 0; i < AID_COUNT; i++) {
		if (isAdemcoId(ids[i], id, len)) {
			return (AdemcoPacketId)(i);
		}
	}

	return AID_INVALID;
}

static void getNowTimestamp(char* buff)
{
	time_t now = time(NULL);
	struct tm* tm = localtime(&now);
	snprintf(buff, ADEMCO_PACKET_TIMESTAMP_LEN+1, "_%02d:%02d:%02d,%02d-%02d-%04d",
			 tm->tm_hour, tm->tm_min, tm->tm_sec,
			 tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);
}

static int ademcoMakeEmptyDataPacket(uint8_t* dst_buff, int len, const char* id, uint16_t seq, const char* acct, AdemcoId ademcoId)
{
	char buff[ADEMCO_PACKET_MAX_LEN];
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
	if (acct != NULL) {
		memcpy(p, acct, strlen(acct));
		p += strlen(acct);
	} else {
		snprintf((char*)p, 7, "%06X", ademcoId);
		p += 6;
	}
	*p++ = '[';
	*p++ = ']';
	getNowTimestamp(p);
	p += ADEMCO_PACKET_TIMESTAMP_LEN;
	*p++ = ADEMCO_PACKET_SUFIX;

	char temp[5];
	int packet_len = p - buff;
	//                           \n  crc len  \r
	int ademco_len = packet_len - 1 - 4 - 4 - 1;
	snprintf(temp, 5, "%04X", ademco_len);
	memcpy(plen, temp, 4);
	uint16_t crc = ademcoCRC16(pid, ademco_len, 0);
	snprintf(temp, 5, "%04X", crc);
	memcpy(pcrc, temp, 4);

	if (dst_buff == NULL || len < packet_len) {
		return packet_len;
	} else if (dst_buff != NULL && len >= packet_len) {
		memcpy(dst_buff, buff, packet_len);
		dst_buff[packet_len] = '\0'; // for debug convenience
		return packet_len;
	}
	return 0;
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
	p += ademcoAppendDataSegment(p, ademcoId, ademcoEvent, gg, zone);
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
	uint16_t crc = ademcoCRC16(pid, ademco_len, 0);
	snprintf(temp, 5, "%04X", crc);
	memcpy(pcrc, temp, 4);

	if (dst_buff == NULL || len < packet_len) {
		return packet_len;
	} else if (dst_buff != NULL && len >= packet_len) {
		memcpy(dst_buff, buff, packet_len);
		dst_buff[packet_len] = '\0'; // for debug convenience
		return packet_len;
	}
	return 0;
}

AdemcoParseResult ademcoPacketParse(const uint8_t* buff, int len, AdemcoPacket* pkt, int* cb_commited)
{
	if (len < 9) { return RESULT_NOT_ENOUGH; }
	do{
		const uint8_t* p = buff;
		if (*p++ != ADEMCO_PACKET_PREFIX) {
			break;
		}

		char temp[5];

		// crc
		memcpy(temp, p, 4);
		temp[4] = '\0';
		uint16_t crc = strtoul(temp, NULL, 16);
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
			return RESULT_ERROR;
		}

		const uint8_t* pid = p;
		const uint8_t* pcr = pid + pkt->len;

		if (pcr - buff > len || *pcr != ADEMCO_PACKET_SUFIX) {
			break;
		}

		pkt->crc = ademcoCRC16(pid, pkt->len, 0);
		if (pkt->crc != crc) {
			break;
		}

		// id
		if (*pid != '\"') {
			break;
		}
		p = pid + 1;
		while (p < pcr && *p != '\"') { p++; }
		if (p >= pcr || *p != '\"') { break; }
		pkt->id = getAdemcoId((const char*)pid, ++p - pid);
		if (pkt->id == AID_INVALID) { break; }

		// seq
		const uint8_t* pseq = p;
		while (p < pcr && *p != 'R' && *p != 'L') { p++; }
		if (p - pseq != 4 || (*p != 'R' && *p != 'L')) { break; }
		memcpy(temp, pseq, 4); temp[4] = '\0';
		pkt->seq = strtoul(temp, NULL, 10);
		if (pkt->seq >= 10000) { pkt->seq = 0; }

		// *rrcvr
		if (*p == 'R') { // rrcvr exists
			const uint8_t* prcvr = p;
			while (p < pcr && *p != 'L' && *p != '#') { p++; }
			if (p >= pcr || (*p != 'L' && *p != '#')) { break; }
			// only check if format is correct, ignore it's content
		} else if (*p == 'L') { // rrcvr not exits
			// pass
		} else {
			break;
		}

		// lpref
		if (*p != 'L') { break; }
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
		if (p == NULL || p >= pcr || *p != '[' || p - pacct >= ADEMCO_PACKET_ACCT_MAX_LEN) { break; }
		strncpy(pkt->acct, (const char*)pacct, p - pacct);
		pkt->acct[p - pacct] = '\0';

		// data
		const uint8_t* pdata = p;
		while (p < pcr && *p != ']') { p++; }
		if (p >= pcr || *p != ']') { break; }
		if (ademcoParseDataSegment(pdata, ++p - pdata, &pkt->data) != RESULT_OK) { break; }

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

			if (p >= pcr || *p != ']' || *(p + 1) != '_') { break; }
			pkt->xdata.lenghFormat = xlf;
			pkt->xdata.raw_len = ++p - pxdata;
			memcpy(pkt->xdata.raw, pxdata, pkt->xdata.raw_len);
		} else {
			pkt->xdata.raw_len = 0;
		}

		// timestamp
		if (pcr - p != ADEMCO_PACKET_TIMESTAMP_LEN) { break; }
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

		if (p++ != pcr) { break; }
		*cb_commited = p - buff;
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
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_NETMOD:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE, HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD, HZP_DOOR_RING, };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_LCD:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE, HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_WIRED:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_TRUE_COLOR:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_3_SECTION:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_IOT:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_GPRS_PHONE:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL, };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
	case HMT_NB:
	{
		HbZoneProperty hzps[] = { HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE, HZP_DURESS, HZP_GAS, HZP_WATER, HZP_DOOR_RING, };
		memcpy(props, hzps, sizeof(hzps) / sizeof(HbZoneProperty));
		return sizeof(hzps) / sizeof(HbZoneProperty);
	}
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
	}
	return 0;
}

AdemcoZone hbWiredZoneMax(HbMachineType type)
{
	switch (type) {
	case HMT_NETMOD: return 8;
	case HMT_TRUE_COLOR: return 8;
	case HMT_3_SECTION: return 68;
	}
	return 0;
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

const wchar_t* hbMachineTypeToStringChinese(HbMachineType type)
{
	switch (type) {
	case HMT_WIFI:			return L"0 WiFi主机";
	case HMT_CAMERA:		return L"1 摄像头主机";
	case HMT_GPRS_IOT:		return L"2 简化的物联卡主机";
	case HMT_NETMOD:		return L"3 网络模块+工程主机";
	case HMT_GPRS:			return L"4 GPRS主机";
	case HMT_LCD:			return L"5 液晶主机";
	case HMT_WIRED:			return L"6 网线主机";
	case HMT_TRUE_COLOR:	return L"7 真彩主机";
	case HMT_3_SECTION:		return L"8 三区段主机";
	case HMT_IOT:			return L"9 物联卡主机";
	case HMT_GPRS_PHONE:	return L"10 GPRS主机能打电话";
	case HMT_NB:			return L"11 NB报警接收主机";
	default:				return L"未知主机";
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
	case HZP_REMOTE_CONTROL:	return "HZP_REMOTE_CONTROL";
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

const wchar_t* hbZonePropertyToStringChinese(HbZoneProperty zp)
{
	switch (zp) {
	case HZP_BUGLAR:			return L"匪警全局";
	case HZP_EMERGENCY:			return L"匪警紧急";
	case HZP_FIRE:				return L"火警防区";
	case HZP_DURESS:			return L"胁迫防区";
	case HZP_GAS:				return L"燃气防区";
	case HZP_WATER:				return L"淹水防区";
	case HZP_SUB_MACHINE:		return L"分机";
	case HZP_REMOTE_CONTROL:	return L"遥控器";
	case HZP_BUGLAR_HALF:		return L"匪警半局";
	case HZP_SHIELD:			return L"屏蔽防区";
	case HZP_DOOR_RING:			return L"门铃防区";
	case HZP_RESERVED_0B:		return L"保留";
	case HZP_RESERVED_0C:		return L"保留";
	case HZP_RESERVED_0D:		return L"保留";
	case HZP_RESERVED_0E:		return L"保留";
	case HZP_BYPASS:			return L"旁路防区";
	default:					return L"无效属性";
	}
}

const char* hbGetZoneFormatString(HbMachineType type)
{
	uint16_t zone = hbZoneMax(type);
	if (10 < zone && zone < 100) { return "%02d"; } 
	else if (zone < 1000) { return "%03d"; } 
	else { return "%d"; }
}

const wchar_t* hbGetZoneFormatStringW(HbMachineType type)
{
	uint16_t zone = hbZoneMax(type);
	if (10 < zone && zone < 100) { return L"%02d"; } 
	else if (zone < 1000) { return L"%03d"; } 
	else { return L"%d"; }
}

int hbHiLoArrayToDecStr(char* str, const uint8_t* arr, int len)
{
	char* p = str;
	for (int i = 0; i < len; i++) {
		char c = (arr[i] >> 4) & 0x0F;
		if (c > 9) { return p - str; }
		*p++ = c;
		c = (arr[i] & 0x0F);
		if (c > 9) { return p - str; }
		*p++ = c;
	}
	return p - str;
}

int hbDecStrToHiLoArray(uint8_t* arr, const char* str, int len)
{
	uint8_t* p = arr;
	for (int i = 0; i < len; i += 2) {
		char hi = str[i];
		if ('0' <= hi && hi <= '9') {
			if (i < len) {
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
	return p - arr;
}


