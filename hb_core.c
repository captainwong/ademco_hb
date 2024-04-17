#include "hb_core.h"

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
