/*
* Ademco library
* captainwong
* 2022-6-13 rewrited this C version
*/





#ifndef __ADEMCO_H__
#define __ADEMCO_H__

#pragma once

#ifdef ADEMCO_EXPORTS

# if defined __GNUC__
#  define ADEMCO_EXPORT_SYMBOL __attribute__ ((visibility("default")))
# elif defined(_MSC_VER)
#  define ADEMCO_EXPORT_SYMBOL __declspec(dllexport)
# else
#  define ADEMCO_EXPORT_SYMBOL 
# endif

#else // ADEMCO_EXPORTS

# define ADEMCO_EXPORT_SYMBOL 

#endif


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

#define ADEMCO_OK 0
#define ADEMCO_ERR -1

#define ADEMCO_PACKET_ACCT_MAX_LEN 64 // #acct֮acct��󳤶�
#define ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN 2 // ��data��[]����
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN 21 // �ǿ�data�γ��ȣ�acct����6
#define ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX 64 // �ǿ�data�γ��ȣ�acct����δ֪
#define ADEMCO_PACKET_MAX_LEN 1024
#define CONGWIN_FE100_PACKET_LEN 31 // ����FE100Э�鳤��
#define ADEMCO_PACKET_TIMESTAMP_LEN 20
#define ADEMCO_PACKET_XDATA_MAX_LEN 256

// Prototypes
typedef uint32_t AdemcoId;
typedef uint8_t  AdemcoGG;
typedef uint32_t AdemcoZone;

//! ������ID��Χ
#define ADEMCO_ID_MIN 1
#define ADEMCO_ID_MAX 999999 // �����Կ��ǣ���󰲶��� ID Ϊ 0x0F423F
#define ADEMCO_ID_SENTINEL (ADEMCO_ID_MAX + 1)
static inline int ademcoIsValidAdemcoId(AdemcoId ademcoId) {
	return ADEMCO_ID_MIN <= ademcoId && ademcoId <= ADEMCO_ID_MAX; 
}

//! ������Ϊ0ʱ��ʾ��������
#define ADEMCO_ZONE_FOR_MACHINE_SELF 0
//! ���������ŷ�Χ
#define ADEMCO_ZONE_MIN 1
//! ���κ��������ͣ����ķ�����
#define ADEMCO_ZONE_MAX 999
#define ADEMCO_ZONE_SENTINEL (ADEMCO_ZONE_MAX + 1)

//! ���κ��������ͣ��������Ƿ�Ϸ������԰���0������
static inline int ademcoIsValidZone(AdemcoZone zone) {
	return ADEMCO_ZONE_FOR_MACHINE_SELF <= zone && zone <= ADEMCO_ZONE_MAX;
}

//! ���κ��������ͣ��������Ƿ�Ϸ��������԰���0������
static inline int ademcoIsValidZoneStrict(AdemcoZone zone) {
	return ADEMCO_ZONE_MIN <= zone && zone <= ADEMCO_ZONE_MAX;
}

#define ADEMCO_GG_MIN 0
#define ADEMCO_GG_MAX 99
#define ADEMCO_GG_SENTINEL 100

//! ����������״̬GG��Χ 1~3
#define ADEMCO_3SECTION_MACHINE_GG_MIN 1
#define ADEMCO_3SECTION_MACHINE_GG_MAX 3

//! �Ƿ�Ϸ�������������״̬GG
static inline int ademcoIsValid3SectionMachineGG(AdemcoGG gg) {
	return ADEMCO_3SECTION_MACHINE_GG_MIN <= gg && gg <= ADEMCO_3SECTION_MACHINE_GG_MAX;
}

