/*
  isobus_c.cpp: Access class for the managed ISO11783-CAN-instances

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "isobus_c.h"

#include <IsoAgLib/util/iassert.h>

#ifdef DEF_Stream_IMPL
  #include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
  #include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#endif
#ifdef USE_ISO_PROPRIETARY_PGN
  #include <IsoAgLib/comm/Part3_ProprietaryMessages/impl/proprietarymessagehandler_c.h>
#endif
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.h>
#endif
#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Part7_ProcessData/impl/process_c.h>
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
#ifdef USE_TIME_GPS
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.h>
#endif
#ifdef USE_ISO_FILESERVER_CLIENT
  #include <IsoAgLib/comm/Part13_FileServer_Client/impl/fsmanager_c.h>
#endif


namespace __IsoAgLib {

bool
IsoBus_c::init (uint8_t aui8_busNumber)
{
  /// CAN-Bus
  const bool cb_canSuccess =
    getCanInstance4Comm().init (aui8_busNumber, 250, Ident_c::ExtendedIdent);

  /// Part 5 - Network Management
  getIsoRequestPgnInstance4Comm().init();
  getIsoMonitorInstance4Comm().init();
  getIsoFilterManagerInstance4Comm().init();

  /// Part 3 - Data Link
  #ifdef DEF_Stream_IMPL
    getMultiReceiveInstance4Comm().init();
    getMultiSendInstance4Comm().init();
  #endif
  /// Part 3 - Proprietary PGNs
  #ifdef USE_ISO_PROPRIETARY_PGN
  getProprietaryMessageHandlerInstance4Comm().init();
  #endif
  /// Part 6 - Virtual Terminal (Client)
  #ifdef USE_ISO_TERMINAL
    getIsoTerminalInstance4Comm().init();
  #endif
  /// Part 7 - Application (Tractor-Client)
  #ifdef USE_TRACTOR_GENERAL
    getTracGeneralInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_FACILITIES
    getTracFacilitiesInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_MOVE
    getTracMoveInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_PTO
    getTracPtoInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_LIGHT
    getTracLightInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_AUX
    getTracAuxInstance4Comm().init();
  #endif
  #ifdef USE_TIME_GPS
    getTimePosGpsInstance4Comm().init();
  #endif
  /// Part 10 - Task Controller (Client)
  #ifdef USE_PROCESS
    getProcessInstance4Comm().init();
  #endif
  /// Part 12 - Diagnostics Services
  // nop - only additional items for Part 5 currently
  /// Part 13 - File Server (Client)
  #ifdef USE_ISO_FILESERVER_CLIENT
    getFsManagerInstance4Comm().init();
  #endif

  return cb_canSuccess;
}


void
IsoBus_c::close()
{
  /// Part 13 - File Server (Client)
  #ifdef USE_ISO_FILESERVER_CLIENT
    getFsManagerInstance4Comm().close();
  #endif
  /// Part 12 - Diagnostics Services
  // nop - only additional items for Part 5 currently
  /// Part 10 - Task Controller (Client)
  #ifdef USE_PROCESS
    getProcessInstance4Comm().close();
  #endif
  /// Part 7 - Application (Tractor-Client)
  #ifdef USE_TIME_GPS
    getTimePosGpsInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_AUX
    getTracAuxInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_LIGHT
    getTracLightInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_PTO
    getTracPtoInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_MOVE
    getTracMoveInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_FACILITIES
    getTracFacilitiesInstance4Comm().close();
  #endif
  #ifdef USE_TRACTOR_GENERAL
    getTracGeneralInstance4Comm().close();
  #endif
  /// Part 6 - Virtual Terminal (Client)
  #ifdef USE_ISO_TERMINAL
    getIsoTerminalInstance4Comm().close();
  #endif
  /// Part 3 - Proprietary PGNs
  #ifdef USE_ISO_PROPRIETARY_PGN
  getProprietaryMessageHandlerInstance4Comm().close();
  #endif
  /// Part 3 - Data Link
  #ifdef DEF_Stream_IMPL
    getMultiSendInstance4Comm().close();
    getMultiReceiveInstance4Comm().close();
  #endif
  /// Part 5 - Network Management
  getIsoFilterManagerInstance4Comm().close();
  getIsoMonitorInstance4Comm().close();
  getIsoRequestPgnInstance4Comm().close();

  /// CAN-Bus
  getCanInstance4Comm().close();
}


IsoBus_c&
IsoBus_c::operator<<(CanPkgExt_c& acrc_src)
{ // when some IsoBus-module is send, the IsoBus has to be initialized!
  isoaglib_assert (getCanInstance4Comm().isReady2Send());
  if ( ! getCanInstance4Comm().isReady2Send() )
    return *this;

  // when ISO is compiled, we must make sure, that the ISO specific
  // resolving is only used for extended ident messages
  if ( acrc_src.identType() == Ident_c::ExtendedIdent )
  {
    // check if source and destination address are valid
    if ( ! acrc_src.resolveSendingInformation() )
    { // preconditions for correct sending are not fullfilled -> set error state
      getILibErrInstance().registerError(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can);
      return *this;
    }
  }
  (void) getCanInstance4Comm().operator<<( static_cast<CanPkg_c&>(acrc_src) );
  return *this;
}


FilterBox_c*
IsoBus_c::insertStandardIsoFilter(
  __IsoAgLib::CanCustomer_c& ar_customer,
  uint32_t aui32_pgn,
  bool ab_reconfigImmediate)
{
  int8_t i8_dataLen = 0;

  switch(aui32_pgn)
  {
    case REQUEST_PGN_MSG_PGN:
    case (REQUEST_PGN_MSG_PGN | 0xFF): //xxeaffxx
      i8_dataLen = 3;
      break;

    case ACKNOWLEDGEMENT_PGN: /**variable data len : see ISO/CD ISO-11783-12 */
    case CLIENT_TO_FS_PGN:    /** variable data len : see ISO-11783 -13 */
    case FS_TO_CLIENT_PGN:    /** variable data len : see ISO-11783 -13 */
    case PROPRIETARY_A_PGN: /** multipacket supported, data len 0..1785 bytes*/
    case PROPRIETARY_A2_PGN: /** multipacket supported, data len 0..1785 bytes*/
    case SOFTWARE_IDENTIFICATION_PGN: /** variable data len: see ISO/CD ISO-11783-12 */
    case ECU_IDENTIFICATION_INFORMATION_PGN: /** variable data len: see ISO/CD ISO-11783-12 */
    case PROPRIETARY_B_PGN: /** variable data len: see SAE J1939 71 */
      i8_dataLen = -1;
      break;

    default:
      i8_dataLen= 8;
      break;
  }

  return insertFilter (ar_customer,( 0x3FFFF00UL), (aui32_pgn << 8), ab_reconfigImmediate, i8_dataLen);
}

IsoBus_c &getIsoBusInstance( uint8_t aui8_instance)
{
  MACRO_MULTITON_GET_INSTANCE_BODY(IsoBus_c, PRT_INSTANCE_CNT, aui8_instance);
}


} // __IsoAgLib
