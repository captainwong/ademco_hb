/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.hb3344.ademco;

public class HbTimer {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected HbTimer(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(HbTimer obj) {
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
        libademcoJNI.delete_HbTimer(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setArmAt(HbMachineTimePoint value) {
    libademcoJNI.HbTimer_armAt_set(swigCPtr, this, HbMachineTimePoint.getCPtr(value), value);
  }

  public HbMachineTimePoint getArmAt() {
    long cPtr = libademcoJNI.HbTimer_armAt_get(swigCPtr, this);
    return (cPtr == 0) ? null : new HbMachineTimePoint(cPtr, false);
  }

  public void setDisarmAt(HbMachineTimePoint value) {
    libademcoJNI.HbTimer_disarmAt_set(swigCPtr, this, HbMachineTimePoint.getCPtr(value), value);
  }

  public HbMachineTimePoint getDisarmAt() {
    long cPtr = libademcoJNI.HbTimer_disarmAt_get(swigCPtr, this);
    return (cPtr == 0) ? null : new HbMachineTimePoint(cPtr, false);
  }

  public HbTimer() {
    this(libademcoJNI.new_HbTimer(), true);
  }

}
