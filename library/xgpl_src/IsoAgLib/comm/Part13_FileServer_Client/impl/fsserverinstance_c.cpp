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

void FsServerInstance_c::setVolumes(IsoAgLib::iFsDirList v_inVolumes)
{
  IsoAgLib::iFsDirectoryPtr ps_tmpDir;

  while (!v_volumes.empty())
  {
    ps_tmpDir = v_volumes.back();
    v_volumes.pop_back();
    if (ps_tmpDir->pui8_filename != NULL)
      delete [] ps_tmpDir->pui8_filename;
    delete ps_tmpDir;
  }

  IsoAgLib::iFsDirectoryPtr ps_newDir;
  char * fileName;
  IsoAgLib::iFsDirList::iterator it_volumesEnd = v_inVolumes.end();
  for (IsoAgLib::iFsDirList::iterator it_volumes = v_inVolumes.begin(); it_volumes != it_volumesEnd; ++it_volumes)
  {
    ps_tmpDir = *it_volumes;
    ps_newDir = new IsoAgLib::iFsDirectory();
    *ps_newDir = *ps_tmpDir;

    fileName = (char *)ps_tmpDir->pui8_filename;
    ps_newDir->pui8_filename = new uint8_t[CNAMESPACE::strlen(fileName) + 1];
    CNAMESPACE::strcpy((char *)ps_newDir->pui8_filename, fileName);

    v_volumes.push_back(ps_newDir);
  }

  en_initStatus = unreported;
}

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

FsServerInstance_c::~FsServerInstance_c()
{
  IsoAgLib::iFsDirectoryPtr ps_tmpDir;

  while (!v_volumes.empty())
  {
    ps_tmpDir = v_volumes.back();
    v_volumes.pop_back();
    if (ps_tmpDir->pui8_filename != NULL)
      delete [] ps_tmpDir->pui8_filename;
    delete ps_tmpDir;
  }
}

}
