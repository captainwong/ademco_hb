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

#if ADEMCO_USE_UNIQUE_BUF
static char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif

#define ADEMCO_STRINGIFY(v) ADEMCO_STRINGIFY_HELPER(v)
#define ADEMCO_STRINGIFY_HELPER(v) #v

#define ADEMCO_VERSION_STRING_BASE ADEMCO_STRINGIFY(ADEMCO_VERSION_MAJOR) "." ADEMCO_STRINGIFY(ADEMCO_VERSION_MINOR) "." ADEMCO_STRINGIFY(ADEMCO_VERSION_PATCH)

#if ADEMCO_VERSION_IS_RELEASE
#define ADEMCO_VERSION_STRING ADEMCO_VERSION_STRING_BASE
#else
#define ADEMCO_VERSION_STRING ADEMCO_VERSION_STRING_BASE "-" ADEMCO_VERSION_SUFFIX
#endif

uint32_t ademco_version(void) {
    return ADEMCO_VERSION_HEX;
}

#if ADEMCO_ENABLE_TO_STRING
const char* ademco_version_string(void) {
    return ADEMCO_VERSION_STRING;
}
#endif

uint8_t ademco_hex2char(uint8_t h) {
    h &= 0x0F;
    if (h > 9)
        return 'A' + h - 10;
    else
        return '0' + h;
}

uint8_t ademco_char2hex(uint8_t c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else
        return 0xFF;
}

void ademco_print(const ademco_char_t* p, size_t len) {
    while (len--) {
        if (isprint(*(const uint8_t*)p))
            putchar(*p);
        else
            printf("\\x%02X", *(const uint8_t*)p);
        p++;
    }
    printf("\n");
}

uint8_t ademco_encode_signal_strength(uint8_t strength) {
    if (strength > ADEMCO_SIGNAL_STRENGTH_MAX)
        strength = ADEMCO_SIGNAL_STRENGTH_MAX;
    return bcd_encode(strength);
}

uint8_t ademco_decode_signal_strength(uint8_t bcd_strength) {
    uint8_t strength = bcd_decode(bcd_strength);
    if (strength > ADEMCO_SIGNAL_STRENGTH_MAX)
        strength = ADEMCO_SIGNAL_STRENGTH_MAX;
    return strength;
}

bool ademco_is_valid_account(const char* acct) {
    size_t ADEMCO_BUF_MEMORY_MODIFIER len = 0;
    int ADEMCO_BUF_MEMORY_MODIFIER ishex = 0;
    while (*acct && len < ADEMCO_PACKET_ACCT_MAX_LEN) {
        if (isdigit(*acct)) {
            len++;
        } else if (isxdigit(*acct) && len < ADEMCO_PACKET_ACCT_MAC_LEN) {
            len++;
            ishex = 1;
        } else {
            return 0;
        }
        acct++;
    }
    return *acct == '\0' &&
           (ishex ? len == ADEMCO_PACKET_ACCT_MAC_LEN : (len >= ADEMCO_PACKET_ACCT_MIN_LEN));
}

bool ademco_is_valid_password(const char* pwd) {
    size_t ADEMCO_BUF_MEMORY_MODIFIER len = 0;
    while (*pwd && len < ADEMCO_PACKET_PWD_MAX_LEN) {
        if (isdigit(*pwd)) {
            len++;
        } else {
            return false;
        }
        pwd++;
    }
    return *pwd == '\0' && len >= ADEMCO_PACKET_PWD_MIN_LEN;
}

bool ademco_is_machine_status_event(ademco_event_t ademco_event) {
    return ademco_event == EVENT_ARM_AWAY ||
           ademco_event == EVENT_ARM_STAY ||
           ademco_event == EVENT_ARM_STAY_1456 ||
           ademco_event == EVENT_DISARM;
}

bool ademco_is_machine_type_event(ademco_event_t ademco_event) {
    return ademco_event == EVENT_I_AM_NET_MODULE ||
           ademco_event == EVENT_I_AM_GPRS ||
           ademco_event == EVENT_I_AM_LCD ||
           ademco_event == EVENT_I_AM_WIRE ||
           ademco_event == EVENT_I_AM_WIFI ||
           ademco_event == EVENT_I_AM_3_SECTION ||
           ademco_event == EVENT_I_AM_IOT ||
           ademco_event == EVENT_I_AM_TRUE_COLOR ||
           ademco_event == EVENT_I_AM_GPRS_IOT ||
           ademco_event == EVENT_I_AM_GPRS_PHONE ||
           ademco_event == EVENT_I_AM_NB ||
           ademco_event == EVENT_I_AM_WIFI2;
}

bool ademco_is_event_need_control_source(ademco_event_t ademco_event) {
    return ademco_event == EVENT_ARM_AWAY ||
           ademco_event == EVENT_ARM_STAY ||
           ademco_event == EVENT_ARM_STAY_1456 ||
           ademco_event == EVENT_DISARM ||
           ademco_event == EVENT_EMERGENCY ||
           ademco_event == EVENT_DURESS ||
           ademco_event == EVENT_PHONE_USER_CANCLE_ALARM;
}

ademco_event_t ademco_get_exception_event_by_recover_event(ademco_event_t recover_event) {
    switch (recover_event) {
        case EVENT_DISCONNECT_RECOVER:
            return EVENT_DISCONNECT;
        case EVENT_LOST_RECOVER:
            return EVENT_LOST;
        case EVENT_SERIAL_485_RECOVER:
            return EVENT_SERIAL_485_DIS;
        case EVENT_SUB_MACHINE_SENSOR_RECOVER:
            return EVENT_SUB_MACHINE_SENSOR_EXCEPTION;
        case EVENT_SUB_MACHINE_POWER_RECOVER:
            return EVENT_SUB_MACHINE_POWER_EXCEPTION;
        case EVENT_LOW_BATTERY_RECOVER:
            return EVENT_LOW_BATTERY;
        case EVENT_POWER_EXCEPTION_RECOVER:
            return EVENT_POWER_EXCEPTION;
        case EVENT_OTHER_EXCEPTION_RECOVER:
            return EVENT_OTHER_EXCEPTION;
        case EVENT_AC_RECOVER:
            return EVENT_AC_BROKEN;
        case EVENT_SOLAR_DISTURB_RECOVER:
            return EVENT_SOLAR_DISTURB;
        case EVENT_BAD_BATTERY_RECOVER:
            return EVENT_BAD_BATTERY;
        default:
            return EVENT_INVALID_EVENT;
    }
}

