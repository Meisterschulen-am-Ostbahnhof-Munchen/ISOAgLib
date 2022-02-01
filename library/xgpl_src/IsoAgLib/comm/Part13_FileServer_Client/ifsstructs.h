/*
  ifsstructs.h

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
#ifndef IFSSTRUCTS_H
#define IFSSTRUCTS_H

#include <IsoAgLib/isoaglib_config.h>

#include <cstdlib>
#include <vector>


// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  * struct used to save all available information on a directory.
  */
struct iFsDirectory
{
  uint8_t *pui8_filename;
  bool b_caseSensitive;
  bool b_removable;
  bool b_longFilenames;
  bool b_isDirectory;
  bool b_isVolume;
  bool b_hidden;
  bool b_readOnly;
  uint16_t ui16_date;
  uint16_t ui16_time;
  uint32_t ui32_size;
};

typedef struct iFsDirectory * iFsDirectoryPtr;
typedef std::vector<iFsDirectoryPtr> iFsDirList;

struct iFsWhitelist_c
{
  /// @todo Extend by flag for long-filename (and probably Case(in)sensitive names)
  bool b_requestRemoveableMedium;
  int32_t i32_requestedManufacturer;
  uint8_t ui8_minSimultaniouslyOpenFiles;
};

typedef std::vector<iFsWhitelist_c> iFsWhitelistList;

enum iFsCommandErrors
{
  fsCommandNoError,
  fsCommandBusy,
  fsCommandNotPressent,
  fsCommandWrongFlag,
  fsWrongFsVersion
};

enum iFsError
{
  fsSuccess = 0x00,
  fsAccessDenied,
  fsInvalidAccess,
  fsTooManyFilesOpen,
  fsFileOrPathNotFound,
  fsInvalidHandle,
  fsInvalidGivenSourceName,
  fsInvalidGivenDestinationName,
  fsVolumeOutOfFreeSpace,
  fsFailureDuringAWriteOperation,
  fsVolumeIsPossiblyNotInitialized,
  fsFailureDuringAReadOperation,
  fsFunctionNotSupported,
  fsInvalidRequestLength = 0x2A,
  fsOutOfMemory,
  fsAnyOtherError,
  fsEndOfFileReached,
  //start of self-defined error codes
  fsFileserverNotResponding = 0xB0,
  fsFileserverVersionNotSupported
};

//End namespace
}

#endif
