/*
  dtccontainer.h - Stores and manages array of DTC elements

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
#ifndef DTC_CONTAINER_H
#define DTC_CONTAINER_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/iliberr_c.h>

#include "../idiagnosticstypes.h"


namespace __IsoAgLib {

/**
  This class stores and manages array of DTC elements
  @author Antoine Kandera, reviewed by Martin Wodok
*/
class DtcContainer_c
{
public:
  static const uint16_t scui16_sizeDTCList = (CONFIG_MAX_ACTIVE_DTCS) + (CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS);

  DtcContainer_c() {}
  ~DtcContainer_c() {}

  /**
    Get index of the DTC with key (SPN,FMI)
    @return index in [0..scui16_sizeDTCList-1] if found
            scui16_sizeDTCList if not found
  */
  uint16_t getDTCIndex(uint32_t SPN, IsoAgLib::FailureModeIndicator_en FMI) const;

  /**
    Get index of first not defined DTC
    @return index in [0..scui16_sizeDTCList-1] if found
            scui16_sizeDTCList if not found
  */
  uint16_t getFreeDTCIndex() const;

  /**
    @param ab_searchForActiveDtc : true for number of active DTC, false for number of previously active DTC
    @return number of active / previously active DTC
  */
  uint16_t getNumberOfDtc(bool ab_searchForActiveDtc) const;

  /** Clear the Previous Active Trouble Codes */
  void dtcClearPrevious();

  /**
    deliver an iDtc_s reference from a specific index with operator[]
    @param aui16_index index of delivered DTC [0..scui16_sizeDTCList-1]
    NOTE: no test if the index is valid !
  */
  IsoAgLib::iDtc_s& operator[](uint16_t aui16_index) { isoaglib_header_assert(aui16_index < scui16_sizeDTCList); return marr_dtc[aui16_index];}

  /**
    deliver an iDtc_s const reference from a specific index with operator[]
    @param aui16_index index of delivered DTC [0..scui16_sizeDTCList-1]
    NOTE: no test if the index is valid !
  */
  const IsoAgLib::iDtc_s& operator[](uint16_t aui16_index) const { isoaglib_header_assert(aui16_index < scui16_sizeDTCList); return marr_dtc[aui16_index];}

private:
  /// "List" of all DTCs
  /// For the real amount of the active / prev. active DTCs,
  /// iterate through "marr_dtc" and count all entries with "ui32_spn != spiNone"
  IsoAgLib::iDtc_s marr_dtc [scui16_sizeDTCList];
};

}  // __IsoAgLib
#endif