// Ademco events
typedef enum AdemcoEvent {
	EVENT_INVALID_EVENT							= 0,

	// -------------------��׼������Э���¼�--------------------------------
	// ������ֻ�״̬����
	EVENT_ARM									= 3400, // ����
	EVENT_DISARM								= 1400, // ����
	EVENT_HALFARM_1456							= 1456, // �벼��
	EVENT_HALFARM								= 3456, // �벼��
	EVENT_EMERGENCY								= 1120, // ��������

	// ��������
	EVENT_BURGLAR								= 1130, // ����
	EVENT_DOORRINGING							= 1134, // ����
	EVENT_FIRE									= 1110, // ��
	EVENT_DURESS								= 1121, // в��
	EVENT_GAS									= 1151, // ú��
	EVENT_WATER									= 1113, // ˮ��
	EVENT_TAMPER								= 1137, // ��������
	EVENT_ZONE_TAMPER							= 1383, // ��������
	EVENT_BYPASS								= 1570, // ��·
	EVENT_BYPASS_RESUME							= 3570, // �����·

	// �����쳣
	EVENT_AC_BROKE								= 1301, // ����AC����
	EVENT_AC_RECOVER							= 3301, // ����AC����
	EVENT_LOWBATTERY							= 1302, // �͵�
	EVENT_BATTERY_RECOVER						= 3302, // ����
	EVENT_BADBATTERY							= 1311, // ����
	EVENT_BADBATTERY_RECOVER					= 3311, // ����ָ�
	EVENT_SOLARDISTURB							= 1387, // ����
	EVENT_SOLARDISTURB_RECOVER					= 3387, // ���Żָ�
	EVENT_DISCONNECT							= 1381, // ʧЧ
	EVENT_RECONNECT								= 3381, // �ָ�
	EVENT_BATTERY_EXCEPTION						= 1384, // ��Դ����
	EVENT_BATTERY_EXCEPTION_RECOVER				= 3384, // ��Դ���ϻָ�
	EVENT_OTHER_EXCEPTION						= 1380, // ��������
	EVENT_OTHER_EXCEPTION_RECOVER				= 3380, // �������ϻָ�
	EVENT_LOST									= 1393, // ʧ��
	EVENT_LOST_RECOVER							= 3393, // ʧ���ָ�

	// ��׼�¼�������������
	EVENT_3100									= 3100, // Һ����������쳣ָʾ
	
	// ------------------˽���¼�-----------------------------------------
	EVENT_SERIAL485DIS							= 1485, // ����ģ��������485�������ӶϿ�
	EVENT_SERIAL485CONN							= 3485, // ����ģ��������485�������ӻָ�

	EVENT_CONN_HANGUP							= 1700, // ��·����
	EVENT_CONN_RESUME							= 3700, // ��·�ָ�

	EVENT_DISARM_PWD_ERR						= 1701, // �����������

	EVENT_SUB_MACHINE_SENSOR_EXCEPTION			= 1702, // �ֻ�̽ͷ�쳣
	EVENT_SUB_MACHINE_SENSOR_RESUME				= 3702, // �ֻ�̽ͷ�ָ�
	EVENT_SUB_MACHINE_POWER_EXCEPTION			= 1703, // �ֻ���Դ�쳣
	EVENT_SUB_MACHINE_POWER_RESUME				= 3703, // �ֻ���Դ�ָ�

	EVENT_COM_PASSTHROUGH						= 1704, // ����͸��
	EVENT_ENTER_SET_MODE						= 2704, // ��������״̬
	EVENT_EXIT_SET_MODE							= 3704,	// �Ӿ����ķ��ͣ�Ϊֹͣ��Ҫ�������������ͣ�Ϊ�ܾ���Ҫ

	EVENT_QUERY_SUB_MACHINE						= 1705, // ��ѯ�ֻ���Ϣ
	EVENT_WRITE_TO_MACHINE						= 1706, // д��������Ϣ

	EVENT_I_AM_NET_MODULE						= 1707, // ��������--����ģ��
	EVENT_I_AM_GPRS								= 1717, // ��������--GPRS����
	EVENT_I_AM_LCD_MACHINE						= 1727, // ��������--Һ������
	EVENT_I_AM_WIRE_MACHINE						= 1737, // ��������--��������
	EVENT_I_AM_WIFI_MACHINE						= 1747, // ��������--WiFi����
	EVENT_I_AM_3_SECTION_MACHINE				= 1757, // ��������--����������
	EVENT_I_AM_IOT_MACHINE						= 1767, // ��������--����������
	EVENT_I_AM_TRUE_COLOR						= 1777, // ��������--�������
	EVENT_I_AM_GPRS_IOT							= 1787, // ��������--����������
	EVENT_I_AM_GPRS_PHONE						= 1797, // ��������--GRPS�������绰����
	EVENT_I_AM_NB_MACHINE						= 1807, // ��������--NB������������

	EVENT_PHONE_USER_SOS						= 1709, // �ֻ��û�SOS
	EVENT_PHONE_USER_CANCLE_ALARM				= 1711, // �ֻ��û�����

	EVENT_ENTER_SETTING_MODE					= 1712, // ������������״̬
	EVENT_EXIT_SETTING_MODE						= 3712, // �����˳�����״̬
	EVENT_RESTORE_FACTORY_SETTINGS_710			= 1710, // �����ָ���������
	EVENT_RESTORE_FACTORY_SETTINGS				= 1713, // �����ָ���������

	// �˴��绰���ý�Ϊ��������֧�ֵĵ绰���ã��밢�������޹�
	EVENT_SIM_IS_IOT_CARD						= 1756, // ����SIM��Ϊ������������������绰����
	EVENT_SIM_IS_IOT_PLATFORM_CARD				= 2756, // ����SIM��Ϊƽ̨������������������绰����
	EVENT_SIM_IS_NOT_IOT_CARD					= 3756, // ����SIM��Ϊ��������������������绰����

	EVENT_WHAT_IS_YOUR_TYPE						= 1798, // ��Ҫ��������
	EVENT_SIGNAL_STRENGTH_CHANGED				= 1799, // �����ź�ǿ�ȱ仯

	// 2021��1��24��17:06:55�޸ģ��Ա����3B0 3B2
	EVENT_OFFLINE								= 1944, // ��������
	EVENT_ONLINE								= 1946, // ��������

#define ADEMCO_EVENT_SENTINEL					10000

}AdemcoEvent;

