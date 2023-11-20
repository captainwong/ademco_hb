/*
 * Ademco library
 * captainwong
 * 2022-6-13 rewrited this C version
 */

#include "ademco.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

#define dline dprintf("%s %d\n", __FILE__, __LINE__);
#define dmsg \
    dline;   \
    dprintf

#define ADEMCO_STRINGIFY(v) ADEMCO_STRINGIFY_HELPER(v)
#define ADEMCO_STRINGIFY_HELPER(v) #v

#define ADEMCO_VERSION_STRING_BASE  ADEMCO_STRINGIFY(ADEMCO_VERSION_MAJOR) "." \
                                    ADEMCO_STRINGIFY(ADEMCO_VERSION_MINOR) "." \
                                    ADEMCO_STRINGIFY(ADEMCO_VERSION_PATCH)

#if ADEMCO_VERSION_IS_RELEASE
# define ADEMCO_VERSION_STRING  ADEMCO_VERSION_STRING_BASE
#else
# define ADEMCO_VERSION_STRING  ADEMCO_VERSION_STRING_BASE "-" ADEMCO_VERSION_SUFFIX
#endif

unsigned int ademcoVersion(void) {
    return ADEMCO_VERSION_HEX;
}

const char* ademcoVersionString(void) {
    return ADEMCO_VERSION_STRING;
}

static uint8_t hex2char(uint8_t h) {
    h &= 0x0F;
    if (h > 9)
        return 'A' + h - 10;
    else
        return '0' + h;
}

static uint8_t char2hex(uint8_t c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else
        return 0xFF;
}

void ademcoPrint(const ademco_char_t* p, size_t len) {
    while (len--) {
        if (isprint(*(const uint8_t*)p))
            putchar(*p);
        else
            printf("\\x%02X", *(const uint8_t*)p);
        p++;
    }
    printf("\n");
}

uint8_t ademcoEncodeSignalStrength(uint8_t strength) {
    if (strength > ADEMCO_SIGNAL_STRENGTH_MAX)
        strength = ADEMCO_SIGNAL_STRENGTH_MAX;
    return ((strength / 10) << 4) | (strength % 10);
}

uint8_t ademcoDecodeSignalStrength(uint8_t code) {
    uint8_t strength = ((code >> 4) & 0x0F) * 10 + (code & 0x0F);
    if (strength > ADEMCO_SIGNAL_STRENGTH_MAX)
        strength = ADEMCO_SIGNAL_STRENGTH_MAX;
    return strength;
}

int ademcoIsValidAccount(const char* acct) {
    while (*acct) {
        if (!isxdigit(*acct++))
            return 0;
    }
    return 1;
}

int ademcoIsMachineStatusEvent(AdemcoEvent ademcoEvent) {
    return ademcoEvent == EVENT_ARM ||
           ademcoEvent == EVENT_HALFARM ||
           ademcoEvent == EVENT_HALFARM_1456 ||
           ademcoEvent == EVENT_DISARM;
}

int ademcoIsMachineTypeEvent(AdemcoEvent ademcoEvent) {
    return ademcoEvent == EVENT_I_AM_NET_MODULE ||
           ademcoEvent == EVENT_I_AM_GPRS ||
           ademcoEvent == EVENT_I_AM_LCD_MACHINE ||
           ademcoEvent == EVENT_I_AM_WIRE_MACHINE ||
           ademcoEvent == EVENT_I_AM_WIFI_MACHINE ||
           ademcoEvent == EVENT_I_AM_3_SECTION_MACHINE ||
           ademcoEvent == EVENT_I_AM_IOT_MACHINE ||
           ademcoEvent == EVENT_I_AM_TRUE_COLOR ||
           ademcoEvent == EVENT_I_AM_GPRS_IOT ||
           ademcoEvent == EVENT_I_AM_GPRS_PHONE ||
           ademcoEvent == EVENT_I_AM_NB_MACHINE ||
           ademcoEvent == EVENT_I_AM_WIFI2_MACHINE;
}

int ademcoIsEventNeedControlSource(AdemcoEvent ademcoEvent) {
    return ademcoEvent == EVENT_ARM ||
           ademcoEvent == EVENT_HALFARM ||
           ademcoEvent == EVENT_HALFARM_1456 ||
           ademcoEvent == EVENT_DISARM ||
           ademcoEvent == EVENT_EMERGENCY ||
           ademcoEvent == EVENT_DURESS ||
           ademcoEvent == EVENT_PHONE_USER_CANCLE_ALARM;
}

