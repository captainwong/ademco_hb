#include "hb_com.h"

#include <string.h>

size_t hb_get_available_zone_properties_by_type(hb_machine_type_t type,
                                                hb_zone_property_t props[HZP_COUNT]) {
    switch (type) {
        case HMT_WIFI:
            break;
        case HMT_CAMERA:
            break;
        case HMT_GPRS_IOT:
        case HMT_GPRS:
        case HMT_WIFI2: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_NETMOD: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_SUB_MACHINE,
                HZP_REMOTE_CONTROL,
                HZP_BUGLAR_HALF,
                HZP_SHIELD,
                HZP_DOOR_RING,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_LCD: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE,
                HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_WIRED: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_TRUE_COLOR: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_THREE_SECTION: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_IOT: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_GPRS_PHONE: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_NB: {
            hb_zone_property_t hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_DOOR_RING,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        default:
            return 0;
    }
    return 0;
}

ademco_zone_t hb_get_max_zone_by_type(hb_machine_type_t type) {
    switch (type) {
        case HMT_GPRS_IOT:
        case HMT_GPRS:
        case HMT_WIRED:
        case HMT_WIFI2:
            return 99;

        case HMT_NETMOD:
        case HMT_NB:
            return 999;

        case HMT_LCD:
            return 249;

        case HMT_TRUE_COLOR:
            return 68;

        case HMT_THREE_SECTION:
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

int hb_is_valid_zone_by_type(hb_machine_type_t type, ademco_zone_t zone) {
    return ADEMCO_ZONE_FOR_MACHINE_SELF <= zone &&
           zone <= hb_get_max_zone_by_type(type);
}

int hb_is_valid_zone_by_type_strict(hb_machine_type_t type, ademco_zone_t zone) {
    return ADEMCO_ZONE_MIN <= zone && zone <= hb_get_max_zone_by_type(type);
}

int hb_is_machine_on_sale(hb_machine_type_t type) {
    return type == HMT_GPRS_IOT ||
           type == HMT_NETMOD ||
           type == HMT_GPRS ||
           type == HMT_IOT ||
           type == HMT_LCD ||
           type == HMT_WIRED ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION ||
           type == HMT_GPRS_PHONE ||
           type == HMT_NB ||
           type == HMT_WIFI2;
}

int hb_machine_can_arm(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type) && (type != HMT_NB);
}

int hb_machine_can_disarm(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type) && (type != HMT_NB);
}

int hb_machine_can_config(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type);
}

int hb_machine_can_half_arm(hb_machine_type_t type) {
    return type == HMT_NETMOD || type == HMT_LCD;
}

int hb_machine_can_report_signal_strength(hb_machine_type_t type) {
    return type == HMT_GPRS ||
           type == HMT_GPRS_IOT ||
           type == HMT_IOT ||
           type == HMT_LCD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION ||
           type == HMT_GPRS_PHONE ||
           type == HMT_NB ||
           type == HMT_WIFI2;
}

int hb_machine_can_report_by_sms(hb_machine_type_t type) {
    return type == HMT_GPRS ||
           type == HMT_LCD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION ||
           type == HMT_GPRS_PHONE;
}

int hb_machine_has_wired_zones(hb_machine_type_t type) {
    return type == HMT_NETMOD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION;
}

ademco_zone_t hb_wired_zone_min(hb_machine_type_t type) {
    switch (type) {
        case HMT_NETMOD:
            return 1;
        case HMT_TRUE_COLOR:
            return 1;
        case HMT_THREE_SECTION:
            return 61;
        default:
            return 0;
    }
}

ademco_zone_t hb_wired_zone_max(hb_machine_type_t type) {
    switch (type) {
        case HMT_NETMOD:
            return 8;
        case HMT_TRUE_COLOR:
            return 8;
        case HMT_THREE_SECTION:
            return 68;
        default:
            return 0;
    }
}

