/*
  diagnosticprotocol_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTIC_PROTOCOL_C_H
#define DIAGNOSTIC_PROTOCOL_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>
#include "../idiagnosticprotocoltypes.h"

#include <map>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class IdentItem_c;

#define MAX_OPTION_BYTES 2

/**
  This class implements the handling of Diagnostic Protocol Message
  @author Antoine Kandera
  @author Martin Wodok
*/
class DiagnosticProtocol_c : public IsoRequestPgnHandler_c
{
public:
  struct Functionality_s {
    uint8_t generation;
    uint8_t number_of_options;                // TBD : replace with vector<uint8_t> ?
    uint8_t options_bytes[MAX_OPTION_BYTES];  // TBD : replace with vector<uint8_t> ?
  };
  typedef std::map<IsoAgLib::FunctionalitiesCharacteristics_t,Functionality_s> FunctionitiesArr;
  typedef std::map<IsoAgLib::FunctionalitiesCharacteristics_t,Functionality_s>::iterator FunctionitiesArrIter;
  typedef std::map<IsoAgLib::FunctionalitiesCharacteristics_t,Functionality_s>::const_iterator FunctionitiesArrCIter;
  typedef std::pair<IsoAgLib::FunctionalitiesCharacteristics_t,Functionality_s> FunctionitiesArrPair;

public:
  DiagnosticProtocol_c (IdentItem_c&, const IsoAgLib::EcuDiagnosticProtocolIdentificationBitMask_t& protocol);
  ~DiagnosticProtocol_c();

  void init();
  void close();

  bool addAefFunctionalitiesVirtualTerminal             (bool implement, uint8_t version, const IsoAgLib::VirtualTerminalOptionsBitMask_t& options);
  bool addAefFunctionalitiesAuxControlType1             (bool implement, uint8_t version, const IsoAgLib::AuxControlType1OptionsBitMask_t& options);
  bool addAefFunctionalitiesAuxControlType2             (bool implement, uint8_t version, const IsoAgLib::AuxControlType2OptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerBasic         (bool implement, uint8_t version, const IsoAgLib::TaskControllerBasicOptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerGeo           (bool implement, uint8_t version, const IsoAgLib::TaskControllerGeoOptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections);
  bool addAefFunctionalitiesBasicTractorECU             (bool implement, uint8_t version, const IsoAgLib::BasicTractorECUOptionsBitMask_t& options);
  bool addAefFunctionalitiesAdvanceTractorECU           (bool implement, uint8_t version, const IsoAgLib::AdvanceTractorECUOptionsBitMask_t& options);
  bool addAefFunctionalitiesSequenceControl             (bool implement, uint8_t version, const IsoAgLib::SequenceControlOptionsBitMask_t& options);
  bool addAefFunctionalitiesStopAllImplement            (bool implement, uint8_t version, const IsoAgLib::StopAllImplementOptionsBitMask_t& options);
  bool addAefFunctionalitiesFileServer                  (bool implement, uint8_t version, const IsoAgLib::FileServerOptionsBitMask_t& options);

  bool addAefFunctionalitiesDiagnosticTool();
  bool addAefFunctionalitiesDiagnosticECU();
    
  bool remAefFunctionalities(IsoAgLib::FunctionalitiesCharacteristics_t);

  void setUseAEFSpecifications(bool useAEF) { mb_useAefSpecifications = useAEF; mb_arrIsDirty = true; }

private:
  bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/, int32_t );

  void sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,int32_t ai32_pgn);

  void updatePackage();

  bool addFunctionality(IsoAgLib::FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description);

  uint8_t getGeneration(IsoAgLib::FunctionalitiesCharacteristics_t functionality, uint8_t version) const;
  
#ifdef USE_TEMPLATES
  template <class T> bool fillStructure(IsoAgLib::FunctionalitiesCharacteristics_t functionality, uint8_t generation, const T& options);
#endif

private: // attributes
  IdentItem_c& mrc_identItem;
  uint8_t mui8_protocol;

  uint8_t *mcstr_currentFunctionalities;
  uint16_t mui16_arrayLenght;

  FunctionitiesArr marr_functionalities;
  bool mb_arrIsDirty;

  bool mb_useAefSpecifications;

private:
  /** not copyable : copy constructor is only declared, never defined */
  DiagnosticProtocol_c(const DiagnosticProtocol_c&);
  /** not copyable : copy operator is only declared, never defined */
  DiagnosticProtocol_c& operator=(const DiagnosticProtocol_c&); 
};

#ifdef USE_TEMPLATES
template <class T>
bool DiagnosticProtocol_c::fillStructure(IsoAgLib::FunctionalitiesCharacteristics_t functionality, uint8_t version, const T& options)
{
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);
  functionality_description.number_of_options = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter) 
    functionality_description.options_bytes[counter] = options.getByte(counter);

  // add functionality
  return addFunctionality(functionality, functionality_description);
}
#endif

} // namespace __IsoAgLib
#endif
