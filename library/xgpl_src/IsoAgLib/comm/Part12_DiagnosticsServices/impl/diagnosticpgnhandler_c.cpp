/*
  diagnosticpgnhandler_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "diagnosticpgnhandler_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/util/iassert.h>
#include <algorithm>

#if DEBUG_DIAGNOSTICPGN
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

// not done as private member function because of recursive-include problems.
// IdentItem_c needs DiagnosticPgnHandler_c because of the enums.
// Actually those enums should then just be moved out into its own file
// then the problems would vanish and it would be clean. But the define is
// okay for this case here, as the singletons are currently all done by define.
#define getSingletonVecKey() mrc_identItem.getSingletonVecKey()


namespace __IsoAgLib
{

DiagnosticPgnHandler_c::DiagnosticPgnHandler_c ( IdentItem_c& arc_identItem ) :
    mrc_identItem ( arc_identItem ),
    mstr_EcuIdentification ( "" ),
    mstr_SwIdentification ( "" ),
    mb_certificationIsSet ( false )
{
  STL_NAMESPACE::memset ( m_certification,0,sizeof ( uint8_t ) *8 );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, SOFTWARE_IDENTIFICATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, ECU_IDENTIFICATION_INFORMATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, ECU_DIAGNOSTIC_PROTOCOL_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, ISOBUS_CERTIFICATION_PGN );
//__IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN);
//__IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN);
//__IsoAgLib::getIsoRequestPgnInstance4Comm().registerPGN ( *this, DIAGNOSTIC_DATA_CLEAR_PGN);
}


DiagnosticPgnHandler_c::~DiagnosticPgnHandler_c()
{
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, SOFTWARE_IDENTIFICATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_IDENTIFICATION_INFORMATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_DIAGNOSTIC_PROTOCOL_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ISOBUS_CERTIFICATION_PGN );
//__IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN);
//__IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN);
//__IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, DIAGNOSTIC_DATA_CLEAR_PGN);
}


bool DiagnosticPgnHandler_c::processMsgRequestPGN ( uint32_t rui32_pgn, __IsoAgLib::IsoItem_c* /*rpc_isoItemSender*/, __IsoAgLib::IsoItem_c* rpc_isoItemReceiver )
{ // ~X2C
  static __IsoAgLib::MultiSend_c::sendSuccess_t st_sendSuccessDONTCAREFOR;
  if ( !mrc_identItem.isClaimedAddress() ) return false;
  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) ) return false; // request not adressed to us!

  switch ( rui32_pgn )
  {
    case ISOBUS_CERTIFICATION_PGN:
      if (mb_certificationIsSet &&
          __IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket(
            mrc_identItem.isoName(),
            m_certification,
            8,
            ISOBUS_CERTIFICATION_PGN,
            st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ISOBUS_CERTIFICATION " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

    case ECU_DIAGNOSTIC_PROTOCOL_PGN: {
      static const uint8_t diagProtocolId[8] = {
        0, // Only ISO 11783 Level 1 diagnostics
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF// Reserved bytes according to the standard
      };

      if (__IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket(
            mrc_identItem.isoName(),
            diagProtocolId,
            8,
            ECU_DIAGNOSTIC_PROTOCOL_PGN,
            st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_DIAGNOSTIC_PROTOCOL: first byte (diag protocol id) is " << uint16_t (diagProtocolId[0]) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break; }

    case SOFTWARE_IDENTIFICATION_PGN:
      if (!mstr_SwIdentification.empty() &&
          __IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket(
            mrc_identItem.isoName(),
            ( uint8_t* ) mstr_SwIdentification.c_str(),
            mstr_SwIdentification.size(),
            SOFTWARE_IDENTIFICATION_PGN,
            st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with SOFTWARE_IDENTIFICATION_PGN: " << mstr_SwIdentification << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

    case ECU_IDENTIFICATION_INFORMATION_PGN:
      if (!mstr_EcuIdentification.empty() &&
           __IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket(
            mrc_identItem.isoName(),
            ( uint8_t* ) mstr_EcuIdentification.c_str(),
            mstr_EcuIdentification.size(),
            ECU_IDENTIFICATION_INFORMATION_PGN,
            st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_IDENTIFICATION_INFORMATION_PGN: " << mstr_EcuIdentification << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

#if 0
    case ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN: {
      static const uint8_t activeTroubleCodes[6] = {
        0xFF,0xFF,0x00,0x00,0x00
      };
      if ( __IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket ( mrc_identItem.isoName(),
                                                                    activeTroubleCodes,
                                                                    6,
                                                                    ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN,
                                                                    st_sendSuccessDONTCAREFOR ))
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break; }

    case PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN:
      static const uint8_t previouslyActiveTroubleCodes[6] = {
        0xFF,0xFF,0x00,0x00,0x00
      };
      if ( __IsoAgLib::getMultiSendInstance().sendIsoBroadcastOrSinglePacket ( mrc_identItem.isoName(),
                                                                    previouslyActiveTroubleCodes,
                                                                    6,
                                                                    PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN,
                                                                    st_sendSuccessDONTCAREFOR ))
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

    case DIAGNOSTIC_DATA_CLEAR_PGN:
      // in future: clear the DTCs actually ;-)
      if (rpc_isoItemReceiver != NULL)
      { // Request was sent to DESTINATION
        __IsoAgLib::getIsoRequestPgnInstance4Comm().answerRequestPGNwithACK ( *mrc_identItem.getIsoItem(), 0x00 ); // Positive ACKNOWLEDGE
      }
      else
      { // Request was sent to GLOBAL
        // don't respond with ACKNOWLEDGE_PGN
      }
      return true;
#endif
  default:
    isoaglib_assert(!"Not registered for this PGN.");
    break;
  }

  // something wrong happend - answer with CannotRespondNow
  //  (couldn't multisend or Identification not yet ready)
  __IsoAgLib::getIsoRequestPgnInstance4Comm().answerRequestPGNwithACK ( *mrc_identItem.getIsoItem(), 0x03 ); // CannotRespondNow ACKNOWLEDGE

#if DEBUG_DIAGNOSTICPGN
  INTERNAL_DEBUG_DEVICE << "Couldn't response to RequestPGN with PGN=" << rui32_pgn << ". " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  return true;  // we shouldn't reach here without coming into one of the above conditions! - so true is okay!
} // -X2C

// The given strings should not have more than 200 characters and can't contain '*' characters as these are used as field delimiters
bool isValid ( const STL_NAMESPACE::string& astr_text )
{
  return ( astr_text.length() < 200  // The strings + '*' should not exceed 200 characters
           && astr_text.find ( '*' ) == STL_NAMESPACE::string::npos );
}

bool DiagnosticPgnHandler_c::setEcuIdentification(
  const STL_NAMESPACE::string& astr_partNr,
  const STL_NAMESPACE::string& astr_serialNr,
  const STL_NAMESPACE::string& astr_location,
  const STL_NAMESPACE::string& astr_type,
  const STL_NAMESPACE::string& astr_manufacturerName)
{
  if ( ! isValid ( astr_partNr ) ) return false;
  if ( ! isValid ( astr_serialNr ) ) return false;
  if ( ! isValid ( astr_location ) ) return false;
  if ( ! isValid ( astr_type ) ) return false;
  if ( ! isValid ( astr_manufacturerName ) ) return false;

  mstr_EcuIdentification = astr_partNr + "*" + astr_serialNr + "*" + astr_location + "*" + astr_type + "*" + astr_manufacturerName + "*";
  return true;
}

bool DiagnosticPgnHandler_c::setSwIdentification ( const STL_NAMESPACE::string& astr_SwId )
{
  if ( (astr_SwId.length() > 200 ) || (astr_SwId.length() < 1 ))
    return false;

  if (astr_SwId[astr_SwId.length()-1] != '*')
    return false; // last character must be '*'

  uint8_t ui8_fields = STL_NAMESPACE::count(
      astr_SwId.begin(),
      astr_SwId.end(),
      '*');

  mstr_SwIdentification = "0" + astr_SwId; // Reserve the first character for the number of fields
  mstr_SwIdentification[0]= ui8_fields;  // insert the number of fields 

  return true;
}

bool
DiagnosticPgnHandler_c::setCertificationData(
  uint16_t ui16_year,
  IsoAgLib::CertificationRevision_t a_revision,
  IsoAgLib::CertificationLabType_t a_laboratoryType,
  uint16_t aui16_laboratoryId,
  const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask,
  uint16_t aui16_referenceNumber)
{
  if ( ( ui16_year < 2000 ) || ( ui16_year > 2061 ) )
  return false;
  if ( aui16_laboratoryId > 2047 ) // Lab ID is only 11 bits wide
    return false;

  m_certification[0] = 0x00 | ( ( a_revision & 0x03 ) << 6 ) | ( ( ui16_year - 2000 ) & 0x3F );
  m_certification[1] = 0x00 | ( ( aui16_laboratoryId & 0x07 ) << 5 ) | ( ( a_laboratoryType & 0x07 ) << 1 ) | ( ( a_revision & 0x04 ) >> 2 );
  m_certification[2] = 0x00 | ( ( aui16_laboratoryId >> 3 ) & 0xFF );
  m_certification[3] = acrc_certificationBitMask.getByte ( 0 );
  m_certification[4] = acrc_certificationBitMask.getByte ( 1 );
  m_certification[5] = acrc_certificationBitMask.getByte ( 2 );
  m_certification[6] = ( aui16_referenceNumber & 0xFF );
  m_certification[7] = ( aui16_referenceNumber >> 8 ) & 0xFF ;

  mb_certificationIsSet = true;
  return true;
}

}
