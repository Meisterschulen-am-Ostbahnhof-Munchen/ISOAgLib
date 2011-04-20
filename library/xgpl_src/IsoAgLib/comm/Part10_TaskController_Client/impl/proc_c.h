/*
  proc_c.h: class with special (enum) types for ProcessData management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROC_H
#define PROC_H


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  class with special (enum) types for ProcessData management
  @author Dipl.-Inform. Achim Spangler
*/
class Proc_c {
public:

/** enum type definition as part of class */
  enum type_t {NullType = 0, DistProp = 1, TimeProp = 4, ValIncr = 8, 
               MaximumThreshold = 0x10, MinimumThreshold = 0x20, OnChange = 0x40, Counter };

  /** enum type for distinguish between Local or Remote data */
  enum homeEcu_t {Local = 1, Remote = 2};

  /** enum type for setting test conditions for setpoint validity */
  enum testSetpoint_t{TestAllowed = 1, TestMaster = 2};

  /** enum type for to sending values */
  enum doSend_t {DoNone = 0, DoVal = 1, DoMed = 2, DoInteg = 4, DoDelta = 8, DoAccel = 16, DoMin = 32, DoMax = 64,
                 // ISO specific
                 DoValForExactSetpoint = 0x80, DoValForDefaultSetpoint = 0x100, DoValForMinSetpoint = 0x200, DoValForMaxSetpoint = 0x400,
                 DoValForMinMeasurement = 0x800, DoValForMaxMeasurement = 0x1000
                };

  /** enum type for proportional type used for calculating accumulated vaslues */
  enum accumProp_t {AccumNone, AccumDist = 1, AccumTime = 4};

  /** enum type for special DDIs */
  enum specialDDI_t {defaultDataLoggingDDI = 0xDFFF};

  /** enum type for task status */
  enum taskStatus_t {NoTask = 0, Initial = 1, Running = 2, Suspended = 3, Finished = 4 };

  enum thresholdValues_t {ThresholdMaximumStopVal = int32_t(0x80000001),
                          ThresholdMinimumStopVal = int32_t(0x7fffffff),
                          ThresholdChangeStopVal = 0,
                          ThresholdEachChangeVal = 1 };

  /** default unused constructor */
  Proc_c(){};
  /** default unused destructor */
  ~Proc_c(){};
};

}
#endif