ademco_event_level_t ademco_get_event_level(ademco_event_t ademco_event) {
    switch (ademco_event) {
        case EVENT_ARM_AWAY:
        case EVENT_DISARM:
        case EVENT_ARM_STAY:
        case EVENT_ARM_STAY_1456:
            return EVENT_LEVEL_STATUS;

        case EVENT_BAD_BATTERY_RECOVER:
        case EVENT_AC_RECOVER:
        case EVENT_DISCONNECT_RECOVER:
        case EVENT_LOST_RECOVER:
        case EVENT_SERIAL_485_RECOVER:
        case EVENT_SUB_MACHINE_SENSOR_RECOVER:
        case EVENT_SUB_MACHINE_POWER_RECOVER:
        case EVENT_LOW_BATTERY_RECOVER:
        case EVENT_POWER_EXCEPTION_RECOVER:
        case EVENT_OTHER_EXCEPTION_RECOVER:
        case EVENT_SOLAR_DISTURB_RECOVER:
            return EVENT_LEVEL_EXCEPTION_RECOVER;

        case EVENT_AC_BROKEN:
        case EVENT_LOW_BATTERY:
        case EVENT_BAD_BATTERY:
        case EVENT_SOLAR_DISTURB:
        case EVENT_DISCONNECT:
        case EVENT_LOST:
        case EVENT_SUB_MACHINE_SENSOR_EXCEPTION:
        case EVENT_SUB_MACHINE_POWER_EXCEPTION:
        case EVENT_POWER_EXCEPTION:
        case EVENT_OTHER_EXCEPTION:
            return EVENT_LEVEL_EXCEPTION;

        case EVENT_SERIAL_485_DIS:
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

#if ADEMCO_ENABLE_TO_STRING
const char* ademco_event_to_string(ademco_event_t ademco_event) {
    switch (ademco_event) {
#define XX(code, name, zh) \
    case EVENT_##name:     \
        return #name;

        // ----标准安定宝协议事件----
        ADEMCO_STATUS_EVENTS_MAP(XX)
        ADEMCO_ALARM_EVENTS_MAP(XX)
        ADEMCO_ALARM_RECOVER_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_RECOVER_EVENTS_MAP(XX)

        // ---私有事件---------
        ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
        default:
            return "Undefined ademco_event_t";
    }
}
#endif

#if ADEMCO_ENABLE_CHINESE
const char* ademco_event_to_string_chinese(ademco_event_t ademco_event) {
    switch (ademco_event) {
#define XX(code, name, zh) \
    case EVENT_##name:     \
        return zh;

        // ----标准安定宝协议事件----
        ADEMCO_STATUS_EVENTS_MAP(XX)
        ADEMCO_ALARM_EVENTS_MAP(XX)
        ADEMCO_ALARM_RECOVER_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_EVENTS_MAP(XX)
        ADEMCO_EXEPTION_RECOVER_EVENTS_MAP(XX)

        // ---私有事件---------
        ADEMCO_HB_EVENTS_MAP(XX)
#undef XX
        default:
            return "未定义的安定宝事件";
    }
}
#endif

#ifndef ADEMCO_USE_EXTERNAL_CRC16
static const uint16_t ADEMCO_CONST_MODIFIER crc16Table[256] = {
    /* DEFINE THE FIRST ORDER POLYINOMIAL TABLE */
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,  // 0x00
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,  // 0x08
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,  // 0x10
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,  // 0x18
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,  // 0x20
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,  // 0x28
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,  // 0x30
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,  // 0x38
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,  // 0x40
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,  // 0x48
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,  // 0x50
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,  // 0x58
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,  // 0x60
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,  // 0x68
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,  // 0x70
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,  // 0x78
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,  // 0x80
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,  // 0x88
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,  // 0x90
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,  // 0x98
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,  // 0xA0
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,  // 0xA8
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,  // 0xB0
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,  // 0xB8
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,  // 0xC0
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,  // 0xC8
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,  // 0xD0
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,  // 0xD8
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,  // 0xE0
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,  // 0xE8
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,  // 0xF0
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,  // 0xF8
};

uint16_t ademco_crc16(const ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* buf, size_t len) {
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc = 0;
    while (len--)
        crc = (crc >> 8) ^ crc16Table[(crc ^ *buf++) & 0xFF];
    return crc;
}
#endif  // ADEMCO_USE_EXTERNAL_CRC16

size_t ademco_append_data(ademco_char_t* buf, const char* acct, ademco_id_t ademco_id,
                          ademco_event_t ademco_event, ademco_gg_t gg, ademco_zone_t zone) {
    char* p = (char*)buf;
    uint8_t i;
    if (ademco_event == EVENT_INVALID_EVENT) {
        *p++ = '[';
        *p++ = ']';
        return ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
    } else {
        *p++ = '[';

        *p++ = '#';
        ademco_id %= ADEMCO_ID_SENTINEL;
        if (acct && acct[0] && ademco_id == 0) {
            for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *acct; i++) {
                *p++ = *acct++;
            }
        } else {
            p += ademco_dec_to_hex_str(p, ademco_id, 6);
        }

        *p++ = '|';

        *p++ = (uint8_t)(ademco_event / 1000) + '0';
        *p++ = (uint8_t)((ademco_event % 1000) / 100) + '0';
        *p++ = (uint8_t)((ademco_event % 100) / 10) + '0';
        *p++ = (uint8_t)(ademco_event % 10) + '0';

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

        zone %= ADEMCO_ZONE_SENTINEL;
        *p++ = (uint8_t)(zone / 100) + '0';
        *p++ = (uint8_t)((zone % 100) / 10) + '0';
        *p++ = (uint8_t)(zone % 10) + '0';

        *p++ = ']';

        // assert(p - (char*)buf == ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN);

        *p = '\0';  // for debug convenience

        return p - (char*)buf;
    }
}

size_t ademco_append_data2(ademco_data_t* ademco_data, const char* acct,
                           ademco_id_t ademco_id, ademco_event_t ademco_event,
                           ademco_gg_t gg, ademco_zone_t zone) {
    ademco_data->ademco_id = ademco_id;
    ademco_data->ademco_event = ademco_event;
    ademco_data->gg = gg;
    ademco_data->zone = zone;
    return ademco_data->raw_len = ademco_append_data(ademco_data->raw,
                                                     acct, ademco_id,
                                                     ademco_event, gg, zone);
}

size_t ademco_append_data3(ademco_data_t* ademco_data, const char* acct) {
    char* p = (char*)ademco_data->raw;
    uint8_t i;
    if (ademco_data->ademco_event == EVENT_INVALID_EVENT) {
        *p++ = '[';
        *p++ = ']';
        return ademco_data->raw_len = ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
    } else {
        *p++ = '[';

        *p++ = '#';
        ademco_data->ademco_id %= ADEMCO_ID_SENTINEL;
        if (acct && acct[0] && ademco_data->ademco_id == 0) {
            for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *acct; i++) {
                *p++ = *acct++;
            }
        } else {
            p += ademco_dec_to_hex_str(p, ademco_data->ademco_id, 6);
        }

        *p++ = '|';

        *p++ = (uint8_t)(ademco_data->ademco_event / 1000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_event % 1000) / 100) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_event % 100) / 10) + '0';
        *p++ = (uint8_t)(ademco_data->ademco_event % 10) + '0';

        *p++ = ' ';

        if (ademco_data->gg == 0xEE) {
            *p++ = 'E';
            *p++ = 'E';
        } else if (ademco_data->gg == 0xCC) {
            *p++ = 'C';
            *p++ = 'C';
        } else {
            *p++ = ((ademco_data->gg / 10) & 0x0F) + '0';
            *p++ = ((ademco_data->gg % 10) & 0x0F) + '0';
        }

        *p++ = ' ';

        ademco_data->zone %= ADEMCO_ZONE_SENTINEL;
        *p++ = (uint8_t)(ademco_data->zone / 100) + '0';
        *p++ = (uint8_t)((ademco_data->zone % 100) / 10) + '0';
        *p++ = (uint8_t)(ademco_data->zone % 10) + '0';

        *p++ = ']';

        // assert(p - (char*)buf == ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN);

        *p = '\0';  // for debug convenience

        return ademco_data->raw_len = p - (char*)ademco_data->raw;
    }
}

