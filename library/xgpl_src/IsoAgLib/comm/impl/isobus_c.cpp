/*
  isobus_c.cpp: Access class for the managed ISO11783-CAN-instances

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "isobus_c.h"

#include <IsoAgLib/util/iassert.h>

// (E)TP is always included when ISO11783 support is enabled!
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#ifdef USE_ISO_PROPRIETARY_PGN
  #include <IsoAgLib/comm/Part3_ProprietaryMessages/impl/proprietarymessagehandler_c.h>
#endif
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#ifdef HAL_USE_SPECIFIC_FILTERS
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#endif
#ifdef USE_ISO_VIRTUALTERMINAL_CLIENT
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclient_c.h>
#endif
#ifdef USE_ISO_TASKCONTROLLER_CLIENT
  #include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#endif
#ifdef USE_TRACTOR_GENERAL
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracgeneral_c.h>
#endif
#ifdef USE_TRACTOR_MOVE
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif
#ifdef USE_TRACTOR_PTO
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracpto_c.h>
#endif
#ifdef USE_TRACTOR_FACILITIES
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracfacilities_c.h>
#endif
#ifdef USE_TRACTOR_LIGHT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/traclight_c.h>
#endif
#ifdef USE_TRACTOR_AUX
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracaux_c.h>
#endif
#ifdef USE_TRACTOR_GUIDANCE
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracguidance_c.h>
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracguidancecommand_c.h>
#endif
#ifdef USE_TIME_GPS
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.h>
#endif
#ifdef USE_TIME_DATE
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/timedate_c.h>
#endif
#ifdef USE_GNSS
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/gnss_c.h>
#endif
#ifdef USE_TRACTOR
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractor_c.h>
#endif
#ifdef USE_TRACTOR_GROUND_BASED
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorgroundbased_c.h>
#endif
#ifdef USE_TRACTOR_MACHINE_SELECTED
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractormachineselected_c.h>
#endif
#ifdef USE_TRACTOR_FRONT_HITCH
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorfronthitch_c.h>
#endif
#ifdef USE_TRACTOR_REAR_HITCH
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorrearhitch_c.h>
#endif
#ifdef USE_ISB_CLIENT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/isbclient_c.h>
#endif
#ifdef USE_ISO_FILESERVER_CLIENT
  #include <IsoAgLib/comm/Part13_FileServer_Client/impl/fsmanager_c.h>
#endif


namespace __IsoAgLib {

bool
IsoBus_c::init (uint8_t aui8_busNumber)
{
  if (getCanInstance( getMultitonInst() ).initialized())
    return false;

  /// CAN-Bus
  const bool cb_canSuccess =
    getCanInstance( getMultitonInst() ).init (aui8_busNumber, 250);

  if (!cb_canSuccess)
    return false;

  /// Part 5 - Network Management
  getIsoRequestPgnInstance( getMultitonInst() ).init();
  getIsoMonitorInstance( getMultitonInst() ).init();
#ifdef HAL_USE_SPECIFIC_FILTERS
  getIsoFilterManagerInstance( getMultitonInst() ).init();
#endif

  /// Part 3 - Data Link
  getMultiReceiveInstance( getMultitonInst() ).init();
  getMultiSendInstance( getMultitonInst() ).init();

  /// Part 3 - Proprietary PGNs
  #ifdef USE_ISO_PROPRIETARY_PGN
  getProprietaryMessageHandlerInstance( getMultitonInst() ).init();
  #endif

  /// Part 6 - Virtual Terminal (Client)
  #ifdef USE_ISO_VIRTUALTERMINAL_CLIENT
    getVtClientInstance( getMultitonInst() ).init();
  #endif

  /// Part 7 - Application (Tractor-Client)
  #ifdef USE_TRACTOR_GENERAL
    getTracGeneralInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_FACILITIES
    getTracFacilitiesInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_MOVE
    getTracMoveInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_PTO
    getTracPtoInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_LIGHT
    getTracLightInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_AUX
    getTracAuxInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_GUIDANCE
    getTracGuidanceInstance( getMultitonInst() ).init();
    getTracGuidanceCommandInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TIME_GPS
    getTimePosGpsInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TIME_DATE
    getTimeDateInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_GNSS
    getGnssInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_ISB_CLIENT
    getIsbClientInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR
    getTractorInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_GROUND_BASED
    getTractorGroundBasedInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_MACHINE_SELECTED
    getTractorMachineSelectedInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_FRONT_HITCH
    getTractorFrontHitchInstance( getMultitonInst() ).init();
  #endif
  #ifdef USE_TRACTOR_REAR_HITCH
    getTractorRearHitchInstance( getMultitonInst() ).init();
  #endif

  #ifdef USE_ISO_TASKCONTROLLER_CLIENT
    getTcClientInstance( getMultitonInst() ).init();
  #endif

  /// Part 12 - Diagnostics Services
  // nop - only additional items for Part 5 currently

  /// Part 13 - File Server (Client)
  #ifdef USE_ISO_FILESERVER_CLIENT
    getFsManagerInstance( getMultitonInst() ).init();
  #endif

  return true;
}


bool
IsoBus_c::close()
{
  if (!getCanInstance( getMultitonInst() ).initialized())
    return false;

  /// Part 13 - File Server (Client)
  #ifdef USE_ISO_FILESERVER_CLIENT
    getFsManagerInstance( getMultitonInst() ).close();
  #endif

  /// Part 12 - Diagnostics Services
  // nop - only additional items for Part 5 currently

  /// Part 10 - Task Controller (Client)
  #ifdef USE_ISO_TASKCONTROLLER_CLIENT
    getTcClientInstance( getMultitonInst() ).close();
  #endif

  /// Part 7 - Application (Tractor-Client)
  #ifdef USE_TRACTOR_REAR_HITCH
    getTractorRearHitchInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_FRONT_HITCH
    getTractorFrontHitchInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_MACHINE_SELECTED
    getTractorMachineSelectedInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_GROUND_BASED
    getTractorGroundBasedInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR
    getTractorInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_ISB_CLIENT
    getIsbClientInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_GNSS
    getGnssInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TIME_DATE
    getTimeDateInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TIME_GPS
    getTimePosGpsInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_GUIDANCE
    getTracGuidanceCommandInstance( getMultitonInst() ).close();
    getTracGuidanceInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_AUX
    getTracAuxInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_LIGHT
    getTracLightInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_PTO
    getTracPtoInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_MOVE
    getTracMoveInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_FACILITIES
    getTracFacilitiesInstance( getMultitonInst() ).close();
  #endif
  #ifdef USE_TRACTOR_GENERAL
    getTracGeneralInstance( getMultitonInst() ).close();
  #endif

  /// Part 6 - Virtual Terminal (Client)
  #ifdef USE_ISO_VIRTUALTERMINAL_CLIENT
    getVtClientInstance( getMultitonInst() ).close();
  #endif

  /// Part 3 - Proprietary PGNs
  #ifdef USE_ISO_PROPRIETARY_PGN
  getProprietaryMessageHandlerInstance( getMultitonInst() ).close();
  #endif

  /// Part 3 - Data Link
  getMultiSendInstance( getMultitonInst() ).close();
  getMultiReceiveInstance( getMultitonInst() ).close();

  /// Part 5 - Network Management
#ifdef HAL_USE_SPECIFIC_FILTERS
  getIsoFilterManagerInstance( getMultitonInst() ).close();
#endif
  getIsoMonitorInstance( getMultitonInst() ).close();
  getIsoRequestPgnInstance( getMultitonInst() ).close();

  /// CAN-Bus
  getCanInstance( getMultitonInst() ).close();

  return true;
}


IsoBus_c&
IsoBus_c::operator<<(CanPkgExt_c& acrc_src)
{
  isoaglib_assert( acrc_src.identType() == Ident_c::ExtendedIdent );

  // check if source and destination address are valid
  if ( ! acrc_src.resolveSendingInformation( getMultitonInst() ) )
  { // preconditions for correct sending are not fullfilled -> set error state
#if 0
    /* check is temporary disabled! */
    getILibErrInstance().registerError(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can);
#endif
    return *this;
  }

  (void) getCanInstance4Comm().operator<<( static_cast<CanPkg_c&>(acrc_src) );
  return *this;
}

IsoBus_c&
IsoBus_c::operator<<(CanPkg_c& acrc_src)
{
  if( acrc_src.identType() != Ident_c::StandardIdent )
  {
    return *this;
  }

  (void) getCanInstance( getMultitonInst() ).operator<<( acrc_src );
  return *this;
}


IsoBus_c &getIsoBusInstance( unsigned instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(IsoBus_c, PRT_INSTANCE_CNT, instance);
}


} // __IsoAgLib
