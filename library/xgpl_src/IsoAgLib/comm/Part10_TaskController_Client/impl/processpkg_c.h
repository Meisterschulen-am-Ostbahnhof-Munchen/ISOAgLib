/*
  processpkg_c.h: data object for Process-Data messages

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESSPKG_C_H
#define PROCESSPKG_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

namespace __IsoAgLib {

class ProcessPkg_c : public CanPkgExt_c
{
public:
  ProcessPkg_c( const CanPkg_c& arc_src, multiton ai_multitonInst = multitonSTD );
  ProcessPkg_c( IsoAgLib::ProcData::CommandType_t, uint16_t element, uint16_t ddi, int32_t value );
  ProcessPkg_c( uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t );

  ~ProcessPkg_c() {}

public:
  int32_t mi32_pdValue;

  IsoAgLib::ProcData::CommandType_t men_command;
  uint16_t mui16_element;
  uint16_t mui16_DDI;

private:
  /** not copyable : copy constructor/operator only declared, not defined */
  ProcessPkg_c( const ProcessPkg_c& );
  ProcessPkg_c& operator=( const ProcessPkg_c& );
};

}

#endif
