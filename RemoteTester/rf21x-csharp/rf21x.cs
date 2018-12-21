/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class rf21x {
  public static RF21xHidSerialNumbers getHidSerialNumber() {
    RF21xHidSerialNumbers ret = new RF21xHidSerialNumbers(rf21xPINVOKE.getHidSerialNumber(), true);
    return ret;
  }

  public static RF21xHidSerialNumbers getHidTempPath() {
    RF21xHidSerialNumbers ret = new RF21xHidSerialNumbers(rf21xPINVOKE.getHidTempPath(), true);
    return ret;
  }

  public static bool checkHidCable(string tempPath) {
    bool ret = rf21xPINVOKE.checkHidCable(tempPath);
    if (rf21xPINVOKE.SWIGPendingException.Pending) throw rf21xPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static readonly int RF21X_DT_Unknow = rf21xPINVOKE.RF21X_DT_Unknow_get();
  public static readonly int RF21X_DT_RF217 = rf21xPINVOKE.RF21X_DT_RF217_get();
  public static readonly int RF21X_DT_RF218 = rf21xPINVOKE.RF21X_DT_RF218_get();
  public static readonly int RF21X_DT_RF219 = rf21xPINVOKE.RF21X_DT_RF219_get();
  public static readonly int RF21X_DT_RF215 = rf21xPINVOKE.RF21X_DT_RF215_get();
  public static readonly int RF21X_MT_Unknow = rf21xPINVOKE.RF21X_MT_Unknow_get();
  public static readonly int RF21X_MT_Teacher = rf21xPINVOKE.RF21X_MT_Teacher_get();
  public static readonly int RF21X_MT_Student = rf21xPINVOKE.RF21X_MT_Student_get();
  public static readonly int RF21X_MT_SetId = rf21xPINVOKE.RF21X_MT_SetId_get();
  public static readonly int RF21X_QT_Homewrok = rf21xPINVOKE.RF21X_QT_Homewrok_get();
  public static readonly int RF21X_QT_Unknow = rf21xPINVOKE.RF21X_QT_Unknow_get();
  public static readonly int RF21X_QT_Rush = rf21xPINVOKE.RF21X_QT_Rush_get();
  public static readonly int RF21X_QT_Single = rf21xPINVOKE.RF21X_QT_Single_get();
  public static readonly int RF21X_QT_Multiple = rf21xPINVOKE.RF21X_QT_Multiple_get();
  public static readonly int RF21X_QT_Number = rf21xPINVOKE.RF21X_QT_Number_get();
  public static readonly int RF21X_QT_Text = rf21xPINVOKE.RF21X_QT_Text_get();
  public static readonly int RF21X_QT_Classify = rf21xPINVOKE.RF21X_QT_Classify_get();
  public static readonly int RF21X_QT_Sort = rf21xPINVOKE.RF21X_QT_Sort_get();
  public static readonly int RF21X_Qt_JudgeOrVote = rf21xPINVOKE.RF21X_Qt_JudgeOrVote_get();
  public static readonly int RF21X_QT_SelectId = rf21xPINVOKE.RF21X_QT_SelectId_get();
  public static readonly int RF21X_QT_Control = rf21xPINVOKE.RF21X_QT_Control_get();
}