ademco_parse_result_t ademco_parse_data(const ademco_char_t* packet, size_t packet_len,
                                        ademco_data_t* ademco_data,
                                        ademco_parse_error_t* err) {
    if (packet_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN &&
        packet[0] == '[' && packet[1] == ']') {
        memset(ademco_data, 0, sizeof(*ademco_data));
        strcpy((char*)ademco_data->raw, "[]");
        ademco_data->ademco_event = EVENT_INVALID_EVENT;
        ademco_data->ademco_id = 0;
        ademco_data->gg = 0;
        ademco_data->zone = 0;
        ademco_data->raw_len = 2;
        return ADEMCO_PARSE_RESULT_OK;
    } else if (packet_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN &&
               packet[0] == '[' &&
               packet[packet_len - 1] == ']') {  // [#000000|1400 00 000]
        const char* p = packet + 2;
        size_t ADEMCO_BUF_MEMORY_MODIFIER acct_len = packet_len - 15;
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
        static
#endif
            char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX] = {0};
#endif

        if (acct_len < 6) {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "acct_len < 6");
            return ADEMCO_PARSE_RESULT_ERROR;
        } else if (acct_len > ADEMCO_PACKET_ACCT_MAX_LEN) {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "acct_len > ADEMCO_PACKET_ACCT_MAX_LEN");
            return ADEMCO_PARSE_RESULT_ERROR;
        }

        strncpy(buf, (const char*)p, acct_len);
        buf[acct_len] = '\0';
        ademco_data->ademco_id = (ademco_id_t)strtoul(buf, NULL, 16);
        if (ademco_data->ademco_id > ADEMCO_ID_SENTINEL) {
            // ademco_id maybe acct like "861234567890"
            ademco_data->ademco_id = ADEMCO_ID_INVALID;
		}
        p += acct_len;

        if (*p++ != '|') {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "*p++ != '|'");
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        strncpy(buf, (const char*)p, 4);
        buf[4] = '\0';
        ademco_data->ademco_event = (ademco_event_t)(atoi(buf));
        p += 4;

        if (*p++ != ' ') {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "*p++ != ' '");
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        if (*p == 'E' && *(p + 1) == 'E') {
            ademco_data->gg = 0xEE;
        } else if (*p == 'C' && *(p + 1) == 'C') {
            ademco_data->gg = 0xCC;
        } else {
            ademco_data->gg = (ademco_gg_t)((*p - '0') * 10 + (*(p + 1) - '0'));
        }
        p += 2;

        if (*p++ != ' ') {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "*p++ != ' '");
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        strncpy(buf, (const char*)p, 3);
        buf[3] = '\0';
        ademco_data->zone = (ademco_zone_t)atoi(buf);

        if (ademco_data->raw != packet) {
            memcpy(ademco_data->raw, packet, packet_len);
        }
        ademco_data->raw_len = packet_len;
        return ADEMCO_PARSE_RESULT_OK;
    }

    ADEMCO_FILL_PARSE_ERROR(err, 0, "dig in source code for more info");
    return ADEMCO_PARSE_RESULT_ERROR;
}

size_t ademco_data_to_congwin_fe100(ademco_char_t* fe100, size_t fe100_len,
                                    const ademco_data_t* ademco_data) {
    if (ademco_data->raw_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN) {
        if (fe100_len < 2) return 0;
        fe100[0] = '\n';
        fe100[1] = '\r';
        return 2;
    } else if (ademco_data->raw_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN) {
        char* p;
        if (fe100_len < CONGWIN_FE100_PACKET_LEN) return 0;

        p = fe100;
        *p++ = '\n';
        *p++ = ' ';

        *p++ = '0';  // RG
        *p++ = '0';
        *p++ = ' ';

        *p++ = (uint8_t)(ademco_data->ademco_id / 10000000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 10000000) / 1000000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 1000000) / 100000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 100000) / 10000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 10000) / 1000) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 1000) / 100) + '0';
        *p++ = (uint8_t)((ademco_data->ademco_id % 100) / 10) + '0';
        *p++ = (uint8_t)(ademco_data->ademco_id % 10) + '0';

        *p++ = ' ';  // 18
        *p++ = '1';
        *p++ = '8';
        *p++ = ' ';

        // E for open, R for close
        if (ademco_data->ademco_event / 1000 == 1 ||
            ademco_data->ademco_event == EVENT_ARM_STAY)
            *p++ = 'E';
        else
            *p++ = 'R';

        if (ademco_data->ademco_event == EVENT_OFFLINE) {
            *p++ = '3';
            *p++ = 'B';
            *p++ = '2';
        } else if (ademco_data->ademco_event == EVENT_ONLINE) {
            *p++ = '3';
            *p++ = 'B';
            *p++ = '0';
        } else {
            *p++ = '0' + (uint8_t)((ademco_data->ademco_event % 1000) / 100);
            *p++ = '0' + ((ademco_data->ademco_event % 100) / 10);
            *p++ = '0' + (ademco_data->ademco_event % 10);
        }

        *p++ = ' ';  // FE100, gg === 0
        *p++ = '0';
        *p++ = '0';
        *p++ = ' ';

        if (ademco_is_machine_status_event(ademco_data->ademco_event))
            *p++ = 'U';  // user event
        else
            *p++ = 'C';  // zone event

        *p++ = '0' + (uint8_t)((ademco_data->zone % 1000) / 100);
        *p++ = '0' + ((ademco_data->zone % 100) / 10);
        *p++ = '0' + (ademco_data->zone % 10);

        *p++ = ' ';
        *p++ = '\r';
        return p - fe100;
    }
    return 0;
}

void ademco_xdata_init(ademco_xdata_t* xdat) {
    memset(xdat, 0, sizeof *xdat);
}

bool ademco_xdata_convert(ademco_xdata_t* xdat,
                          ademco_xdata_length_format_t xlf) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_XDATA_MAX_LEN];
#endif
    size_t ADEMCO_BUF_MEMORY_MODIFIER len;

    if (!xdat) {
        return false;
    }
    if (xdat->lenghFormat == xlf) {
        return false;
    }
    len = ademco_xdata_get_valid_content_len(xdat);

    if (xlf == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX) {
        if (len + 6 > ADEMCO_PACKET_XDATA_MAX_LEN)
            return false;
        xdat->raw[5 + ademco_xdata_get_valid_content_len(xdat)] = '\0';
        len = ademco_hex_str_to_array((uint8_t*)buf,
                                      ademco_xdata_get_valid_content_addr(xdat),
                                      0x0F);
        ademco_make_xdata(xdat,
                          ADEMCO_XDATA_LENGTH_FMT_TWO_HEX,
                          ADEMCO_XDATA_TRANSFORM_AS_IS,
                          buf, len);
    } else {
        memcpy(buf, ademco_xdata_get_valid_content_addr(xdat), len);
        ademco_make_xdata(xdat,
                          ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL,
                          ADEMCO_XDATA_TRANSFORM_TO_ASCII,
                          buf, len);
    }
    return true;
}

#ifdef SWIG
static
#endif
const char* ademco_xdata_get_valid_content_addr(const ademco_xdata_t* xdat) {
    if (xdat->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
        xdat->raw_len > 4)
        return xdat->raw + 3;
    else if (xdat->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
             xdat->raw_len > 6)
        return xdat->raw + 5;
    else
        return NULL;
}

