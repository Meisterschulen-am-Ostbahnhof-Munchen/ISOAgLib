/*
  diagnosticpgnhandler_c.cpp

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

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
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <stdlib.h>

#include <IsoAgLib/comm/Part12_DiagnosticsServices/impl/diagnosticprotocol_c.h>

#if DEBUG_DIAGNOSTICPGN
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

// not done as private member function because of recursive-include problems.
// IdentItem_c needs DiagnosticPgnHandler_c because of the enums.
// Actually those enums should then just be moved out into its own file
// then the problems would vanish and it would be clean. But the define is
// okay for this case here, as the singletons are currently all done by define.
#define getMultitonInst() mrc_identItem.getMultitonInst()


namespace __IsoAgLib
{

DiagnosticPgnHandler_c::DiagnosticPgnHandler_c ( IdentItem_c& arc_identItem ) :
    mrc_identItem ( arc_identItem ),
    mpc_diagnosticProtocol(NULL),
    mcstr_EcuIdentification ( NULL ),
    mcstr_SwIdentification ( NULL),
    mb_certificationIsSet ( false )
{
  mpc_diagnosticProtocol = new DiagnosticProtocol_c(arc_identItem, EcuDiagnosticProtocolIdentificationBitMask_t());
  // m_certification is not set as mb_certificationIsSet indicates it not being used!
}


DiagnosticPgnHandler_c::~DiagnosticPgnHandler_c()
{
  delete mpc_diagnosticProtocol;

  if (mcstr_EcuIdentification)
    CNAMESPACE::free (mcstr_EcuIdentification);

  if (mcstr_SwIdentification)
    CNAMESPACE::free (mcstr_SwIdentification);
}


void
DiagnosticPgnHandler_c::init()
{
  mpc_diagnosticProtocol->init();

  getIsoRequestPgnInstance4Comm().registerPGN ( *this, SOFTWARE_IDENTIFICATION_PGN );
  getIsoRequestPgnInstance4Comm().registerPGN ( *this, ECU_IDENTIFICATION_INFORMATION_PGN );
  getIsoRequestPgnInstance4Comm().registerPGN ( *this, ISOBUS_CERTIFICATION_PGN );
}


void
DiagnosticPgnHandler_c::close()
{
  mpc_diagnosticProtocol->close();

  getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, SOFTWARE_IDENTIFICATION_PGN );
  getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_IDENTIFICATION_INFORMATION_PGN );
  getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ISOBUS_CERTIFICATION_PGN );
}


bool DiagnosticPgnHandler_c::processMsgRequestPGN ( uint32_t rui32_pgn, IsoItem_c* /*rpc_isoItemSender*/, IsoItem_c* rpc_isoItemReceiver, int32_t )
{
  if ( !mrc_identItem.isClaimedAddress() ) return false;
  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) ) return false; // request not adressed to us!

  switch ( rui32_pgn )
  {
    case ISOBUS_CERTIFICATION_PGN:
      if (mb_certificationIsSet)
      {
        sendSinglePacket(m_certification,ISOBUS_CERTIFICATION_PGN);
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ISOBUS_CERTIFICATION " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      break;

    case SOFTWARE_IDENTIFICATION_PGN:
      if (mcstr_SwIdentification != NULL)
      {
        if (getCStringLength (mcstr_SwIdentification) < 9)
        {
          sendSinglePacket((uint8_t *) mcstr_SwIdentification,SOFTWARE_IDENTIFICATION_PGN);
          return true;
        }
        else
        {
          if ( getMultiSendInstance4Comm().sendIsoBroadcast(
                mrc_identItem.isoName(),
                (uint8_t *) mcstr_SwIdentification,
                getCStringLength (mcstr_SwIdentification),
                SOFTWARE_IDENTIFICATION_PGN,
                NULL) )
          { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
            INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with SOFTWARE_IDENTIFICATION_PGN: " << mcstr_SwIdentification << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
            return true;
          }
          // else
        }
      } // else
      break;

    case ECU_IDENTIFICATION_INFORMATION_PGN:
      if (mcstr_EcuIdentification != NULL)
      {
        if (getCStringLength (mcstr_EcuIdentification) < 9)
        {
          sendSinglePacket((uint8_t *) mcstr_EcuIdentification,ECU_IDENTIFICATION_INFORMATION_PGN);
          return true;
        }
        else
        {
          if ( getMultiSendInstance4Comm().sendIsoBroadcast(
                mrc_identItem.isoName(),
                (uint8_t *) mcstr_EcuIdentification,
                getCStringLength (mcstr_EcuIdentification),
                ECU_IDENTIFICATION_INFORMATION_PGN,
                NULL) )
          { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
            INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_IDENTIFICATION_INFORMATION_PGN: " << mcstr_EcuIdentification << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
            return true;
          }
          // else
        }
      } // else
      break;

  default:
    isoaglib_assert(!"Not registered for this PGN.");
    break;
  }

#if DEBUG_DIAGNOSTICPGN
  INTERNAL_DEBUG_DEVICE << "Couldn't response to RequestPGN with PGN=" << rui32_pgn << ". " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  // something wrong happend - answer with CannotRespondNow
  //  (couldn't multisend or Identification not yet ready)
  getIsoRequestPgnInstance4Comm().answerRequestPGNwithACK ( *mrc_identItem.getIsoItem(), 0x03 ); // CannotRespondNow ACKNOWLEDGE
  return true;
}


