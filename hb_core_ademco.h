#ifndef __HB_CORE_ADEMCO_H__
#define __HB_CORE_ADEMCO_H__

#include "ademco.h"
#include "hb_core.h"
#include "hb_rf.h"

#ifdef __cplusplus
extern "C" {
#endif

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
