/*
  fsserverinstance_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "fsserverinstance_c.h"
#include "../ifsserverinstance_c.h"

namespace __IsoAgLib
{

FsServerInstance_c::FsServerInstance_c(const IsoItem_c &pref_newItem, const FsManager_c &ref_fsManager) : c_fsManager(ref_fsManager), pc_newItem(pref_newItem)
{
  i32_lastTime = -1;
  i32_propertiesReqeusted = -1;
  b_propertiesSet = false;
  en_initStatus = online;
}

void FsServerInstance_c::updateServerStatus(uint8_t rui8_busy, uint8_t rui8_nrOpenFiles, int32_t ri32_lastTime)
{
  en_busy = FsServerInstance_c::FsState_en(rui8_busy);
  ui8_nrOpenFiles = rui8_nrOpenFiles;
  i32_lastTime = ri32_lastTime;
}

void FsServerInstance_c::setFsProperties(uint8_t rui8_versionNumber, uint8_t rui8_maximumNrFiles, bool rb_isMultivolumes)
{
  ui8_versionNumber = rui8_versionNumber;
  ui8_maximumNrFiles = rui8_maximumNrFiles;
  b_isMultivolumes = rb_isMultivolumes;
  b_propertiesSet = true;
}

/** explicit conversion to reference of interface class type */
IsoAgLib::iFsServerInstance_c* FsServerInstance_c::toInterfacePointer()
{
  return static_cast<IsoAgLib::iFsServerInstance_c*>(this);
}

}
