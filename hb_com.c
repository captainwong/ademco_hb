#include "hb_com.h"

#include <string.h>

int hbGetAvailableZoneProperties(HbMachineType type,
                                 HbZoneProperty props[HZP_COUNT]) {
    switch (type) {
        case HMT_WIFI:
            break;
        case HMT_CAMERA:
            break;
        case HMT_GPRS_IOT:
        case HMT_GPRS:
        case HMT_WIFI2: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_NETMOD: {
            HbZoneProperty hzps[] = {
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
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_LCD: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_SUB_MACHINE,
                HZP_REMOTE_CONTROL, HZP_BUGLAR_HALF, HZP_SHIELD,
                HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_WIRED: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_TRUE_COLOR: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_THREE_SECTION: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_IOT: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR, HZP_EMERGENCY, HZP_FIRE,
                HZP_DURESS, HZP_GAS, HZP_WATER, HZP_REMOTE_CONTROL,
                HZP_SHIELD, HZP_DOOR_RING, HZP_BYPASS};
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_GPRS_PHONE: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_REMOTE_CONTROL,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        case HMT_NB: {
            HbZoneProperty hzps[] = {
                HZP_BUGLAR,
                HZP_EMERGENCY,
                HZP_FIRE,
                HZP_DURESS,
                HZP_GAS,
                HZP_WATER,
                HZP_DOOR_RING,
            };
            memcpy(props, hzps, sizeof(hzps));
            return sizeof(hzps) / sizeof(HbZoneProperty);
        }
        default:
            return 0;
    }
    return 0;
}

uint16_t hbZoneMax(HbMachineType type) {
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

int hbIsValidZone(HbMachineType type, uint16_t zone) {
    return ADEMCO_ZONE_FOR_MACHINE_SELF <= zone &&
           zone <= hbZoneMax(type);
}

int hbIsValidZoneStrict(HbMachineType type, uint16_t zone) {
    return ADEMCO_ZONE_MIN <= zone && zone <= hbZoneMax(type);
}

int hbMachineIsSelling(HbMachineType type) {
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

int hbMachineCanArm(HbMachineType type) {
    return hbMachineIsSelling(type) && (type != HMT_NB);
}

int hbMachineCanDisarm(HbMachineType type) {
    return hbMachineIsSelling(type) && (type != HMT_NB);
}

int hbMachineCanEnterSettings(HbMachineType type) {
    return hbMachineIsSelling(type);
}

int hbMachineCanHalfArm(HbMachineType type) {
    return type == HMT_NETMOD || type == HMT_LCD;
}

int hbMachineCanReportSignalStrength(HbMachineType type) {
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

int hbMachineCanReportBySMS(HbMachineType type) {
    return type == HMT_GPRS ||
           type == HMT_LCD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION ||
           type == HMT_GPRS_PHONE;
}

int hbMachineHasWiredZone(HbMachineType type) {
    return type == HMT_NETMOD ||
           type == HMT_TRUE_COLOR ||
           type == HMT_THREE_SECTION;
}

AdemcoZone hbWiredZoneMin(HbMachineType type) {
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

AdemcoZone hbWiredZoneMax(HbMachineType type) {
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

int hbMachineCanDirectlyWriteZone(HbMachineType type) {
    return type == HMT_NETMOD;
}

int hbMachineCanLinkSubMachine(HbMachineType type) {
    return type == HMT_NETMOD;
}

int hbZonePropCanReportLost(HbZoneProperty zp) {
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

void hbInitMachineTimer(HbMachineTimer* timer) {
    memset(timer->data, 25, sizeof(timer));
}

int hbIsValidTimePoint(HbMachineTimePoint* tp) {
    return tp &&
           tp->hour < 24 &&
           tp->minute < 60;
}

void hbTimePointToGreenwich(HbMachineTimePoint* tp) {
    if (tp && tp->hour != 25)
        tp->hour = (tp->hour + 16) % 24;
}

void hbTimePointFromGreenwich(HbMachineTimePoint* tp) {
    if (tp && tp->hour != 25)
        tp->hour = (tp->hour + 8) % 24;
}

int hbIsValidTimer(HbTimer* timer) {
    return timer &&
           hbIsValidTimePoint(&timer->armAt) &&
           hbIsValidTimePoint(&timer->disarmAt);
}

int hbIsValidMachineTimer(HbMachineTimer* timer) {
    return timer &&
           hbIsValidTimer(&timer->timer[0]) &&
           hbIsValidTimer(&timer->timer[1]);
}

void hbMachineTimerToGreenwich(HbMachineTimer* timer) {
    hbTimePointToGreenwich(&timer->timer[0].armAt);
    hbTimePointToGreenwich(&timer->timer[0].disarmAt);
    hbTimePointToGreenwich(&timer->timer[1].armAt);
    hbTimePointToGreenwich(&timer->timer[1].disarmAt);
}

void hbMachineTimerFromGreenwich(HbMachineTimer* timer) {
    hbTimePointFromGreenwich(&timer->timer[0].armAt);
    hbTimePointFromGreenwich(&timer->timer[0].disarmAt);
    hbTimePointFromGreenwich(&timer->timer[1].armAt);
    hbTimePointFromGreenwich(&timer->timer[1].disarmAt);
}

AdemcoEvent hbMachineStatusToAdemcoEvent(HbMachineStatus status) {
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

HbMachineStatus hbMachineStatusFromAdemcoEvent(AdemcoEvent ademcoEvent) {
    switch (ademcoEvent) {
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

AdemcoEvent hbMachineTypeToAdemcoEvent(HbMachineType type) {
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

HbMachineType hbMachineTypeFromAdemcoEvent(AdemcoEvent ademcoEvent) {
    switch (ademcoEvent) {
#define XX(hmt, ae) \
    case ae:        \
        return hmt;

        HMT_EVENT_MAP(XX)
#undef XX

        default:
            return HMT_INVALID;
    }
}

AdemcoEvent hbZonePropertyToAdemcoEvent(HbZoneProperty zp) {
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

const char* hbMachineStatusToString(HbMachineStatus status) {
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

const char* hbMachineTypeToString(HbMachineType type) {
    switch (type) {
#define XX(name, value, str) \
    case HMT_##name:         \
        return #name;
        HB_MACHINE_TYPE_MAP(XX)
#undef XX

        default:
            return "Unknown HbMachineType";
    }
}

const char* hbZonePropertyToString(HbZoneProperty zp) {
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
const char* hbMachineStatusToStringChinese(HbMachineStatus status) {
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

const char* hbMachineTypeToStringChinese(HbMachineType type) {
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

const char* hbZonePropertyToStringChinese(HbZoneProperty zp) {
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
#endif

const char* hbGetZoneFormatString(HbMachineType type) {
    uint16_t zone = hbZoneMax(type);
    if (10 < zone && zone < 100)
        return "%02d";
    else if (zone < 1000)
        return "%03d";
    else
        return "%d";
}

void hbSum(uint8_t* data, size_t len) {
    uint8_t sum = 0;
    uint8_t* p = data;
    while (p != data + len - 1)
        sum += *p++;
    *p = sum;
}

int hbCheckSum(const uint8_t* data, size_t len) {
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

HbComRequestType hbComParseRequest(const uint8_t* data,
                                   size_t len,
                                   HbComData* cd) {
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
                        if (len != HbComReq_A0_len) {
                            break;
                        }
                        if (memcmp(HbComReq_A0_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_A0;
                    }
                    case 0xA1:  // EB AB 3F A1 76
                    {
                        if (len != HbComReq_A1_len) {
                            break;
                        }
                        if (memcmp(HbComReq_A1_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_A1;
                    }
                    case 0xA2:  // EB AB 3F A2 77
                    {
                        if (len != HbComReq_A2_len) {
                            break;
                        }
                        if (memcmp(HbComReq_A2_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_A2;
                    }
                    case 0xA5:  // EB AB 3F A5 7A
                    {
                        if (len != HbComReq_A5_len) {
                            break;
                        }
                        if (memcmp(HbComReq_A5_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_A5;
                    }
                    case 0xAC:  // EB AB 3F AC 81
                    {
                        if (len != HbComReq_AC_len) {
                            break;
                        }
                        if (memcmp(HbComReq_AC_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_AC;
                    }
                    case 0xAD:  // EB AB 3F AD 82
                    {
                        if (len != HbComReq_AD_len) {
                            break;
                        }
                        if (memcmp(HbComReq_AD_data, data, len) != 0) {
                            break;
                        }
                        copy_data_to_com;
                        return HbComReq_AD;
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
                if (len == HbComReq_READ_acct_len &&
                    memcmp(data, HbComReq_READ_acct_data, len) == 0) {
                    copy_data_to_com;
                    return HbComReq_RD_acct;
                }
                break;
            }
            case 0xCB: {
                if (data[2] != 0x3F) {
                    break;
                }

                if (data[3] == 0x09 &&
                    data[4] == 0xA3 &&
                    len == HbComReq_A3_len) {
                    // EB CB 3F 09 A3 P1 P2 P3 SUM
                    if (hbCheckSum(data, len)) {
                        copy_data_to_com;
                        return HbComReq_A3;
                    }
                } else if (data[3] == 0x0F &&
                           data[4] == 0x4D &&
                           len == HbComReq_WRITE_acct_len) {
                    if (hbCheckSum(data, len)) {
                        copy_data_to_com;
                        return HbComReq_WR_acct;
                    }
                } else if (data[3] == 0x0E &&
                           data[4] == 0xA7 &&
                           len == HbComReq_A7_len) {
                    // EB CB 3F 0E A7 H1 M1 H2 M2 H3 M3 H4 M4 SUM
                    if (hbCheckSum(data, len)) {
                        copy_data_to_com;
                        return HbComReq_A7;
                    }
                }

                /*else if (data[3] == 0x08 && data[4] == 0xA9 && len == A9_len) {
                        A7 req; memcpy(req.data, data, req.len); sum(req);
                        if (data[len - 1] == req.data[len - 1]) { return A7; }
                }*/

                else if (data[3] == 0x08 &&
                         data[4] == 0xAA &&
                         len == HbComReq_AA_len) {
                    // EB CB 3F 08 AA P1 P2 SUM
                    if (hbCheckSum(data, len)) {
                        copy_data_to_com;
                        return HbComReq_AA;
                    }
                }

                /*else if (data[3] == 0x08 && data[4] == 0xAE && len == AE_len) {
                        AA req; memcpy(req.data, data, req.len); sum(req);
                        if (data[len - 1] == req.data[len - 1]) { return AA; }
                }*/

                else if (data[3] == 0x06 && data[4] == 0xB0 &&
                         len == HbComReq_B0_len &&
                         memcmp(HbComReq_B0_data, data, len) == 0) {
                    // EB CB 3F 06 B0 AB
                    copy_data_to_com;
                    return HbComReq_B0;
                }
            }
        }
    } while (0);
    return HbComReq_Invalid;
}

HbComRequestType hbComParseXDataRequest(const AdemcoXDataSegment* xdata,
                                        HbComData* cd) {
    if (!xdata) {
        return HbComReq_Invalid;
    }
    return hbComParseRequest(
        (const uint8_t*)ademcoXDataGetValidContentAddr(xdata),
        ademcoXDataGetValidContentLen(xdata),
        cd);
}

HbComResponseType hbComParseResponse(const uint8_t* data,
                                     size_t len,
                                     HbComData* cd) {
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
                if (len != HbComResp_A0_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A0;
                }
                break;
            }

            case 0xA2:  // EB BA 3F PN P0 A2 [Z, P]xN P1 SUM
            {
                if (len != data[3]) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A2;
                }
                break;
            }

            case 0xA3: {
                if (len != HbComResp_A3_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A3;
                }
                break;
            }

            case 0xA4: {
                if (len != HbComResp_A4_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A4;
                }
                break;
            }

            case 0xA6: {
                if (len != HbComResp_A6_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A6;
                }
                break;
            }

            case 0xA7: {
                if (len != HbComResp_A7_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A7;
                }
                break;
            }

            case 0xA8: {
                if (len != HbComResp_A8_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_A8;
                }
                break;
            }

            case 0xA9: {
                // TODO
                break;
            }

            case 0xAB: {
                if (len != HbComResp_AB_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_AB;
                }
                break;
            }

            case 0xAD:  // EB BA 3F PN P0 AD P1 DATA P2 SUM
            {
                if (len != data[3]) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_AD;
                }
                break;
            }

            case 0xAF:  // TODO
            {
                break;
            }

            case 0xB1:  // EB BA 3F 08 P0 B1 P1 SUM
            {
                if (len != HbComResp_B1_len) {
                    break;
                }
                if (hbCheckSum(data, len)) {
                    copy_data_to_com;
                    return HbComResp_B1;
                }
                break;
            }
        }
    } while (0);

    // 处理读写主机账号回应
    do {
        if (len >= HbComResp_WRITE_acct_len &&
            memcmp(HbComResp_WRITE_acct_data, data, HbComResp_WRITE_acct_len) == 0) {
            return HbComResp_WA;
        }

        if (len >= HbComResp_READ_acct_len &&
            memcmp(HbComResp_READ_acct_head, data, sizeof(HbComResp_READ_acct_head) - 1) == 0 &&
            hbCheckSum(data, len)) {
            copy_data_to_com;
            return HbComResp_RA;
        }
    } while (0);
    return HbComResp_Invalid;
}

HbComResponseType hbComParseXDataResponse(const AdemcoXDataSegment* xdata,
                                          HbComData* cd) {
    if (!xdata) {
        return HbComResp_Invalid;
    }
    return hbComParseResponse(
        (const uint8_t*)ademcoXDataGetValidContentAddr(xdata),
        ademcoXDataGetValidContentLen(xdata),
        cd);
}

void hbComMakeReqA0_getMachineStatus(HbComData* data) {
    memcpy(data->data, HbComReq_A0_data, HbComReq_A0_len);
    data->len = HbComReq_A0_len;
}

void hbComMakeReqA1_getMachineZones(HbComData* data) {
    memcpy(data->data, HbComReq_A1_data, HbComReq_A1_len);
    data->len = HbComReq_A1_len;
}

void hbComMakeReqA2_getMoreMachineZones(HbComData* data) {
    memcpy(data->data, HbComReq_A2_data, HbComReq_A2_len);
    data->len = HbComReq_A2_len;
}

void hbComMakeReqA3_modifyMachineZone(HbComData* data,
                                      uint8_t zone,
                                      HbZoneProperty prop,
                                      HbComReq_A3_op op) {
    memcpy(data->data, HbComReq_A3_head, 5);
    data->data[5] = zone;
    data->data[6] = prop;
    data->data[7] = op;
    data->len = HbComReq_A3_len;
    hbSum(data->data, data->len);
}

void hbComMakeReqA5_getMachineTimer(HbComData* data) {
    memcpy(data->data, HbComReq_A5_data, HbComReq_A5_len);
    data->len = HbComReq_A5_len;
}

void hbComMakeReqA7_setMachineTimer(HbComData* data,
                                    HbMachineTimer* timer) {
    memcpy(data->data, HbComReq_A7_head, 5);
    memcpy(data->data + 5, timer, sizeof(*timer));
    data->len = HbComReq_A7_len;
    hbSum(data->data, data->len);
}

void hbComMakeReqAA_modifyMachineZoneLostConfig(HbComData* data,
                                                uint8_t zone,
                                                int on) {
    memcpy(data->data, HbComReq_AA_head, 5);
    data->data[5] = zone;
    data->data[6] = !!on;
    data->len = HbComReq_AA_len;
    hbSum(data->data, data->len);
}

void hbComMakeReqAC_getMachineZoneLostConfig(HbComData* data) {
    memcpy(data->data, HbComReq_AC_data, HbComReq_AC_len);
    data->len = HbComReq_AC_len;
}

void hbComMakeReqAD_getMoreMachineZoneLostConfig(HbComData* data) {
    memcpy(data->data, HbComReq_AD_data, HbComReq_AD_len);
    data->len = HbComReq_AD_len;
}

void hbComMakeReqAE_set3SectionMachineStatus(HbComData* data,
                                             HbCom_3sec_which p1,
                                             HbCom_3sec_status status) {
    memcpy(data->data, HbComReq_AE_head, 5);
    data->data[5] = p1;
    data->data[6] = status;
    data->len = HbComReq_AE_len;
    hbSum(data->data, data->len);
}

void hbComMakeReqB0_get3SectionMachineStatus(HbComData* data) {
    memcpy(data->data, HbComReq_B0_data, HbComReq_B0_len);
    data->len = HbComReq_B0_len;
}

void hbComMakeReqRead_acct(HbComData* data) {
    memcpy(data->data, HbComReq_READ_acct_data, HbComReq_READ_acct_len);
    data->len = HbComReq_READ_acct_len;
}

void hbComMakeReqWrite_acct(HbComData* data, const char* acct) {
    memcpy(data->data, HbComReq_WRITE_acct_head, 5);
    ademcoDecStrToHiLoArray2(data->data + 5, ADEMCO_PACKET_ACCT_MAX_LEN / 2, acct);
    data->len = HbComReq_WRITE_acct_len;
    hbSum(data->data, data->len);
}

void hbComResp_A2_Iter_init(HbComResp_A2_Iter* iter,
                            const HbComData* com) {
    if (!iter || !com) return;
    memcpy(&iter->com, com, sizeof(HbComData));
    iter->i = 0;
    if (com->len > HbComResp_A2_len_min) {
        iter->total = (com->len - HbComResp_A2_len_min) / 2;
        iter->p1 = (com->data[com->len - 2] == 0xFF)
                       ? HbComResp_A2_p1_nomore
                       : HbComResp_A2_p1_more;
    } else {
        iter->total = 0;
        iter->p1 = HbComResp_A2_p1_nomore;
    }
}

HbComResp_A2_p1 hbComResp_A2_Iter_next(HbComResp_A2_Iter* iter,
                                       HbZoneAndProperty* zp) {
    if (iter->i == iter->total)
        return HbComResp_A2_p1_nomore;
    zp->zone = iter->com.data[6 + iter->i * 2];
    zp->prop = (HbZoneProperty)iter->com.data[7 + iter->i * 2];
    iter->i++;
    return HbComResp_A2_p1_more;
}

void hbComMakeRespA0_getMachineStatus(HbComData* data,
                                      HbMachineStatus status,
                                      HbMachineType type) {
    memcpy(data->data, HbComResp_A0_head, 6);
    data->data[6] = status;
    data->data[7] = type;
    data->len = HbComResp_A0_len;
    hbSum(data->data, data->len);
}

void hbComMakeRespA2_getMachineZones(HbComData* data,
                                     size_t count,
                                     AdemcoZone* zones,
                                     HbZoneProperty* props,
                                     HbComResp_A2_p1 p1) {
    if (count > HbComResp_A2_max_zone) {
        return;
    }
    memcpy(data->data, HbComResp_A2_head, 6);
    data->data[3] = (HbComResp_A2_len_min + count * 2) & 0xFF;
    uint8_t* p = data->data + 6;
    for (size_t i = 0; i < count; i++) {
        *p++ = zones[i] & 0xFF;
        *p++ = props[i];
    }
    *p++ = (count > 0) ? (uint8_t)p1 : (uint8_t)HbComResp_A2_p1_nomore;
    data->len = (++p - data->data) & 0xFF;
    hbSum(data->data, data->len);
}

void hbComMakeRespA3_waitingSignal(HbComData* data) {
    memcpy(data->data, HbComResp_A3_data, HbComResp_A3_len);
    data->len = HbComResp_A3_len;
}

void hbComMakeRespA4_modifyMachineZone(HbComData* data,
                                       AdemcoZone zone,
                                       HbZoneProperty prop,
                                       HbComResp_A4_p3 p3) {
    memcpy(data->data, HbComResp_A4_head, 6);
    data->data[6] = zone & 0xFF;
    data->data[7] = prop;
    data->data[8] = p3;
    data->len = HbComResp_A4_len;
    hbSum(data->data, data->len);
}

void hbComMakeRespA6_getMachineTimer(HbComData* data,
                                     HbMachineTimer* timer) {
    memcpy(data->data, HbComResp_A6_head, 6);
    memcpy(data->data + 6, timer->data, sizeof(*timer));
    data->len = HbComResp_A6_len;
    hbSum(data->data, data->len);
}

void hbComMakeRespA7_setMachineTimer(HbComData* data) {
    memcpy(data->data, HbComResp_A7_data, HbComResp_A7_len);
    data->len = HbComResp_A7_len;
}

void hbComMakeRespA8_reject(HbComData* data) {
    memcpy(data->data, HbComResp_A8_data, HbComResp_A8_len);
    data->len = HbComResp_A8_len;
}

void hbComMakeRespAB_modifyMachineZoneLostConfig(HbComData* data,
                                                 AdemcoZone zone,
                                                 HbComResp_AB_p2 p2) {
    memcpy(data->data, HbComResp_AB_head, 6);
    data->data[6] = zone & 0xFF;
    data->data[7] = p2;
    data->len = HbComResp_AB_len;
    hbSum(data->data, data->len);
}

void hbComMakeRespAD_getMachineZoneLostConfig(HbComData* data,
                                              HbComResp_AD_p1 p1,
                                              size_t count,
                                              AdemcoZone* zones,
                                              HbComResp_AD_p2 p2) {
    if (count > HbComResp_AD_max_zone) {
        return;
    }
    memcpy(data->data, HbComResp_AD_head, 6);
    data->data[3] = (HbComResp_AD_len_min +
                     (p1 == HbComResp_AD_p1_single ? count : count * 2)) &
                    0xFF;
    data->data[6] = p1;
    uint8_t* p = data->data + 7;
    for (size_t i = 0; i < count; i++) {
        if (p1 == HbComResp_AD_p1_single) {
            *p++ = zones[i] & 0xFF;
        } else {
            *p++ = zones[i] >> 8 & 0xFF;
            *p++ = zones[i] & 0xFF;
        }
    }
    *p++ = (count > 0) ? (uint8_t)p2 : (uint8_t)HbComResp_AD_p2_nomore;
    data->len = (++p - data->data) & 0xFF;
    hbSum(data->data, data->len);
}

void hbComMakeRespAF_set3SectionMachineStatus(HbComData* data,
                                              HbCom_3sec_which p1,
                                              HbCom_3sec_status status) {
    memcpy(data->data, HbComResp_AF_head, 6);
    data->data[6] = p1;
    data->data[7] = status;
    data->len = HbComResp_AF_len;
    hbSum(data->data, data->len);
}

void hbComMakeRespB1_get3SectionMachineStatus(HbComData* data,
                                              HbMachineStatus statusMachine,
                                              HbMachineStatus statusSec1,
                                              HbMachineStatus statusSec2,
                                              HbMachineStatus statusSec3) {
    memcpy(data->data, HbComResp_B1_head, 6);
    data->data[6] = ((uint8_t)statusMachine << 6) |
                    ((uint8_t)statusSec1 << 4) |
                    ((uint8_t)statusSec2 << 2) |
                    ((uint8_t)statusSec3);
    data->len = HbComResp_B1_len;
    hbSum(data->data, data->len);
}

void hbComDataToAdemcoXData(const HbComData* const data,
                            AdemcoXDataSegment* xdata,
                            AdemcoXDataLengthFormat xlf,
                            AdemcoXDataTransform xtr) {
    ademcoMakeXData(xdata, xlf, xtr, (const char*)data->data, data->len);
}