AdemcoEvent ademcoGetExceptionEventByResumeEvent(AdemcoEvent resumeEvent) {
    switch (resumeEvent) {
        case EVENT_RECONNECT:
            return EVENT_DISCONNECT;
        case EVENT_LOST_RECOVER:
            return EVENT_LOST;
        case EVENT_SERIAL485CONN:
            return EVENT_SERIAL485DIS;
        case EVENT_SUB_MACHINE_SENSOR_RESUME:
            return EVENT_SUB_MACHINE_SENSOR_EXCEPTION;
        case EVENT_SUB_MACHINE_POWER_RESUME:
            return EVENT_SUB_MACHINE_POWER_EXCEPTION;
        case EVENT_BATTERY_RECOVER:
            return EVENT_LOWBATTERY;
        case EVENT_BATTERY_EXCEPTION_RECOVER:
            return EVENT_BATTERY_EXCEPTION;
        case EVENT_OTHER_EXCEPTION_RECOVER:
            return EVENT_OTHER_EXCEPTION;
        case EVENT_AC_RECOVER:
            return EVENT_AC_BROKE;
        case EVENT_SOLARDISTURB_RECOVER:
            return EVENT_SOLARDISTURB;
        case EVENT_BADBATTERY_RECOVER:
            return EVENT_BADBATTERY;
        default:
            return EVENT_INVALID_EVENT;
    }
}

