#include "hb_rf.h"

///////////////////////////// RF functions /////////////////////////////////

hb_rf_code_t hb_rf_code_from_u8(uint8_t rf_code) {
    switch (rf_code) {
#define XX(_code, name, desc) \
    case _code:               \
        return HB_RF_CODE_##name;
        HB_RF_CODE_MAP(XX)
#undef XX

        default:
            return HB_RF_CODE_INVALID;
    }
}

const char* hb_rf_code_to_string(hb_rf_code_t rf_code) {
    switch (rf_code) {
#define XX(_code, name, desc) \
    case HB_RF_CODE_##name:   \
        return #name;
        HB_RF_CODE_MAP(XX)
#undef XX

        default:
            return "Unkown hb_rf_code_t";
    }
}

#if HB_ENABLE_CHINESE
const char* hb_rf_code_to_string_chinese(hb_rf_code_t rf_code) {
    switch (rf_code) {
#define XX(_code, name, desc) \
    case HB_RF_CODE_##name:   \
        return desc;
        HB_RF_CODE_MAP(XX)
#undef XX

        default:
            return "未知无线码";
    }
}

#endif /* HB_ENABLE_CHINESE */
