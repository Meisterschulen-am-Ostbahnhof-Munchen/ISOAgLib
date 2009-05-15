#include "diagnosticpgnhandler_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>

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

}

DiagnosticPgnHandler_c::~DiagnosticPgnHandler_c()
{
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, SOFTWARE_IDENTIFICATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_IDENTIFICATION_INFORMATION_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_DIAGNOSTIC_PROTOCOL_PGN );
  __IsoAgLib::getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ISOBUS_CERTIFICATION_PGN );
}


bool DiagnosticPgnHandler_c::processMsgRequestPGN ( uint32_t rui32_pgn, __IsoAgLib::IsoItem_c* /*rpc_isoItemSender*/, __IsoAgLib::IsoItem_c* rpc_isoItemReceiver )
{ // ~X2C
  static __IsoAgLib::MultiSend_c::sendSuccess_t st_sendSuccessDONTCAREFOR;
  if ( !mrc_identItem.isClaimedAddress() ) return false;
  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) ) return false; // request not adressed to us!
  static const uint8_t diagProtocolId[8] = {0, // We do not support any diagnostic protocol
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
                                           }; // Reserved bytes according to the standard

  switch ( rui32_pgn )
  {
    case ISOBUS_CERTIFICATION_PGN:
      if ( mb_certificationIsSet && __IsoAgLib::getMultiSendInstance().sendIsoBroadcast ( mrc_identItem.isoName(),
																																													m_certification,
																																													8,
																																													ISOBUS_CERTIFICATION_PGN,
																																													st_sendSuccessDONTCAREFOR )
         )
      { // Message successfully transmitted to multisend -> return true
#ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ISOBUS_CERTIFICATION ";
#endif
        return true;
      }
      break;
    case ECU_DIAGNOSTIC_PROTOCOL_PGN:
      if ( __IsoAgLib::getMultiSendInstance().sendIsoBroadcast ( mrc_identItem.isoName(),
																																 diagProtocolId,
																																 8,
																																 ECU_DIAGNOSTIC_PROTOCOL_PGN,
																																 st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#ifdef DEBUG
INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_DIAGNOSTIC_PROTOCOL : " << mstr_multiSendBufferSwIdent << INTERNAL_DEBUG_DEVICE_ENDL ; 
#endif
        return true;
      }
      break;

    case SOFTWARE_IDENTIFICATION_PGN:
      if ( !mstr_SwIdentification.empty()
           && __IsoAgLib::getMultiSendInstance().sendIsoBroadcast ( mrc_identItem.isoName(),
																																		( uint8_t* ) mstr_SwIdentification.c_str(),
																																		mstr_SwIdentification.size(),
																																		SOFTWARE_IDENTIFICATION_PGN,
																																		st_sendSuccessDONTCAREFOR ) )
      { // Message successfully transmitted to multisend -> return true
#ifdef DEBUG
INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with SOFTWARE_IDENTIFICATION_PGN : " << mstr_multiSendBufferSwIdent << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

    case ECU_IDENTIFICATION_INFORMATION_PGN:
      if ( !mstr_EcuIdentification.empty()
           && __IsoAgLib::getMultiSendInstance().sendIsoBroadcast ( mrc_identItem.isoName(),
																																		( uint8_t* ) mstr_EcuIdentification.c_str(),
																																		mstr_EcuIdentification.size(),
																																		ECU_IDENTIFICATION_INFORMATION_PGN,
																																		st_sendSuccessDONTCAREFOR ))
      { // Message successfully transmitted to multisend -> return true
#ifdef DEBUG
				INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_IDENTIFICATION_INFORMATION_PGN : " << mstr_multiSendBufferSwIdent << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;
  }

  // something wrong happend - answer with CannotRespondNow (ACK_PGN with cmdByte CannotRespondNow)
  //  (couldn't multisend or Identification not yet ready)
  __IsoAgLib::getIsoRequestPgnInstance4Comm().answerRequestPGNwithCannotRespondNow ( *mrc_identItem.getIsoItem() );

  return true;  // we shouldn't reach here without coming into one of the above conditions! - so true is okay!
} // -X2C

// The given strings should not have more than 200 characters and can't contain '*' characters as these are used as field delimiters
bool isValid ( const STL_NAMESPACE::string& astr_text )
{
  return ( astr_text.length() < 200  // The strings + '*' should not exceed 200 characters
						&& astr_text.find ( '*' ) == STL_NAMESPACE::string::npos );
}

bool DiagnosticPgnHandler_c::setEcuIdentification ( const STL_NAMESPACE::string& astr_partNr, const STL_NAMESPACE::string& astr_serialNr,const STL_NAMESPACE::string& astr_manufacturerName )
{
  if ( ! isValid ( astr_partNr ) ) return false;
  if ( ! isValid ( astr_serialNr ) ) return false;
  if ( ! isValid ( astr_manufacturerName ) ) return false;

  mstr_EcuIdentification = astr_partNr + "*" + astr_serialNr + "*" + astr_manufacturerName + "*";
  return true;
}

bool DiagnosticPgnHandler_c::setSwIdentification ( const STL_NAMESPACE::string& astr_SwId )
{
	if (astr_SwId.length() > 200 )
		return false;

  uint8_t ui8_fields = 1;
  for ( int i = 1; i < int(astr_SwId.length()) -1 ; i++ )
  {
    if ( astr_SwId[i] == '*' || astr_SwId[i] == '#' )
      ui8_fields ++;
  }

  mstr_SwIdentification = "0" + astr_SwId; // Reserve the first character for the number of fields
  mstr_SwIdentification[0]= ui8_fields;  // insert the number of fields 
  
  return true;
}

bool DiagnosticPgnHandler_c::setCertificationData( uint16_t ui16_year ,CertificationRevision_t a_revision,CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
																									 const CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber )
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