AdemcoEventLevel ademcoGetEventLevel(AdemcoEvent ademcoEvent) {
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
            // case EVENT_DOORRINGING: //case EVENT_CONN_HANGUP: //case EVENT_CONN_RESUME:
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

const char* ademcoEventToString(AdemcoEvent ademcoEvent) {
    switch (ademcoEvent) {
#define XX(name, code, zh) \
    case EVENT_##name:     \
        return #name;

        // ----标准安定宝协议事件----
        ADEMCO_STATUS_EVENTS_MAP(XX)
        ADEMCO_ALARM_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_EVENTS_MAP(XX)
        ADEMCO_RESUME_EVENTS_MAP(XX)

        // ---私有事件---------
        ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
        default:
            return "Undefined AdemcoEvent";
    }
}

#ifdef ADEMCO_ENABLE_CHINESE
const char* ademcoEventToStringChinese(AdemcoEvent ademcoEvent) {
    switch (ademcoEvent) {
#define XX(name, code, zh) \
    case EVENT_##name:     \
        return zh;

        // ----标准安定宝协议事件----
        ADEMCO_STATUS_EVENTS_MAP(XX)
        ADEMCO_ALARM_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_EVENTS_MAP(XX)
        ADEMCO_RESUME_EVENTS_MAP(XX)

        // ---私有事件---------
        ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
        default:
            return "未定义的安定宝事件";
    }
}
#endif

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

uint16_t ademcoCRC16(const ademco_char_t* buff, size_t len) {
    uint16_t crc = 0;
    while (len--)
        crc = (crc >> 8) ^ crc16Table[(crc ^ *buff++) & 0xFF];
    return crc;
}

size_t ademcoAppendDataSegment(ademco_char_t* buff, const char* acct, AdemcoId ademcoId,
                               AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone) {
    char* p = (char*)buff;
    if (ademcoEvent == EVENT_INVALID_EVENT) {
        *p++ = '[';
        *p++ = ']';
        return ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
    } else {
        *p++ = '[';

        *p++ = '#';
        if (acct && ademcoId == 0) {
            p += snprintf(p, 20, "%s", acct);
        } else {
            snprintf(p, 6 + 1, "%06X", ademcoId % ADEMCO_ID_SENTINEL);
            p += 6;
        }

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

        // assert(p - (char*)buff == ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN);

        *p = '\0';  // for debug convenience

        return p - (char*)buff;
    }
}

size_t ademcoAppendDataSegment2(AdemcoDataSegment* dataSegment, const char* acct,
                                AdemcoId ademcoId, AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone) {
    dataSegment->ademcoId = ademcoId;
    dataSegment->ademcoEvent = ademcoEvent;
    dataSegment->gg = gg;
    dataSegment->zone = zone;
    return dataSegment->raw_len = ademcoAppendDataSegment(dataSegment->raw,
                                                          acct, ademcoId,
                                                          ademcoEvent, gg, zone);
}

AdemcoParseResult ademcoParseDataSegment(const ademco_char_t* packet, size_t packet_len,
                                         AdemcoDataSegment* dataSegment, AdemcoParseError* err) {
    if (packet_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN &&
        packet[0] == '[' && packet[1] == ']') {
        memset(dataSegment, 0, sizeof(*dataSegment));
        strcpy((char*)dataSegment->raw, "[]");
        dataSegment->raw_len = 2;
        return RESULT_OK;
    } else if (packet_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN &&
               packet[0] == '[' &&
               packet[packet_len - 1] == ']') {  // [#000000|1400 00 000]
        do {
            const char* p = packet + 2;
            size_t acct_len = packet_len - 15;
            if (acct_len < 6) {
                ADEMCO_FILL_PARSE_ERROR(err, "acct_len < 6");
                break;
            }

            char temp[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX] = {0};
            strncpy(temp, (const char*)p, acct_len);
            temp[acct_len] = '\0';
            dataSegment->ademcoId = (AdemcoId)strtoul(temp, NULL, 16);
            p += acct_len;

            if (*p++ != '|') {
                ADEMCO_FILL_PARSE_ERROR(err, "*p++ != '|'");
                break;
            }
            strncpy(temp, (const char*)p, 4);
            temp[4] = '\0';
            dataSegment->ademcoEvent = (AdemcoEvent)(atoi(temp));
            p += 4;

            if (*p++ != ' ') {
                ADEMCO_FILL_PARSE_ERROR(err, "*p++ != ' '");
                break;
            }
            if (*p == 'E' && *(p + 1) == 'E') {
                dataSegment->gg = 0xEE;
            } else if (*p == 'C' && *(p + 1) == 'C') {
                dataSegment->gg = 0xCC;
            } else {
                dataSegment->gg = (AdemcoGG)((*p - '0') * 10 + (*(p + 1) - '0'));
            }
            p += 2;

            if (*p++ != ' ') {
                ADEMCO_FILL_PARSE_ERROR(err, "*p++ != ' '");
                break;
            }
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

    ADEMCO_FILL_PARSE_ERROR(err, "dig in source code for more info");
    return RESULT_ERROR;
}

size_t ademcoDataSegmentToCongwinFe100(ademco_char_t* fe100, size_t fe100_len,
                                       const AdemcoDataSegment* dataSegment) {
    if (dataSegment->raw_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN) {
        if (fe100_len < 2) return 0;
        fe100[0] = '\n';
        fe100[1] = '\r';
        return 2;
    } else if (dataSegment->raw_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN) {
        if (fe100_len < CONGWIN_FE100_PACKET_LEN) return 0;

        char* p = fe100;
        *p++ = '\n';
        *p++ = ' ';

        *p++ = '0';  // RG
        *p++ = '0';
        *p++ = ' ';

        snprintf(p,
                 fe100_len - (p - fe100),
                 "%08d",
                 dataSegment->ademcoId % 100000000);
        p += 8;

        *p++ = ' ';  // 18
        *p++ = '1';
        *p++ = '8';
        *p++ = ' ';

        // E for open, R for close
        if (dataSegment->ademcoEvent / 1000 == 1 ||
            dataSegment->ademcoEvent == EVENT_HALFARM)
            *p++ = 'E';
        else
            *p++ = 'R';

        if (dataSegment->ademcoEvent == EVENT_OFFLINE) {
            *p++ = '3';
            *p++ = 'B';
            *p++ = '2';
        } else if (dataSegment->ademcoEvent == EVENT_ONLINE) {
            *p++ = '3';
            *p++ = 'B';
            *p++ = '0';
        } else {
            snprintf(p,
                     fe100_len - (p - fe100),
                     "%03d",
                     dataSegment->ademcoEvent % 1000);
            p += 3;
        }

        *p++ = ' ';  // FE100, gg === 0
        *p++ = '0';
        *p++ = '0';
        *p++ = ' ';

        if (ademcoIsMachineStatusEvent(dataSegment->ademcoEvent))
            *p++ = 'U';  // user event
        else
            *p++ = 'C';  // zone event

        snprintf(p,
                 fe100_len - (p - fe100),
                 "%03d",
                 dataSegment->zone % 1000);
        p += 3;

        *p++ = ' ';
        *p++ = '\r';
        return p - fe100;
    }
    return 0;
}

void ademcoXDataInit(AdemcoXDataSegment* xdata) {
    memset(xdata, 0, sizeof *xdata);
}

int ademcoXDataConvert(AdemcoXDataSegment* xdata,
                       AdemcoXDataLengthFormat xlf) {
    if (!xdata) {
        return ADEMCO_ERR;
    }
    if (xdata->lenghFormat == xlf) {
        return ADEMCO_OK;
    }

    size_t len = ademcoXDataGetValidContentLen(xdata);
    char raw[ADEMCO_PACKET_XDATA_MAX_LEN];

    if (xlf == TWO_HEX) {
        if (len + 6 > ADEMCO_PACKET_XDATA_MAX_LEN)
            return ADEMCO_ERR;
        xdata->raw[5 + ademcoXDataGetValidContentLen(xdata)] = '\0';
        len = ademcoHexStrToArray((uint8_t*)raw,
                                  ademcoXDataGetValidContentAddr(xdata),
                                  0x0F);
        ademcoMakeXData(xdata, TWO_HEX,
                        AdemcoXDataTransform_as_is,
                        raw, len);
    } else {
        memcpy(raw, ademcoXDataGetValidContentAddr(xdata), len);
        ademcoMakeXData(xdata, FOUR_DECIMAL,
                        AdemcoXDataTransform_to_ascii,
                        raw, len);
    }
    return ADEMCO_OK;
}

#ifdef SWIG
static
#endif
const char* ademcoXDataGetValidContentAddr(const AdemcoXDataSegment* xdata) {
    if (xdata->lenghFormat == TWO_HEX && xdata->raw_len > 4)
        return xdata->raw + 3;
    else if (xdata->lenghFormat == FOUR_DECIMAL && xdata->raw_len > 6)
        return xdata->raw + 5;
    else
        return NULL;
}

int ademcoXDataMemcmp(const AdemcoXDataSegment* xdata,
                      const void* buf, size_t buf_len) {
    if (ademcoXDataGetValidContentLen(xdata) != buf_len) return 0;
    const void* p = ademcoXDataGetValidContentAddr(xdata);
    if (!p) return 0;
    return memcmp(p, buf, buf_len);
}

size_t ademcoXDataGetValidContentLen(const AdemcoXDataSegment* xdata) {
    if (xdata->lenghFormat == TWO_HEX && xdata->raw_len > 4)
        return xdata->raw_len - 4;
    else if (xdata->lenghFormat == FOUR_DECIMAL && xdata->raw_len > 6)
        return xdata->raw_len - 6;
    else
        return 0;
}

size_t ademcoXDataCopy(AdemcoXDataSegment* dst,
                       const AdemcoXDataSegment* src) {
    if (dst && src && src->raw_len > 0) {
        memcpy(dst, src, sizeof(AdemcoXDataSegment));
        return src->raw_len;
    }
    return 0;
}

int ademcoMakeXData(AdemcoXDataSegment* xdata,
                    AdemcoXDataLengthFormat xlf,
                    AdemcoXDataTransform xtr,
                    const ademco_char_t* content, size_t len) {
    char transformed[ADEMCO_PACKET_XDATA_MAX_LEN];
    size_t translen;
    len &= 0xFFFF;
    translen = len & 0xFFFF;

    if (xtr == AdemcoXDataTransform_as_is)
        memcpy(transformed, content, len);
    else if (len * 2 < ADEMCO_PACKET_XDATA_MAX_LEN)
        translen = ademcoHexArrayToStr(transformed,
                                       (const uint8_t*)content, len) & 0xFFFF;
    else
        return ADEMCO_ERR;

    xdata->lenghFormat = xlf;
    if (xlf == TWO_HEX &&
        translen + 4 < ADEMCO_PACKET_XDATA_MAX_LEN) {
        ademco_char_t* p = xdata->raw;
        *p++ = '[';
        *p++ = (translen & 0xFFFF) >> 8;
        *p++ = (translen & 0xFFFF) & 0xFF;
        memcpy(p, transformed, translen);
        p += translen;
        *p++ = ']';
        xdata->raw_len = p - xdata->raw;
        return ADEMCO_OK;
    } else if (xlf == FOUR_DECIMAL &&
               translen + 6 < ADEMCO_PACKET_XDATA_MAX_LEN) {
        ademco_char_t* p = xdata->raw;
        *p++ = '[';
        char temp[5];
        snprintf(temp, sizeof(temp), "%04zX", translen & 0xFFFF);
        memcpy(p, temp, 4);
        p += 4;
        memcpy(p, transformed, translen);
        p += translen;
        *p++ = ']';
        xdata->raw_len = p - xdata->raw;
        return ADEMCO_OK;
    } else
        return ADEMCO_ERR;
}

int isAdemcoPacketId(const char* standard, const char* id, size_t len) {
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

AdemcoPacketId getAdemcoPacketId(const char* id, size_t len) {
    for (int i = 0; i < AID_COUNT; i++)
        if (isAdemcoPacketId(ids[i], id, len))
            return (AdemcoPacketId)(i);
    return AID_INVALID;
}

const char* ademcoPacketIdToString(AdemcoPacketId id) {
    if (0 <= id && id < AID_COUNT)
        return ids[id];
    else
        return "Unkown AdemcoPacketId";
}

static void getNowTimestamp(char* buff) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    snprintf(buff, ADEMCO_PACKET_TIMESTAMP_LEN + 1,
             "_%02d:%02d:%02d,%02d-%02d-%04d",
             tm->tm_hour, tm->tm_min, tm->tm_sec,
             tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);
}

static void getGmtTimestamp(char* buff) {
    time_t now = time(NULL);
    struct tm* tm = gmtime(&now);
    snprintf(buff, ADEMCO_PACKET_TIMESTAMP_LEN + 1,
             "_%02d:%02d:%02d,%02d-%02d-%04d",
             tm->tm_hour, tm->tm_min, tm->tm_sec,
             tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);
}

size_t ademcoMakeEmptyDataPacket(ademco_char_t* dst_buff, size_t len,
                                 const char* id, uint16_t seq,
                                 const char* acct, AdemcoId ademcoId) {
    char buff[ADEMCO_PACKET_MAX_LEN];
    char* p = buff;
    char* pcrc = buff + 1;
    char* plen = buff + 5;
    char* pid = buff + 9;

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
    size_t packet_len = p - buff;
    //                           \n  crc len  \r
    size_t ademco_len = packet_len - 1 - 4 - 4 - 1;
    snprintf(temp, 5, "%04zX", ademco_len);
    memcpy(plen, temp, 4);
    uint16_t crc = ademcoCRC16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademcoMakeAdmEmptyDataPacket(ademco_char_t* dst_buff, size_t len,
                                    const char* id, uint16_t seq,
                                    const char* acct, AdemcoId ademcoId) {
    char buff[ADEMCO_PACKET_MAX_LEN];
    char* p = buff;
    char* pcrc = buff + 1;
    char* plen = buff + 5;
    char* pid = buff + 9;

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
    getGmtTimestamp((char*)p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    char temp[5];
    size_t packet_len = p - buff;
    //                           \n  crc len  \r
    size_t ademco_len = packet_len - 1 - 4 - 4 - 1;
    snprintf(temp, 5, "%04zX", ademco_len);
    memcpy(plen, temp, 4);
    uint16_t crc = ademcoCRC16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademcoMakeNullPacket(ademco_char_t* buff, size_t len,
                            uint16_t seq, const char* acct,
                            AdemcoId ademcoId) {
    return ademcoMakeEmptyDataPacket(buff, len,
                                     ADEMCO_PACKET_ID_NULL,
                                     seq, acct, ademcoId);
}

size_t ademcoMakeAdmNullPacket(ademco_char_t* buff, size_t len,
                               uint16_t seq, const char* acct,
                               AdemcoId ademcoId) {
    return ademcoMakeAdmEmptyDataPacket(buff, len,
                                        ADEMCO_PACKET_ID_NULL,
                                        seq, acct, ademcoId);
}

size_t ademcoMakeAckPacket(ademco_char_t* buff, size_t len,
                           uint16_t seq, const char* acct,
                           AdemcoId ademcoId) {
    return ademcoMakeEmptyDataPacket(buff, len,
                                     ADEMCO_PACKET_ID_ACK,
                                     seq, acct, ademcoId);
}

size_t ademcoMakeNakPacket(ademco_char_t* buff, size_t len,
                           uint16_t seq, const char* acct,
                           AdemcoId ademcoId) {
    return ademcoMakeEmptyDataPacket(buff, len,
                                     ADEMCO_PACKET_ID_NAK,
                                     seq, acct, ademcoId);
}

size_t ademcoMakeHbPacket(ademco_char_t* dst_buff, size_t len,
                          uint16_t seq, const char* acct,
                          AdemcoId ademcoId, AdemcoEvent ademcoEvent,
                          AdemcoGG gg, AdemcoZone zone,
                          const AdemcoXDataSegment* xdata) {
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
    p += ademcoAppendDataSegment(p, acct, ademcoId, ademcoEvent, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        memcpy(p, xdata->raw, xdata->raw_len);
        p += xdata->raw_len;
    }
    getNowTimestamp(p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    char temp[5];
    size_t packet_len = p - buff;
    //                           \n  crc len  \r
    size_t ademco_len = packet_len - 1 - 4 - 4 - 1;
    snprintf(temp, 5, "%04zX", ademco_len);
    memcpy(plen, temp, 4);
    uint16_t crc = ademcoCRC16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    }
    return 0;
}

size_t ademcoMakeAdmPacket(ademco_char_t* dst_buff, size_t len,
                           uint16_t seq, const char* acct, AdemcoId ademcoId,
                           AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone,
                           const AdemcoXDataSegment* xdata) {
    char buff[ADEMCO_PACKET_MAX_LEN];
    char* p = buff;
    char* pcrc = buff + 1;
    char* plen = buff + 5;
    char* pid = buff + 9;

    buff[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, ADEMCO_PACKET_ID_ADM_CID, strlen(ADEMCO_PACKET_ID_ADM_CID));
    p = pid + strlen(ADEMCO_PACKET_ID_ADM_CID);
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
    p += ademcoAppendDataSegment(p, acct, ademcoId, ademcoEvent, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        memcpy(p, xdata->raw, xdata->raw_len);
        p += xdata->raw_len;
    }
    getGmtTimestamp(p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    char temp[5];
    size_t packet_len = p - buff;
    //                           \n  crc len  \r
    size_t ademco_len = packet_len - 1 - 4 - 4 - 1;
    snprintf(temp, 5, "%04zX", ademco_len);
    memcpy(plen, temp, 4);
    uint16_t crc = ademcoCRC16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    }
    return 0;
}

static void copyAcct2AdemcoPacket(AdemcoPacket* pkt, const char* acct) {
    if (acct) {
        size_t len = strlen(acct);
        if (len > ADEMCO_PACKET_ACCT_MAX_LEN)
            len = ADEMCO_PACKET_ACCT_MAX_LEN;
        memcpy(pkt->acct, acct, len);
        pkt->acct[len] = '\0';
    } else
        pkt->acct[0] = '\0';
}

size_t ademcoMakeNullPacket2(AdemcoPacket* pkt, uint16_t seq,
                             const char* acct, AdemcoId ademcoId) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->data.ademcoId = ademcoId;
    pkt->id = AID_NULL;
    return pkt->raw_len = ademcoMakeNullPacket(pkt->raw, sizeof(pkt->raw),
                                               seq, acct, ademcoId);
}

size_t ademcoMakeAckPacket2(AdemcoPacket* pkt, uint16_t seq,
                            const char* acct, AdemcoId ademcoId) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->data.ademcoId = ademcoId;
    pkt->id = AID_ACK;
    return pkt->raw_len = ademcoMakeAckPacket(pkt->raw, sizeof(pkt->raw),
                                              seq, acct, ademcoId);
}

size_t ademcoMakeNakPacket2(AdemcoPacket* pkt, uint16_t seq,
                            const char* acct, AdemcoId ademcoId) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->data.ademcoId = ademcoId;
    pkt->id = AID_NAK;
    return pkt->raw_len = ademcoMakeNakPacket(pkt->raw, sizeof(pkt->raw),
                                              seq, acct, ademcoId);
}

size_t ademcoMakeHbPacket2(AdemcoPacket* pkt, uint16_t seq,
                           const char* acct, AdemcoId ademcoId,
                           AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone,
                           const AdemcoXDataSegment* xdata) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademcoAppendDataSegment2(&pkt->data, acct, ademcoId, ademcoEvent, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        pkt->xdata.lenghFormat = xdata->lenghFormat;
        memcpy(pkt->xdata.raw, xdata->raw, xdata->raw_len);
        pkt->xdata.raw_len = xdata->raw_len;
    } else
        memset(&pkt->xdata, 0, sizeof(*xdata));
    pkt->id = AID_HB;
    return pkt->raw_len = ademcoMakeHbPacket(pkt->raw, sizeof(pkt->raw),
                                             seq, acct, ademcoId, ademcoEvent,
                                             gg, zone, xdata);
}

size_t ademcoMakeAdmPacket2(AdemcoPacket* pkt, uint16_t seq,
                            const char* acct, AdemcoId ademcoId,
                            AdemcoEvent ademcoEvent, AdemcoGG gg, AdemcoZone zone,
                            const AdemcoXDataSegment* xdata) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademcoAppendDataSegment2(&pkt->data, acct, ademcoId, ademcoEvent, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        pkt->xdata.lenghFormat = xdata->lenghFormat;
        memcpy(pkt->xdata.raw, xdata->raw, xdata->raw_len);
        pkt->xdata.raw_len = xdata->raw_len;
    } else
        memset(&pkt->xdata, 0, sizeof(*xdata));
    pkt->id = AID_ADM_CID;
    return pkt->raw_len = ademcoMakeAdmPacket(pkt->raw, sizeof(pkt->raw),
                                              seq, acct, ademcoId, ademcoEvent,
                                              gg, zone, xdata);
}

AdemcoParseResult ademcoPacketParse(const ademco_char_t* buff, size_t len,
                                    AdemcoPacket* pkt, size_t* cb_commited,
                                    AdemcoParseError* err) {
    if (len < 9) {
        return RESULT_NOT_ENOUGH;
    }
    do {
        const char* p = buff;
        if (*p++ != ADEMCO_PACKET_PREFIX) {
            ADEMCO_FILL_PARSE_ERROR(err, "*p++ != ADEMCO_PACKET_PREFIX");
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
        size_t len_needed = 9 + pkt->len + 1;
        if (len < len_needed)
            return RESULT_NOT_ENOUGH;
        if (len_needed >= ADEMCO_PACKET_MAX_LEN) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "len_needed >= ADEMCO_PACKET_MAX_LEN");
            return RESULT_ERROR;
        }

        const char* pid = p;
        const char* pcr = pid + pkt->len;

        if (pcr > buff + len || *pcr != ADEMCO_PACKET_SUFIX) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "pcr > buff + len || *pcr != ADEMCO_PACKET_SUFIX");
            break;
        }

        pkt->crc = ademcoCRC16(pid, pkt->len);
        if (pkt->crc != crc) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "pkt->crc != crc");
            break;
        }

        // id
        if (*pid != '\"') {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "*pid != '\"'");
            break;
        }
        p = pid + 1;
        while (p < pcr && *p != '\"') {
            p++;
        }
        if (p >= pcr || *p != '\"') {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "p >= pcr || *p != '\"'");
            break;
        }
        pkt->id = getAdemcoPacketId((const char*)pid, ++p - pid);
        if (pkt->id == AID_INVALID) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "pkt->id == AID_INVALID");
            break;
        }

        // seq
        const char* pseq = p;
        while (p < pcr && *p != 'R' && *p != 'L') {
            p++;
        }
        if (p - pseq != 4 || (*p != 'R' && *p != 'L')) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "p - pseq != 4 || (*p != 'R' && *p != 'L')");
            break;
        }
        memcpy(temp, pseq, 4);
        temp[4] = '\0';
        pkt->seq = strtoul(temp, NULL, 10) & 0xFFFF;
        if (pkt->seq >= 10000) {
            pkt->seq = 0;
        }

        // *rrcvr
        if (*p == 'R') {  // rrcvr exists
            // const char* prcvr = p;
            while (p < pcr && *p != 'L' && *p != '#') {
                p++;
            }
            if (p >= pcr || (*p != 'L' && *p != '#')) {
                dline;
                ADEMCO_FILL_PARSE_ERROR(err, "p >= pcr || (*p != 'L' && *p != '#')");
                break;
            }
            // only check if format is correct, ignore it's content
        } else if (*p == 'L') {  // rrcvr not exits
                                 // pass
        } else {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "both LPREF and RRCVR not found");
            break;
        }

        // lpref
        if (*p != 'L') {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "LPREF not found");
            break;
        }
        while (p < pcr && *p != '#') {
            p++;
        }
        // only check if format is correct, ignore it's content
        if (p >= pcr || *p != '#') {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "# not found");
            break;
        }
        // lpref passed

        // #acct
        const char* pacct = ++p;
        while (p < pcr && *p != '[') {
            if (!isxdigit(*p)) {
                p = NULL;
                dline;
                ADEMCO_FILL_PARSE_ERROR(err, "acct contains non-hex characters");
                break;
            }
            p++;
        }
        if (p == NULL ||
            p >= pcr ||
            *p != '[' ||
            p - pacct >= ADEMCO_PACKET_ACCT_MAX_LEN) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "data's [ not found");
            break;
        }
        strncpy(pkt->acct, (const char*)pacct, p - pacct);
        pkt->acct[p - pacct] = '\0';

        // data
        const char* pdata = p;
        while (p < pcr && *p != ']') {
            p++;
        }
        if (p >= pcr || *p != ']') {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "data's ] not found");
            break;
        }
        if (ademcoParseDataSegment(pdata, ++p - pdata, &pkt->data, err) != RESULT_OK) {
            dline;
            break;
        }

        // *xdata
        if (*p == '[') {  // xdata exists
            const char* pxdata = p++;
            AdemcoXDataLengthFormat xlf = FOUR_DECIMAL;
            size_t valid_len;
            for (int i = 0; i < 4; i++)
                if (!isxdigit(*(uint8_t*)(p + i)))
                    xlf = TWO_HEX;

            if (xlf == FOUR_DECIMAL) {
                memcpy(temp, pxdata + 1, 4);
                temp[4] = '\0';
                valid_len = strtoul(temp, NULL, 16);
                p += 4 + valid_len;
            } else {
                valid_len = (pxdata[1] << 8) | pxdata[2];
                p += 2 + valid_len;
            }

            if (p >= pcr || *p != ']' || *(p + 1) != '_') {
                dline;
                ADEMCO_FILL_PARSE_ERROR(err, "xdata's ] not found or next char is not _");
                break;
            }
            pkt->xdata.lenghFormat = xlf;
            pkt->xdata.raw_len = ++p - pxdata;
            memcpy(pkt->xdata.raw, pxdata, pkt->xdata.raw_len);
        } else {
            pkt->xdata.raw_len = 0;
        }

        // timestamp
        if (pcr - p != ADEMCO_PACKET_TIMESTAMP_LEN) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "timestamp length not correct");
            break;
        }
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
            if (pkt->timestamp < 0) {  // use local time instead
                pkt->timestamp = time(NULL);
            }
            p += ADEMCO_PACKET_TIMESTAMP_LEN;
        }

        if (p++ != pcr) {
            dline;
            ADEMCO_FILL_PARSE_ERROR(err, "packet length not correct");
            break;
        }
        pkt->raw_len = *cb_commited = p - buff;
        if (pkt->raw != buff)
            memcpy(pkt->raw, buff, pkt->raw_len);
        return RESULT_OK;
    } while (0);

    return RESULT_ERROR;
}