bool ademco_xdata_memeq(const ademco_xdata_t* xdat,
                        const void* buf, size_t buf_len) {
    const void* p;
    if (ademco_xdata_get_valid_content_len(xdat) != buf_len) return false;
    p = ademco_xdata_get_valid_content_addr(xdat);
    if (!p) return 0;
    return memcmp(p, buf, buf_len) == 0;
}

size_t ademco_xdata_get_valid_content_len(const ademco_xdata_t* xdat) {
    if (!xdat) return 0;
    if (xdat->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
        xdat->raw_len > 4)
        return xdat->raw_len - 4;
    else if (xdat->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
             xdat->raw_len > 6)
        return xdat->raw_len - 6;
    else
        return 0;
}

size_t ademco_xdata_copy(ademco_xdata_t* dst, const ademco_xdata_t* src) {
    if (dst && src && src->raw_len > 0) {
        memcpy(dst, src, sizeof(ademco_xdata_t));
        return src->raw_len;
    }
    return 0;
}

bool ademco_make_xdata(ademco_xdata_t* xdat,
                       ademco_xdata_length_format_t xlf,
                       ademco_xdata_transform_t xtr,
                       const ademco_char_t* content, size_t len) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_XDATA_MAX_LEN];
#endif
    uint32_t ADEMCO_BUF_MEMORY_MODIFIER translen;
    len &= 0xFFFF;
    translen = len & 0xFFFF;

    if (xtr == ADEMCO_XDATA_TRANSFORM_AS_IS)
        memcpy(buf, content, len);
    else if (len * 2 < ADEMCO_PACKET_XDATA_MAX_LEN)
        translen = ademco_hex_array_to_str(buf,
                                           (const uint8_t*)content, len) &
                   0xFFFF;
    else
        return false;

    xdat->lenghFormat = xlf;
    if (xlf == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
        translen + 4 < ADEMCO_PACKET_XDATA_MAX_LEN) {
        ademco_char_t* p = xdat->raw;
        *p++ = '[';
        *p++ = (translen & 0xFFFF) >> 8;
        *p++ = (translen & 0xFFFF) & 0xFF;
        memcpy(p, buf, translen);
        p += translen;
        *p++ = ']';
        xdat->raw_len = p - xdat->raw;
        return true;
    } else if (xlf == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
               translen + 6 < ADEMCO_PACKET_XDATA_MAX_LEN) {
        ademco_char_t* p = xdat->raw;
        *p++ = '[';
        p += ademco_dec_to_hex_str(p, translen, 4);
        memcpy(p, buf, translen);
        p += translen;
        *p++ = ']';
        xdat->raw_len = p - xdat->raw;
        return true;
    } else
        return false;
}

bool ademco_is_valid_packet_id(const char* standard, const char* id, size_t len) {
    while (*standard && len--) {
        if (*standard++ != *id++)
            return false;
    }
    return true;
}

ademco_packet_id_t ademco_packet_id_from_string(const char* id, size_t len) {
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_NULL, id, len))
        return AID_NULL;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_ACK, id, len))
        return AID_ACK;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_HB, id, len))
        return AID_HB;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_ADM_CID, id, len))
        return AID_ADM_CID;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_NAK, id, len))
        return AID_NAK;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_DUH, id, len))
        return AID_DUH;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_MOD_REG, id, len))
        return AID_MOD_REG;
    if (ademco_is_valid_packet_id(ADEMCO_PACKET_ID_REG_RSP, id, len))
        return AID_REG_RSP;

    return AID_INVALID;
}

const char* ademco_packet_id_to_string(ademco_packet_id_t id) {
    switch (id) {
        case AID_NULL:
            return ADEMCO_PACKET_ID_NULL;
        case AID_ACK:
            return ADEMCO_PACKET_ID_ACK;
        case AID_NAK:
            return ADEMCO_PACKET_ID_NAK;
        case AID_DUH:
            return ADEMCO_PACKET_ID_DUH;
        case AID_HB:
            return ADEMCO_PACKET_ID_HB;
        case AID_ADM_CID:
            return ADEMCO_PACKET_ID_ADM_CID;
        case AID_MOD_REG:
            return ADEMCO_PACKET_ID_MOD_REG;
        case AID_REG_RSP:
            return ADEMCO_PACKET_ID_REG_RSP;
        default:
            return NULL;
    }
}

static void getNowTimestamp(char* buf) {
    time_t ADEMCO_BUF_MEMORY_MODIFIER now = time(NULL);

#if ADEMCO_DISABLE_GMTIME
    struct tm ADEMCO_BUF_MEMORY_MODIFIER tm = {0};
    _localtime_r(&now, &tm);
#else
    struct tm ADEMCO_BUF_MEMORY_MODIFIER tm = *localtime(&now);
#endif  // ADEMCO_DISABLE_GMTIME
    tm.tm_mon += 1;
    tm.tm_year += 1900;
    *buf++ = '_';
    *buf++ = (uint8_t)(tm.tm_hour / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_hour % 10) + '0';
    *buf++ = ':';
    *buf++ = (uint8_t)(tm.tm_min / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_min % 10) + '0';
    *buf++ = ':';
    *buf++ = (uint8_t)(tm.tm_sec / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_sec % 10) + '0';
    *buf++ = ',';
    *buf++ = (uint8_t)(tm.tm_mon / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_mon % 10) + '0';
    *buf++ = '-';
    *buf++ = (uint8_t)(tm.tm_mday / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_mday % 10) + '0';
    *buf++ = '-';
    *buf++ = (uint8_t)(tm.tm_year / 1000) + '0';
    *buf++ = (uint8_t)((tm.tm_year % 1000) / 100) + '0';
    *buf++ = (uint8_t)((tm.tm_year % 100) / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_year % 10) + '0';
    *buf = '\0';
}

static void getGmtTimestamp(char* buf) {
#ifdef ADEMCO_DISABLE_GMTIME
    getNowTimestamp(buf);
#else
    time_t ADEMCO_BUF_MEMORY_MODIFIER now = time(NULL);
    struct tm ADEMCO_BUF_MEMORY_MODIFIER tm = *gmtime(&now);
    tm.tm_mon += 1;
    tm.tm_year += 1900;
    *buf++ = '_';
    *buf++ = (uint8_t)(tm.tm_hour / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_hour % 10) + '0';
    *buf++ = ':';
    *buf++ = (uint8_t)(tm.tm_min / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_min % 10) + '0';
    *buf++ = ':';
    *buf++ = (uint8_t)(tm.tm_sec / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_sec % 10) + '0';
    *buf++ = ',';
    *buf++ = (uint8_t)(tm.tm_mon / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_mon % 10) + '0';
    *buf++ = '-';
    *buf++ = (uint8_t)(tm.tm_mday / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_mday % 10) + '0';
    *buf++ = '-';
    *buf++ = (uint8_t)(tm.tm_year / 1000) + '0';
    *buf++ = (uint8_t)((tm.tm_year % 1000) / 100) + '0';
    *buf++ = (uint8_t)((tm.tm_year % 100) / 10) + '0';
    *buf++ = (uint8_t)(tm.tm_year % 10) + '0';
    *buf = '\0';
#endif
}