/// Internal helper functions:
/// (currently only for diagnostics)
/// /////////////////////////

/// The given strings should not have more than 200 characters and
/// can't contain '*' characters as these are used as field delimiters
/// @return length of the string (0..x) if valid, <0 if not valid!
int getLengthIfValid (const char *acstr_text)
{
  isoaglib_assert (acstr_text);

  int len = 0;
  while (acstr_text[len] != 0x00)
  {
    // check current...
    if (acstr_text[len] == '*')
    { // string contains forbidden '*'
      return -1;
    }
    // advance...
    ++len;
    if (len == 200)
    { // string to long - not valid
      return -2;
    }
  }

  return len;
}


bool
DiagnosticPgnHandler_c::setEcuIdentification(
  const char *acstr_partNr,
  const char *acstr_serialNr,
  const char *acstr_location,
  const char *acstr_type,
  const char *acstr_manufacturerName)
{
  isoaglib_assert (acstr_partNr);
  isoaglib_assert (acstr_serialNr);
  isoaglib_assert (acstr_location);
  isoaglib_assert (acstr_type);
  isoaglib_assert (acstr_manufacturerName);

  int len1 = getLengthIfValid (acstr_partNr);
  int len2 = getLengthIfValid (acstr_serialNr);
  int len3 = getLengthIfValid (acstr_location);
  int len4 = getLengthIfValid (acstr_type);
  int len5 = getLengthIfValid (acstr_manufacturerName);

  if ( (len1 < 0) || (len2 < 0) || (len3 < 0) || (len4 < 0) || (len5 < 0) )
    return false; // wrong sublength(s) or * in substring(s)

  if (mcstr_EcuIdentification)
    CNAMESPACE::free (mcstr_EcuIdentification);

  // string deparated by * and terminated by 0x00
  int newLen = len1 + 1 + len2 + 1 + len3 + 1 + len4 + 1 + len5 + 1 + 1;

  mcstr_EcuIdentification = (char *) CNAMESPACE::malloc (sizeof (char) * newLen);

  char *destPtr = mcstr_EcuIdentification;
  addCStringWithoutTermination (&destPtr, acstr_partNr);
  addCStringWithoutTermination (&destPtr, "*");
  addCStringWithoutTermination (&destPtr, acstr_serialNr);
  addCStringWithoutTermination (&destPtr, "*");
  addCStringWithoutTermination (&destPtr, acstr_location);
  addCStringWithoutTermination (&destPtr, "*");
  addCStringWithoutTermination (&destPtr, acstr_type);
  addCStringWithoutTermination (&destPtr, "*");
  addCStringWithoutTermination (&destPtr, acstr_manufacturerName);
  addCStringWithoutTermination (&destPtr, "*");
  *destPtr++ = 0x00;

  // detect buffer-overruns!
  isoaglib_assert (newLen == (destPtr - mcstr_EcuIdentification));

  return true;
}


bool
DiagnosticPgnHandler_c::setSwIdentification ( const char *acstr_SwId )
{
  int swIdLen = getCStringLength (acstr_SwId);

  if ( (swIdLen > 200 ) || (swIdLen < 1 ) )
    return false;

  if (acstr_SwId [swIdLen-1] != '*')
    return false; // last character must be '*'

  int numStars = getCStringCount (acstr_SwId, '*');

  if (mcstr_SwIdentification)
    CNAMESPACE::free (mcstr_SwIdentification);

  int newLen = 1+swIdLen+1; // // uint8,swid,0x00
  mcstr_SwIdentification = (char *) CNAMESPACE::malloc (sizeof (char) * newLen);

  char *destPtr = mcstr_SwIdentification;
  *destPtr++ = uint8_t (numStars); // number of fields
  addCStringWithoutTermination (&destPtr, acstr_SwId);
  *destPtr++ = 0x00;

  // detect buffer-overruns!
  isoaglib_assert (newLen == (destPtr - mcstr_SwIdentification));

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

void
DiagnosticPgnHandler_c::sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,
                                          int32_t ai32_pgn)
{
  CanPkgExt_c pkg;
  pkg.setIsoPri (6);
  pkg.setMonitorItemForSA (mrc_identItem.getIsoItem());
  pkg.setLen (8);
  pkg.setIsoPgn(ai32_pgn);
  for (unsigned ui = 0 ; ui < 8; ++ui)
    pkg.setUint8Data (ui, rhpb_data[ui]);

  getIsoBusInstance4Comm() << pkg;
}

DiagnosticProtocol_c& DiagnosticPgnHandler_c::getDiagnosticProtocol()
{
  return *mpc_diagnosticProtocol;
}

}