// �������¼�����
typedef enum AdemcoEventLevel
{
	EVENT_LEVEL_NULL,
	EVENT_LEVEL_STATUS,				// ����״̬
	EVENT_LEVEL_EXCEPTION_RESUME,	// ��ɫ����
	EVENT_LEVEL_EXCEPTION,			// ��ɫ����
	EVENT_LEVEL_ALARM,				// ��ɫ����
}AdemcoEventLevel;

// ������Э��������
typedef enum AdemcoParseResult {
	RESULT_OK, //! ��������ȷ��������һ��
	RESULT_NOT_ENOUGH, //! ��������ȷ����������һ������Ҫ�������ݣ��´μ�������
	RESULT_ERROR, //! ��������������ݣ�Ӧ�����������ݰ�
}AdemcoParseResult;

// ������Э��data��
typedef struct AdemcoDataSegment {
	uint8_t raw[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX];
	int raw_len;
	AdemcoId ademcoId;
	AdemcoEvent ademcoEvent;
	AdemcoGG gg;
	AdemcoZone zone;
}AdemcoDataSegment;

typedef enum XDataLengthFormat {
	TWO_HEX, // ����hex�ֽڱ�ʾ����
	FOUR_DECIMAL, // �ĸ�decimal char ��ʾ����
}XDataLengthFormat;

typedef struct XData {
	XDataLengthFormat lenghFormat;
	uint8_t raw[ADEMCO_PACKET_XDATA_MAX_LEN];
	int raw_len;
}XData;

#define ADEMCO_PACKET_ID_NULL	 "\"NULL\""
#define ADEMCO_PACKET_ID_ACK	 "\"ACK\""
#define ADEMCO_PACKET_ID_NAK     "\"NAK\""
#define ADEMCO_PACKET_ID_DUH     "\"DUH\""
#define ADEMCO_PACKET_ID_HB      "\"HENG-BO\""
#define ADEMCO_PACKET_ID_ADM_CID "\"ADM-CID\""
#define ADEMCO_PACKET_ID_MOD_REG "\"MODULE_REG\""
#define ADEMCO_PACKET_ID_REG_RSP "\"REG-RSP\""

typedef enum AdemcoPacketId {
	AID_NULL,
	AID_ACK,
	AID_NAK,
	AID_DUH,
	AID_HB,
	AID_ADM_CID,
	AID_MOD_REG,
	AID_REG_RSP,
	AID_COUNT,
	AID_INVALID = -1,
}AdemcoPacketId;

#define ADEMCO_RRCVR_DEFAULT "R123ABC"
#define ADEMCO_LPREF_DEFAULT "L456DEF"

#define ADEMCO_PACKET_PREFIX '\n'
#define ADEMCO_PACKET_SUFIX  '\r'

typedef struct AdemcoPacket {
	uint16_t crc;
	int len;
	AdemcoPacketId id;
	int seq;
	char acct[ADEMCO_PACKET_ACCT_MAX_LEN + 1];
	AdemcoDataSegment data;
	XData xdata;
	time_t timestamp;
}AdemcoPacket;


// Functions