size_t ademco_make_empty_data_packet(ademco_char_t* dst_buff, size_t len,
                                     const char* id, uint16_t seq,
                                     const char* acct, ademco_id_t ademco_id) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif
    char ADEMCO_BUF_MEMORY_MODIFIER* p = buf;
    char ADEMCO_BUF_MEMORY_MODIFIER* pcrc = buf + 1;
    char ADEMCO_BUF_MEMORY_MODIFIER* plen = buf + 5;
    char ADEMCO_BUF_MEMORY_MODIFIER* pid = buf + 9;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER packet_len, ademco_len;
    uint8_t i;

    buf[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, id, strlen(id));
    p = pid + strlen(id);
    seq %= 10000;
    *p++ = (uint8_t)(seq / 1000) + '0';
    *p++ = (uint8_t)((seq % 1000) / 100) + '0';
    *p++ = (uint8_t)((seq % 100) / 10) + '0';
    *p++ = (uint8_t)(seq % 10) + '0';
    memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
    p += strlen(ADEMCO_RRCVR_DEFAULT);
    memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
    p += strlen(ADEMCO_LPREF_DEFAULT);
    *p++ = '#';
    if (acct && acct[0]) {
        for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *acct; i++) {
            *p++ = *acct++;
        }
    } else {
        p += ademco_dec_to_hex_str(p, ademco_id, 6);
    }
    *p++ = '[';
    *p++ = ']';
    getNowTimestamp((char*)p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    packet_len = (p - buf) & 0xFFFF;
    //                       \n  crc len  \r
    ademco_len = packet_len - 1 - 4 - 4 - 1;
    ademco_dec_to_hex_str(plen, ademco_len, 4);
    crc = ademco_crc16(pid, ademco_len);
    ademco_dec_to_hex_str(pcrc, crc, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buf, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademco_make_adm_empty_data_packet(ademco_char_t* dst_buff, size_t len,
                                         const char* id, uint16_t seq,
                                         const char* acct, ademco_id_t ademco_id) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif
    char ADEMCO_BUF_MEMORY_MODIFIER* p = buf;
    char ADEMCO_BUF_MEMORY_MODIFIER* pcrc = buf + 1;
    char ADEMCO_BUF_MEMORY_MODIFIER* plen = buf + 5;
    char ADEMCO_BUF_MEMORY_MODIFIER* pid = buf + 9;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER packet_len, ademco_len;
    uint8_t i;

    buf[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, id, strlen(id));
    p = pid + strlen(id);
    seq %= 10000;
    *p++ = (uint8_t)(seq / 1000) + '0';
    *p++ = (uint8_t)((seq % 1000) / 100) + '0';
    *p++ = (uint8_t)((seq % 100) / 10) + '0';
    *p++ = (uint8_t)(seq % 10) + '0';
    p += 4;
    memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
    p += strlen(ADEMCO_RRCVR_DEFAULT);
    memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
    p += strlen(ADEMCO_LPREF_DEFAULT);
    *p++ = '#';
    if (acct && acct[0]) {
        for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *acct; i++) {
            *p++ = *acct++;
        }
    } else {
        p += ademco_dec_to_hex_str(p, ademco_id, 6);
    }
    *p++ = '[';
    *p++ = ']';
    getGmtTimestamp((char*)p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    packet_len = (p - buf) & 0xFFFF;
    //                       \n  crc len  \r
    ademco_len = packet_len - 1 - 4 - 4 - 1;
    ademco_dec_to_hex_str(plen, ademco_len, 4);
    crc = ademco_crc16(pid, ademco_len);
    ademco_dec_to_hex_str(pcrc, crc, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buf, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademco_make_null_packet(ademco_char_t* buf, size_t len,
                               uint16_t seq, const char* acct,
                               ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buf, len,
                                         ADEMCO_PACKET_ID_NULL,
                                         seq, acct, ademco_id);
}

size_t ademco_make_ack_packet(ademco_char_t* buf, size_t len,
                              uint16_t seq, const char* acct,
                              ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buf, len,
                                         ADEMCO_PACKET_ID_ACK,
                                         seq, acct, ademco_id);
}

size_t ademco_make_nak_packet(ademco_char_t* buf, size_t len,
                              uint16_t seq, const char* acct,
                              ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buf, len,
                                         ADEMCO_PACKET_ID_NAK,
                                         seq, acct, ademco_id);
}

size_t ademco_make_hb_packet(ademco_char_t* dst_buff, size_t len,
                             uint16_t seq, const char* acct,
                             ademco_id_t ademco_id, ademco_event_t ademco_event,
                             ademco_gg_t gg, ademco_zone_t zone,
                             const ademco_xdata_t* xdat) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif
    char ADEMCO_BUF_MEMORY_MODIFIER* p = buf;
    char ADEMCO_BUF_MEMORY_MODIFIER* pcrc = buf + 1;
    char ADEMCO_BUF_MEMORY_MODIFIER* plen = buf + 5;
    char ADEMCO_BUF_MEMORY_MODIFIER* pid = buf + 9;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER packet_len, ademco_len;
    uint8_t i;

    buf[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, ADEMCO_PACKET_ID_HB, strlen(ADEMCO_PACKET_ID_HB));
    p = pid + strlen(ADEMCO_PACKET_ID_HB);
    seq %= 10000;
    *p++ = (uint8_t)(seq / 1000) + '0';
    *p++ = (uint8_t)((seq % 1000) / 100) + '0';
    *p++ = (uint8_t)((seq % 100) / 10) + '0';
    *p++ = (uint8_t)(seq % 10) + '0';
    memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
    p += strlen(ADEMCO_RRCVR_DEFAULT);
    memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
    p += strlen(ADEMCO_LPREF_DEFAULT);
    *p++ = '#';
    if (acct && acct[0]) {
        char ADEMCO_BUF_MEMORY_MODIFIER* src = (char*)acct;
        for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *src; i++) {
            *p++ = *src++;
        }
    } else {
        p += ademco_dec_to_hex_str(p, ademco_id, 6);
    }
    p += ademco_append_data(p, acct, ademco_id, ademco_event, gg, zone);
    if (xdat && xdat->raw_len > 0) {
        memcpy(p, xdat->raw, xdat->raw_len);
        p += xdat->raw_len;
    }
    getNowTimestamp(p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    packet_len = (p - buf) & 0xFFFF;
    //                       \n  crc len  \r
    ademco_len = packet_len - 1 - 4 - 4 - 1;
    ademco_dec_to_hex_str(plen, ademco_len, 4);
    crc = ademco_crc16(pid, ademco_len);
    ademco_dec_to_hex_str(pcrc, crc, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buf, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    }
    return 0;
}