size_t ademcoHiLoArrayToDecStr(ademco_char_t* str, const uint8_t* arr, size_t len) {
    char* p = str;
    for (size_t i = 0; i < len; i++) {
        char c = (arr[i] >> 4) & 0x0F;
        if (c > 9) {
            return p - str;
        }
        *p++ = c + '0';
        c = (arr[i] & 0x0F);
        if (c > 9) {
            return p - str;
        }
        *p++ = c + '0';
    }
    return p - str;
}

size_t ademcoHiLoArrayToDecStr2(ademco_char_t* str, const uint8_t* arr, size_t len) {
    char* p = str;
    for (size_t i = 0; i < len; i++) {
        char c = (arr[i] >> 4) & 0x0F;
        if (c > 9) {
            if (i < 6) {
                *p++ = c - 10 + 'A';
            } else {
                return p - str;
            }
        } else {
            *p++ = c + '0';
        }

        c = (arr[i] & 0x0F);
        if (c > 9) {
            if (i < 6) {
                *p++ = c - 10 + 'A';
            } else {
                return p - str;
            }
        } else {
            *p++ = c + '0';
        }
    }
    return p - str;
}

size_t ademcoDecStrToHiLoArray(uint8_t* arr, size_t len, const char* str) {
    char* p = (char*)arr;
    size_t slen = str ? strlen(str) : 0;
    if (slen > len * 2)
        slen = len * 2;
    for (size_t i = 0; i < slen; i += 2) {
        char hi = str[i];
        if ('0' <= hi && hi <= '9') {
            if (i + 1 < slen) {
                char lo = str[i + 1];
                if ('0' <= lo && lo <= '9')
                    *p++ = (hi << 4) | (lo & 0x0F);
                else {
                    *p++ = (hi << 4) | 0x0F;
                    break;
                }
            } else {
                *p++ = (hi << 4) | 0x0F;
                break;
            }
        } else
            break;
    }
    while ((char*)arr + len > p)
        *p++ = 0xFF;
    return len;
}

