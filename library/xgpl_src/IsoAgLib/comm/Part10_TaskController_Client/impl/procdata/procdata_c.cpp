/*
  procdata_c.cpp: Class for handling Process Data (from DDOP)

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
#include "procdata_c.h"

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {


  ProcData_c::ProcData_c( )
    : PdLocal_c()
#ifndef NDEBUG
    , m_dpd( NULL )
    , m_det( NULL )
#endif
  {
  }


  void ProcData_c::init(
      const DeviceObjectDpd_c& dpd,
      const DeviceObjectDet_c& det,
      SetpointHandler_c* setpointhandler )
  {
    PdLocal_c::init(
      dpd.ddi(),
      det.elementNumber(),
      dpd.method(),
      dpd.hasProperty( IsoAgLib::ProcData::Settable ),
      dpd.hasProperty( IsoAgLib::ProcData::ControlSource ),
      setpointhandler );

#ifndef NDEBUG
    m_dpd = &dpd;
    m_det = &det;
#endif

    isoaglib_assert(
      ( DDI() != IsoAgLib::ProcData::DefaultDataLoggingDDI ) ||
        ( isMethodSet( IsoAgLib::ProcData::TimeInterval ) &&
          isMethodSet( IsoAgLib::ProcData::DistInterval ) &&
          isMethodSet( IsoAgLib::ProcData::ThresholdLimit ) &&
          isMethodSet( IsoAgLib::ProcData::OnChange ) &&
          isMethodSet( IsoAgLib::ProcData::Total ) ) );

    isoaglib_assert(
      !dpd.hasProperty( IsoAgLib::ProcData::ControlSource ) ||
        ( isMethodSet( IsoAgLib::ProcData::TimeInterval ) &&
          isMethodSet( IsoAgLib::ProcData::OnChange ) ) );
  }


  inline void
  ProcData_c::close()
  {
  }

}