//! �Ƿ�����״̬�¼�
ADEMCO_EXPORT_SYMBOL int ademcoIsMachineStatusEvent(AdemcoEvent ademcoEvent);
//! �Ƿ����������¼�
ADEMCO_EXPORT_SYMBOL int ademcoIsMachineTypeEvent(AdemcoEvent ademcoEvent);
//! �¼��Ƿ���Ҫ����Դ
ADEMCO_EXPORT_SYMBOL int ademcoIsEventNeedControlSource(AdemcoEvent ademcoEvent);
//! ��ȡ�������¼�����
ADEMCO_EXPORT_SYMBOL AdemcoEventLevel ademcoGetEventLevel(AdemcoEvent ademcoEvent);
//! ��ȡ�쳣�ָ��¼�����Ӧ���쳣�¼�
ADEMCO_EXPORT_SYMBOL AdemcoEvent ademcoGetExceptionEventByResumeEvent(AdemcoEvent resumeEvent);

ADEMCO_EXPORT_SYMBOL int ademcoAppendDataSegment(uint8_t* packet, AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone);
ADEMCO_EXPORT_SYMBOL int ademcoAppendDataSegment2(uint8_t* packet, const AdemcoDataSegment* dataSegment);
ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoParseDataSegment(const uint8_t* packet, int packet_len, AdemcoDataSegment* dataSegment);
// return 0 for empty packet, CONGWIN_FE100_PACKET_LEN for non-empty packet
ADEMCO_EXPORT_SYMBOL int ademcoDataSegmentToCongwinFe100(uint8_t* fe100, const AdemcoDataSegment* dataSegment);

ADEMCO_EXPORT_SYMBOL int isAdemcoId(const char* standard, const char* id, int len);
ADEMCO_EXPORT_SYMBOL AdemcoPacketId getAdemcoId(const char* id, int len);

/*
* ademcoMake*Packet functions
* call with buff = NULL or len < length needed, will return length needed.
* if buff != NULL and len >= length needed, return length needed and copy data to buff
* otherwise return 0 for fail
*/

ADEMCO_EXPORT_SYMBOL int ademcoMakeNullPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeAckPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeNakPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId);
ADEMCO_EXPORT_SYMBOL int ademcoMakeHbPacket(uint8_t* buff, int len, uint16_t seq, const char* acct, AdemcoId ademcoId, 
											AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone, XData* xdata);

ADEMCO_EXPORT_SYMBOL AdemcoParseResult ademcoPacketParse(const uint8_t* buff, int len, AdemcoPacket* pkt, int* cb_commited);

ADEMCO_EXPORT_SYMBOL uint16_t ademcoCRC8(char c, uint16_t crc);
ADEMCO_EXPORT_SYMBOL uint16_t ademcoCRC16(const uint8_t* buff, int len, uint16_t crc);


typedef enum HbMachineStatus {
	HMS_ARM,
	HMS_HALF_ARM,
	HMS_DISARM,
	HMS_SETTING,
	HMS_COUNT,
	HMS_INVALID = -1,
}HbMachineStatus;

typedef enum HbMachineType {
	//! 0 WiFi���� ����ͣ�ã�
	HMT_WIFI,
	//! 1 ��������� ��δ���ã�
	HMT_CAMERA,
	//! 2 �򻯵����������� [ HB-5050G-4GW ]
	HMT_GPRS_IOT,
	//! 3 ����ģ������ [ HB-G250 ]
	HMT_NETMOD,
	//! 4 �Ľ�����ʽ���� [ HB-4040G HB-5050G HB-5050-4G ]
	HMT_GPRS,
	//! 5 Һ������ [ HB-BJQ560 HB-BJQ-560B ]
	HMT_LCD,
	//! 6 �������� [ HB-4040R HB-5050R ]
	HMT_WIRED,
	//! 7 ������� [ HTZ-G1000 HTZ-G1000-4G ]
	HMT_TRUE_COLOR,
	//! 8 ���������� [ HTZ-G1000 HTZ-G1000-4G ]
	HMT_3_SECTION,
	//! 9 ���������� [ HB-2050-4GW ]
	HMT_IOT,
	//! 10 GPRS�������Դ�绰 [ HB-2050 ]
	HMT_GPRS_PHONE,
	//! 11 NB������������
	HMT_NB,

	HMT_COUNT,
	HMT_INVALID = -1,
}HbMachineType;