size_t ademco_make_adm_packet(ademco_char_t* dst_buff, size_t len,
                              uint16_t seq, const char* acct,
                              ademco_id_t ademco_id,
                              ademco_event_t ademco_event,
                              ademco_gg_t gg, ademco_zone_t zone,
                              const ademco_xdata_t* xdat) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif
    char ADEMCO_BUF_MEMORY_MODIFIER* p = buf;
    char ADEMCO_BUF_MEMORY_MODIFIER* pcrc = buf + 1;
    char ADEMCO_BUF_MEMORY_MODIFIER* plen = buf + 5;
    char ADEMCO_BUF_MEMORY_MODIFIER* pid = buf + 9;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER packet_len, ademco_len;
    uint8_t i;

    buf[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, ADEMCO_PACKET_ID_ADM_CID, strlen(ADEMCO_PACKET_ID_ADM_CID));
    p = pid + strlen(ADEMCO_PACKET_ID_ADM_CID);
    seq %= 10000;
    *p++ = (uint8_t)(seq / 1000) + '0';
    *p++ = (uint8_t)((seq % 1000) / 100) + '0';
    *p++ = (uint8_t)((seq % 100) / 10) + '0';
    *p++ = (uint8_t)(seq % 10) + '0';
    memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
    p += strlen(ADEMCO_RRCVR_DEFAULT);
    memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
    p += strlen(ADEMCO_LPREF_DEFAULT);
    *p++ = '#';
    if (acct && acct[0]) {
        char ADEMCO_BUF_MEMORY_MODIFIER* src = (char*)acct;
        for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && *src; i++) {
            *p++ = *src++;
        }
    } else {
        p += ademco_dec_to_hex_str(p, ademco_id, 6);
    }
    p += ademco_append_data(p, acct, ademco_id, ademco_event, gg, zone);
    if (xdat && xdat->raw_len > 0) {
        memcpy(p, xdat->raw, xdat->raw_len);
        p += xdat->raw_len;
    }
    getGmtTimestamp(p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    packet_len = (p - buf) & 0xFFFF;
    //                       \n  crc len  \r
    ademco_len = packet_len - 1 - 4 - 4 - 1;
    ademco_dec_to_hex_str(plen, ademco_len, 4);
    crc = ademco_crc16(pid, ademco_len);
    ademco_dec_to_hex_str(pcrc, crc, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buf, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    }
    return 0;
}

static void copyAcct2AdemcoPacket(ademco_packet_t* pkt, const char* acct) {
    if (acct) {
        size_t ADEMCO_BUF_MEMORY_MODIFIER len = strlen(acct);
        if (len > ADEMCO_PACKET_ACCT_MAX_LEN)
            len = ADEMCO_PACKET_ACCT_MAX_LEN;
        memcpy(pkt->acct, acct, len);
        pkt->acct[len] = '\0';
    } else
        pkt->acct[0] = '\0';
}

size_t ademco_make_null_packet2(ademco_packet_t* pkt, uint16_t seq,
                                const char* acct, ademco_id_t ademco_id) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->dat.ademco_id = ademco_id;
    pkt->id = AID_NULL;
    return pkt->raw_len = ademco_make_null_packet(pkt->raw, sizeof(pkt->raw),
                                                  seq, acct, ademco_id);
}

size_t ademco_make_ack_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->dat.ademco_id = ademco_id;
    pkt->id = AID_ACK;
    return pkt->raw_len = ademco_make_ack_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id);
}

size_t ademco_make_nak_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->dat.ademco_id = ademco_id;
    pkt->id = AID_NAK;
    return pkt->raw_len = ademco_make_nak_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id);
}

size_t ademco_make_hb_packet2(ademco_packet_t* pkt, uint16_t seq,
                              const char* acct, ademco_id_t ademco_id,
                              ademco_event_t ademco_event, ademco_gg_t gg,
                              ademco_zone_t zone,
                              const ademco_xdata_t* xdat) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademco_append_data2(&pkt->dat, acct, ademco_id, ademco_event, gg, zone);
    if (xdat && xdat->raw_len > 0) {
        pkt->xdat.lenghFormat = xdat->lenghFormat;
        memcpy(pkt->xdat.raw, xdat->raw, xdat->raw_len);
        pkt->xdat.raw_len = xdat->raw_len;
    } else
        memset(&pkt->xdat, 0, sizeof(*xdat));
    pkt->id = AID_HB;
    return pkt->raw_len = ademco_make_hb_packet(pkt->raw, sizeof(pkt->raw),
                                                seq, acct, ademco_id,
                                                ademco_event,
                                                gg, zone, xdat);
}

size_t ademco_make_hb_packet3(ademco_packet_t* pkt) {
#if !ADEMCO_USE_UNIQUE_BUF
#if ADEMCO_USE_STATIC_BUF
    static
#endif
        char ADEMCO_BUF_MEMORY_MODIFIER buf[ADEMCO_PACKET_MAX_LEN];
#endif
    char ADEMCO_BUF_MEMORY_MODIFIER* p = buf;
    char ADEMCO_BUF_MEMORY_MODIFIER* pcrc = buf + 1;
    char ADEMCO_BUF_MEMORY_MODIFIER* plen = buf + 5;
    char ADEMCO_BUF_MEMORY_MODIFIER* pid = buf + 9;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER packet_len, ademco_len;
    uint8_t i;

    buf[0] = ADEMCO_PACKET_PREFIX;
    memcpy(pid, ADEMCO_PACKET_ID_HB, strlen(ADEMCO_PACKET_ID_HB));
    p = pid + strlen(ADEMCO_PACKET_ID_HB);
    *p++ = (uint8_t)(pkt->seq / 1000) + '0';
    *p++ = (uint8_t)((pkt->seq % 1000) / 100) + '0';
    *p++ = (uint8_t)((pkt->seq % 100) / 10) + '0';
    *p++ = (uint8_t)(pkt->seq % 10) + '0';
    memcpy(p, ADEMCO_RRCVR_DEFAULT, strlen(ADEMCO_RRCVR_DEFAULT));
    p += strlen(ADEMCO_RRCVR_DEFAULT);
    memcpy(p, ADEMCO_LPREF_DEFAULT, strlen(ADEMCO_LPREF_DEFAULT));
    p += strlen(ADEMCO_LPREF_DEFAULT);
    *p++ = '#';
    if (pkt->acct[0] != '\0') {
        for (i = 0; i < ADEMCO_PACKET_ACCT_MAX_LEN && pkt->acct[i]; i++) {
            *p++ = pkt->acct[i];
        }
    } else {
        p += ademco_dec_to_hex_str(p, pkt->dat.ademco_id, 6);
    }
    ademco_append_data3(&pkt->dat, pkt->acct);
    memcpy(p, pkt->dat.raw, pkt->dat.raw_len);
    p += pkt->dat.raw_len;
    if (pkt->xdat.raw_len > 0) {
        memcpy(p, pkt->xdat.raw, pkt->xdat.raw_len);
        p += pkt->xdat.raw_len;
    }
    getNowTimestamp(p);
    p += ADEMCO_PACKET_TIMESTAMP_LEN;
    *p++ = ADEMCO_PACKET_SUFIX;

    packet_len = (p - buf) & 0xFFFF;
    //                       \n  crc len  \r
    ademco_len = packet_len - 1 - 4 - 4 - 1;
    ademco_dec_to_hex_str(plen, ademco_len, 4);
    crc = ademco_crc16(pid, ademco_len);
    ademco_dec_to_hex_str(pcrc, crc, 4);

    memcpy(pkt->raw, buf, packet_len);
    pkt->raw[packet_len] = '\0';  // for debug convenience
    return pkt->raw_len = packet_len;
}

size_t ademco_make_adm_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id,
                               ademco_event_t ademco_event, ademco_gg_t gg,
                               ademco_zone_t zone,
                               const ademco_xdata_t* xdat) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademco_append_data2(&pkt->dat, acct, ademco_id, ademco_event, gg, zone);
    if (xdat && xdat->raw_len > 0) {
        pkt->xdat.lenghFormat = xdat->lenghFormat;
        memcpy(pkt->xdat.raw, xdat->raw, xdat->raw_len);
        pkt->xdat.raw_len = xdat->raw_len;
    } else
        memset(&pkt->xdat, 0, sizeof(*xdat));
    pkt->id = AID_ADM_CID;
    return pkt->raw_len = ademco_make_adm_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id,
                                                 ademco_event,
                                                 gg, zone, xdat);
}

