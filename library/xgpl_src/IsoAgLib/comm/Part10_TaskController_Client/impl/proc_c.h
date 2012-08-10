/*
  proc_c.h: class with special (enum) types for ProcessData management

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROC_H
#define PROC_H

#include <IsoAgLib/hal/hal_typedef.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  class with special (enum) types for ProcessData management
  @author Dipl.-Inform. Achim Spangler
*/
class Proc_c {
public:

/** enum type definition as part of class */
  enum measurementCommand_t { MeasurementCommandTimeProp =          0x4,
                              MeasurementCommandDistProp =          0x5,
                              MeasurementCommandMinimumThreshold =  0x6,
                              MeasurementCommandMaximumThreshold =  0x7,
                              MeasurementCommandOnChange =          0x8 };

  enum triggerMethod_t { MethodTimeInterval = 0,
                         MethodDistInterval = 1,
                         MethodThresholdLimit = 2, 
                         MethodOnChange = 3,
                         MethodTotal = 4 };

  enum nackResponse_t { NackProcessDataCommandNotSupported = 0x01, // Bit 0 = 1 Process Data Command not supported
                        NackInvalidElementNumber = 0x02, // Bit 1 = 1 Invalid Element Number
                        NackDDINoSupportedByElement = 0x04, // Bit 2 = 1 DDI not supported by element
                        NackTriggerMethodNotSupported = 0x08, // Bit 3 = 1 Trigger method not supported
                        NackProcessDataNotSetable = 0x10, // Bit 4 = 1 Process Data not setable
                        NackInvalidOrUnsupportedIntervalOrThreshold = 0x20, // Bit 5 = 1 Invalid or unsupported interval or threshold
                        NackReserved1 = 0x40,
                        NackReserved2 = 0x80,
                        NackUndefined };

  static bool isMethodSet(uint8_t value, triggerMethod_t method) { return ((value & (0x1 << method)) != 0 ); }

  /** enum type for to sending values */
  enum doSend_t {DoNone = 0, DoVal = 1};

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