typedef enum HbZoneProperty {
	//! �˾�ȫ��
	HZP_BUGLAR,
	//! �˾�����
	HZP_EMERGENCY,
	//! �𾯷���
	HZP_FIRE,
	//! в�ȷ���
	HZP_DURESS,
	//! ȼ������
	HZP_GAS,
	//! ��ˮ����
	HZP_WATER,
	//! �ֻ�
	HZP_SUB_MACHINE,
	//! ң����
	HZP_REMOTE_CONTROL,
	//! �˾����
	HZP_BUGLAR_HALF,
	//! ���η���
	HZP_SHIELD,
	//! �������
	HZP_DOOR_RING,
	//! ����
	HZP_RESERVED_0B,
	//! ����
	HZP_RESERVED_0C,
	//! ����
	HZP_RESERVED_0D,
	//! ����
	HZP_RESERVED_0E,
	//! ��·����
	//! 2019��9��2��16:01:58 ����𽨷��������
	HZP_BYPASS,

	HZP_COUNT,
	HZP_INVALID = -1,
}HbZoneProperty;

//! �������Ÿ����ͺŲ�ͬ����ͬ
ADEMCO_EXPORT_SYMBOL uint16_t hbZoneMax(HbMachineType type);
//! �������Ƿ�Ϸ������԰���0������
ADEMCO_EXPORT_SYMBOL int hbIsValidZone(HbMachineType type, uint16_t zone);
//! �������Ƿ�Ϸ��������Կ��԰���0������
ADEMCO_EXPORT_SYMBOL int hbIsValidZoneStrict(HbMachineType type, uint16_t zone);
//! �����Ƿ���Ͷ��ʹ��
ADEMCO_EXPORT_SYMBOL int hbMachineIsSelling(HbMachineType type);
//! �����Ƿ���в�������
ADEMCO_EXPORT_SYMBOL int hbMachineCanArm(HbMachineType type);
//! �����Ƿ���г�������
ADEMCO_EXPORT_SYMBOL int hbMachineCanDisarm(HbMachineType type);
//! �����Ƿ���Խ�������״̬
ADEMCO_EXPORT_SYMBOL int hbMachineCanEnterSettings(HbMachineType type);
//! �����Ƿ���а벼������
ADEMCO_EXPORT_SYMBOL int hbMachineCanHalfArm(HbMachineType type);
//! �����Ƿ���Ա����ź�ǿ��
ADEMCO_EXPORT_SYMBOL int hbMachineCanReportSignalStrength(HbMachineType type);
//! ���������Ƿ���Զ��ű���������ͨ�������簢�������ȣ�
ADEMCO_EXPORT_SYMBOL int hbMachineCanReportBySMS(HbMachineType type);
//! �����Ƿ�֧�����߷���
ADEMCO_EXPORT_SYMBOL int hbMachineHasWiredZone(HbMachineType type);
//! ������С���߷�����
ADEMCO_EXPORT_SYMBOL AdemcoZone hbWiredZoneMin(HbMachineType type);
//! ����������߷�����
ADEMCO_EXPORT_SYMBOL AdemcoZone hbWiredZoneMax(HbMachineType type);
//! �����Ƿ����ֱ��д��������ݣ�������룩
ADEMCO_EXPORT_SYMBOL int hbMachineCanDirectlyWriteZone(HbMachineType type);
//! �����Ƿ���Թ��طֻ�
ADEMCO_EXPORT_SYMBOL int hbMachineCanLinkSubMachine(HbMachineType type);
//! ���ݷ��������ж��Ƿ�֧��ʧ������
//! ʧ��������������ʵ�ֵĸ���������û��ϵ��������Ϊ������ֻ���������ԵĲſ�������
ADEMCO_EXPORT_SYMBOL int hbZonePropCanReportLost(HbZoneProperty zp);











// ��һ���Ըߵ��ֽڱ�ʾ��ʮ����������ת��Ϊ10�������ַ�������0xf���'0'~'9'�ַ�ֹͣ�������ַ�������
// ʾ�����������飺0x18 0x24 0x08 0x88 0x10 0x1f, 0xff������ַ���"18240888101"
ADEMCO_EXPORT_SYMBOL int hbHiLoArrayToDecStr(char* str, const uint8_t* arr, int len);
// ��һ��10�������ַ���תΪ�ߵ��ֽڱ�ʾ�����飬��ʡ�ռ䣬�������鳤��
ADEMCO_EXPORT_SYMBOL int hbDecStrToHiLoArray(uint8_t* arr, const char* str, int len);


#ifdef __cplusplus
}
#endif

#endif // __ADEMCO_H__
