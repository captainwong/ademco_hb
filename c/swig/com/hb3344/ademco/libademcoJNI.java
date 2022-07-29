/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public class libademcoJNI {
  public final static native long new_intp();
  public final static native long copy_intp(int jarg1);
  public final static native void delete_intp(long jarg1);
  public final static native void intp_assign(long jarg1, int jarg2);
  public final static native int intp_value(long jarg1);
  public final static native long new_uint16p();
  public final static native long copy_uint16p(int jarg1);
  public final static native void delete_uint16p(long jarg1);
  public final static native void uint16p_assign(long jarg1, int jarg2);
  public final static native int uint16p_value(long jarg1);
  public final static native long new_uint32p();
  public final static native long copy_uint32p(long jarg1);
  public final static native void delete_uint32p(long jarg1);
  public final static native void uint32p_assign(long jarg1, long jarg2);
  public final static native long uint32p_value(long jarg1);
  public final static native int ADEMCO_OK_get();
  public final static native int ADEMCO_ERR_get();
  public final static native int ADEMCO_PACKET_ACCT_MAX_LEN_get();
  public final static native int ADEMCO_PACKET_DATA_SEGMENT_EMPTY_LEN_get();
  public final static native int ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_get();
  public final static native int ADEMCO_PACKET_DATA_SEGMENT_FULL_LEN_MAX_get();
  public final static native int ADEMCO_PACKET_MAX_LEN_get();
  public final static native int CONGWIN_FE100_PACKET_LEN_get();
  public final static native int ADEMCO_PACKET_TIMESTAMP_LEN_get();
  public final static native int ADEMCO_PACKET_XDATA_MAX_LEN_get();
  public final static native int ADEMCO_ID_MIN_get();
  public final static native int ADEMCO_ID_MAX_get();
  public final static native int ADEMCO_ID_SENTINEL_get();
  public final static native int ademcoIsValidAdemcoId(long jarg1);
  public final static native int ADEMCO_ZONE_FOR_MACHINE_SELF_get();
  public final static native int ADEMCO_ZONE_MIN_get();
  public final static native int ADEMCO_ZONE_MAX_get();
  public final static native int ADEMCO_ZONE_SENTINEL_get();
  public final static native int ademcoIsValidZone(long jarg1);
  public final static native int ademcoIsValidZoneStrict(long jarg1);
  public final static native int ADEMCO_GG_MIN_get();
  public final static native int ADEMCO_GG_MAX_get();
  public final static native int ADEMCO_GG_SENTINEL_get();
  public final static native int EVENT_INVALID_EVENT_get();
  public final static native int EVENT_ARM_get();
  public final static native int EVENT_DISARM_get();
  public final static native int EVENT_HALFARM_1456_get();
  public final static native int EVENT_HALFARM_get();
  public final static native int EVENT_EMERGENCY_get();
  public final static native int EVENT_BURGLAR_get();
  public final static native int EVENT_DOORRINGING_get();
  public final static native int EVENT_FIRE_get();
  public final static native int EVENT_DURESS_get();
  public final static native int EVENT_GAS_get();
  public final static native int EVENT_WATER_get();
  public final static native int EVENT_TAMPER_get();
  public final static native int EVENT_ZONE_TAMPER_get();
  public final static native int EVENT_BYPASS_get();
  public final static native int EVENT_BYPASS_RESUME_get();
  public final static native int EVENT_AC_BROKE_get();
  public final static native int EVENT_AC_RECOVER_get();
  public final static native int EVENT_LOWBATTERY_get();
  public final static native int EVENT_BATTERY_RECOVER_get();
  public final static native int EVENT_BADBATTERY_get();
  public final static native int EVENT_BADBATTERY_RECOVER_get();
  public final static native int EVENT_SOLARDISTURB_get();
  public final static native int EVENT_SOLARDISTURB_RECOVER_get();
  public final static native int EVENT_DISCONNECT_get();
  public final static native int EVENT_RECONNECT_get();
  public final static native int EVENT_BATTERY_EXCEPTION_get();
  public final static native int EVENT_BATTERY_EXCEPTION_RECOVER_get();
  public final static native int EVENT_OTHER_EXCEPTION_get();
  public final static native int EVENT_OTHER_EXCEPTION_RECOVER_get();
  public final static native int EVENT_LOST_get();
  public final static native int EVENT_LOST_RECOVER_get();
  public final static native int EVENT_3100_get();
  public final static native int EVENT_SERIAL485DIS_get();
  public final static native int EVENT_SERIAL485CONN_get();
  public final static native int EVENT_CONN_HANGUP_get();
  public final static native int EVENT_CONN_RESUME_get();
  public final static native int EVENT_DISARM_PWD_ERR_get();
  public final static native int EVENT_SUB_MACHINE_SENSOR_EXCEPTION_get();
  public final static native int EVENT_SUB_MACHINE_SENSOR_RESUME_get();
  public final static native int EVENT_SUB_MACHINE_POWER_EXCEPTION_get();
  public final static native int EVENT_SUB_MACHINE_POWER_RESUME_get();
  public final static native int EVENT_COM_PASSTHROUGH_get();
  public final static native int EVENT_ENTER_SET_MODE_get();
  public final static native int EVENT_EXIT_SET_MODE_get();
  public final static native int EVENT_QUERY_SUB_MACHINE_get();
  public final static native int EVENT_WRITE_TO_MACHINE_get();
  public final static native int EVENT_I_AM_NET_MODULE_get();
  public final static native int EVENT_I_AM_GPRS_get();
  public final static native int EVENT_I_AM_LCD_MACHINE_get();
  public final static native int EVENT_I_AM_WIRE_MACHINE_get();
  public final static native int EVENT_I_AM_WIFI_MACHINE_get();
  public final static native int EVENT_I_AM_3_SECTION_MACHINE_get();
  public final static native int EVENT_I_AM_IOT_MACHINE_get();
  public final static native int EVENT_I_AM_TRUE_COLOR_get();
  public final static native int EVENT_I_AM_GPRS_IOT_get();
  public final static native int EVENT_I_AM_GPRS_PHONE_get();
  public final static native int EVENT_I_AM_NB_MACHINE_get();
  public final static native int EVENT_PHONE_USER_SOS_get();
  public final static native int EVENT_PHONE_USER_CANCLE_ALARM_get();
  public final static native int EVENT_ENTER_SETTING_MODE_get();
  public final static native int EVENT_EXIT_SETTING_MODE_get();
  public final static native int EVENT_RESTORE_FACTORY_SETTINGS_710_get();
  public final static native int EVENT_RESTORE_FACTORY_SETTINGS_get();
  public final static native int EVENT_SIM_IS_IOT_CARD_get();
  public final static native int EVENT_SIM_IS_IOT_PLATFORM_CARD_get();
  public final static native int EVENT_SIM_IS_NOT_IOT_CARD_get();
  public final static native int EVENT_WHAT_IS_YOUR_TYPE_get();
  public final static native int EVENT_SIGNAL_STRENGTH_CHANGED_get();
  public final static native int EVENT_OFFLINE_get();
  public final static native int EVENT_ONLINE_get();
  public final static native long AdemcoEvents_get();
  public final static native void AdemcoDataSegment_raw_set(long jarg1, AdemcoDataSegment jarg1_, byte[] jarg2);
  public final static native byte[] AdemcoDataSegment_raw_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native void AdemcoDataSegment_raw_len_set(long jarg1, AdemcoDataSegment jarg1_, int jarg2);
  public final static native int AdemcoDataSegment_raw_len_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native void AdemcoDataSegment_ademcoId_set(long jarg1, AdemcoDataSegment jarg1_, long jarg2);
  public final static native long AdemcoDataSegment_ademcoId_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native void AdemcoDataSegment_ademcoEvent_set(long jarg1, AdemcoDataSegment jarg1_, int jarg2);
  public final static native int AdemcoDataSegment_ademcoEvent_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native void AdemcoDataSegment_gg_set(long jarg1, AdemcoDataSegment jarg1_, long jarg2);
  public final static native long AdemcoDataSegment_gg_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native void AdemcoDataSegment_zone_set(long jarg1, AdemcoDataSegment jarg1_, long jarg2);
  public final static native long AdemcoDataSegment_zone_get(long jarg1, AdemcoDataSegment jarg1_);
  public final static native long new_AdemcoDataSegment();
  public final static native void delete_AdemcoDataSegment(long jarg1);
  public final static native void AdemcoXDataSegment_lenghFormat_set(long jarg1, AdemcoXDataSegment jarg1_, int jarg2);
  public final static native int AdemcoXDataSegment_lenghFormat_get(long jarg1, AdemcoXDataSegment jarg1_);
  public final static native void AdemcoXDataSegment_raw_set(long jarg1, AdemcoXDataSegment jarg1_, byte[] jarg2);
  public final static native byte[] AdemcoXDataSegment_raw_get(long jarg1, AdemcoXDataSegment jarg1_);
  public final static native void AdemcoXDataSegment_raw_len_set(long jarg1, AdemcoXDataSegment jarg1_, int jarg2);
  public final static native int AdemcoXDataSegment_raw_len_get(long jarg1, AdemcoXDataSegment jarg1_);
  public final static native long new_AdemcoXDataSegment();
  public final static native void delete_AdemcoXDataSegment(long jarg1);
  public final static native String ADEMCO_PACKET_ID_NULL_get();
  public final static native String ADEMCO_PACKET_ID_ACK_get();
  public final static native String ADEMCO_PACKET_ID_NAK_get();
  public final static native String ADEMCO_PACKET_ID_DUH_get();
  public final static native String ADEMCO_PACKET_ID_HB_get();
  public final static native String ADEMCO_PACKET_ID_ADM_CID_get();
  public final static native String ADEMCO_PACKET_ID_MOD_REG_get();
  public final static native String ADEMCO_PACKET_ID_REG_RSP_get();
  public final static native int AID_INVALID_get();
  public final static native String ADEMCO_RRCVR_DEFAULT_get();
  public final static native String ADEMCO_LPREF_DEFAULT_get();
  public final static native char ADEMCO_PACKET_PREFIX_get();
  public final static native char ADEMCO_PACKET_SUFIX_get();
  public final static native void AdemcoPacket_crc_set(long jarg1, AdemcoPacket jarg1_, int jarg2);
  public final static native int AdemcoPacket_crc_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_len_set(long jarg1, AdemcoPacket jarg1_, int jarg2);
  public final static native int AdemcoPacket_len_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_id_set(long jarg1, AdemcoPacket jarg1_, int jarg2);
  public final static native int AdemcoPacket_id_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_seq_set(long jarg1, AdemcoPacket jarg1_, int jarg2);
  public final static native int AdemcoPacket_seq_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_acct_set(long jarg1, AdemcoPacket jarg1_, String jarg2);
  public final static native String AdemcoPacket_acct_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_data_set(long jarg1, AdemcoPacket jarg1_, long jarg2, AdemcoDataSegment jarg2_);
  public final static native long AdemcoPacket_data_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_xdata_set(long jarg1, AdemcoPacket jarg1_, long jarg2, AdemcoXDataSegment jarg2_);
  public final static native long AdemcoPacket_xdata_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_timestamp_set(long jarg1, AdemcoPacket jarg1_, long jarg2);
  public final static native long AdemcoPacket_timestamp_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_raw_set(long jarg1, AdemcoPacket jarg1_, byte[] jarg2);
  public final static native byte[] AdemcoPacket_raw_get(long jarg1, AdemcoPacket jarg1_);
  public final static native void AdemcoPacket_raw_len_set(long jarg1, AdemcoPacket jarg1_, int jarg2);
  public final static native int AdemcoPacket_raw_len_get(long jarg1, AdemcoPacket jarg1_);
  public final static native long new_AdemcoPacket();
  public final static native void delete_AdemcoPacket(long jarg1);
  public final static native int ademcoIsMachineStatusEvent(int jarg1);
  public final static native int ademcoIsMachineTypeEvent(int jarg1);
  public final static native int ademcoIsEventNeedControlSource(int jarg1);
  public final static native int ademcoGetEventLevel(int jarg1);
  public final static native int ademcoGetExceptionEventByResumeEvent(int jarg1);
  public final static native String ademcoEventToString(int jarg1);
  public final static native String ademcoEventToStringChinese(int jarg1);
  public final static native int ademcoAppendDataSegment(byte[] jarg1, long jarg2, int jarg3, long jarg4, long jarg5);
  public final static native int ademcoAppendDataSegment2(long jarg1, AdemcoDataSegment jarg1_, long jarg2, int jarg3, long jarg4, long jarg5);
  public final static native int ademcoParseDataSegment(byte[] jarg1, int jarg2, long jarg3, AdemcoDataSegment jarg3_);
  public final static native int ademcoDataSegmentToCongwinFe100(byte[] jarg1, long jarg2, AdemcoDataSegment jarg2_);
  public final static native int ademcoXDataGetValidContentLen(long jarg1, AdemcoXDataSegment jarg1_);
  public final static native int ademcoXDataMemcmp(long jarg1, AdemcoXDataSegment jarg1_, long jarg2, int jarg3);
  public final static native int ademcoMakeXData(long jarg1, AdemcoXDataSegment jarg1_, int jarg2, int jarg3, byte[] jarg4, int jarg5);
  public final static native int isAdemcoPacketId(String jarg1, String jarg2, int jarg3);
  public final static native int getAdemcoPacketId(String jarg1, int jarg2);
  public final static native String admecoPacketIdToString(int jarg1);
  public final static native int ademcoMakeEmptyDataPacket(byte[] jarg1, int jarg2, String jarg3, int jarg4, String jarg5, long jarg6);
  public final static native int ademcoMakeNullPacket(byte[] jarg1, int jarg2, int jarg3, String jarg4, long jarg5);
  public final static native int ademcoMakeAckPacket(byte[] jarg1, int jarg2, int jarg3, String jarg4, long jarg5);
  public final static native int ademcoMakeNakPacket(byte[] jarg1, int jarg2, int jarg3, String jarg4, long jarg5);
  public final static native int ademcoMakeHbPacket(byte[] jarg1, int jarg2, int jarg3, String jarg4, long jarg5, int jarg6, long jarg7, long jarg8, long jarg9, AdemcoXDataSegment jarg9_);
  public final static native int ademcoMakeNullPacket2(long jarg1, AdemcoPacket jarg1_, int jarg2, String jarg3, long jarg4);
  public final static native int ademcoMakeAckPacket2(long jarg1, AdemcoPacket jarg1_, int jarg2, String jarg3, long jarg4);
  public final static native int ademcoMakeNakPacket2(long jarg1, AdemcoPacket jarg1_, int jarg2, String jarg3, long jarg4);
  public final static native int ademcoMakeHbPacket2(long jarg1, AdemcoPacket jarg1_, int jarg2, String jarg3, long jarg4, int jarg5, long jarg6, long jarg7, long jarg8, AdemcoXDataSegment jarg8_);
  public final static native int ademcoPacketParse(byte[] jarg1, int jarg2, long jarg3, AdemcoPacket jarg3_, long jarg4);
  public final static native int ademcoCRC16(byte[] jarg1, int jarg2);
  public final static native int HB_3SECTION_MACHINE_GG_MIN_get();
  public final static native int HB_3SECTION_MACHINE_GG_MAX_get();
  public final static native int hbIsValid3SectionMachineGG(long jarg1);
  public final static native int HMS_INVALID_get();
  public final static native int HMT_INVALID_get();
  public final static native int HZP_INVALID_get();
  public final static native void HbMachineTimePoint_hour_set(long jarg1, HbMachineTimePoint jarg1_, short jarg2);
  public final static native short HbMachineTimePoint_hour_get(long jarg1, HbMachineTimePoint jarg1_);
  public final static native void HbMachineTimePoint_minute_set(long jarg1, HbMachineTimePoint jarg1_, short jarg2);
  public final static native short HbMachineTimePoint_minute_get(long jarg1, HbMachineTimePoint jarg1_);
  public final static native long new_HbMachineTimePoint();
  public final static native void delete_HbMachineTimePoint(long jarg1);
  public final static native void HbTimer_armAt_set(long jarg1, HbTimer jarg1_, long jarg2, HbMachineTimePoint jarg2_);
  public final static native long HbTimer_armAt_get(long jarg1, HbTimer jarg1_);
  public final static native void HbTimer_disarmAt_set(long jarg1, HbTimer jarg1_, long jarg2, HbMachineTimePoint jarg2_);
  public final static native long HbTimer_disarmAt_get(long jarg1, HbTimer jarg1_);
  public final static native long new_HbTimer();
  public final static native void delete_HbTimer(long jarg1);
  public final static native void HbMachineTimer_timer_set(long jarg1, HbMachineTimer jarg1_, long jarg2, HbTimer jarg2_);
  public final static native long HbMachineTimer_timer_get(long jarg1, HbMachineTimer jarg1_);
  public final static native void HbMachineTimer_data_set(long jarg1, HbMachineTimer jarg1_, byte[] jarg2);
  public final static native byte[] HbMachineTimer_data_get(long jarg1, HbMachineTimer jarg1_);
  public final static native long new_HbMachineTimer();
  public final static native void delete_HbMachineTimer(long jarg1);
  public final static native int HB_COM_DATA_MAX_LEN_get();
  public final static native void HbComData_data_set(long jarg1, HbComData jarg1_, byte[] jarg2);
  public final static native byte[] HbComData_data_get(long jarg1, HbComData jarg1_);
  public final static native void HbComData_len_set(long jarg1, HbComData jarg1_, int jarg2);
  public final static native int HbComData_len_get(long jarg1, HbComData jarg1_);
  public final static native long new_HbComData();
  public final static native void delete_HbComData(long jarg1);
  public final static native int HbComReq_Invalid_get();
  public final static native int HbComResp_Invalid_get();
  public final static native int HbComReq_A0_len_get();
  public final static native String HbComReq_A0_data_get();
  public final static native int HbComResp_A0_len_get();
  public final static native String HbComResp_A0_head_get();
  public final static native int HbComReq_A1_len_get();
  public final static native String HbComReq_A1_data_get();
  public final static native int HbComResp_A2_len_min_get();
  public final static native int HbComResp_A2_max_zone_get();
  public final static native int HbComResp_A2_len_max_get();
  public final static native int HbComResp_A2_p1_nomore_get();
  public final static native int HbComResp_A2_p1_more_get();
  public final static native String HbComResp_A2_head_get();
  public final static native int HbComReq_A2_len_get();
  public final static native String HbComReq_A2_data_get();
  public final static native int HbComReq_A3_len_get();
  public final static native String HbComReq_A3_head_get();
  public final static native int HbComReq_A3_p3_del_get();
  public final static native int HbComReq_A3_p3_learn_get();
  public final static native int HbComReq_A3_p3_stop_get();
  public final static native int HbComReq_A3_p3_modify_get();
  public final static native int HbComResp_A3_len_get();
  public final static native String HbComResp_A3_data_get();
  public final static native int HbComResp_A4_len_get();
  public final static native String HbComResp_A4_head_get();
  public final static native int HbComResp_A4_p3_fail_get();
  public final static native int HbComResp_A4_p3_ok_get();
  public final static native int HbComResp_A4_p3_dup_get();
  public final static native int HbComResp_A4_p3_ne_get();
  public final static native int HbComReq_A5_len_get();
  public final static native String HbComReq_A5_data_get();
  public final static native int HbComResp_A6_len_get();
  public final static native String HbComResp_A6_head_get();
  public final static native int HbComReq_A7_len_get();
  public final static native String HbComReq_A7_head_get();
  public final static native int HbComResp_A7_len_get();
  public final static native String HbComResp_A7_data_get();
  public final static native int HbComResp_A8_len_get();
  public final static native String HbComResp_A8_data_get();
  public final static native int HbComReq_AA_len_get();
  public final static native String HbComReq_AA_head_get();
  public final static native int HbComResp_AB_len_get();
  public final static native String HbComResp_AB_head_get();
  public final static native int HbComResp_AB_p2_close_get();
  public final static native int HbComResp_AB_p2_open_get();
  public final static native int HbComResp_AB_p2_reject_get();
  public final static native int HbComReq_AC_len_get();
  public final static native String HbComReq_AC_data_get();
  public final static native int HbComReq_AD_len_get();
  public final static native String HbComReq_AD_data_get();
  public final static native int HbComResp_AD_len_min_get();
  public final static native int HbComResp_AD_max_zone_get();
  public final static native int HbComResp_AD_len_max_get();
  public final static native int HbComResp_AD_p1_single_get();
  public final static native int HbComResp_AD_p1_double_get();
  public final static native int HbComResp_AD_p2_nomore_get();
  public final static native int HbComResp_AD_p2_more_get();
  public final static native String HbComResp_AD_head_get();
  public final static native int HbCom_3sec_arm_get();
  public final static native int HbCom_3sec_disarm_get();
  public final static native int HbComReq_AE_len_get();
  public final static native String HbComReq_AE_head_get();
  public final static native int HbComResp_AF_len_get();
  public final static native String HbComResp_AF_head_get();
  public final static native int HbComReq_B0_len_get();
  public final static native String HbComReq_B0_data_get();
  public final static native int HbComResp_B1_len_get();
  public final static native String HbComResp_B1_head_get();
  public final static native int HbComReq_RD_acct_len_get();
  public final static native String HbComReq_RD_acct_data_get();
  public final static native int HbComResp_RD_acct_len_get();
  public final static native String HbComResp_RD_acct_head_get();
  public final static native int HbComReq_WR_acct_len_get();
  public final static native String HbComReq_WR_acct_head_get();
  public final static native int HbComResp_WR_acct_len_get();
  public final static native String HbComResp_WR_acct_head_get();
  public final static native long hbZoneProperties_get();
  public final static native int hbGetAvailableZoneProperties(int jarg1, long jarg2);
  public final static native int hbZoneMax(int jarg1);
  public final static native int hbIsValidZone(int jarg1, int jarg2);
  public final static native int hbIsValidZoneStrict(int jarg1, int jarg2);
  public final static native int hbMachineIsSelling(int jarg1);
  public final static native int hbMachineCanArm(int jarg1);
  public final static native int hbMachineCanDisarm(int jarg1);
  public final static native int hbMachineCanEnterSettings(int jarg1);
  public final static native int hbMachineCanHalfArm(int jarg1);
  public final static native int hbMachineCanReportSignalStrength(int jarg1);
  public final static native int hbMachineCanReportBySMS(int jarg1);
  public final static native int hbMachineHasWiredZone(int jarg1);
  public final static native long hbWiredZoneMin(int jarg1);
  public final static native long hbWiredZoneMax(int jarg1);
  public final static native int hbMachineCanDirectlyWriteZone(int jarg1);
  public final static native int hbMachineCanLinkSubMachine(int jarg1);
  public final static native int hbZonePropCanReportLost(int jarg1);
  public final static native void hbInitMachineTimer(long jarg1, HbMachineTimer jarg1_);
  public final static native int hbIsValidTimer(long jarg1, HbTimer jarg1_);
  public final static native int hbMachineStatusToAdemcoEvent(int jarg1);
  public final static native int hbMachineStatusFromAdemcoEvent(int jarg1);
  public final static native int hbMachineTypeToAdemcoEvent(int jarg1);
  public final static native int hbMachineTypeFromAdemcoEvent(int jarg1);
  public final static native int hbZonePropertyToAdemcoEvent(int jarg1);
  public final static native String hbMachineStatusToString(int jarg1);
  public final static native String hbMachineStatusToStringChinese(int jarg1);
  public final static native String hbMachineTypeToString(int jarg1);
  public final static native String hbMachineTypeToStringChinese(int jarg1);
  public final static native String hbZonePropertyToString(int jarg1);
  public final static native String hbZonePropertyToStringChinese(int jarg1);
  public final static native String hbGetZoneFormatString(int jarg1);
  public final static native void hbSum(byte[] jarg1, int jarg2);
  public final static native int hbCheckSum(byte[] jarg1, int jarg2);
  public final static native int hbComParseRequest(byte[] jarg1, int jarg2);
  public final static native int hbComParseXDataRequest(long jarg1, AdemcoXDataSegment jarg1_);
  public final static native int hbComParseResponse(byte[] jarg1, int jarg2);
  public final static native void hbComMakeReqA0_getMachineStatus(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqA1_getMachineZones(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqA2_getMoreMachineZones(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqA3_modifyMachineZone(long jarg1, HbComData jarg1_, short jarg2, int jarg3, int jarg4);
  public final static native void hbComMakeReqA5_getMachineTimer(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqA7_setMachineTimer(long jarg1, HbComData jarg1_, long jarg2, HbMachineTimer jarg2_);
  public final static native void hbComMakeReqAA_modifyMachineZoneLostConfig(long jarg1, HbComData jarg1_, short jarg2, int jarg3);
  public final static native void hbComMakeReqAC_getMachineZoneLostConfig(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqAD_getMoreMachineZoneLostConfig(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqAE_set3SectionMachineStatus(long jarg1, HbComData jarg1_, int jarg2, int jarg3);
  public final static native void hbComMakeReqB0_get3SectionMachineStatus(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqRD_acct(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeReqWR_acct(long jarg1, HbComData jarg1_, String jarg2);
  public final static native void hbComMakeRespA0_getMachineStatus(long jarg1, HbComData jarg1_, int jarg2, int jarg3);
  public final static native void hbComMakeRespA2_getMachineZones(long jarg1, HbComData jarg1_, int jarg2, long jarg3, long jarg4, int jarg5);
  public final static native void hbComMakeRespA3_waitingSignal(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeRespA4_modifyMachineZone(long jarg1, HbComData jarg1_, long jarg2, int jarg3, int jarg4);
  public final static native void hbComMakeRespA6_getMachineTimer(long jarg1, HbComData jarg1_, long jarg2, HbMachineTimer jarg2_);
  public final static native void hbComMakeRespA7_setMachineTimer(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeRespA8_reject(long jarg1, HbComData jarg1_);
  public final static native void hbComMakeRespAB_modifyMachineZoneLostConfig(long jarg1, HbComData jarg1_, long jarg2, int jarg3);
  public final static native void hbComMakeRespAD_getMachineZoneLostConfig(long jarg1, HbComData jarg1_, int jarg2, int jarg3, long jarg4, int jarg5);
  public final static native void hbComMakeRespAF_set3SectionMachineStatus(long jarg1, HbComData jarg1_, int jarg2, int jarg3);
  public final static native void hbComMakeRespB1_get3SectionMachineStatus(long jarg1, HbComData jarg1_, int jarg2, int jarg3, int jarg4, int jarg5);
  public final static native void hbComDataToAdemcoXData(long jarg1, HbComData jarg1_, long jarg2, AdemcoXDataSegment jarg2_, int jarg3, int jarg4);
  public final static native int hbHiLoArrayToDecStr(String jarg1, byte[] jarg2, int jarg3);
  public final static native int hbDecStrToHiLoArray(byte[] jarg1, int jarg2, String jarg3);
  public final static native int hbHexArrayToStr(String jarg1, byte[] jarg2, int jarg3);
  public final static native int hbHexStrToArray(byte[] jarg1, String jarg2, short jarg3);
  public final static native int hbHexStrToArrayN(byte[] jarg1, String jarg2, int jarg3, short jarg4);
  public final static native int hbHexStrToArrayN_allow_non_hex_str(byte[] jarg1, String jarg2, int jarg3, short jarg4);
}