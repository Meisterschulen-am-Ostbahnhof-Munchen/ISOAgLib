/*
  fsserverinstance_c.cpp

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

// own
#include "fsserverinstance_c.h"
#include "../ifsserverinstance_c.h"
#include "fsmanager_c.h"

// ISOAgLib
#include <IsoAgLib/util/iassert.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#pragma warning( disable : 4996 )
#endif

namespace __IsoAgLib
{

FsServerInstance_c::FsServerInstance_c(const IsoItem_c &pref_newItem, FsManager_c &ref_fsManager)
  : c_fsManager(ref_fsManager)
  , pc_newItem(pref_newItem)
  , en_busy(busy_none)
  , ui8_nrOpenFiles(0)
  , i32_lastTime(-1)
  , e_versionNumber(FsVersionDIS)
  , ui8_maximumNrFiles(0)
  , ui8_capabilities(0)
  , v_volumes()
  , men_state(offline)
{
#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "Fileserver created offline (received Address Claim)." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
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

#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "Fileserver destroyed (stopped Address Claim)." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


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
    ps_newDir->pui8_filename = new uint8_t[std::strlen(fileName) + 1];
    std::strcpy((char *)ps_newDir->pui8_filename, fileName);

    v_volumes.push_back(ps_newDir);
  }
}


void FsServerInstance_c::setFsProperties(FsVersion_en re_versionNumber, uint8_t rui8_maximumNrFiles, uint8_t rui8_capabilities)
{
  e_versionNumber = re_versionNumber;
  ui8_maximumNrFiles = rui8_maximumNrFiles;
  ui8_capabilities = rui8_capabilities;
}

/** explicit conversion to reference of interface class type */
IsoAgLib::iFsServerInstance_c* FsServerInstance_c::toInterfacePointer()
{
  return static_cast<IsoAgLib::iFsServerInstance_c*>(this);
}


void 
FsServerInstance_c::processFsToGlobal( const CanPkgExt_c& pkg )
{
  if (getState() == unusable)
    return;

  switch (pkg.getUint8Data(0))
  {
    // FS Status
    case 0x00:
      switch (getState())
      {
        case FsServerInstance_c::offline:
          // FS comes (back) online!
          setState (online);
          break;

        default:
          // nothing to do if we weren't offline
          break;
      }
      en_busy = FsBusy_en(pkg.getUint8Data(1));
      ui8_nrOpenFiles = pkg.getUint8Data(2);
      i32_lastTime = pkg.time();
      break;
  }
}


void
FsServerInstance_c::timeEvent()
{
  // if we're offline,
  switch (getState())
  {
    case offline:    // nothing to do until we're going online (by FS Stat Msg)
    case unusable:   // nothing to do if we're in an error-state. We're just unusable then.
      break;

    case usablePending: // this state was just to decouple the notification-callback from the delete of the FsCommand, which caused MultiReceive problems (two commands with registered MR)
      setState( usable );
      break;

    case online:
    case usable:
      // detect time-out
      if (getLastTime() != -1 && (HAL::getTime () - getLastTime()) > 6000)
        setState (offline);
      break;
  }
}

void
FsServerInstance_c::setState (FsState_en aen_newState)
{
  // Store old state for later change-detection
  FsState_en en_oldState = men_state;

  // Set new state
  men_state = aen_newState;

#if DEBUG_FILESERVER
  switch (en_oldState)
  {
    case offline:
      INTERNAL_DEBUG_DEVICE << "Fileserver was offline ==> ";
      break;
    case online:
      INTERNAL_DEBUG_DEVICE << "Fileserver was online ==> ";
      break;
    case usablePending:
      INTERNAL_DEBUG_DEVICE << "Fileserver was usablePending ==> ";
      break;
    case usable:
      INTERNAL_DEBUG_DEVICE << "Fileserver was usable ==> ";
      break;
    case unusable:
      INTERNAL_DEBUG_DEVICE << "Fileserver was unusable ==> ";
      break;
    default:
      isoaglib_assert(!"FS-Client: FsServerInstance_c: Was currently in an invalid state!");
      break;
  }
  switch (aen_newState)
  {
    case offline:
      INTERNAL_DEBUG_DEVICE << "Fileserver dropped offline (>6s loss of FS Status Message)." << INTERNAL_DEBUG_DEVICE_ENDL;
      break;
    case online:
      INTERNAL_DEBUG_DEVICE << "Fileserver got online (first reception of FS Status Message)." << INTERNAL_DEBUG_DEVICE_ENDL;
      break;
    case usablePending:
      INTERNAL_DEBUG_DEVICE << "Fileserver got usablePending (initialization complete and about to notify client on this FS)." << INTERNAL_DEBUG_DEVICE_ENDL;
      break;
    case usable:
      INTERNAL_DEBUG_DEVICE << "Fileserver got usable (clients do now know of this FS and can use it)." << INTERNAL_DEBUG_DEVICE_ENDL;
      break;
    case unusable:
      INTERNAL_DEBUG_DEVICE << "Fileserver encountered an error and is unusable." << INTERNAL_DEBUG_DEVICE_ENDL;
      break;
    default:
      isoaglib_assert(!"FS-Client: FsServerInstance_c: Trying to set to invalid state!");
      break;
  }
#endif

  /// Propagate state change (if it is a change) to FsManager_c
  if (aen_newState != en_oldState)
  { // let all FsCSC know that the FS has changed its state...
    c_fsManager.notifyOnFileserverStateChange (*this, en_oldState);
  }
}


}
