#ifndef IFSSTRCUTS_H
#define IFSSTRCUTS_H

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

struct iFsWhitelist_c
{
  bool b_requestRemoveableMedium;
  int32_t i32_requestedManufacturer;
  uint8_t ui8_minSimultaniouslyOpenFiles;
};

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

#endif
