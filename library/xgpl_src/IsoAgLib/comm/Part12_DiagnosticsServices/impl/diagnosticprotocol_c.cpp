/*
  diagnosticprotocol_c.cpp

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "diagnosticprotocol_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <stdlib.h>

#include <functional>
#include <numeric>

#if DEBUG_DIAGNOSTICPROTOCOL
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

DiagnosticProtocol_c::DiagnosticProtocol_c ( IdentItem_c& arc_identItem ) :
    mrc_identItem ( arc_identItem ),
    mcstr_currentFunctionalities ( NULL ),
    mui16_arrayLenght( 0 ),
    marr_functionalities (),
    mb_arrIsDirty ( true ),
    mb_useAefSpecifications ( false )
{
  //static const uint8_t diagProtocolId[8] = {
  //  0, // Only ISO 11783 Level 1 diagnostics
  //  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF// Reserved bytes according to the standard
  //};

  //mui16_arrayLenght = 8;
  //mcstr_currentFunctionalities = (uint8_t *) CNAMESPACE::malloc (sizeof (uint8_t) * mui16_arrayLenght);
  //CNAMESPACE::memcpy(mcstr_currentFunctionalities, diagProtocolId, mui16_arrayLenght);
  
}


DiagnosticProtocol_c::~DiagnosticProtocol_c()
{
  if (mcstr_currentFunctionalities)
    CNAMESPACE::free (mcstr_currentFunctionalities);
}


void
DiagnosticProtocol_c::init()
{
  getIsoRequestPgnInstance4Comm().registerPGN ( *this, ECU_DIAGNOSTIC_PROTOCOL_PGN );
}


void
DiagnosticProtocol_c::close()
{
  getIsoRequestPgnInstance4Comm().unregisterPGN ( *this, ECU_DIAGNOSTIC_PROTOCOL_PGN );
}

struct Calculator
{
  uint16_t operator() ( uint16_t result, const DiagnosticProtocol_c::FunctionitiesArr::value_type& obj )
  {
      return (result + 2 + obj.second.number_of_options);
  }
};

void DiagnosticProtocol_c::updatePackage()
{
  // destroy previously constructed message
  if (mcstr_currentFunctionalities)
    CNAMESPACE::free (mcstr_currentFunctionalities);

  // compute new length
  mui16_arrayLenght = 2; // header size
  if ( mb_useAefSpecifications )
  {
    mui16_arrayLenght += CNAMESPACE::accumulate(marr_functionalities.begin(),marr_functionalities.end(), 0, Calculator());
  }
  if (mui16_arrayLenght < 8) mui16_arrayLenght = 8;

  mcstr_currentFunctionalities = (uint8_t *) CNAMESPACE::malloc (sizeof (uint8_t) * mui16_arrayLenght);
  mcstr_currentFunctionalities[0] = 0x00; // @TBD
  uint16_t counter = 1;
  if ( mb_useAefSpecifications )
  {
    mcstr_currentFunctionalities[counter++] = marr_functionalities.size();
    for (FunctionitiesArrIter iter = marr_functionalities.begin();
         iter != marr_functionalities.end();
         ++iter )
    {
      mcstr_currentFunctionalities[counter++] = iter->first; // TBD counter++ or ++counter ?
      mcstr_currentFunctionalities[counter++] = iter->second.generation;
      mcstr_currentFunctionalities[counter++] = iter->second.number_of_options;
      isoaglib_assert(iter->second.number_of_options < MAX_OPTION_BYTES);
      for (uint8_t option = 0; option < iter->second.number_of_options; ++option) // TBD : and option < MAX_OPTION_BYTES ?
        mcstr_currentFunctionalities[counter++] = iter->second.options_bytes[option];
    }
  }
  for (; counter < mui16_arrayLenght; ++counter)
  {
    mcstr_currentFunctionalities[counter] = 0xFF;
  }
  isoaglib_assert(mui16_arrayLenght == counter);
  mb_arrIsDirty = false;
}


bool DiagnosticProtocol_c::processMsgRequestPGN ( uint32_t rui32_pgn, IsoItem_c* /*rpc_isoItemSender*/, IsoItem_c* rpc_isoItemReceiver, int32_t )
{
  if ( !mrc_identItem.isClaimedAddress() ) return false;
  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) ) return false; // request not adressed to us!

  if ( ECU_DIAGNOSTIC_PROTOCOL_PGN == rui32_pgn )
  {
    // update package if necessary
    if (mb_arrIsDirty)
    {
      updatePackage();
    }

    // send package
    if (mui16_arrayLenght < 9)
    {
      sendSinglePacket((uint8_t *) mcstr_currentFunctionalities,ECU_DIAGNOSTIC_PROTOCOL_PGN);
#if DEBUG_DIAGNOSTICPGN
      INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_DIAGNOSTIC_PROTOCOL_PGN: first byte (diag protocol id) is " << uint16_t (mcstr_currentFunctionalities[0]) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return true;
    }
    else
    {
      if ( getMultiSendInstance4Comm().sendIsoBroadcast(
            mrc_identItem.isoName(),
            (uint8_t *) mcstr_currentFunctionalities,
            mui16_arrayLenght,
            ECU_DIAGNOSTIC_PROTOCOL_PGN,
            NULL) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN ECU_DIAGNOSTIC_PROTOCOL_PGN"  << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        return true;
      }
      // else 
    }
  }
  else
  {
    isoaglib_assert(!"Not registered for this PGN.");
  }

