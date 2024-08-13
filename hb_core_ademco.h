#ifndef __HB_CORE_ADEMCO_H__
#define __HB_CORE_ADEMCO_H__

#include "ademco.h"
#include "hb_core.h"
#include "hb_rf.h"

#ifdef __cplusplus
extern "C" {
#endif

// 主机类型与安定宝事件码对照表
#define HMT_EVENT_MAP(XX)                       \
    XX(HMT_WIFI, EVENT_I_AM_WIFI)               \
    XX(HMT_GPRS_IOT, EVENT_I_AM_GPRS_IOT)       \
    XX(HMT_NETMOD, EVENT_I_AM_NET_MODULE)       \
    XX(HMT_GPRS, EVENT_I_AM_GPRS)               \
    XX(HMT_LCD, EVENT_I_AM_LCD)                 \
    XX(HMT_WIRED, EVENT_I_AM_WIRE)              \
    XX(HMT_TRUE_COLOR, EVENT_I_AM_TRUE_COLOR)   \
    XX(HMT_THREE_SECTION, EVENT_I_AM_3_SECTION) \
    XX(HMT_IOT, EVENT_I_AM_IOT)                 \
    XX(HMT_GPRS_PHONE, EVENT_I_AM_GPRS_PHONE)   \
    XX(HMT_NB, EVENT_I_AM_NB)                   \
    XX(HMT_WIFI2, EVENT_I_AM_WIFI2)

// 防区属性与安定宝事件码对照表
#define HZP_EVENT_MAP(XX)               \
    XX(HZP_BURGLAR, EVENT_BURGLAR)      \
    XX(HZP_BURGLAR_HALF, EVENT_BURGLAR) \
    XX(HZP_EMERGENCY, EVENT_EMERGENCY)  \
    XX(HZP_FIRE, EVENT_FIRE)            \
    XX(HZP_DURESS, EVENT_DURESS)        \
    XX(HZP_GAS, EVENT_GAS)              \
    XX(HZP_WATER, EVENT_WATER)          \
    XX(HZP_DOOR_RING, EVENT_DOOR_RING)

HB_EXPORT_SYMBOL
ademco_event_t hb_machine_status_to_ademco_event(hb_machine_status_t status);

HB_EXPORT_SYMBOL
hb_machine_status_t hb_machine_status_from_ademco_event(ademco_event_t ademco_event);

HB_EXPORT_SYMBOL
ademco_event_t hb_machine_type_to_ademco_event(hb_machine_type_t type);

HB_EXPORT_SYMBOL
hb_machine_type_t hb_machine_type_from_ademco_event(ademco_event_t ademco_event);

HB_EXPORT_SYMBOL
ademco_event_t hb_zone_property_to_ademco_event(hb_zone_property_t zp);

HB_EXPORT_SYMBOL
ademco_event_t hb_rf_code_to_ademco_event(uint8_t rf_code);

#ifdef __cplusplus
}
#endif

#endif /* __HB_CORE_ADEMCO_H__ */