size_t ademcoDecStrToHiLoArray2(uint8_t* arr, size_t len, const char* str) {
    char* p = (char*)arr;
    size_t slen = str ? strlen(str) : 0;
    if (slen > len * 2)
        slen = len * 2;
    for (size_t i = 0; i < slen; i += 2) {
        char hi = str[i];
        if (isxdigit(hi)) {
            if (i + 1 < slen) {
                char lo = str[i + 1];
                if (isxdigit(lo))
                    *p++ = (char2hex(hi) << 4) | (char2hex(lo) & 0x0F);
                else {
                    *p++ = (char2hex(hi) << 4) | 0x0F;
                    break;
                }
            } else {
                *p++ = (char2hex(hi) << 4) | 0x0F;
                break;
            }
        } else {
            break;
        }
    }
    while ((char*)arr + len > p)
        *p++ = 0xFF;
    return len;
}

size_t ademcoHexArrayToStr(char* str, const uint8_t* arr, size_t len) {
    char* p = str;
    for (size_t i = 0; i < len; i++) {
        *p++ = hex2char((arr[i] >> 4) & 0x0F);
        *p++ = hex2char(arr[i] & 0x0F);
    }
    return p - str;
}

size_t ademcoHexStrToArray(uint8_t* arr, const char* str, uint8_t padding) {
    uint8_t* p = arr;
    uint8_t hi = 0, lo = 0;
    size_t slen = str ? strlen(str) : 0;
    padding &= 0x0F;
    for (size_t i = 0; i < slen / 2; i++) {
        char c = str[i * 2];
        if ((hi = char2hex(c)) == 0xFF)
            return 0;
        if (i * 2 + 1 < slen) {
            c = str[i * 2 + 1];
            if ((lo = char2hex(c)) == 0xFF)
                return 0;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}

size_t ademcoHexStrToArrayN(uint8_t* arr, const char* str,
                            size_t len, uint8_t padding) {
    uint8_t* p = arr;
    uint8_t hi = 0, lo = 0;
    padding &= 0x0F;
    for (size_t i = 0; i < len / 2; i++) {
        char c = str[i * 2];
        if ((hi = char2hex(c)) == 0xFF)
            return 0;
        if (i * 2 + 1 < len) {
            c = str[i * 2 + 1];
            if ((lo = char2hex(c)) == 0xFF)
                return 0;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}

size_t ademcoHexStrToArrayN_allow_non_hex_str(uint8_t* arr,
                                              const char* str,
                                              size_t len,
                                              uint8_t padding) {
    uint8_t* p = arr;
    uint8_t hi = 0, lo = 0;
    padding &= 0x0F;
    for (size_t i = 0; i < len / 2; i++) {
        char c = str[i * 2];
        if ((hi = char2hex(c)) == 0xFF)
            hi = padding;
        if (i * 2 + 1 < len) {
            c = str[i * 2 + 1];
            if ((lo = char2hex(c)) == 0xFF)
                lo = padding;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}
