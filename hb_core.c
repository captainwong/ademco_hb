#include "hb_core.h"

#include <string.h>

uint8_t hb_get_available_zone_properties_by_type(hb_machine_type_t type,
                                                 hb_zone_property_t props[HZP_COUNT]) {
    switch (type) {
        case HMT_WIFI:
            break;
        case HMT_CAMERA:
            break;
        case HMT_GPRS_IOT:
        case HMT_GPRS:
        case HMT_WIFI2: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR,
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
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_SUB_MACHINE,
                HZP_REMOTE_CONTROL,
                HZP_BURGLAR_HALF,
                HZP_BYPASS,
                HZP_DOOR_RING,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_LCD: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE,
                HZP_REMOTE_CONTROL, HZP_BURGLAR_HALF,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_WIRED: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR,
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
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_THREE_SECTION: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_IOT: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(hb_zone_property_t);
        }
        case HMT_GPRS_PHONE: {
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR,
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
            hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER hzps[] = {
                HZP_BURGLAR,
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

bool hb_machine_supports_zone_property(hb_machine_type_t type, hb_zone_property_t prop) {
    hb_zone_property_t ADEMCO_BUF_MEMORY_MODIFIER props[HZP_COUNT];
    uint8_t count = hb_get_available_zone_properties_by_type(type, props), i;
    for (i = 0; i < count; i++) {
        if (props[i] == prop) {
            return true;
        }
    }
    return false;
}

uint16_t hb_get_max_zone_by_type(hb_machine_type_t type) {
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

bool hb_is_valid_zone_by_type(hb_machine_type_t type, uint16_t zone) {
    return 0 <= zone &&
           zone <= hb_get_max_zone_by_type(type);
}

bool hb_is_valid_zone_by_type_strict(hb_machine_type_t type, uint16_t zone) {
    return 1 <= zone && zone <= hb_get_max_zone_by_type(type);
}

bool hb_is_machine_on_sale(hb_machine_type_t type) {
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

bool hb_machine_can_arm_away(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type) && (type != HMT_NB);
}

bool hb_machine_can_disarm(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type) && (type != HMT_NB);
}

bool hb_machine_can_config(hb_machine_type_t type) {
    return hb_is_machine_on_sale(type);
}

bool hb_machine_can_arm_stay(hb_machine_type_t type) {
    return type == HMT_NETMOD || type == HMT_LCD;
}

bool hb_machine_can_report_signal_strength(hb_machine_type_t type) {
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

bool hb_machine_can_report_by_sms(hb_machine_type_t type) {
    return type == HMT_GPRS ||
           type == HMT_LCD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION ||
           type == HMT_GPRS_PHONE;
}

bool hb_machine_has_wired_zones(hb_machine_type_t type) {
    return type == HMT_NETMOD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION;
}

uint16_t hb_wired_zone_min(hb_machine_type_t type) {
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

uint16_t hb_wired_zone_max(hb_machine_type_t type) {
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

bool hb_machine_can_write_zone(hb_machine_type_t type) {
    return type == HMT_NETMOD;
}

bool hb_machine_can_link_sub_machine(hb_machine_type_t type) {
    return type == HMT_NETMOD;
}

bool hb_zone_can_report_lost(hb_zone_property_t zp) {
    switch (zp) {
        case HZP_BURGLAR:
        case HZP_EMERGENCY:
        case HZP_DURESS:
        case HZP_SUB_MACHINE:
        case HZP_BURGLAR_HALF:
        case HZP_BYPASS:
            return 1;
        default:
            return 0;
    }
}

void hb_machine_timer_one_init(hb_machine_timer_one_t* timer) {
    memset(timer, 0xFF, sizeof(*timer));
}

void hb_machine_timer_init(hb_machine_timer_t* timer) {
    memset(timer->dat, 0xFF, sizeof(*timer));
}

uint8_t hb_is_valid_time_point(const hb_machine_time_point_t* tp) {
    return tp &&
           tp->hour < 24 &&
           tp->minute < 60;
}

uint8_t hb_time_point_equal(const hb_machine_time_point_t* tp1, const hb_machine_time_point_t* tp2) {
    return tp1->hour == tp2->hour && tp1->minute == tp2->minute;
}

uint8_t hb_is_valid_timer_one(const hb_machine_timer_one_t* timer) {
    return timer &&
#if HB_ENABLE_TIMER_ARM_STAY
           (timer->arm_what == HB_TIMER_ARM_WHAT_ARM_AWAY || timer->arm_what == HB_TIMER_ARM_WHAT_ARM_STAY) &&
#endif
           hb_is_valid_time_point(&timer->arm_at) &&
           hb_is_valid_time_point(&timer->disarm_at) &&
           !hb_time_point_equal(&timer->arm_at, &timer->disarm_at);
}

uint8_t hb_is_valid_machine_timer(const hb_machine_timer_t* timer) {
    return timer &&
           hb_is_valid_timer_one(&timer->timer[0]) &&
           hb_is_valid_timer_one(&timer->timer[1]);
}

#if HB_ENABLE_TO_STRING
const char* hb_machine_status_to_string(hb_machine_status_t status) {
    switch (status) {
#define XX(value, name, str) \
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
#define XX(value, name, str) \
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
#define XX(value, name, str) \
    case HZP_##name:         \
        return #name;
        HB_ZONE_PROPERTY_MAP(XX)
#undef XX

        default:
            return "InvalidZoneProperty";
    }
}

#if HB_ENABLE_CHINESE
const char* hb_machine_status_to_string_chinese(hb_machine_status_t status) {
    switch (status) {
#define XX(value, name, str) \
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
#define XX(value, name, str) \
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
#define XX(value, name, str) \
    case HZP_##name:         \
        return str;
        HB_ZONE_PROPERTY_MAP(XX)
#undef XX

        default:
            return "无效属性";
    }
}
#endif  // HB_ENABLE_CHINESE

const char* hb_get_zone_format_str(hb_machine_type_t type) {
    uint16_t zone = hb_get_max_zone_by_type(type);
    if (10 < zone && zone < 100)
        return "%02d";
    else if (zone < 1000)
        return "%03d";
    else
        return "%d";
}
#endif  // HB_ENABLE_TO_STRING