#if DEBUG_DIAGNOSTICPGN
  INTERNAL_DEBUG_DEVICE << "Couldn't response to RequestPGN with PGN=" << rui32_pgn << ". " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  // something wrong happend - answer with CannotRespondNow
  //  (couldn't multisend or Identification not yet ready)
  getIsoRequestPgnInstance4Comm().answerRequestPGNwithACK ( *mrc_identItem.getIsoItem(), 0x03 ); // CannotRespondNow ACKNOWLEDGE
  return true;
}

void
DiagnosticProtocol_c::sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data, int32_t ai32_pgn)
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

bool DiagnosticProtocol_c::addFunctionality(IsoAgLib::FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description)
{
  FunctionitiesArrIter iter = marr_functionalities.find(functionality);
  if ( iter != marr_functionalities.end() )
  {
    marr_functionalities.insert( FunctionitiesArrPair(functionality, functionality_description) );
    mb_arrIsDirty = true;
    return true;
  }
  return false; // already there
}

bool DiagnosticProtocol_c::addAefFunctionalitiesVirtualTerminal(bool implement, uint8_t /*version*/, const ArrVirtualTerminalOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::VirtualTerminalWorkingSet : IsoAgLib::VirtualTerminal );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType1(bool implement, uint8_t /*version*/, const ArrAuxControlType1Options& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AuxiliaryControlType1Inputs : IsoAgLib::AuxiliaryControlType1Functions );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType2(bool implement, uint8_t /*version*/, const ArrAuxControlType2Options& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AuxiliaryControlType2Inputs : IsoAgLib::AuxiliaryControlType2Functions );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerBasic(bool implement, uint8_t /*version*/, const ArrTaskControllerBasicOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerBasicWorkingSet : IsoAgLib::TaskControllerBasic );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerGeo(bool implement, uint8_t /*version*/, const ArrTaskControllerGeoOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerGeoWorkingSet : IsoAgLib::TaskControllerGeo );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t /*version*/, uint8_t /*numberOfBooms*/, uint8_t /*numberOfSections*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerSectionControlWorkingSet : IsoAgLib::TaskControllerSectionControl );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesBasicTractorECU(bool implement, uint8_t /*version*/, const ArrBasicTractorECUOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::BasicTractorECUImplementSet : IsoAgLib::BasicTractorECU );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAdvanceTractorECU(bool implement, uint8_t /*version*/, const ArrAdvanceTractorECUOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AdvanceTractorECUImplementSet : IsoAgLib::AdvanceTractorECU );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesSequenceControl(bool implement, uint8_t /*version*/, const ArrSequenceControlOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::SequenceControlClient : IsoAgLib::SequenceControlServer );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesStopAllImplement(bool implement, uint8_t /*version*/, const ArrStopAllImplementOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::StopAllImplementOperationsImplementSet : IsoAgLib::StopAllImplementOperationsInput );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesFileServer(bool implement, uint8_t /*version*/, const ArrFileServerOptions& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::FileServerClient : IsoAgLib::FileServer );
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticTool()
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = IsoAgLib::DiagnosticTool;
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticECU()
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = IsoAgLib::DiagnosticECU;
  Functionality_s functionality_description;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::remAefFunctionalities(IsoAgLib::FunctionalitiesCharacteristics_t functionality)
{
  FunctionitiesArrIter iter = marr_functionalities.find(functionality);
  if ( iter != marr_functionalities.end() )
  {
    marr_functionalities.erase(iter);
    mb_arrIsDirty = true;
    return true;
  }
  return false; // not found
}

}
