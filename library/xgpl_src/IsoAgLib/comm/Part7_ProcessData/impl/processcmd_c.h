/*
  processcmd_c.h: extracts general data from ISO process messages

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESSCMD_H
#define PROCESSCMD_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/hal_typedef.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  extracts process command info from ISO process messages
*/
class ProcessCmd_c {
public:

  /** enum for specification of defined setpoint types */
  enum ValueGroup_t { noValue = 0, exactValue = 1, minValue = 2, maxValue = 4, defaultValue = 8 };

  /** enum for general commands */
  enum CommandType_t {
    noCommand                             = 0,
    requestConfiguration                  = 0x01,
    configurationResponse                 = 0x02,
    setValue                              = 0x03,
    requestValue                          = 0x04,
    taskControllerStatus                  = 0x05,
    workingsetMasterMaintenance           = 0x06,
    nack                                  = 0x09,
    measurementStop                       = 0x07,

    // measurement commands: command & 0x10 != 0
    measurementReset                      = 0x10,
    // ISO measurement
    measurementTimeValueStart             = 0x15,
    measurementDistanceValueStart         = 0x16,
    measurementMaximumThresholdValueStart = 0x17,
    measurementMinimumThresholdValueStart = 0x18,
    measurementChangeThresholdValueStart  = 0x19

  };

  /** constructor */
  ProcessCmd_c();

  /** read access for isSetpoint */
  bool checkIsSetpoint() const { return mb_isSetpoint; };

  /** read access for isRequest */
  bool checkIsRequest() const { return mb_isRequest; };

  /** read access for men_valueGroup */
  ValueGroup_t getValueGroup() const { return men_valueGroup; };

  /** read access for men_command */
  CommandType_t getCommand() const { return men_command; };

  /** set values, called in ProcessPkg_c::resolveCommand() */
  void setValues(bool mb_isSetpoint, bool mb_isRequest, ValueGroup_t men_valueGroup,
                 CommandType_t men_command);

private:

  bool mb_isSetpoint;
  bool mb_isRequest;

  /** command affects min, max, exact or default value */
  ValueGroup_t men_valueGroup;

  /** current command */
  CommandType_t men_command;

};

}
#endif
