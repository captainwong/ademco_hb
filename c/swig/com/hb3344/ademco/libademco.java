/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public class libademco implements libademcoConstants {
  public static SWIGTYPE_p_int new_intp() {
    long cPtr = libademcoJNI.new_intp();
    return (cPtr == 0) ? null : new SWIGTYPE_p_int(cPtr, false);
  }

  public static SWIGTYPE_p_int copy_intp(int value) {
    long cPtr = libademcoJNI.copy_intp(value);
    return (cPtr == 0) ? null : new SWIGTYPE_p_int(cPtr, false);
  }

  public static void delete_intp(SWIGTYPE_p_int obj) {
    libademcoJNI.delete_intp(SWIGTYPE_p_int.getCPtr(obj));
  }

  public static void intp_assign(SWIGTYPE_p_int obj, int value) {
    libademcoJNI.intp_assign(SWIGTYPE_p_int.getCPtr(obj), value);
  }

  public static int intp_value(SWIGTYPE_p_int obj) {
    return libademcoJNI.intp_value(SWIGTYPE_p_int.getCPtr(obj));
  }

  public static SWIGTYPE_p_unsigned_short new_uint16p() {
    long cPtr = libademcoJNI.new_uint16p();
    return (cPtr == 0) ? null : new SWIGTYPE_p_unsigned_short(cPtr, false);
  }

  public static SWIGTYPE_p_unsigned_short copy_uint16p(int value) {
    long cPtr = libademcoJNI.copy_uint16p(value);
    return (cPtr == 0) ? null : new SWIGTYPE_p_unsigned_short(cPtr, false);
  }

  public static void delete_uint16p(SWIGTYPE_p_unsigned_short obj) {
    libademcoJNI.delete_uint16p(SWIGTYPE_p_unsigned_short.getCPtr(obj));
  }

  public static void uint16p_assign(SWIGTYPE_p_unsigned_short obj, int value) {
    libademcoJNI.uint16p_assign(SWIGTYPE_p_unsigned_short.getCPtr(obj), value);
  }

  public static int uint16p_value(SWIGTYPE_p_unsigned_short obj) {
    return libademcoJNI.uint16p_value(SWIGTYPE_p_unsigned_short.getCPtr(obj));
  }

  public static SWIGTYPE_p_unsigned_int new_uint32p() {
    long cPtr = libademcoJNI.new_uint32p();
    return (cPtr == 0) ? null : new SWIGTYPE_p_unsigned_int(cPtr, false);
  }

  public static SWIGTYPE_p_unsigned_int copy_uint32p(long value) {
    long cPtr = libademcoJNI.copy_uint32p(value);
    return (cPtr == 0) ? null : new SWIGTYPE_p_unsigned_int(cPtr, false);
  }

  public static void delete_uint32p(SWIGTYPE_p_unsigned_int obj) {
    libademcoJNI.delete_uint32p(SWIGTYPE_p_unsigned_int.getCPtr(obj));
  }

  public static void uint32p_assign(SWIGTYPE_p_unsigned_int obj, long value) {
    libademcoJNI.uint32p_assign(SWIGTYPE_p_unsigned_int.getCPtr(obj), value);
  }

  public static long uint32p_value(SWIGTYPE_p_unsigned_int obj) {
    return libademcoJNI.uint32p_value(SWIGTYPE_p_unsigned_int.getCPtr(obj));
  }

  public static int ademcoIsValidAdemcoId(long ademcoId) {
    return libademcoJNI.ademcoIsValidAdemcoId(ademcoId);
  }

  public static int ademcoIsValidZone(long zone) {
    return libademcoJNI.ademcoIsValidZone(zone);
  }

  public static int ademcoIsValidZoneStrict(long zone) {
    return libademcoJNI.ademcoIsValidZoneStrict(zone);
  }

  public static SWIGTYPE_p_AdemcoEvent getAdemcoEvents() {
    long cPtr = libademcoJNI.AdemcoEvents_get();
    return (cPtr == 0) ? null : new SWIGTYPE_p_AdemcoEvent(cPtr, false);
  }

  public static int ademcoIsMachineStatusEvent(AdemcoEvent ademcoEvent) {
    return libademcoJNI.ademcoIsMachineStatusEvent(ademcoEvent.swigValue());
  }

  public static int ademcoIsMachineTypeEvent(AdemcoEvent ademcoEvent) {
    return libademcoJNI.ademcoIsMachineTypeEvent(ademcoEvent.swigValue());
  }

  public static int ademcoIsEventNeedControlSource(AdemcoEvent ademcoEvent) {
    return libademcoJNI.ademcoIsEventNeedControlSource(ademcoEvent.swigValue());
  }

  public static AdemcoEventLevel ademcoGetEventLevel(AdemcoEvent ademcoEvent) {
    return AdemcoEventLevel.swigToEnum(libademcoJNI.ademcoGetEventLevel(ademcoEvent.swigValue()));
  }

  public static AdemcoEvent ademcoGetExceptionEventByResumeEvent(AdemcoEvent resumeEvent) {
    return AdemcoEvent.swigToEnum(libademcoJNI.ademcoGetExceptionEventByResumeEvent(resumeEvent.swigValue()));
  }

  public static String ademcoEventToString(AdemcoEvent ademcoEvent) {
    return libademcoJNI.ademcoEventToString(ademcoEvent.swigValue());
  }

  public static String ademcoEventToStringChinese(AdemcoEvent ademcoEvent) {
    return libademcoJNI.ademcoEventToStringChinese(ademcoEvent.swigValue());
  }

  public static int ademcoAppendDataSegment(byte[] packet, long ademcoId, AdemcoEvent ademcoEvent, long gg, long zone) {
    return libademcoJNI.ademcoAppendDataSegment(packet, ademcoId, ademcoEvent.swigValue(), gg, zone);
  }

  public static int ademcoAppendDataSegment2(AdemcoDataSegment dataSegment, long ademcoId, AdemcoEvent ademcoEvent, long gg, long zone) {
    return libademcoJNI.ademcoAppendDataSegment2(AdemcoDataSegment.getCPtr(dataSegment), dataSegment, ademcoId, ademcoEvent.swigValue(), gg, zone);
  }

  public static AdemcoParseResult ademcoParseDataSegment(byte[] packet, int packet_len, AdemcoDataSegment dataSegment) {
    return AdemcoParseResult.swigToEnum(libademcoJNI.ademcoParseDataSegment(packet, packet_len, AdemcoDataSegment.getCPtr(dataSegment), dataSegment));
  }

  public static int ademcoDataSegmentToCongwinFe100(byte[] fe100, AdemcoDataSegment dataSegment) {
    return libademcoJNI.ademcoDataSegmentToCongwinFe100(fe100, AdemcoDataSegment.getCPtr(dataSegment), dataSegment);
  }

  public static int ademcoXDataGetValidContentLen(AdemcoXDataSegment xdata) {
    return libademcoJNI.ademcoXDataGetValidContentLen(AdemcoXDataSegment.getCPtr(xdata), xdata);
  }

  public static int ademcoXDataMemcmp(AdemcoXDataSegment xdata, SWIGTYPE_p_void buf, int buf_len) {
    return libademcoJNI.ademcoXDataMemcmp(AdemcoXDataSegment.getCPtr(xdata), xdata, SWIGTYPE_p_void.getCPtr(buf), buf_len);
  }

  public static int ademcoMakeXData(AdemcoXDataSegment xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr, byte[] content, int len) {
    return libademcoJNI.ademcoMakeXData(AdemcoXDataSegment.getCPtr(xdata), xdata, xlf.swigValue(), xtr.swigValue(), content, len);
  }

  public static int isAdemcoPacketId(String standard, String id, int len) {
    return libademcoJNI.isAdemcoPacketId(standard, id, len);
  }

  public static AdemcoPacketId getAdemcoPacketId(String id, int len) {
    return AdemcoPacketId.swigToEnum(libademcoJNI.getAdemcoPacketId(id, len));
  }

  public static String admecoPacketIdToString(AdemcoPacketId id) {
    return libademcoJNI.admecoPacketIdToString(id.swigValue());
  }

  public static int ademcoMakeEmptyDataPacket(byte[] dst_buff, int len, String id, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeEmptyDataPacket(dst_buff, len, id, seq, acct, ademcoId);
  }

  public static int ademcoMakeNullPacket(byte[] buff, int len, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeNullPacket(buff, len, seq, acct, ademcoId);
  }

  public static int ademcoMakeAckPacket(byte[] buff, int len, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeAckPacket(buff, len, seq, acct, ademcoId);
  }

  public static int ademcoMakeNakPacket(byte[] buff, int len, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeNakPacket(buff, len, seq, acct, ademcoId);
  }

  public static int ademcoMakeHbPacket(byte[] buff, int len, int seq, String acct, long ademcoId, AdemcoEvent ademcoEvent, long gg, long zone, AdemcoXDataSegment xdata) {
    return libademcoJNI.ademcoMakeHbPacket(buff, len, seq, acct, ademcoId, ademcoEvent.swigValue(), gg, zone, AdemcoXDataSegment.getCPtr(xdata), xdata);
  }

  public static int ademcoMakeNullPacket2(AdemcoPacket pkt, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeNullPacket2(AdemcoPacket.getCPtr(pkt), pkt, seq, acct, ademcoId);
  }

  public static int ademcoMakeAckPacket2(AdemcoPacket pkt, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeAckPacket2(AdemcoPacket.getCPtr(pkt), pkt, seq, acct, ademcoId);
  }

  public static int ademcoMakeNakPacket2(AdemcoPacket pkt, int seq, String acct, long ademcoId) {
    return libademcoJNI.ademcoMakeNakPacket2(AdemcoPacket.getCPtr(pkt), pkt, seq, acct, ademcoId);
  }

  public static int ademcoMakeHbPacket2(AdemcoPacket pkt, int seq, String acct, long ademcoId, AdemcoEvent ademcoEvent, long gg, long zone, AdemcoXDataSegment xdata) {
    return libademcoJNI.ademcoMakeHbPacket2(AdemcoPacket.getCPtr(pkt), pkt, seq, acct, ademcoId, ademcoEvent.swigValue(), gg, zone, AdemcoXDataSegment.getCPtr(xdata), xdata);
  }

  public static AdemcoParseResult ademcoPacketParse(byte[] buff, int len, AdemcoPacket pkt, SWIGTYPE_p_int cb_commited) {
    return AdemcoParseResult.swigToEnum(libademcoJNI.ademcoPacketParse(buff, len, AdemcoPacket.getCPtr(pkt), pkt, SWIGTYPE_p_int.getCPtr(cb_commited)));
  }

  public static int ademcoCRC16(byte[] buff, int len) {
    return libademcoJNI.ademcoCRC16(buff, len);
  }

  public static int hbIsValid3SectionMachineGG(long gg) {
    return libademcoJNI.hbIsValid3SectionMachineGG(gg);
  }

  public static SWIGTYPE_p_HbZoneProperty getHbZoneProperties() {
    long cPtr = libademcoJNI.hbZoneProperties_get();
    return (cPtr == 0) ? null : new SWIGTYPE_p_HbZoneProperty(cPtr, false);
  }

  public static int hbGetAvailableZoneProperties(HbMachineType type, SWIGTYPE_p_HbZoneProperty props) {
    return libademcoJNI.hbGetAvailableZoneProperties(type.swigValue(), SWIGTYPE_p_HbZoneProperty.getCPtr(props));
  }

  public static int hbZoneMax(HbMachineType type) {
    return libademcoJNI.hbZoneMax(type.swigValue());
  }

  public static int hbIsValidZone(HbMachineType type, int zone) {
    return libademcoJNI.hbIsValidZone(type.swigValue(), zone);
  }

  public static int hbIsValidZoneStrict(HbMachineType type, int zone) {
    return libademcoJNI.hbIsValidZoneStrict(type.swigValue(), zone);
  }

  public static int hbMachineIsSelling(HbMachineType type) {
    return libademcoJNI.hbMachineIsSelling(type.swigValue());
  }

  public static int hbMachineCanArm(HbMachineType type) {
    return libademcoJNI.hbMachineCanArm(type.swigValue());
  }

  public static int hbMachineCanDisarm(HbMachineType type) {
    return libademcoJNI.hbMachineCanDisarm(type.swigValue());
  }

  public static int hbMachineCanEnterSettings(HbMachineType type) {
    return libademcoJNI.hbMachineCanEnterSettings(type.swigValue());
  }

  public static int hbMachineCanHalfArm(HbMachineType type) {
    return libademcoJNI.hbMachineCanHalfArm(type.swigValue());
  }

  public static int hbMachineCanReportSignalStrength(HbMachineType type) {
    return libademcoJNI.hbMachineCanReportSignalStrength(type.swigValue());
  }

  public static int hbMachineCanReportBySMS(HbMachineType type) {
    return libademcoJNI.hbMachineCanReportBySMS(type.swigValue());
  }

  public static int hbMachineHasWiredZone(HbMachineType type) {
    return libademcoJNI.hbMachineHasWiredZone(type.swigValue());
  }

  public static long hbWiredZoneMin(HbMachineType type) {
    return libademcoJNI.hbWiredZoneMin(type.swigValue());
  }

  public static long hbWiredZoneMax(HbMachineType type) {
    return libademcoJNI.hbWiredZoneMax(type.swigValue());
  }

  public static int hbMachineCanDirectlyWriteZone(HbMachineType type) {
    return libademcoJNI.hbMachineCanDirectlyWriteZone(type.swigValue());
  }

  public static int hbMachineCanLinkSubMachine(HbMachineType type) {
    return libademcoJNI.hbMachineCanLinkSubMachine(type.swigValue());
  }

  public static int hbZonePropCanReportLost(HbZoneProperty zp) {
    return libademcoJNI.hbZonePropCanReportLost(zp.swigValue());
  }

  public static void hbInitMachineTimer(HbMachineTimer timer) {
    libademcoJNI.hbInitMachineTimer(HbMachineTimer.getCPtr(timer), timer);
  }

  public static int hbIsValidTimer(HbTimer timer) {
    return libademcoJNI.hbIsValidTimer(HbTimer.getCPtr(timer), timer);
  }

  public static AdemcoEvent hbMachineStatusToAdemcoEvent(HbMachineStatus status) {
    return AdemcoEvent.swigToEnum(libademcoJNI.hbMachineStatusToAdemcoEvent(status.swigValue()));
  }

  public static HbMachineStatus hbMachineStatusFromAdemcoEvent(AdemcoEvent ademcoEvent) {
    return HbMachineStatus.swigToEnum(libademcoJNI.hbMachineStatusFromAdemcoEvent(ademcoEvent.swigValue()));
  }

  public static AdemcoEvent hbMachineTypeToAdemcoEvent(HbMachineType type) {
    return AdemcoEvent.swigToEnum(libademcoJNI.hbMachineTypeToAdemcoEvent(type.swigValue()));
  }

  public static HbMachineType hbMachineTypeFromAdemcoEvent(AdemcoEvent ademcoEvent) {
    return HbMachineType.swigToEnum(libademcoJNI.hbMachineTypeFromAdemcoEvent(ademcoEvent.swigValue()));
  }

  public static AdemcoEvent hbZonePropertyToAdemcoEvent(HbZoneProperty zp) {
    return AdemcoEvent.swigToEnum(libademcoJNI.hbZonePropertyToAdemcoEvent(zp.swigValue()));
  }

  public static String hbMachineStatusToString(HbMachineStatus status) {
    return libademcoJNI.hbMachineStatusToString(status.swigValue());
  }

  public static String hbMachineStatusToStringChinese(HbMachineStatus status) {
    return libademcoJNI.hbMachineStatusToStringChinese(status.swigValue());
  }

  public static String hbMachineTypeToString(HbMachineType type) {
    return libademcoJNI.hbMachineTypeToString(type.swigValue());
  }

  public static String hbMachineTypeToStringChinese(HbMachineType type) {
    return libademcoJNI.hbMachineTypeToStringChinese(type.swigValue());
  }

  public static String hbZonePropertyToString(HbZoneProperty zp) {
    return libademcoJNI.hbZonePropertyToString(zp.swigValue());
  }

  public static String hbZonePropertyToStringChinese(HbZoneProperty zp) {
    return libademcoJNI.hbZonePropertyToStringChinese(zp.swigValue());
  }

  public static String hbGetZoneFormatString(HbMachineType type) {
    return libademcoJNI.hbGetZoneFormatString(type.swigValue());
  }

  public static void hbSum(byte[] data, int len) {
    libademcoJNI.hbSum(data, len);
  }

  public static int hbCheckSum(byte[] data, int len) {
    return libademcoJNI.hbCheckSum(data, len);
  }

  public static HbComRequestType hbComParseRequest(byte[] buff, int len) {
    return HbComRequestType.swigToEnum(libademcoJNI.hbComParseRequest(buff, len));
  }

  public static HbComRequestType hbComParseXDataRequest(AdemcoXDataSegment xdata) {
    return HbComRequestType.swigToEnum(libademcoJNI.hbComParseXDataRequest(AdemcoXDataSegment.getCPtr(xdata), xdata));
  }

  public static HbComResponseType hbComParseResponse(byte[] buff, int len) {
    return HbComResponseType.swigToEnum(libademcoJNI.hbComParseResponse(buff, len));
  }

  public static void hbComMakeReqA0_getMachineStatus(HbComData data) {
    libademcoJNI.hbComMakeReqA0_getMachineStatus(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqA1_getMachineZones(HbComData data) {
    libademcoJNI.hbComMakeReqA1_getMachineZones(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqA2_getMoreMachineZones(HbComData data) {
    libademcoJNI.hbComMakeReqA2_getMoreMachineZones(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqA3_modifyMachineZone(HbComData data, short zone, HbZoneProperty prop, HbComReq_A3_op op) {
    libademcoJNI.hbComMakeReqA3_modifyMachineZone(HbComData.getCPtr(data), data, zone, prop.swigValue(), op.swigValue());
  }

  public static void hbComMakeReqA5_getMachineTimer(HbComData data) {
    libademcoJNI.hbComMakeReqA5_getMachineTimer(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqA7_setMachineTimer(HbComData data, HbMachineTimer timer) {
    libademcoJNI.hbComMakeReqA7_setMachineTimer(HbComData.getCPtr(data), data, HbMachineTimer.getCPtr(timer), timer);
  }

  public static void hbComMakeReqAA_modifyMachineZoneLostConfig(HbComData data, short zone, int on) {
    libademcoJNI.hbComMakeReqAA_modifyMachineZoneLostConfig(HbComData.getCPtr(data), data, zone, on);
  }

  public static void hbComMakeReqAC_getMachineZoneLostConfig(HbComData data) {
    libademcoJNI.hbComMakeReqAC_getMachineZoneLostConfig(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqAD_getMoreMachineZoneLostConfig(HbComData data) {
    libademcoJNI.hbComMakeReqAD_getMoreMachineZoneLostConfig(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqAE_set3SectionMachineStatus(HbComData data, HbCom_3sec_which p1, HbCom_3sec_status status) {
    libademcoJNI.hbComMakeReqAE_set3SectionMachineStatus(HbComData.getCPtr(data), data, p1.swigValue(), status.swigValue());
  }

  public static void hbComMakeReqB0_get3SectionMachineStatus(HbComData data) {
    libademcoJNI.hbComMakeReqB0_get3SectionMachineStatus(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqRD_acct(HbComData data) {
    libademcoJNI.hbComMakeReqRD_acct(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeReqWR_acct(HbComData data, String acct) {
    libademcoJNI.hbComMakeReqWR_acct(HbComData.getCPtr(data), data, acct);
  }

  public static void hbComMakeRespA0_getMachineStatus(HbComData data, HbMachineStatus status, HbMachineType type) {
    libademcoJNI.hbComMakeRespA0_getMachineStatus(HbComData.getCPtr(data), data, status.swigValue(), type.swigValue());
  }

  public static void hbComMakeRespA2_getMachineZones(HbComData data, int count, SWIGTYPE_p_unsigned_int zones, SWIGTYPE_p_HbZoneProperty props, HbComResp_A2_p1 p1) {
    libademcoJNI.hbComMakeRespA2_getMachineZones(HbComData.getCPtr(data), data, count, SWIGTYPE_p_unsigned_int.getCPtr(zones), SWIGTYPE_p_HbZoneProperty.getCPtr(props), p1.swigValue());
  }

  public static void hbComMakeRespA3_waitingSignal(HbComData data) {
    libademcoJNI.hbComMakeRespA3_waitingSignal(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeRespA4_modifyMachineZone(HbComData data, long zone, HbZoneProperty prop, HbComResp_A4_p3 p3) {
    libademcoJNI.hbComMakeRespA4_modifyMachineZone(HbComData.getCPtr(data), data, zone, prop.swigValue(), p3.swigValue());
  }

  public static void hbComMakeRespA6_getMachineTimer(HbComData data, HbMachineTimer timer) {
    libademcoJNI.hbComMakeRespA6_getMachineTimer(HbComData.getCPtr(data), data, HbMachineTimer.getCPtr(timer), timer);
  }

  public static void hbComMakeRespA7_setMachineTimer(HbComData data) {
    libademcoJNI.hbComMakeRespA7_setMachineTimer(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeRespA8_reject(HbComData data) {
    libademcoJNI.hbComMakeRespA8_reject(HbComData.getCPtr(data), data);
  }

  public static void hbComMakeRespAB_modifyMachineZoneLostConfig(HbComData data, long zone, HbComResp_AB_p2 p2) {
    libademcoJNI.hbComMakeRespAB_modifyMachineZoneLostConfig(HbComData.getCPtr(data), data, zone, p2.swigValue());
  }

  public static void hbComMakeRespAD_getMachineZoneLostConfig(HbComData data, HbComResp_AD_p1 p1, int count, SWIGTYPE_p_unsigned_int zones, HbComResp_AD_p2 p2) {
    libademcoJNI.hbComMakeRespAD_getMachineZoneLostConfig(HbComData.getCPtr(data), data, p1.swigValue(), count, SWIGTYPE_p_unsigned_int.getCPtr(zones), p2.swigValue());
  }

  public static void hbComMakeRespAF_set3SectionMachineStatus(HbComData data, HbCom_3sec_which p1, HbCom_3sec_status status) {
    libademcoJNI.hbComMakeRespAF_set3SectionMachineStatus(HbComData.getCPtr(data), data, p1.swigValue(), status.swigValue());
  }

  public static void hbComMakeRespB1_get3SectionMachineStatus(HbComData data, HbMachineStatus statusMachine, HbMachineStatus statusSec1, HbMachineStatus statusSec2, HbMachineStatus statusSec3) {
    libademcoJNI.hbComMakeRespB1_get3SectionMachineStatus(HbComData.getCPtr(data), data, statusMachine.swigValue(), statusSec1.swigValue(), statusSec2.swigValue(), statusSec3.swigValue());
  }

  public static void hbComDataToAdemcoXData(HbComData data, AdemcoXDataSegment xdata, AdemcoXDataLengthFormat xlf, AdemcoXDataTransform xtr) {
    libademcoJNI.hbComDataToAdemcoXData(HbComData.getCPtr(data), data, AdemcoXDataSegment.getCPtr(xdata), xdata, xlf.swigValue(), xtr.swigValue());
  }

  public static int hbHiLoArrayToDecStr(String str, byte[] arr, int len) {
    return libademcoJNI.hbHiLoArrayToDecStr(str, arr, len);
  }

  public static int hbDecStrToHiLoArray(byte[] arr, int len, String str) {
    return libademcoJNI.hbDecStrToHiLoArray(arr, len, str);
  }

  public static int hbHexArrayToStr(String str, byte[] arr, int len) {
    return libademcoJNI.hbHexArrayToStr(str, arr, len);
  }

  public static int hbHexStrToArray(byte[] arr, String str, short padding) {
    return libademcoJNI.hbHexStrToArray(arr, str, padding);
  }

  public static int hbHexStrToArrayN(byte[] arr, String str, int len, short padding) {
    return libademcoJNI.hbHexStrToArrayN(arr, str, len, padding);
  }

  public static int hbHexStrToArrayN_allow_non_hex_str(byte[] arr, String str, int len, short padding) {
    return libademcoJNI.hbHexStrToArrayN_allow_non_hex_str(arr, str, len, padding);
  }

}