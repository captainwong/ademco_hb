#include <libhbiot/rf/hb_rf.h>

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

void hb_rf_addr_reset(hb_rf_addr_t* addr) {
    if (addr == NULL) {
        return;
    }
    addr->hi = 0xFF;
    addr->mi = 0xFF;
    addr->lo = 0xFF;
}

bool hb_rf_addr_equal(const hb_rf_addr_t* addr1,
                      const hb_rf_addr_t* addr2) {
    if (addr1 == NULL || addr2 == NULL) {
        return false;
    }
    return (addr1->hi == addr2->hi) &&
           (addr1->mi == addr2->mi) &&
           (addr1->lo == addr2->lo);
}

void hb_rf_addr_from_packet(hb_rf_addr_t* addr, const hb_rf_packet_t* pkt) {
    if (addr == NULL || pkt == NULL) {
        return;
    }
    addr->hi = pkt->hi;
    addr->mi = pkt->mi;
    addr->lo = pkt->lo;
}

uint32_t hb_rf_addr_to_u32(const hb_rf_addr_t* addr) {
    if (addr == NULL) {
        return 0;
    }
    return ((uint32_t)addr->hi << 16) | ((uint32_t)addr->mi << 8) | (uint32_t)addr->lo;
}

void hb_rf_addr_from_u32(hb_rf_addr_t* addr, uint32_t u32_addr) {
    if (addr == NULL) {
        return;
    }
    addr->hi = (u32_addr >> 16) & 0xFF;
    addr->mi = (u32_addr >> 8) & 0xFF;
    addr->lo = u32_addr & 0xFF;
}

bool hb_rf_packet_eq(const hb_rf_packet_t* a, const hb_rf_packet_t* b) {
    if (a == NULL || b == NULL) {
        return false;
    }
    return (a->mi == b->mi) &&
           (a->lo == b->lo) &&
           (a->cmd == b->cmd) &&
           (a->hi == b->hi);  // no need to check sum, as it is calculated from the other fields
}