ademco_parse_result_t ademco_parse_packet(const ademco_char_t* buf,
                                          size_t len,
                                          ademco_packet_t* pkt,
                                          size_t* cb_commited,
                                          ademco_parse_error_t* err) {
    const char *p, *q, *pid, *pend, *pacct, *pdat;
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER h;
    uint16_t ADEMCO_BUF_MEMORY_MODIFIER crc;
    size_t ADEMCO_BUF_MEMORY_MODIFIER len_needed;

    if (len < 9) {
        ADEMCO_FILL_PARSE_ERROR(err, 0, "ADEMCO_PARSE_RESULT_NOT_ENOUGH");
        return ADEMCO_PARSE_RESULT_NOT_ENOUGH;
    }

    *cb_commited = 0;
    while (*buf != ADEMCO_PACKET_PREFIX && len > 0) {
        buf++;
        len--;
        (*cb_commited)++;
    }

    if (*cb_commited > 0) {
        ADEMCO_FILL_PARSE_ERROR(err, 0, "fixed prefix not found");
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    p = buf;
    if (*p++ != ADEMCO_PACKET_PREFIX) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "fixed prefix not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // crc
    crc = 0;
    for (q = p; p - q < 4; p++) {
        h = ademco_char2hex(*p);
        if (h == 0xFF) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "crc contains non-hex characters");
            *cb_commited += p - buf;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        crc = (crc << 4) | h;
    }

    // len
    pkt->len = 0;
    for (q = p; p - q < 4; p++) {
        h = ademco_char2hex(*p);
        if (h == 0xFF) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "len contains non-hex characters");
            *cb_commited += p - buf;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        pkt->len = (pkt->len << 4) | h;
    }
    len_needed = 9 + pkt->len + 1;
    if (len < len_needed) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "ADEMCO_PARSE_RESULT_NOT_ENOUGH");
        return ADEMCO_PARSE_RESULT_NOT_ENOUGH;
    } else if (len_needed >= ADEMCO_PACKET_MAX_LEN) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "len_needed >= ADEMCO_PACKET_MAX_LEN");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    pid = p;
    pend = pid + pkt->len;

    if (pend > buf + len) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "suffix out of range");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    if (*pend != ADEMCO_PACKET_SUFIX) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "suffix not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    pkt->crc = ademco_crc16(pid, pkt->len);
    if (pkt->crc != crc) {
        ADEMCO_FILL_PARSE_ERROR(err, 1, "crc error");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // id
    if (*pid != '\"') {
        ADEMCO_FILL_PARSE_ERROR(err, pid - buf, "left \" of id not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    p = pid + 1;
    while (p < pend && *p != '\"') {
        p++;
    }
    if (p >= pend || *p != '\"') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "right \" of id not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    pkt->id = ademco_packet_id_from_string((const char*)pid, ++p - pid);
    if (pkt->id == AID_INVALID) {
        ADEMCO_FILL_PARSE_ERROR(err, pid - buf, "unknown ademco id");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // seq
    pkt->seq = 0;
    for (q = p; p - q < 4; p++) {
        if ('0' <= *p && *p <= '9') {
            pkt->seq = (pkt->seq * 10) + (*p - '0');
        } else {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "seq contains non-digit characters");
            // some apps use FFFF, wtf
            // return ADEMCO_PARSE_RESULT_ERROR;
        }
    }
    if (pkt->seq >= 10000) {
        pkt->seq = 0;
    }

    // *rrcvr
    if (*p == 'R') {  // rrcvr exists
        // const char* prcvr = p;
        while (p < pend && *p != 'L' && *p != '#') {
            p++;
        }
        if (p >= pend || (*p != 'L' && *p != '#')) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "p >= pend || (*p != 'L' && *p != '#')");
            *cb_commited += p - buf;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        // only check if format is correct, ignore it's content
    } else if (*p == 'L') {  // rrcvr not exits
                             // pass
    } else {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "both LPREF and RRCVR not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // lpref
    if (*p != 'L') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "LPREF not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    while (p < pend && *p != '#') {
        p++;
    }
    // only check if format is correct, ignore it's content
    if (p >= pend || *p != '#') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "# not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    // lpref passed

    // #acct
    pacct = ++p;
    while (p - pacct < ADEMCO_PACKET_ACCT_MAX_LEN && *p != '[') {
        if (!isxdigit(*p)) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "acct contains non-hex characters");
            *cb_commited += p - buf;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        p++;
    }
    if (*p != '[') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "acct too long");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    strncpy(pkt->acct, (const char*)pacct, p - pacct);
    pkt->acct[p - pacct] = '\0';

    // dat
    pdat = p;
    if (*p != '[') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "dat's [ not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    while (p < pend && *p != ']') {
        p++;
    }
    if (p >= pend || *p != ']') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "dat's ] not found");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    if (ademco_parse_data(pdat, ++p - pdat, &pkt->dat, err) != ADEMCO_PARSE_RESULT_OK) {
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // *xdat
    if (*p == '[') {  // xdat exists
        const char* pxdata = p++;
        ademco_xdata_length_format_t ADEMCO_BUF_MEMORY_MODIFIER xlf = ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL;
        size_t ADEMCO_BUF_MEMORY_MODIFIER valid_len = 0;
        uint8_t ADEMCO_BUF_MEMORY_MODIFIER i;
        for (i = 0; i < 4; i++)
            if (!isxdigit(*(uint8_t*)(p + i)))
                xlf = ADEMCO_XDATA_LENGTH_FMT_TWO_HEX;

        if (xlf == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL) {
            valid_len = (ademco_char2hex(*(pxdata + 1)) << 12) |
                        (ademco_char2hex(*(pxdata + 2)) << 8) |
                        (ademco_char2hex(*(pxdata + 3)) << 4) |
                        (ademco_char2hex(*(pxdata + 4)));
            p += 4 + valid_len;
        } else {
            valid_len = (pxdata[1] << 8) | pxdata[2];
            p += 2 + valid_len;
        }

        if (p >= pend || *p != ']' || *(p + 1) != '_') {
            ADEMCO_FILL_PARSE_ERROR(err, p - buf, "xdat's ] not found or next char is not _");
            *cb_commited += p - buf;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        pkt->xdat.lenghFormat = xlf;
        pkt->xdat.raw_len = ++p - pxdata;
        memcpy(pkt->xdat.raw, pxdata, pkt->xdat.raw_len);
    } else {
        pkt->xdat.raw_len = 0;
    }

    // timestamp, _%02d:%02d:%02d,%02d-%02d-%04d
    // only check lengh, if format is incorrect, use local time instead
    if (pend - p == ADEMCO_PACKET_TIMESTAMP_LEN) {
        struct tm ADEMCO_BUF_MEMORY_MODIFIER tm = {0};
        pkt->timestamp = 0;
        do {
            if (*p++ != '_') {
                break;
            }
            tm.tm_hour = ademco_char2hex(*p++);
            tm.tm_hour = tm.tm_hour * 10 + ademco_char2hex(*p++);
            if (*p++ != ':') {
                break;
            }

            tm.tm_min = ademco_char2hex(*p++);
            tm.tm_min = tm.tm_min * 10 + ademco_char2hex(*p++);
            if (*p++ != ':') {
                break;
            }

            tm.tm_sec = ademco_char2hex(*p++);
            tm.tm_sec = tm.tm_sec * 10 + ademco_char2hex(*p++);
            if (*p++ != ',') {
                break;
            }

            tm.tm_mon = ademco_char2hex(*p++);
            tm.tm_mon = tm.tm_mon * 10 + ademco_char2hex(*p++);
            if (*p++ != '-') {
                break;
            }

            tm.tm_mday = ademco_char2hex(*p++);
            tm.tm_mday = tm.tm_mday * 10 + ademco_char2hex(*p++);
            if (*p++ != '-') {
                break;
            }

            tm.tm_year = ademco_char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + ademco_char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + ademco_char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + ademco_char2hex(*p++);

            tm.tm_year -= 1900;
            tm.tm_mon--;
            tm.tm_isdst = -1;
            pkt->timestamp = mktime(&tm);
        } while (0);

        if (pkt->timestamp <= 0) {  // use local time instead
            pkt->timestamp = time(NULL);
            p = pend;
        }
    } else {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "timestamp length not correct");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    if (p++ != pend) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buf, "packet length not correct");
        *cb_commited += p - buf;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    pkt->raw_len = *cb_commited = p - buf;
    if (pkt->raw != buf)
        memcpy(pkt->raw, buf, pkt->raw_len);
    return ADEMCO_PARSE_RESULT_OK;
}

