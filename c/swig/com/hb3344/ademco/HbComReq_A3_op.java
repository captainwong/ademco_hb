/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public enum HbComReq_A3_op {
  HbComReq_A3_p3_del(libademcoJNI.HbComReq_A3_p3_del_get()),
  HbComReq_A3_p3_learn(libademcoJNI.HbComReq_A3_p3_learn_get()),
  HbComReq_A3_p3_stop(libademcoJNI.HbComReq_A3_p3_stop_get()),
  HbComReq_A3_p3_modify(libademcoJNI.HbComReq_A3_p3_modify_get());

  public final int swigValue() {
    return swigValue;
  }

  public static HbComReq_A3_op swigToEnum(int swigValue) {
    HbComReq_A3_op[] swigValues = HbComReq_A3_op.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (HbComReq_A3_op swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + HbComReq_A3_op.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private HbComReq_A3_op() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private HbComReq_A3_op(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private HbComReq_A3_op(HbComReq_A3_op swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}
