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

const char* ademco_version_string(void) {
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
    return ((strength / 10) << 4) | (strength % 10);
}

uint8_t ademco_decode_signal_strength(uint8_t code) {
    uint8_t strength = ((code >> 4) & 0x0F) * 10 + (code & 0x0F);
    if (strength > ADEMCO_SIGNAL_STRENGTH_MAX)
        strength = ADEMCO_SIGNAL_STRENGTH_MAX;
    return strength;
}

int ademco_is_valid_account(const char* acct) {
    size_t len = 0;
    int ishex = 0;
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

int ademco_is_machine_status_event(ademco_event_t ademco_event) {
    return ademco_event == EVENT_ARM ||
           ademco_event == EVENT_HALFARM ||
           ademco_event == EVENT_HALFARM_1456 ||
           ademco_event == EVENT_DISARM;
}

int ademco_is_machine_type_event(ademco_event_t ademco_event) {
    return ademco_event == EVENT_I_AM_NET_MODULE ||
           ademco_event == EVENT_I_AM_GPRS ||
           ademco_event == EVENT_I_AM_LCD_MACHINE ||
           ademco_event == EVENT_I_AM_WIRE_MACHINE ||
           ademco_event == EVENT_I_AM_WIFI_MACHINE ||
           ademco_event == EVENT_I_AM_3_SECTION_MACHINE ||
           ademco_event == EVENT_I_AM_IOT_MACHINE ||
           ademco_event == EVENT_I_AM_TRUE_COLOR ||
           ademco_event == EVENT_I_AM_GPRS_IOT ||
           ademco_event == EVENT_I_AM_GPRS_PHONE ||
           ademco_event == EVENT_I_AM_NB_MACHINE ||
           ademco_event == EVENT_I_AM_WIFI2_MACHINE;
}

int ademco_is_event_need_control_source(ademco_event_t ademco_event) {
    return ademco_event == EVENT_ARM ||
           ademco_event == EVENT_HALFARM ||
           ademco_event == EVENT_HALFARM_1456 ||
           ademco_event == EVENT_DISARM ||
           ademco_event == EVENT_EMERGENCY ||
           ademco_event == EVENT_DURESS ||
           ademco_event == EVENT_PHONE_USER_CANCLE_ALARM;
}

ademco_event_t ademco_get_exception_event_by_resume_event(ademco_event_t resumeEvent) {
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

ademco_event_level_t ademco_get_event_level(ademco_event_t ademco_event) {
    switch (ademco_event) {
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

const char* ademco_event_to_string(ademco_event_t ademco_event) {
    switch (ademco_event) {
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
            return "Undefined ademco_event_t";
    }
}

#if ADEMCO_ENABLE_CHINESE
const char* ademco_event_to_string_chinese(ademco_event_t ademco_event) {
    switch (ademco_event) {
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

uint16_t ademco_crc16(const ademco_char_t* buff, size_t len) {
    uint16_t crc = 0;
    while (len--)
        crc = (crc >> 8) ^ crc16Table[(crc ^ *buff++) & 0xFF];
    return crc;
}

size_t ademco_append_data(ademco_char_t* buff, const char* acct, ademco_id_t ademco_id,
                          ademco_event_t ademco_event, ademco_gg_t gg, ademco_zone_t zone) {
    char* p = (char*)buff;
    if (ademco_event == EVENT_INVALID_EVENT) {
        *p++ = '[';
        *p++ = ']';
        return ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN;
    } else {
        *p++ = '[';

        *p++ = '#';
        if (acct && ademco_id == 0) {
            p += snprintf(p, 20, "%s", acct);
        } else {
            snprintf(p, 6 + 1, "%06X", ademco_id % ADEMCO_ID_SENTINEL);
            p += 6;
        }

        *p++ = '|';

        snprintf(p, 4 + 1, "%04d", ademco_event % ADEMCO_EVENT_SENTINEL);
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

ademco_parse_result_t ademco_parse_data(const ademco_char_t* packet, size_t packet_len,
                                        ademco_data_t* ademco_data,
                                        ademco_parse_error_t* err) {
    if (packet_len == ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN &&
        packet[0] == '[' && packet[1] == ']') {
        memset(ademco_data, 0, sizeof(*ademco_data));
        strcpy((char*)ademco_data->raw, "[]");
        ademco_data->raw_len = 2;
        return ADEMCO_PARSE_RESULT_OK;
    } else if (packet_len >= ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN &&
               packet[0] == '[' &&
               packet[packet_len - 1] == ']') {  // [#000000|1400 00 000]
        const char* p = packet + 2;
        size_t acct_len = packet_len - 15;
        if (acct_len < 6) {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "acct_len < 6");
            return ADEMCO_PARSE_RESULT_ERROR;
        } else if (acct_len > ADEMCO_PACKET_ACCT_MAX_LEN) {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "acct_len > ADEMCO_PACKET_ACCT_MAX_LEN");
            return ADEMCO_PARSE_RESULT_ERROR;
        }

        char temp[ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX] = {0};
        strncpy(temp, (const char*)p, acct_len);
        temp[acct_len] = '\0';
        ademco_data->ademco_id = (ademco_id_t)strtoul(temp, NULL, 16);
        p += acct_len;

        if (*p++ != '|') {
            ADEMCO_FILL_PARSE_ERROR(err, p - packet, "*p++ != '|'");
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        strncpy(temp, (const char*)p, 4);
        temp[4] = '\0';
        ademco_data->ademco_event = (ademco_event_t)(atoi(temp));
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
        strncpy(temp, (const char*)p, 3);
        temp[3] = '\0';
        ademco_data->zone = (ademco_zone_t)atoi(temp);

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
                 ademco_data->ademco_id % 100000000);
        p += 8;

        *p++ = ' ';  // 18
        *p++ = '1';
        *p++ = '8';
        *p++ = ' ';

        // E for open, R for close
        if (ademco_data->ademco_event / 1000 == 1 ||
            ademco_data->ademco_event == EVENT_HALFARM)
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
            snprintf(p,
                     fe100_len - (p - fe100),
                     "%03d",
                     ademco_data->ademco_event % 1000);
            p += 3;
        }

        *p++ = ' ';  // FE100, gg === 0
        *p++ = '0';
        *p++ = '0';
        *p++ = ' ';

        if (ademco_is_machine_status_event(ademco_data->ademco_event))
            *p++ = 'U';  // user event
        else
            *p++ = 'C';  // zone event

        snprintf(p,
                 fe100_len - (p - fe100),
                 "%03d",
                 ademco_data->zone % 1000);
        p += 3;

        *p++ = ' ';
        *p++ = '\r';
        return p - fe100;
    }
    return 0;
}

void ademco_xdata_init(ademco_xdata_t* xdata) {
    memset(xdata, 0, sizeof *xdata);
}

int ademco_xdata_convert(ademco_xdata_t* xdata,
                         ademco_xdata_length_format_t xlf) {
    if (!xdata) {
        return ADEMCO_ERR;
    }
    if (xdata->lenghFormat == xlf) {
        return ADEMCO_OK;
    }

    size_t len = ademco_xdata_get_valid_content_len(xdata);
    char raw[ADEMCO_PACKET_XDATA_MAX_LEN];

    if (xlf == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX) {
        if (len + 6 > ADEMCO_PACKET_XDATA_MAX_LEN)
            return ADEMCO_ERR;
        xdata->raw[5 + ademco_xdata_get_valid_content_len(xdata)] = '\0';
        len = ademco_hex_str_to_array((uint8_t*)raw,
                                      ademco_xdata_get_valid_content_addr(xdata),
                                      0x0F);
        ademco_make_xdata(xdata,
                          ADEMCO_XDATA_LENGTH_FMT_TWO_HEX,
                          ADEMCO_XDATA_TRANSFORM_AS_IS,
                          raw, len);
    } else {
        memcpy(raw, ademco_xdata_get_valid_content_addr(xdata), len);
        ademco_make_xdata(xdata,
                          ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL,
                          ADEMCO_XDATA_TRANSFORM_TO_ASCII,
                          raw, len);
    }
    return ADEMCO_OK;
}

#ifdef SWIG
static
#endif
const char* ademco_xdata_get_valid_content_addr(const ademco_xdata_t* xdata) {
    if (xdata->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
        xdata->raw_len > 4)
        return xdata->raw + 3;
    else if (xdata->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
             xdata->raw_len > 6)
        return xdata->raw + 5;
    else
        return NULL;
}

int ademco_xdata_memcmp(const ademco_xdata_t* xdata,
                        const void* buf, size_t buf_len) {
    if (ademco_xdata_get_valid_content_len(xdata) != buf_len) return 0;
    const void* p = ademco_xdata_get_valid_content_addr(xdata);
    if (!p) return 0;
    return memcmp(p, buf, buf_len);
}

size_t ademco_xdata_get_valid_content_len(const ademco_xdata_t* xdata) {
    if (xdata->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
        xdata->raw_len > 4)
        return xdata->raw_len - 4;
    else if (xdata->lenghFormat == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
             xdata->raw_len > 6)
        return xdata->raw_len - 6;
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

int ademco_make_xdata(ademco_xdata_t* xdata,
                      ademco_xdata_length_format_t xlf,
                      ademco_xdata_transform_t xtr,
                      const ademco_char_t* content, size_t len) {
    char transformed[ADEMCO_PACKET_XDATA_MAX_LEN];
    size_t translen;
    len &= 0xFFFF;
    translen = len & 0xFFFF;

    if (xtr == ADEMCO_XDATA_TRANSFORM_AS_IS)
        memcpy(transformed, content, len);
    else if (len * 2 < ADEMCO_PACKET_XDATA_MAX_LEN)
        translen = ademco_hex_array_to_str(transformed,
                                           (const uint8_t*)content, len) &
                   0xFFFF;
    else
        return ADEMCO_ERR;

    xdata->lenghFormat = xlf;
    if (xlf == ADEMCO_XDATA_LENGTH_FMT_TWO_HEX &&
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
    } else if (xlf == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL &&
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

int ademco_is_valid_packet_id(const char* standard, const char* id, size_t len) {
    size_t standard_len = strlen(standard);
    len = standard_len < len ? standard_len : len;
    return strncmp(standard, id, len) == 0;
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

ademco_packet_id_t ademco_packet_id_from_string(const char* id, size_t len) {
    for (int i = 0; i < AID_COUNT; i++)
        if (ademco_is_valid_packet_id(ids[i], id, len))
            return (ademco_packet_id_t)(i);
    return AID_INVALID;
}

const char* ademco_packet_id_to_string(ademco_packet_id_t id) {
    if (0 <= id && id < AID_COUNT)
        return ids[id];
    else
        return "Unkown ademco_packet_id_t";
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

size_t ademco_make_empty_data_packet(ademco_char_t* dst_buff, size_t len,
                                     const char* id, uint16_t seq,
                                     const char* acct, ademco_id_t ademco_id) {
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
        snprintf((char*)p, 7, "%06X", ademco_id);
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
    uint16_t crc = ademco_crc16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademco_make_adm_empty_data_packet(ademco_char_t* dst_buff, size_t len,
                                         const char* id, uint16_t seq,
                                         const char* acct, ademco_id_t ademco_id) {
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
        snprintf((char*)p, 7, "%06X", ademco_id);
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
    uint16_t crc = ademco_crc16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    } else
        return 0;
}

size_t ademco_make_null_packet(ademco_char_t* buff, size_t len,
                               uint16_t seq, const char* acct,
                               ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buff, len,
                                         ADEMCO_PACKET_ID_NULL,
                                         seq, acct, ademco_id);
}

size_t ademco_make_ack_packet(ademco_char_t* buff, size_t len,
                              uint16_t seq, const char* acct,
                              ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buff, len,
                                         ADEMCO_PACKET_ID_ACK,
                                         seq, acct, ademco_id);
}

size_t ademco_make_nak_packet(ademco_char_t* buff, size_t len,
                              uint16_t seq, const char* acct,
                              ademco_id_t ademco_id) {
    return ademco_make_empty_data_packet(buff, len,
                                         ADEMCO_PACKET_ID_NAK,
                                         seq, acct, ademco_id);
}

size_t ademco_make_hb_packet(ademco_char_t* dst_buff, size_t len,
                             uint16_t seq, const char* acct,
                             ademco_id_t ademco_id, ademco_event_t ademco_event,
                             ademco_gg_t gg, ademco_zone_t zone,
                             const ademco_xdata_t* xdata) {
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
        snprintf(p, 7, "%06X", ademco_id);
        p += 6;
    }
    p += ademco_append_data(p, acct, ademco_id, ademco_event, gg, zone);
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
    uint16_t crc = ademco_crc16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
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
                              const ademco_xdata_t* xdata) {
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
        snprintf(p, 7, "%06X", ademco_id);
        p += 6;
    }
    p += ademco_append_data(p, acct, ademco_id, ademco_event, gg, zone);
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
    uint16_t crc = ademco_crc16(pid, ademco_len);
    snprintf(temp, 5, "%04X", crc);
    memcpy(pcrc, temp, 4);

    if (dst_buff != NULL && len > packet_len) {
        memcpy(dst_buff, buff, packet_len);
        dst_buff[packet_len] = '\0';  // for debug convenience
        return packet_len;
    }
    return 0;
}

static void copyAcct2AdemcoPacket(ademco_packet_t* pkt, const char* acct) {
    if (acct) {
        size_t len = strlen(acct);
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
    pkt->data.ademco_id = ademco_id;
    pkt->id = AID_NULL;
    return pkt->raw_len = ademco_make_null_packet(pkt->raw, sizeof(pkt->raw),
                                                  seq, acct, ademco_id);
}

size_t ademco_make_ack_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->data.ademco_id = ademco_id;
    pkt->id = AID_ACK;
    return pkt->raw_len = ademco_make_ack_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id);
}

size_t ademco_make_nak_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    pkt->data.ademco_id = ademco_id;
    pkt->id = AID_NAK;
    return pkt->raw_len = ademco_make_nak_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id);
}

size_t ademco_make_hb_packet2(ademco_packet_t* pkt, uint16_t seq,
                              const char* acct, ademco_id_t ademco_id,
                              ademco_event_t ademco_event, ademco_gg_t gg,
                              ademco_zone_t zone,
                              const ademco_xdata_t* xdata) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademco_append_data2(&pkt->data, acct, ademco_id, ademco_event, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        pkt->xdata.lenghFormat = xdata->lenghFormat;
        memcpy(pkt->xdata.raw, xdata->raw, xdata->raw_len);
        pkt->xdata.raw_len = xdata->raw_len;
    } else
        memset(&pkt->xdata, 0, sizeof(*xdata));
    pkt->id = AID_HB;
    return pkt->raw_len = ademco_make_hb_packet(pkt->raw, sizeof(pkt->raw),
                                                seq, acct, ademco_id,
                                                ademco_event,
                                                gg, zone, xdata);
}

size_t ademco_make_adm_packet2(ademco_packet_t* pkt, uint16_t seq,
                               const char* acct, ademco_id_t ademco_id,
                               ademco_event_t ademco_event, ademco_gg_t gg,
                               ademco_zone_t zone,
                               const ademco_xdata_t* xdata) {
    pkt->seq = seq;
    copyAcct2AdemcoPacket(pkt, acct);
    ademco_append_data2(&pkt->data, acct, ademco_id, ademco_event, gg, zone);
    if (xdata && xdata->raw_len > 0) {
        pkt->xdata.lenghFormat = xdata->lenghFormat;
        memcpy(pkt->xdata.raw, xdata->raw, xdata->raw_len);
        pkt->xdata.raw_len = xdata->raw_len;
    } else
        memset(&pkt->xdata, 0, sizeof(*xdata));
    pkt->id = AID_ADM_CID;
    return pkt->raw_len = ademco_make_adm_packet(pkt->raw, sizeof(pkt->raw),
                                                 seq, acct, ademco_id,
                                                 ademco_event,
                                                 gg, zone, xdata);
}

ademco_parse_result_t ademco_parse_packet(const ademco_char_t* buff,
                                          size_t len,
                                          ademco_packet_t* pkt,
                                          size_t* cb_commited,
                                          ademco_parse_error_t* err) {
    const char *p, *q, *pid, *pend, *pacct, *pdata;
    uint16_t crc;
    size_t len_needed;

    if (len < 9) {
        ADEMCO_FILL_PARSE_ERROR(err, 0, "ADEMCO_PARSE_RESULT_NOT_ENOUGH");
        return ADEMCO_PARSE_RESULT_NOT_ENOUGH;
    }

    *cb_commited = 0;
    while (*buff != ADEMCO_PACKET_PREFIX && len > 0) {
        buff++;
        len--;
        (*cb_commited)++;
    }

    if (*cb_commited > 0) {
        ADEMCO_FILL_PARSE_ERROR(err, 0, "fixed prefix not found");
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    p = buff;
    if (*p++ != ADEMCO_PACKET_PREFIX) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "fixed prefix not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // crc
    crc = 0;
    for (q = p; p - q < 4; p++) {
        uint8_t h = char2hex(*p);
        if (h == 0xFF) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "crc contains non-hex characters");
            *cb_commited += p - buff;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        crc = (crc << 4) | h;
    }

    // len
    pkt->len = 0;
    for (q = p; p - q < 4; p++) {
        uint8_t h = char2hex(*p);
        if (h == 0xFF) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "len contains non-hex characters");
            *cb_commited += p - buff;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        pkt->len = (pkt->len << 4) | h;
    }
    len_needed = 9 + pkt->len + 1;
    if (len < len_needed) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "ADEMCO_PARSE_RESULT_NOT_ENOUGH");
        return ADEMCO_PARSE_RESULT_NOT_ENOUGH;
    } else if (len_needed >= ADEMCO_PACKET_MAX_LEN) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "len_needed >= ADEMCO_PACKET_MAX_LEN");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    pid = p;
    pend = pid + pkt->len;

    if (pend > buff + len) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "suffix out of range");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    if (*pend != ADEMCO_PACKET_SUFIX) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "suffix not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    pkt->crc = ademco_crc16(pid, pkt->len);
    if (pkt->crc != crc) {
        ADEMCO_FILL_PARSE_ERROR(err, 1, "crc error");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // id
    if (*pid != '\"') {
        ADEMCO_FILL_PARSE_ERROR(err, pid - buff, "left \" of id not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    p = pid + 1;
    while (p < pend && *p != '\"') {
        p++;
    }
    if (p >= pend || *p != '\"') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "right \" of id not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    pkt->id = ademco_packet_id_from_string((const char*)pid, ++p - pid);
    if (pkt->id == AID_INVALID) {
        ADEMCO_FILL_PARSE_ERROR(err, pid - buff, "unknown ademco id");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // seq
    pkt->seq = 0;
    for (q = p; p - q < 4; p++) {
        if ('0' <= *p && *p <= '9') {
            pkt->seq = (pkt->seq * 10) + (*p - '0');
        } else {
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "seq contains non-digit characters");
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
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "p >= pend || (*p != 'L' && *p != '#')");
            *cb_commited += p - buff;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        // only check if format is correct, ignore it's content
    } else if (*p == 'L') {  // rrcvr not exits
                             // pass
    } else {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "both LPREF and RRCVR not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // lpref
    if (*p != 'L') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "LPREF not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    while (p < pend && *p != '#') {
        p++;
    }
    // only check if format is correct, ignore it's content
    if (p >= pend || *p != '#') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "# not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    // lpref passed

    // #acct
    pacct = ++p;
    while (p - pacct < ADEMCO_PACKET_ACCT_MAX_LEN && *p != '[') {
        if (!isxdigit(*p)) {
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "acct contains non-hex characters");
            *cb_commited += p - buff;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        p++;
    }
    if (*p != '[') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "acct too long");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    strncpy(pkt->acct, (const char*)pacct, p - pacct);
    pkt->acct[p - pacct] = '\0';

    // data
    pdata = p;
    if (*p != '[') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "data's [ not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    while (p < pend && *p != ']') {
        p++;
    }
    if (p >= pend || *p != ']') {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "data's ] not found");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    if (ademco_parse_data(pdata, ++p - pdata, &pkt->data, err) != ADEMCO_PARSE_RESULT_OK) {
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    // *xdata
    if (*p == '[') {  // xdata exists
        const char* pxdata = p++;
        ademco_xdata_length_format_t xlf = ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL;
        size_t valid_len = 0;
        for (int i = 0; i < 4; i++)
            if (!isxdigit(*(uint8_t*)(p + i)))
                xlf = ADEMCO_XDATA_LENGTH_FMT_TWO_HEX;

        if (xlf == ADEMCO_XDATA_LENGTH_FMT_FOUR_DECIMAL) {
            valid_len = (char2hex(*(pxdata + 1)) << 12) |
                        (char2hex(*(pxdata + 2)) << 8) |
                        (char2hex(*(pxdata + 3)) << 4) |
                        (char2hex(*(pxdata + 4)));
            p += 4 + valid_len;
        } else {
            valid_len = (pxdata[1] << 8) | pxdata[2];
            p += 2 + valid_len;
        }

        if (p >= pend || *p != ']' || *(p + 1) != '_') {
            ADEMCO_FILL_PARSE_ERROR(err, p - buff, "xdata's ] not found or next char is not _");
            *cb_commited += p - buff;
            return ADEMCO_PARSE_RESULT_ERROR;
        }
        pkt->xdata.lenghFormat = xlf;
        pkt->xdata.raw_len = ++p - pxdata;
        memcpy(pkt->xdata.raw, pxdata, pkt->xdata.raw_len);
    } else {
        pkt->xdata.raw_len = 0;
    }

    // timestamp, _%02d:%02d:%02d,%02d-%02d-%04d
    // only check lengh, if format is incorrect, use local time instead
    if (pend - p == ADEMCO_PACKET_TIMESTAMP_LEN) {
        struct tm tm = {0};
        pkt->timestamp = 0;
        do {
            if (*p++ != '_') {
                break;
            }
            tm.tm_hour = char2hex(*p++);
            tm.tm_hour = tm.tm_hour * 10 + char2hex(*p++);
            if (*p++ != ':') {
                break;
            }

            tm.tm_min = char2hex(*p++);
            tm.tm_min = tm.tm_min * 10 + char2hex(*p++);
            if (*p++ != ':') {
                break;
            }

            tm.tm_sec = char2hex(*p++);
            tm.tm_sec = tm.tm_sec * 10 + char2hex(*p++);
            if (*p++ != ',') {
                break;
            }

            tm.tm_mon = char2hex(*p++);
            tm.tm_mon = tm.tm_mon * 10 + char2hex(*p++);
            if (*p++ != '-') {
                break;
            }

            tm.tm_mday = char2hex(*p++);
            tm.tm_mday = tm.tm_mday * 10 + char2hex(*p++);
            if (*p++ != '-') {
                break;
            }

            tm.tm_year = char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + char2hex(*p++);
            tm.tm_year = tm.tm_year * 10 + char2hex(*p++);

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
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "timestamp length not correct");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }

    if (p++ != pend) {
        ADEMCO_FILL_PARSE_ERROR(err, p - buff, "packet length not correct");
        *cb_commited += p - buff;
        return ADEMCO_PARSE_RESULT_ERROR;
    }
    pkt->raw_len = *cb_commited = p - buff;
    if (pkt->raw != buff)
        memcpy(pkt->raw, buff, pkt->raw_len);
    return ADEMCO_PARSE_RESULT_OK;
}

size_t ademco_hilo_array_to_dec_str(ademco_char_t* str, const uint8_t* arr, size_t len) {
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

size_t ademco_hilo_array_to_hex_str(ademco_char_t* str, const uint8_t* arr, size_t len) {
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

size_t ademco_dec_str_to_hilo_array(uint8_t* arr, size_t len, const char* str) {
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

size_t ademco_hex_str_to_hilo_array(uint8_t* arr, size_t len, const char* str) {
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

size_t ademco_hex_array_to_str(char* str, const uint8_t* arr, size_t len) {
    char* p = str;
    for (size_t i = 0; i < len; i++) {
        *p++ = hex2char((arr[i] >> 4) & 0x0F);
        *p++ = hex2char(arr[i] & 0x0F);
    }
    return p - str;
}

size_t ademco_hex_str_to_array(uint8_t* arr, const char* str, uint8_t padding) {
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

size_t ademco_hex_str_to_array_n(uint8_t* arr, const char* str,
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

size_t ademco_hex_str_to_array_n_allow_non_hex_str(uint8_t* arr,
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
