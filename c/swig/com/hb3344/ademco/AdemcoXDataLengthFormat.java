/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public enum AdemcoXDataLengthFormat {
  TWO_HEX,
  FOUR_DECIMAL;

  public final int swigValue() {
    return swigValue;
  }

  public static AdemcoXDataLengthFormat swigToEnum(int swigValue) {
    AdemcoXDataLengthFormat[] swigValues = AdemcoXDataLengthFormat.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (AdemcoXDataLengthFormat swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + AdemcoXDataLengthFormat.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private AdemcoXDataLengthFormat() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private AdemcoXDataLengthFormat(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private AdemcoXDataLengthFormat(AdemcoXDataLengthFormat swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}