int hb_machine_can_write_zone(hb_machine_type_t type) {
    return type == HMT_NETMOD;
}

int hb_machine_can_link_sub_machine(hb_machine_type_t type) {
    return type == HMT_NETMOD;
}

int hb_zone_can_report_lost(hb_zone_property_t zp) {
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

void hb_machine_timer_init(hb_machine_timer_t* timer) {
    memset(timer->data, 25, sizeof(timer));
}

int hb_is_valid_time_point(hb_machine_time_point_t* tp) {
    return tp &&
           tp->hour < 24 &&
           tp->minute < 60;
}

void hb_time_point_to_greenwich(hb_machine_time_point_t* tp) {
    if (tp && tp->hour != 25)
        tp->hour = (tp->hour + 16) % 24;
}

void hb_time_point_from_greenwich(hb_machine_time_point_t* tp) {
    if (tp && tp->hour != 25)
        tp->hour = (tp->hour + 8) % 24;
}

int hb_is_valid_timer_one(hb_machine_timer_one_t* timer) {
    return timer &&
           hb_is_valid_time_point(&timer->arm_at) &&
           hb_is_valid_time_point(&timer->disarm_at);
}

int hb_is_valid_machine_timer(hb_machine_timer_t* timer) {
    return timer &&
           hb_is_valid_timer_one(&timer->timer[0]) &&
           hb_is_valid_timer_one(&timer->timer[1]);
}

void hb_machine_timer_to_greenwich(hb_machine_timer_t* timer) {
    hb_time_point_to_greenwich(&timer->timer[0].arm_at);
    hb_time_point_to_greenwich(&timer->timer[0].disarm_at);
    hb_time_point_to_greenwich(&timer->timer[1].arm_at);
    hb_time_point_to_greenwich(&timer->timer[1].disarm_at);
}

void hb_machine_timer_from_greenwich(hb_machine_timer_t* timer) {
    hb_time_point_from_greenwich(&timer->timer[0].arm_at);
    hb_time_point_from_greenwich(&timer->timer[0].disarm_at);
    hb_time_point_from_greenwich(&timer->timer[1].arm_at);
    hb_time_point_from_greenwich(&timer->timer[1].disarm_at);
}

ademco_event_t hb_machine_status_to_ademco_event(hb_machine_status_t status) {
    switch (status) {
#define XX(hms, ae) \
    case hms:       \
        return ae;

        HMS_EVENT_MAP(XX)
#undef XX
        default:
            return EVENT_INVALID_EVENT;
    }
}

hb_machine_status_t hb_machine_status_from_ademco_event(ademco_event_t ademco_event) {
    switch (ademco_event) {
#define XX(hms, ae) \
    case ae:        \
        return hms;

        HMS_EVENT_MAP(XX)
#undef XX

        case EVENT_HALFARM_1456:
            return HMS_HALF_ARM;
        case EVENT_ENTER_SET_MODE:
            return HMS_SETTING;
        default:
            return HMS_INVALID;
    }
}

ademco_event_t hb_machine_type_to_ademco_event(hb_machine_type_t type) {
    switch (type) {
#define XX(hmt, ae) \
    case hmt:       \
        return ae;

        HMT_EVENT_MAP(XX)
#undef XX

        default:
            return EVENT_INVALID_EVENT;
    }
}

hb_machine_type_t hb_machine_type_from_ademco_event(ademco_event_t ademco_event) {
    switch (ademco_event) {
#define XX(hmt, ae) \
    case ae:        \
        return hmt;

        HMT_EVENT_MAP(XX)
#undef XX

        default:
            return HMT_INVALID;
    }
}

ademco_event_t hb_zone_property_to_ademco_event(hb_zone_property_t zp) {
    switch (zp) {
#define XX(hzp, ae) \
    case hzp:       \
        return ae;

        HZP_EVENT_MAP(XX)
#undef XX

        default:
            return EVENT_INVALID_EVENT;
    }
}

const char* hb_machine_status_to_string(hb_machine_status_t status) {
    switch (status) {
#define XX(name, value, str) \
    case HMS_##name:         \
        return #name;
        HB_MACHINE_STATUS_MAP(XX)
#undef XX
        default:
            return "InvalidMachineStatus";
    }
}

const char* hb_machine_type_to_string(hb_machine_type_t type) {
    switch (type) {
#define XX(name, value, str) \
    case HMT_##name:         \
        return #name;
        HB_MACHINE_TYPE_MAP(XX)
#undef XX

        default:
            return "Unknown hb_machine_type_t";
    }
}

const char* hb_zone_property_to_string(hb_zone_property_t zp) {
    switch (zp) {
#define XX(name, value, str) \
    case HZP_##name:         \
        return #name;
        HB_ZONE_PROPERTY_MAP(XX)
#undef XX

        default:
            return "InvalidZoneProperty";
    }
}

const char* hb_com_req_to_string(hb_com_req_t type) {
    switch (type) {
#define XX(name, str)       \
    case HB_COM_REQ_##name: \
        return #name;
        HB_COM_REQUESTS_MAP(XX)
#undef XX

        default:
            return "InvalidRequestType";
    }
}

const char* hb_com_resp_to_string(hb_com_resp_t type) {
    switch (type) {
#define XX(name, str)        \
    case HB_COM_RESP_##name: \
        return #name;
        HB_COM_RESPONSES_MAP(XX)
#undef XX

        default:
            return "InvalidResponseType";
    }
}

#if ADEMCO_ENABLE_CHINESE
const char* hb_machine_status_to_string_chinese(hb_machine_status_t status) {
    switch (status) {
#define XX(name, value, str) \
    case HMS_##name:         \
        return str;
        HB_MACHINE_STATUS_MAP(XX)
#undef XX

        default:
            return "无效状态";
    }
}

const char* hb_machine_type_to_string_chinese(hb_machine_type_t type) {
    switch (type) {
#define XX(name, value, str) \
    case HMT_##name:         \
        return str;
        HB_MACHINE_TYPE_MAP(XX)
#undef XX

        default:
            return "未知机型";
    }
}

const char* hb_zone_property_to_string_chinese(hb_zone_property_t zp) {
    switch (zp) {
#define XX(name, value, str) \
    case HZP_##name:         \
        return str;
        HB_ZONE_PROPERTY_MAP(XX)
#undef XX

        default:
            return "无效属性";
    }
}

const char* hb_com_req_to_string_chinese(hb_com_req_t type) {
    switch (type) {
#define XX(name, str)       \
    case HB_COM_REQ_##name: \
        return str;
        HB_COM_REQUESTS_MAP(XX)
#undef XX

        default:
            return "无效请求";
    }
}

const char* hb_com_resp_to_string_chinese(hb_com_resp_t type) {
    switch (type) {
#define XX(name, str)        \
    case HB_COM_RESP_##name: \
        return str;
        HB_COM_RESPONSES_MAP(XX)
#undef XX

        default:
            return "无效回应";
    }
}

#endif  // ADEMCO_ENABLE_CHINESE

const char* hb_get_zone_format_str(hb_machine_type_t type) {
    ademco_zone_t zone = hb_get_max_zone_by_type(type);
    if (10 < zone && zone < 100)
        return "%02d";
    else if (zone < 1000)
        return "%03d";
    else
        return "%d";
}

void hb_sum(uint8_t* data, size_t len) {
    uint8_t sum = 0;
    uint8_t* p = data;
    while (p != data + len - 1)
        sum += *p++;
    *p = sum;
}

int hb_chksum(const uint8_t* data, size_t len) {
    uint8_t sum = 0;
    const uint8_t* p = data;
    while (p != data + len - 1)
        sum += *p++;
    return sum == *p;
}

#define copy_data_to_com                    \
    if (cd) {                               \
        cd->len = len & 0xFF;               \
        memcpy(cd->data, data, len & 0xFF); \
    }

hb_com_req_t hb_com_parse_req(const uint8_t* data,
                              size_t len,
                              hb_com_data_t* cd) {
    do {
        if (len < 5) {
            break;
        }
        if (data[0] != 0xEB) {
            break;
        }
        switch (data[1]) {
            case 0xAB: {
                if (data[2] != 0x3F) {
                    break;
                }
                switch (data[3]) {
                    case 0xA0:  // EB AB 3F A0 75
                    {
                        if (len != HB_COM_REQ_A0_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_A0_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_A0;
                    }
                    case 0xA1:  // EB AB 3F A1 76
                    {
                        if (len != HB_COM_REQ_A1_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_A1_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_A1;
                    }
                    case 0xA2:  // EB AB 3F A2 77
                    {
                        if (len != HB_COM_REQ_A2_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_A2_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_A2;
                    }
                    case 0xA5:  // EB AB 3F A5 7A
                    {
                        if (len != HB_COM_REQ_A5_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_A5_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_A5;
                    }
                    case 0xAC:  // EB AB 3F AC 81
                    {
                        if (len != HB_COM_REQ_AC_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_AC_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_AC;
                    }
                    case 0xAD:  // EB AB 3F AD 82
                    {
                        if (len != HB_COM_REQ_AD_LEN) {
                            break;
                        }
                        if (memcmp(HB_COM_REQ_AD_DATA, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HB_COM_REQ_AD;
                    }
                    default:
                        break;
                }
                break;
            }
            case 0xBA: {
                if (data[2] != 0x3F) {
                    break;
                }
                if (len == HB_COM_REQ_RA_LEN &&
                    memcmp(data, HB_COM_REQ_RA_DATA, len) == 0) {
                    copy_data_to_com;
                    return HB_COM_REQ_RA;
                }
                break;
            }
            case 0xCB: {
                if (data[2] != 0x3F) {
                    break;
                }

                if (data[3] == 0x09 &&
                    data[4] == 0xA3 &&
                    len == HB_COM_REQ_A3_LEN) {
                    // EB CB 3F 09 A3 P1 P2 P3 SUM
                    if (hb_chksum(data, len)) {
                        copy_data_to_com;
                        return HB_COM_REQ_A3;
                    }
                } else if (data[3] == 0x0F &&
                           data[4] == 0x4D &&
                           len == HB_COM_REQ_WA_LEN) {
                    if (hb_chksum(data, len)) {
                        copy_data_to_com;
                        return HB_COM_REQ_WA;
                    }
                } else if (data[3] == 0x0E &&
                           data[4] == 0xA7 &&
                           len == HB_COM_REQ_A7_LEN) {
                    // EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
                    if (hb_chksum(data, len)) {
                        copy_data_to_com;
                        return HB_COM_REQ_A7;
                    }
                }

                /*else if (data[3] == 0x08 && data[4] == 0xA9 && len == A9_len) {
                        A7 req; memcpy(req.data, data, req.len); sum(req);
                        if (data[len - 1] == req.data[len - 1]) { return A7; }
                }*/

                else if (data[3] == 0x08 &&
                         data[4] == 0xAA &&
                         len == HB_COM_REQ_AA_LEN) {
                    // EB CB 3F 08 AA P1 P2 SUM
                    if (hb_chksum(data, len)) {
                        copy_data_to_com;
                        return HB_COM_REQ_AA;
                    }
                }

                /*else if (data[3] == 0x08 && data[4] == 0xAE && len == AE_len) {
                        AA req; memcpy(req.data, data, req.len); sum(req);
                        if (data[len - 1] == req.data[len - 1]) { return AA; }
                }*/

                else if (data[3] == 0x06 && data[4] == 0xB0 &&
                         len == HB_COM_REQ_B0_LEN &&
                         memcmp(HB_COM_REQ_B0_DATA, data, len) == 0) {
                    // EB CB 3F 06 B0 AB
                    copy_data_to_com;
                    return HB_COM_REQ_B0;
                }
            }
        }
    } while (0);
    return HB_COM_REQ_INVALID;
}

hb_com_req_t hb_com_parse_xdata_req(const ademco_xdata_t* xdata,
                                    hb_com_data_t* cd) {
    if (!xdata) {
        return HB_COM_REQ_INVALID;
    }
    return hb_com_parse_req(
        (const uint8_t*)ademco_xdata_get_valid_content_addr(xdata),
        ademco_xdata_get_valid_content_len(xdata),
        cd);
}

hb_com_resp_t hb_com_parse_resp(const uint8_t* data,
                                size_t len,
                                hb_com_data_t* cd) {
    do {
        if (len < 7) {
            break;
        }  // 所有的 response ，长度最小为 7
        if (data[0] != 0xEB || data[1] != 0xBA || data[2] != 0x3F) {
            break;
        }
        switch (data[5]) {
            case 0xA0:  // EB BA 3F 07 P0 A0 P1 P2 P3 SUM
            {
                if (len != HB_COM_RESP_A0_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A0;
                }
                break;
            }

            case 0xA2:  // EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
            {
                if (len != data[3]) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A2;
                }
                break;
            }

            case 0xA3: {
                if (len != HB_COM_RESP_A3_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A3;
                }
                break;
            }

            case 0xA4: {
                if (len != HB_COM_RESP_A4_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A4;
                }
                break;
            }

            case 0xA6: {
                if (len != HB_COM_RESP_A6_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A6;
                }
                break;
            }

            case 0xA7: {
                if (len != HB_COM_RESP_A7_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A7;
                }
                break;
            }

            case 0xA8: {
                if (len != HB_COM_RESP_A8_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_A8;
                }
                break;
            }

            case 0xA9: {
                // TODO
                break;
            }

            case 0xAB: {
                if (len != HB_COM_RESP_AB_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_AB;
                }
                break;
            }

            case 0xAD:  // EB BA 3F PN P0 AD P1 DATA P2 SUM
            {
                if (len != data[3]) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_AD;
                }
                break;
            }

            case 0xAF:  // TODO
            {
                break;
            }

            case 0xB1:  // EB BA 3F 08 P0 B1 P1 SUM
            {
                if (len != HB_COM_RESP_B1_LEN) {
                    break;
                }
                if (hb_chksum(data, len)) {
                    copy_data_to_com;
                    return HB_COM_RESP_B1;
                }
                break;
            }
        }
    } while (0);

    // 处理读写主机账号回应
    do {
        if (len >= HB_COM_RESP_WA_LEN &&
            memcmp(HB_COM_RESP_WA_DATA, data, HB_COM_RESP_WA_LEN) == 0) {
            return HB_COM_RESP_WA;
        }

        if (len >= HB_COM_RESP_RA_LEN &&
            memcmp(HB_COM_RESP_RA_HEAD, data, sizeof(HB_COM_RESP_RA_HEAD) - 1) == 0 &&
            hb_chksum(data, len)) {
            copy_data_to_com;
            return HB_COM_RESP_RA;
        }
    } while (0);
    return HB_COM_RESP_INVALID;
}

hb_com_resp_t hb_com_parse_xdata_resp(const ademco_xdata_t* xdata,
                                      hb_com_data_t* cd) {
    if (!xdata) {
        return HB_COM_RESP_INVALID;
    }
    return hb_com_parse_resp(
        (const uint8_t*)ademco_xdata_get_valid_content_addr(xdata),
        ademco_xdata_get_valid_content_len(xdata),
        cd);
}

void hb_com_make_req_a0_get_machine_status(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_A0_DATA, HB_COM_REQ_A0_LEN);
    data->len = HB_COM_REQ_A0_LEN;
}

void hb_com_make_req_a1_get_machine_zones(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_A1_DATA, HB_COM_REQ_A1_LEN);
    data->len = HB_COM_REQ_A1_LEN;
}

void hb_com_make_req_a2_get_more_machine_zones(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_A2_DATA, HB_COM_REQ_A2_LEN);
    data->len = HB_COM_REQ_A2_LEN;
}

void hb_com_make_req_a3_modify_machine_zone(hb_com_data_t* data,
                                            uint8_t zone,
                                            hb_zone_property_t prop,
                                            hb_com_req_a3_p3_t op) {
    memcpy(data->data, HB_COM_REQ_A3_DATA, 5);
    data->data[5] = zone;
    data->data[6] = prop;
    data->data[7] = op;
    data->len = HB_COM_REQ_A3_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_req_a5_get_machine_timer(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_A5_DATA, HB_COM_REQ_A5_LEN);
    data->len = HB_COM_REQ_A5_LEN;
}

void hb_com_make_req_a7_set_machine_timer(hb_com_data_t* data,
                                          hb_machine_timer_t* timer) {
    memcpy(data->data, HB_COM_REQ_A7_HEAD, 5);
    memcpy(data->data + 5, timer, sizeof(*timer));
    data->len = HB_COM_REQ_A7_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_req_aa_modify_machine_zone_lost_config(hb_com_data_t* data,
                                                        uint8_t zone,
                                                        int on) {
    memcpy(data->data, HB_COM_REQ_AA_HEAD, 5);
    data->data[5] = zone;
    data->data[6] = !!on;
    data->len = HB_COM_REQ_AA_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_req_ac_get_machine_zone_lost_config(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_AC_DATA, HB_COM_REQ_AC_LEN);
    data->len = HB_COM_REQ_AC_LEN;
}

void hb_com_make_req_ad_get_more_machine_zone_lost_config(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_AD_DATA, HB_COM_REQ_AD_LEN);
    data->len = HB_COM_REQ_AD_LEN;
}

void hb_com_make_req_ae_set_3sec_machine_status(hb_com_data_t* data,
                                                hb_com_3sec_which_t p1,
                                                hb_com_3sec_status_t status) {
    memcpy(data->data, HB_COM_REQ_AE_HEAD, 5);
    data->data[5] = p1;
    data->data[6] = status;
    data->len = HB_COM_REQ_AE_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_req_b0_get_3sec_machine_status(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_B0_DATA, HB_COM_REQ_B0_LEN);
    data->len = HB_COM_REQ_B0_LEN;
}

void hb_com_make_req_ra_read_acct(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_REQ_RA_DATA, HB_COM_REQ_RA_LEN);
    data->len = HB_COM_REQ_RA_LEN;
}

void hb_com_make_req_wa_write_acct(hb_com_data_t* data, const char* acct) {
    memcpy(data->data, HB_COM_REQ_WA_HEAD, 5);
    ademco_hex_str_to_hilo_array(data->data + 5,
                                 ADEMCO_PACKET_ACCT_MAX_LEN / 2,
                                 acct);
    data->len = HB_COM_REQ_WA_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_resp_a2_iter_init(hb_com_resp_a2_iter_t* iter,
                              const hb_com_data_t* com) {
    if (!iter || !com) return;
    iter->com = com;
    iter->i = 0;
    if (com->len > HB_COM_RESP_A2_MIN_LEN) {
        iter->total = (com->len - HB_COM_RESP_A2_MIN_LEN) / 2;
        iter->p1 = (com->data[com->len - 2] == 0xFF)
                       ? HB_COM_RESP_A2_P1_NOMORE
                       : HB_COM_RESP_A2_P1_MORE;
    } else {
        iter->total = 0;
        iter->p1 = HB_COM_RESP_A2_P1_NOMORE;
    }
}

hb_com_resp_a2_p1_t hb_com_resp_a2_iter_next(hb_com_resp_a2_iter_t* iter,
                                             hb_zone_and_property_t* zp) {
    if (iter->i == iter->total)
        return HB_COM_RESP_A2_P1_NOMORE;
    zp->zone = iter->com->data[6 + iter->i * 2];
    zp->prop = (hb_zone_property_t)iter->com->data[7 + iter->i * 2];
    iter->i++;
    return HB_COM_RESP_A2_P1_MORE;
}

void hb_com_make_resp_a0_machine_status(hb_com_data_t* data,
                                        hb_machine_status_t status,
                                        hb_machine_type_t type) {
    memcpy(data->data, HB_COM_RESP_A0_HEAD, 6);
    data->data[6] = status;
    data->data[7] = type;
    data->len = HB_COM_RESP_A0_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_resp_a2_machine_zones(hb_com_data_t* data,
                                       size_t count,
                                       ademco_zone_t* zones,
                                       hb_zone_property_t* props,
                                       hb_com_resp_a2_p1_t p1) {
    if (count > HB_COM_RESP_A2_MAX_ZONE) {
        return;
    }
    memcpy(data->data, HB_COM_RESP_A2_HEAD, 6);
    data->data[3] = (HB_COM_RESP_A2_MIN_LEN + count * 2) & 0xFF;
    uint8_t* p = data->data + 6;
    for (size_t i = 0; i < count; i++) {
        *p++ = zones[i] & 0xFF;
        *p++ = props[i];
    }
    *p++ = (count > 0) ? (uint8_t)p1 : (uint8_t)HB_COM_RESP_A2_P1_NOMORE;
    data->len = (++p - data->data) & 0xFF;
    hb_sum(data->data, data->len);
}

void hb_com_make_resp_a3_waiting_sensor_signal(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_RESP_A3_DATA, HB_COM_RESP_A3_LEN);
    data->len = HB_COM_RESP_A3_LEN;
}

void hb_com_make_resp_a4_modify_zone_result(hb_com_data_t* data,
                                            ademco_zone_t zone,
                                            hb_zone_property_t prop,
                                            hb_com_resp_a4_p3_t p3) {
    memcpy(data->data, HB_COM_RESP_A4_HEAD, 6);
    data->data[6] = zone & 0xFF;
    data->data[7] = prop;
    data->data[8] = p3;
    data->len = HB_COM_RESP_A4_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_resp_a7_to_timer(hb_com_data_t* data, hb_machine_timer_t* timer) {
    memcpy(timer->data, &data->data[2], 8);
}

void hb_com_make_resp_a6_machine_timer(hb_com_data_t* data,
                                       hb_machine_timer_t* timer) {
    memcpy(data->data, HB_COM_RESP_A6_HEAD, 6);
    memcpy(data->data + 6, timer->data, sizeof(*timer));
    data->len = HB_COM_RESP_A6_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_resp_a7_set_machine_timer_result(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_RESP_A7_DATA, HB_COM_RESP_A7_LEN);
    data->len = HB_COM_RESP_A7_LEN;
}

void hb_com_make_resp_a8_reject(hb_com_data_t* data) {
    memcpy(data->data, HB_COM_RESP_A8_DATA, HB_COM_RESP_A8_LEN);
    data->len = HB_COM_RESP_A8_LEN;
}

void hb_com_make_resp_ab_modify_machine_zone_lost_config_result(hb_com_data_t* data,
                                                                ademco_zone_t zone,
                                                                hb_com_resp_ab_p2_t p2) {
    memcpy(data->data, HB_COM_RESP_AB_HEAD, 6);
    data->data[6] = zone & 0xFF;
    data->data[7] = p2;
    data->len = HB_COM_RESP_AB_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_resp_ad_machine_zone_lost_config(hb_com_data_t* data,
                                                  hb_com_resp_ad_p1_t p1,
                                                  size_t count,
                                                  ademco_zone_t* zones,
                                                  hb_com_resp_ad_p2_t p2) {
    if (count > HB_COM_RESP_AD_MAX_ZONE) {
        return;
    }
    memcpy(data->data, HB_COM_RESP_AD_HEAD, 6);
    data->data[3] = (HB_COM_RESP_AD_MIN_LEN +
                     (p1 == HB_COM_RESP_AD_P1_SINGLE ? count : count * 2)) &
                    0xFF;
    data->data[6] = p1;
    uint8_t* p = data->data + 7;
    for (size_t i = 0; i < count; i++) {
        if (p1 == HB_COM_RESP_AD_P1_SINGLE) {
            *p++ = zones[i] & 0xFF;
        } else {
            *p++ = zones[i] >> 8 & 0xFF;
            *p++ = zones[i] & 0xFF;
        }
    }
    *p++ = (count > 0) ? (uint8_t)p2 : (uint8_t)HB_COM_RESP_AD_P2_NOMORE;
    data->len = (++p - data->data) & 0xFF;
    hb_sum(data->data, data->len);
}

void hb_com_resp_ad_iter_init(hb_com_resp_ad_iter_t* iter, const hb_com_data_t* com) {
    if (!iter || !com) return;
    iter->com = com;
    iter->i = 0;
    if (com->len > HB_COM_RESP_AD_MIN_LEN) {
        iter->p1 = com->data[6];
        iter->total = (com->len - HB_COM_RESP_AD_MIN_LEN);
        if (iter->p1 == HB_COM_RESP_AD_P1_DOUBLE) {
            iter->total >>= 1;
        }
        iter->p2 = (com->data[com->len - 2] == 0xFF)
                       ? HB_COM_RESP_AD_P2_NOMORE
                       : HB_COM_RESP_AD_P2_MORE;
    } else {
        iter->total = 0;
        iter->p1 = HB_COM_RESP_AD_P1_SINGLE;
        iter->p2 = HB_COM_RESP_AD_P2_NOMORE;
    }
}

hb_com_resp_ad_p2_t hb_com_resp_ad_iter_next(hb_com_resp_ad_iter_t* iter, ademco_zone_t* zone) {
    if (iter->i == iter->total)
        return HB_COM_RESP_AD_P2_NOMORE;
    if (iter->p1 == HB_COM_RESP_AD_P1_SINGLE) {
        *zone = iter->com->data[7 + iter->i];
    } else {
        *zone = iter->com->data[7 + iter->i * 2] << 8;
        *zone += iter->com->data[7 + iter->i * 2 + 1];
    }
    iter->i++;
    return HB_COM_RESP_AD_P2_MORE;
}

void hb_com_make_resp_af_set_3sec_machine_status_result(hb_com_data_t* data,
                                                        hb_com_3sec_which_t p1,
                                                        hb_com_3sec_status_t status) {
    memcpy(data->data, HB_COM_RESP_AF_HEAD, 6);
    data->data[6] = p1;
    data->data[7] = status;
    data->len = HB_COM_RESP_AF_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_make_resp_b1_3sec_machine_status(hb_com_data_t* data,
                                             hb_machine_status_t statusMachine,
                                             hb_machine_status_t statusSec1,
                                             hb_machine_status_t statusSec2,
                                             hb_machine_status_t statusSec3) {
    memcpy(data->data, HB_COM_RESP_B1_HEAD, 6);
    data->data[6] = ((uint8_t)statusMachine << 6) |
                    ((uint8_t)statusSec1 << 4) |
                    ((uint8_t)statusSec2 << 2) |
                    ((uint8_t)statusSec3);
    data->len = HB_COM_RESP_B1_LEN;
    hb_sum(data->data, data->len);
}

void hb_com_data_to_ademco_xdata(const hb_com_data_t* const data,
                                 ademco_xdata_t* xdata,
                                 ademco_xdata_length_format_t xlf,
                                 ademco_xdata_transform_t xtr) {
    ademco_make_xdata(xdata, xlf, xtr, (const char*)data->data, data->len);
}
