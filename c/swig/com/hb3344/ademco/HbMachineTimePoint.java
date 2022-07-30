/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public class HbMachineTimePoint {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected HbMachineTimePoint(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(HbMachineTimePoint obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libademcoJNI.delete_HbMachineTimePoint(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setHour(short value) {
    libademcoJNI.HbMachineTimePoint_hour_set(swigCPtr, this, value);
  }

  public short getHour() {
    return libademcoJNI.HbMachineTimePoint_hour_get(swigCPtr, this);
  }

  public void setMinute(short value) {
    libademcoJNI.HbMachineTimePoint_minute_set(swigCPtr, this, value);
  }

  public short getMinute() {
    return libademcoJNI.HbMachineTimePoint_minute_get(swigCPtr, this);
  }

  public HbMachineTimePoint() {
    this(libademcoJNI.new_HbMachineTimePoint(), true);
  }

}
