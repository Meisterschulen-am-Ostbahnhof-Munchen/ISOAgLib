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

namespace {
struct Calculator
{
  uint16_t operator() ( uint16_t result, const __IsoAgLib::DiagnosticProtocol_c::FunctionitiesArr::value_type& obj )
  {
      return (result + 2 + obj.second.number_of_options);
  }
};
}

namespace __IsoAgLib
{

DiagnosticProtocol_c::DiagnosticProtocol_c ( IdentItem_c& arc_identItem, const IsoAgLib::EcuDiagnosticProtocolIdentificationBitMask_t& protocol ) :
    mrc_identItem ( arc_identItem ),
    mui8_protocol(protocol.getByte(0)),
    mcstr_currentFunctionalities ( NULL ),
    mui16_arrayLenght( 0 ),
    marr_functionalities (),
    mb_arrIsDirty ( true ),
    mb_useAefSpecifications ( false )
{  
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

  // create array for the message
  mcstr_currentFunctionalities = (uint8_t *) CNAMESPACE::malloc (sizeof (uint8_t) * mui16_arrayLenght);
  
  // fill the array
  mcstr_currentFunctionalities[0] = mui8_protocol; // TBD
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
 
  // in case size is less than 8 -> fill rest of the message with 0xFF
  for (; counter < 8; ++counter)
  {
    mcstr_currentFunctionalities[counter] = 0xFF;
  }
  
  // control size in debug mode
  isoaglib_assert(mui16_arrayLenght == counter);
  
  mb_arrIsDirty = false;
}


bool DiagnosticProtocol_c::processMsgRequestPGN ( uint32_t rui32_pgn, IsoItem_c* /*rpc_isoItemSender*/, IsoItem_c* rpc_isoItemReceiver, int32_t )
{
  if ( !mrc_identItem.isClaimedAddress() ) return false;
  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) ) return false; // request not adressed to us!

  if ( ECU_DIAGNOSTIC_PROTOCOL_PGN == rui32_pgn ) // TBD : is that necessary ?
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

uint8_t DiagnosticProtocol_c::getGeneration(IsoAgLib::FunctionalitiesCharacteristics_t functionality, uint8_t version) const
{
  switch (functionality)
  {
    case IsoAgLib::NoFunctionalitiesReported:
      break;
    case IsoAgLib::VirtualTerminal:
    case IsoAgLib::VirtualTerminalWorkingSet:
      if (version == 2) return 1;
      if (version == 3) return 2;
      if (version == 4) return 3;
      break;
    case IsoAgLib::AuxiliaryControlType1Inputs:
    case IsoAgLib::AuxiliaryControlType1Functions:
      if (version == 2) return 1;
      break;
    case IsoAgLib::AuxiliaryControlType2Inputs:
    case IsoAgLib::AuxiliaryControlType2Functions:
      if (version == 3) return 1;
      break;
    case IsoAgLib::TaskControllerBasic:
    case IsoAgLib::TaskControllerBasicWorkingSet:
    case IsoAgLib::TaskControllerGeo:
    case IsoAgLib::TaskControllerGeoWorkingSet:
    case IsoAgLib::TaskControllerSectionControl:
    case IsoAgLib::TaskControllerSectionControlWorkingSet:
      if (version == 2) return 1;
      break;
    case IsoAgLib::BasicTractorECU:
    case IsoAgLib::BasicTractorECUImplementSet:
    case IsoAgLib::AdvanceTractorECU:
    case IsoAgLib::AdvanceTractorECUImplementSet:
      if (version == 1) return 1;
      break;
    case IsoAgLib::FileServer:
    case IsoAgLib::FileServerClient:
      if (version == 2) return 1;
      if (version == 3) return 2;
      break;
    case IsoAgLib::SequenceControlServer:
    case IsoAgLib::SequenceControlClient:
      if (version == 1) return 1; // TBD : NOT SPECIFIED IN DOC
      break;
    case IsoAgLib::StopAllImplementOperationsInput:
    case IsoAgLib::StopAllImplementOperationsImplementSet:
      if (version == 1) return 1;
      break;
    case IsoAgLib::DiagnosticTool:
    case IsoAgLib::DiagnosticECU:
      if (version == 1) return 1; // TBD : NOT SPECIFIED IN DOC
      break;
    case IsoAgLib::Functionality_ReservedForISO:
        break;
  }
  isoaglib_assert(false);
  return 0;
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

bool DiagnosticProtocol_c::addAefFunctionalitiesVirtualTerminal(bool implement, uint8_t version, const IsoAgLib::VirtualTerminalOptionsBitMask_t& options)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::VirtualTerminalWorkingSet : IsoAgLib::VirtualTerminal );
  
#ifdef USE_TEMPLATES
  return fillStructure(functionality, version, options);
#else
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);

  return addFunctionality(functionality, functionality_description);
#endif
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType1(bool implement, uint8_t version, const IsoAgLib::AuxControlType1OptionsBitMask_t& options)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AuxiliaryControlType1Inputs : IsoAgLib::AuxiliaryControlType1Functions );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);

  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType2(bool implement, uint8_t version, const IsoAgLib::AuxControlType2OptionsBitMask_t& options)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AuxiliaryControlType2Inputs : IsoAgLib::AuxiliaryControlType2Functions );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);

  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerBasic(bool implement, uint8_t version, const IsoAgLib::TaskControllerBasicOptionsBitMask_t& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerBasicWorkingSet : IsoAgLib::TaskControllerBasic );
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerGeo(bool implement, uint8_t version, const IsoAgLib::TaskControllerGeoOptionsBitMask_t& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerGeoWorkingSet : IsoAgLib::TaskControllerGeo );
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::TaskControllerSectionControlWorkingSet : IsoAgLib::TaskControllerSectionControl );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 2;
  functionality_description.options_bytes[0] = numberOfBooms;
  functionality_description.options_bytes[1] = numberOfSections;  

  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesBasicTractorECU(bool implement, uint8_t version, const IsoAgLib::BasicTractorECUOptionsBitMask_t& options)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::BasicTractorECUImplementSet : IsoAgLib::BasicTractorECU );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);

  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAdvanceTractorECU(bool implement, uint8_t version, const IsoAgLib::AdvanceTractorECUOptionsBitMask_t& options)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::AdvanceTractorECUImplementSet : IsoAgLib::AdvanceTractorECU );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);
  
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesSequenceControl(bool implement, uint8_t version, const IsoAgLib::SequenceControlOptionsBitMask_t& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::SequenceControlClient : IsoAgLib::SequenceControlServer );
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesStopAllImplement(bool implement, uint8_t version, const IsoAgLib::StopAllImplementOptionsBitMask_t& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::StopAllImplementOperationsImplementSet : IsoAgLib::StopAllImplementOperationsInput );
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesFileServer(bool implement, uint8_t version, const IsoAgLib::FileServerOptionsBitMask_t& /*options*/)
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = ( implement ? IsoAgLib::FileServerClient : IsoAgLib::FileServer );
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticTool()
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = IsoAgLib::DiagnosticTool;
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, 1); // TBD DiagnosticTool version ?
  functionality_description.number_of_options = 0;
  // TBD fill structure
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticECU()
{
  IsoAgLib::FunctionalitiesCharacteristics_t functionality = IsoAgLib::DiagnosticECU;
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, 1); // TBD DiagnosticECU version ?
  functionality_description.number_of_options = 0;
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
