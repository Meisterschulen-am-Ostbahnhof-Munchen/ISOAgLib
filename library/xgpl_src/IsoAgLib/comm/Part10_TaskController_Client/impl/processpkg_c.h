/*
  processpkg_c.h: data object for Process-Data messages

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESSPKG_C_H
#define PROCESSPKG_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>


namespace __IsoAgLib {

class ProcessPkg_c : public CanPkgExt_c
{
public:
  enum CommandType_t {
    requestConfiguration                  = 0x0,
    configurationResponse                 = 0x1,
    requestValue                          = 0x2,
    setValue                              = 0x3,
    measurementTimeValueStart             = 0x4,
    measurementDistanceValueStart         = 0x5,
    measurementMinimumThresholdValueStart = 0x6,
    measurementMaximumThresholdValueStart = 0x7,
    measurementChangeThresholdValueStart  = 0x8,
    commandReserved1                      = 0x9,
    commandReserved2                      = 0xa,
    commandReserved3                      = 0xb,
    commandReserved4                      = 0xc,
    nack                                  = 0xd,
    taskControllerStatus                  = 0xe,
    workingsetMasterMaintenance           = 0xf,

    CommandUndefined                      = 0x10
  };

  ProcessPkg_c( const CanPkg_c& arc_src, int ai_multitonInst = 0 );
  ProcessPkg_c();
  ~ProcessPkg_c() {}

public:
  int32_t mi32_pdValue;

  CommandType_t men_command;
  uint16_t mui16_element;
  uint16_t mui16_DDI;

private:
  /** not copyable : copy constructor/operator only declared, not defined */
  ProcessPkg_c( const ProcessPkg_c& );
  ProcessPkg_c& operator=( const ProcessPkg_c& );
};

}
#endif
