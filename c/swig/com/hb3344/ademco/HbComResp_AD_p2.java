/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public enum HbComResp_AD_p2 {
  HbComResp_AD_p2_nomore(libademcoJNI.HbComResp_AD_p2_nomore_get()),
  HbComResp_AD_p2_more(libademcoJNI.HbComResp_AD_p2_more_get());

  public final int swigValue() {
    return swigValue;
  }

  public static HbComResp_AD_p2 swigToEnum(int swigValue) {
    HbComResp_AD_p2[] swigValues = HbComResp_AD_p2.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (HbComResp_AD_p2 swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + HbComResp_AD_p2.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private HbComResp_AD_p2() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private HbComResp_AD_p2(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private HbComResp_AD_p2(HbComResp_AD_p2 swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}

