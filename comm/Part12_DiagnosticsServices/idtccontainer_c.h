/*
  idtccontainer_c.h - Interface access class for the DTC container

  (C) Copyright 2015 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDTCCONTAINER_H
#define IDTCCONTAINER_H

#include "impl/dtccontainer_c.h"


namespace IsoAgLib {

  class iDiagnosticsServices_c;
  
/**
  Provides interface for accessing the diagnostics services DM1/DM2/DM3 manager
  NOTE: this class is not Application-constructable
  @short Interface access class managing diagnostics services DM1/DM2/DM3
  @author Antoine Kandera, reviewed by Martin Wodok
*/
class iDtcContainer_c : private __IsoAgLib::DtcContainer_c
{
public:
  using __IsoAgLib::DtcContainer_c::scui16_sizeDTCList;

 /**
    Get index of the DTC with key (SPN,FMI)
    @return index in [0..scui16_sizeDTCList-1] if found
            scui16_sizeDTCList if not found
  */
  uint16_t getDTCIndex(uint32_t SPN, IsoAgLib::FailureModeIndicator_en FMI) const { return __IsoAgLib::DtcContainer_c::getDTCIndex(SPN, FMI); }

  /**
    @param ab_searchForActiveDtc : true for number of active DTC, false for number of previously active DTC
    @return number of active / previously active DTC
  */
  uint16_t getNumberOfDtc(bool ab_searchForActiveDtc) const { return __IsoAgLib::DtcContainer_c::getNumberOfDtc(ab_searchForActiveDtc); }

  /**
    deliver a iDtc_s reference from a specific index with operator[]
    @param aui16_index index of delivered DTC [0..scui16_sizeDTCList-1]
    NOTE: no test if the index is valid !
    Writing to a DTC in this container shall only be done on load of the stored DTCs from non-volatile!
  */
  IsoAgLib::iDtc_s& operator[](uint16_t aui16_index) { return __IsoAgLib::DtcContainer_c::operator[](aui16_index); }

  /**
    deliver a iDtc_s const reference from a specific index with operator[]
    @param aui16_index index of delivered DTC [0..scui16_sizeDTCList-1]
    NOTE: no test if the index is valid !
  */
  const IsoAgLib::iDtc_s& operator[](uint16_t aui16_index) const { return __IsoAgLib::DtcContainer_c::operator[](aui16_index); }

private:

  friend class __IsoAgLib::IdentItem_c;
  friend class iDiagnosticsServices_c;
};

}

#endif
