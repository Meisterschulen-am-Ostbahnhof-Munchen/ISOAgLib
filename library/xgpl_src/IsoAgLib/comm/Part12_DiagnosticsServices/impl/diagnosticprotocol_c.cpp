/*
  diagnosticprotocol_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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

DiagnosticProtocol_c::DiagnosticProtocol_c ( IdentItem_c& arc_identItem, const EcuDiagnosticProtocolIdentificationBitMask_t& protocol ) :
    mrc_identItem ( arc_identItem ),
    mui8_protocol(protocol.getByte(0)),
    mt_multiSendEventHandler(),
    m_currentFunctionalities ( NULL ),
    mui16_arrayLenght( 0 ),
    marr_functionalities (),
    mb_arrIsDirty ( true ),
    mb_useAefSpecifications ( false )
{
}


DiagnosticProtocol_c::~DiagnosticProtocol_c()
{
  if (m_currentFunctionalities)
    CNAMESPACE::free (m_currentFunctionalities);
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
  isoaglib_assert(!mt_multiSendEventHandler.m_isMultiSendRunning);

  // destroy previously constructed message
  if (m_currentFunctionalities)
    CNAMESPACE::free (m_currentFunctionalities);

  // compute new length
  mui16_arrayLenght = 2; // header size
  if ( mb_useAefSpecifications )
  {
    for (FunctionitiesArrIter iter = marr_functionalities.begin();
         iter != marr_functionalities.end();
         ++iter )
    {
      // Functionality - generation - number_of_option_bytes - option_1 - ... - option_n
      mui16_arrayLenght += (3 + iter->second.number_of_option_bytes);
    }
  }
  if (mui16_arrayLenght < 8) mui16_arrayLenght = 8;

  // create array for the message
  m_currentFunctionalities = (uint8_t *) CNAMESPACE::malloc (sizeof (uint8_t) * mui16_arrayLenght);
  
  // fill the array
  m_currentFunctionalities[0] = mui8_protocol;
  uint16_t byte_counter = 1;
  if ( mb_useAefSpecifications )
  {
    m_currentFunctionalities[byte_counter++] = static_cast<uint8_t>(marr_functionalities.size());
    for (FunctionitiesArrIter iter = marr_functionalities.begin();
         iter != marr_functionalities.end();
         ++iter )
    {
      m_currentFunctionalities[byte_counter++] = static_cast<uint8_t>(iter->first);
      m_currentFunctionalities[byte_counter++] = iter->second.generation;
      m_currentFunctionalities[byte_counter++] = iter->second.number_of_option_bytes;
      isoaglib_assert(iter->second.number_of_option_bytes <= MAX_OPTION_BYTES);
      for (uint8_t option = 0; option < iter->second.number_of_option_bytes; ++option)
        m_currentFunctionalities[byte_counter++] = iter->second.options_bytes[option];
    }
  }
 
  // in case size is less than 8 -> fill rest of the message with 0xFF
  for (; byte_counter < 8; ++byte_counter)
  {
    m_currentFunctionalities[byte_counter] = 0xFF;
  }
  
  // control size in debug mode
  isoaglib_assert(mui16_arrayLenght == byte_counter);
}

void DiagnosticProtocol_c::MultiSendEventHandlerProxy_c::reactOnStateChange(const SendStream_c& sendStream)
{
  switch (sendStream.getSendSuccess())
  {
  case SendStream_c::SendAborted:
  case SendStream_c::SendSuccess:
    m_isMultiSendRunning = false;
    break;
  case SendStream_c::Running:
    break;
  }
}

bool DiagnosticProtocol_c::processMsgRequestPGN ( uint32_t rui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* rpc_isoItemReceiver, int32_t )
{
  if ( !mrc_identItem.isClaimedAddress() )
    return false;

  // we're not Network Management, so don't answer requests from 0xFE
  if( apc_isoItemSender == NULL )
    return false;

  if ( ( rpc_isoItemReceiver != NULL ) && ( mrc_identItem.getIsoItem() != rpc_isoItemReceiver ) )
    return false; // request not adressed to us!

  (void)rui32_pgn; // currently only this one PGN handled in here!
  isoaglib_assert(ECU_DIAGNOSTIC_PROTOCOL_PGN == rui32_pgn);

  if (!mt_multiSendEventHandler.m_isMultiSendRunning)
  {
    // update package if necessary
    if (mb_arrIsDirty)
    {
      updatePackage();
      mb_arrIsDirty = false;
    }

    // send package
    if (mui16_arrayLenght < 9)
    {
      sendSinglePacket(m_currentFunctionalities,ECU_DIAGNOSTIC_PROTOCOL_PGN);
#if DEBUG_DIAGNOSTICPGN
      INTERNAL_DEBUG_DEVICE << "Response to RequestPGN with ECU_DIAGNOSTIC_PROTOCOL_PGN: first byte (diag protocol id) is " << uint16_t (m_currentFunctionalities[0]) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return true;
    }
    else
    {
      if ( getMultiSendInstance4Comm().sendIsoBroadcast(
            mrc_identItem.isoName(),
            m_currentFunctionalities,
            mui16_arrayLenght,
            ECU_DIAGNOSTIC_PROTOCOL_PGN,
            &mt_multiSendEventHandler) )
      { // Message successfully transmitted to multisend -> return true
#if DEBUG_DIAGNOSTICPGN
        INTERNAL_DEBUG_DEVICE << "Response to RequestPGN ECU_DIAGNOSTIC_PROTOCOL_PGN"  << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        mt_multiSendEventHandler.m_isMultiSendRunning = true;
        return true;
      }
      // else 
    }
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

uint8_t DiagnosticProtocol_c::getGeneration(FunctionalitiesCharacteristics_t functionality, uint8_t version) const
{
  switch (functionality)
  {
    case VirtualTerminal:
    case VirtualTerminalWorkingSet:
      if (version == 2) return 1;
      if (version == 3) return 2;
      if (version == 4) return 3;
      break;
    case AuxiliaryControlType1Inputs:
    case AuxiliaryControlType1Functions:
      if (version == 2) return 1;
      break;
    case AuxiliaryControlType2Inputs:
    case AuxiliaryControlType2Functions:
      if (version == 3) return 1;
      break;
    case TaskControllerBasic:
    case TaskControllerBasicWorkingSet:
    case TaskControllerGeo:
    case TaskControllerGeoWorkingSet:
    case TaskControllerSectionControl:
    case TaskControllerSectionControlWorkingSet:
      if (version == 2) return 1;
      break;
    case BasicTractorECU:
    case BasicTractorECUImplementSet:
    case AdvanceTractorECU:
    case AdvanceTractorECUImplementSet:
      if (version == 1) return 1;
      break;
    case FileServer:
    case FileServerClient:
      if (version == 2) return 1;
      if (version == 3) return 2;
      break;
    case SequenceControlServer:
    case SequenceControlClient:
      if (version == 1) return 1;
      break;
    case StopAllImplementOperationsInput:
    case StopAllImplementOperationsImplementSet:
      if (version == 1) return 1;
      break;
    case DiagnosticTool:
    case DiagnosticECU:
      if (version == 1) return 1;
      break;
    case Functionality_ReservedForISO:
      break;
  }
  isoaglib_assert(false);
  return 0;
}

bool DiagnosticProtocol_c::addFunctionality(FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description)
{
  if ( marr_functionalities.find(functionality) == marr_functionalities.end() )
  {
    marr_functionalities.insert( FunctionitiesArrPair(functionality, functionality_description) );
    mb_arrIsDirty = true;
    return true;
  }
  return false;
}

bool DiagnosticProtocol_c::addAefFunctionalitiesVirtualTerminal(bool implement, uint8_t version, const VirtualTerminalOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? VirtualTerminalWorkingSet : VirtualTerminal );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType1(bool function, uint8_t version, const AuxControlType1OptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( function ? AuxiliaryControlType1Functions : AuxiliaryControlType1Inputs );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAuxControlType2(bool function, uint8_t version, const AuxControlType2OptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( function ? AuxiliaryControlType2Functions : AuxiliaryControlType2Inputs );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerBasic(bool implement, uint8_t version, const TaskControllerBasicOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? TaskControllerBasicWorkingSet : TaskControllerBasic );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerGeo(bool implement, uint8_t version, const TaskControllerGeoOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? TaskControllerGeoWorkingSet : TaskControllerGeo );
  
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? TaskControllerSectionControlWorkingSet : TaskControllerSectionControl );
  
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_option_bytes = 2;
  functionality_description.options_bytes[0] = numberOfBooms;
  functionality_description.options_bytes[1] = numberOfSections;  

  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesBasicTractorECU(bool implement, uint8_t version, const BasicTractorECUOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? BasicTractorECUImplementSet : BasicTractorECU );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesAdvanceTractorECU(bool implement, uint8_t version, const AdvanceTractorECUOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? AdvanceTractorECUImplementSet : AdvanceTractorECU );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesSequenceControl(bool implement, uint8_t version, const SequenceControlOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? SequenceControlClient : SequenceControlServer );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesStopAllImplement(bool implement, uint8_t version, const StopAllImplementOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? StopAllImplementOperationsImplementSet : StopAllImplementOperationsInput );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesFileServer(bool implement, uint8_t version, const FileServerOptionsBitMask_t& options)
{
  FunctionalitiesCharacteristics_t functionality = ( implement ? FileServerClient : FileServer );
  return fillStructure(functionality, version, options);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticTool()
{
  FunctionalitiesCharacteristics_t functionality = DiagnosticTool;
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, 1); // TBD DiagnosticTool version ?
  functionality_description.number_of_option_bytes = 0;
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::addAefFunctionalitiesDiagnosticECU()
{
  FunctionalitiesCharacteristics_t functionality = DiagnosticECU;
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, 1); // TBD DiagnosticECU version ?
  functionality_description.number_of_option_bytes = 0;
  return addFunctionality(functionality, functionality_description);
}

bool DiagnosticProtocol_c::remAefFunctionalities(FunctionalitiesCharacteristics_t functionality)
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