size_t ademco_dec_to_hex_str(char* str, uint32_t dec, uint8_t hex_len) {
    char* p = str;
    uint8_t i;
    for (i = 0; i < hex_len; i++) {
        *p++ = ademco_hex2char((dec >> (4 * (hex_len - i - 1))) & 0x0F);
    }
    return p - str;
}

size_t ademco_hilo_array_to_dec_str(ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* str,
                                    const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len) {
    char ADEMCO_BUF_MEMORY_MODIFIER* p = str;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    char ADEMCO_BUF_MEMORY_MODIFIER c;
    for (i = 0; i < len; i++) {
        c = (arr[i] >> 4) & 0x0F;
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

size_t ademco_hilo_array_to_hex_str(ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* str,
                                    const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len) {
    char ADEMCO_BUF_MEMORY_MODIFIER* p = str;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    for (i = 0; i < len; i++) {
        char ADEMCO_BUF_MEMORY_MODIFIER c = (arr[i] >> 4) & 0x0F;
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

size_t ademco_dec_str_to_hilo_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len,
                                    const char ADEMCO_BUF_MEMORY_MODIFIER* str) {
    char ADEMCO_BUF_MEMORY_MODIFIER* p = (char ADEMCO_BUF_MEMORY_MODIFIER*)arr;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    size_t ADEMCO_BUF_MEMORY_MODIFIER slen = str ? strlen(str) : 0;
    char ADEMCO_BUF_MEMORY_MODIFIER hi, lo;
    if (slen > len * 2)
        slen = len * 2;
    for (i = 0; i < slen; i += 2) {
        hi = str[i];
        if ('0' <= hi && hi <= '9') {
            if (i + 1 < slen) {
                lo = str[i + 1];
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
    while ((char ADEMCO_BUF_MEMORY_MODIFIER*)arr + len > p)
        *p++ = 0xFF;
    return len;
}

size_t ademco_hex_str_to_hilo_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                    size_t len,
                                    const char ADEMCO_BUF_MEMORY_MODIFIER* str) {
    char ADEMCO_BUF_MEMORY_MODIFIER* p = (char ADEMCO_BUF_MEMORY_MODIFIER*)arr;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    size_t ADEMCO_BUF_MEMORY_MODIFIER slen = str ? strlen(str) : 0;
    char ADEMCO_BUF_MEMORY_MODIFIER hi, lo;
    if (slen > len * 2)
        slen = len * 2;
    for (i = 0; i < slen; i += 2) {
        hi = str[i];
        if (isxdigit(hi)) {
            if (i + 1 < slen) {
                lo = str[i + 1];
                if (isxdigit(lo))
                    *p++ = (ademco_char2hex(hi) << 4) | (ademco_char2hex(lo) & 0x0F);
                else {
                    *p++ = (ademco_char2hex(hi) << 4) | 0x0F;
                    break;
                }
            } else {
                *p++ = (ademco_char2hex(hi) << 4) | 0x0F;
                break;
            }
        } else {
            break;
        }
    }
    while ((char ADEMCO_BUF_MEMORY_MODIFIER*)arr + len > p)
        *p++ = 0xFF;
    return len;
}

size_t ademco_hex_array_to_str(char ADEMCO_BUF_MEMORY_MODIFIER* str, const uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr, size_t len) {
    char ADEMCO_BUF_MEMORY_MODIFIER* p = str;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    for (i = 0; i < len; i++) {
        *p++ = ademco_hex2char((arr[i] >> 4) & 0x0F);
        *p++ = ademco_hex2char(arr[i] & 0x0F);
    }
    return p - str;
}

size_t ademco_hex_str_to_array(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr, const char ADEMCO_BUF_MEMORY_MODIFIER* str, uint8_t padding) {
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER* p = arr;
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER hi = 0, lo = 0;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    size_t ADEMCO_BUF_MEMORY_MODIFIER slen = str ? strlen(str) : 0;
    char ADEMCO_BUF_MEMORY_MODIFIER c;
    padding &= 0x0F;
    for (i = 0; i < slen / 2; i++) {
        c = str[i * 2];
        if ((hi = ademco_char2hex(c)) == 0xFF)
            return 0;
        if (i * 2 + 1 < slen) {
            c = str[i * 2 + 1];
            if ((lo = ademco_char2hex(c)) == 0xFF)
                return 0;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}

size_t ademco_hex_str_to_array_n(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                 const char ADEMCO_BUF_MEMORY_MODIFIER* str,
                                 size_t len, uint8_t padding) {
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER* p = arr;
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER hi = 0, lo = 0;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    char ADEMCO_BUF_MEMORY_MODIFIER c;
    padding &= 0x0F;
    for (i = 0; i < len / 2; i++) {
        c = str[i * 2];
        if ((hi = ademco_char2hex(c)) == 0xFF)
            return 0;
        if (i * 2 + 1 < len) {
            c = str[i * 2 + 1];
            if ((lo = ademco_char2hex(c)) == 0xFF)
                return 0;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}

size_t ademco_hex_str_to_array_n_allow_non_hex_str(uint8_t ADEMCO_BUF_MEMORY_MODIFIER* arr,
                                                   const char ADEMCO_BUF_MEMORY_MODIFIER* str,
                                                   size_t len,
                                                   uint8_t padding) {
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER* p = arr;
    uint8_t ADEMCO_BUF_MEMORY_MODIFIER hi = 0, lo = 0;
    size_t ADEMCO_BUF_MEMORY_MODIFIER i;
    char ADEMCO_BUF_MEMORY_MODIFIER c;
    padding &= 0x0F;
    for (i = 0; i < len / 2; i++) {
        c = str[i * 2];
        if ((hi = ademco_char2hex(c)) == 0xFF)
            hi = padding;
        if (i * 2 + 1 < len) {
            c = str[i * 2 + 1];
            if ((lo = ademco_char2hex(c)) == 0xFF)
                lo = padding;
        } else
            lo = padding;
        *p++ = ((hi << 4) & 0xF0) | (lo & 0x0F);
    }
    return p - arr;
}
