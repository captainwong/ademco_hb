#include "hb_core_ademco.h"

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

        case EVENT_ARM_STAY_1456:
            return HMS_ARM_STAY;
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

ademco_event_t hb_rf_code_to_ademco_event(uint8_t rf_code) {
    switch (rf_code) {
        case HB_RF_CODE_ARM:
            return EVENT_ARM_AWAY;
        case HB_RF_CODE_DISARM:
            return EVENT_DISARM;
        case HB_RF_CODE_ARM_STAY_OR_CLEAR_ALARM:
            return EVENT_ARM_STAY;

        case HB_RF_CODE_EMERGENCY:
            return EVENT_EMERGENCY;
        case HB_RF_CODE_BURGLAR:
            return EVENT_BURGLAR;
        case HB_RF_CODE_FIRE:
            return EVENT_FIRE;
        case HB_RF_CODE_DURESS:
            return EVENT_DURESS;
        case HB_RF_CODE_GAS:
            return EVENT_GAS;
        case HB_RF_CODE_WATER:
            return EVENT_WATER;
        case HB_RF_CODE_TAMPER:
            return EVENT_TAMPER;
        case HB_RF_CODE_S_BATTERY_LOW:
        case HB_RF_CODE_R_BATTERY_LOW:
            return EVENT_LOW_BATTERY;
        case HB_RF_CODE_S_BATTERY_BROKE:
        case HB_RF_CODE_R_BATTERY_BROKE:
            return EVENT_BAD_BATTERY;
        case HB_RF_CODE_BATTERY_RECOVER:
            return EVENT_LOW_BATTERY_RECOVER;
        case HB_RF_CODE_SOLAR_DISTURB:
            return EVENT_SOLAR_DISTURB;
        case HB_RF_CODE_SOLAR_RECOVER:
            return EVENT_SOLAR_DISTURB_RECOVER;
        case HB_RF_CODE_LONGTIME_DISCONN:
            return EVENT_DISCONNECT;
        case HB_RF_CODE_LONGTIME_RECOVER:
            return EVENT_DISCONNECT_RECOVER;
        case HB_RF_CODE_DOOR_RING:
            return EVENT_DOOR_RING;
        case HB_RF_CODE_SM_EXCEPTION:
            return EVENT_SUB_MACHINE_SENSOR_EXCEPTION;
        case HB_RF_CODE_SM_EXCEPT_RECOVER:
            return EVENT_SUB_MACHINE_SENSOR_RECOVER;
        case HB_RF_CODE_SM_POWER_EXCEPT:
            return EVENT_SUB_MACHINE_POWER_EXCEPTION;
        case HB_RF_CODE_SM_POWER_RECOVER:
            return EVENT_SUB_MACHINE_POWER_RECOVER;
        case HB_RF_CODE_AC_BROKE:
            return EVENT_AC_BROKE;
        case HB_RF_CODE_AC_RECOVER:
            return EVENT_AC_RECOVER;

        case HB_RF_CODE_STARTUP:
        case HB_RF_CODE_SHUTDOWN:
        case HB_RF_CODE_SENDER_REBOOT:
        default:
            return EVENT_INVALID_EVENT;
    }
}
