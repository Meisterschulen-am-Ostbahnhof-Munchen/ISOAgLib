/*
  diagnosticprotocol_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTIC_PROTOCOL_C_H
#define DIAGNOSTIC_PROTOCOL_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>
#include "diagnosticprotocoltypes.h"

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
    uint8_t number_of_option_bytes;                // TBD : replace with vector<uint8_t> ?
    uint8_t options_bytes[MAX_OPTION_BYTES];  // TBD : replace with vector<uint8_t> ?
  };
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s> FunctionitiesArr;
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s>::iterator FunctionitiesArrIter;
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s>::const_iterator FunctionitiesArrCIter;
  typedef STL_NAMESPACE::pair<FunctionalitiesCharacteristics_t,Functionality_s> FunctionitiesArrPair;

public:
  DiagnosticProtocol_c (IdentItem_c&, const EcuDiagnosticProtocolIdentificationBitMask_t& protocol);
  ~DiagnosticProtocol_c();

  void init();
  void close();

  bool addAefFunctionalitiesVirtualTerminal             (bool implement, uint8_t version, const VirtualTerminalOptionsBitMask_t& options);
  bool addAefFunctionalitiesAuxControlType1             (bool function,  uint8_t version, const AuxControlType1OptionsBitMask_t& options);
  bool addAefFunctionalitiesAuxControlType2             (bool function,  uint8_t version, const AuxControlType2OptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerBasic         (bool implement, uint8_t version, const TaskControllerBasicOptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerGeo           (bool implement, uint8_t version, const TaskControllerGeoOptionsBitMask_t& options);
  bool addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections);
  bool addAefFunctionalitiesBasicTractorECU             (bool implement, uint8_t version, const BasicTractorECUOptionsBitMask_t& options);
  bool addAefFunctionalitiesAdvanceTractorECU           (bool implement, uint8_t version, const AdvanceTractorECUOptionsBitMask_t& options);
  bool addAefFunctionalitiesSequenceControl             (bool implement, uint8_t version, const SequenceControlOptionsBitMask_t& options);
  bool addAefFunctionalitiesStopAllImplement            (bool implement, uint8_t version, const StopAllImplementOptionsBitMask_t& options);
  bool addAefFunctionalitiesFileServer                  (bool implement, uint8_t version, const FileServerOptionsBitMask_t& options);

  bool addAefFunctionalitiesDiagnosticTool();
  bool addAefFunctionalitiesDiagnosticECU();
    
  bool remAefFunctionalities(FunctionalitiesCharacteristics_t);

  void setUseAEFSpecifications(bool useAEF) { mb_useAefSpecifications = useAEF; mb_arrIsDirty = true; }

private:
  class MultiSendEventHandlerProxy_c : public MultiSendEventHandler_c {
  public:
    MultiSendEventHandlerProxy_c() : m_isMultiSendRunning(false) {}
    ~MultiSendEventHandlerProxy_c() {}
    bool m_isMultiSendRunning;
  private:
    void reactOnStateChange(const SendStream_c& /*sendStream*/);
  private:
    MultiSendEventHandlerProxy_c(MultiSendEventHandlerProxy_c const &);
    MultiSendEventHandlerProxy_c &operator=(MultiSendEventHandlerProxy_c const &);
  }; // MultiSendEventHandlerProxy_c

  bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/, int32_t );

  void sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,int32_t ai32_pgn);

  void updatePackage();

  bool addFunctionality(FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description);

  uint8_t getGeneration(FunctionalitiesCharacteristics_t functionality, uint8_t version) const;
  
  template <class T> bool fillStructure(FunctionalitiesCharacteristics_t functionality,
                                        uint8_t generation,
                                        const T& options);

private: // attributes
  IdentItem_c& mrc_identItem;
  uint8_t mui8_protocol;

  MultiSendEventHandlerProxy_c mt_multiSendEventHandler;

  uint8_t *m_currentFunctionalities;
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

template <class T>
bool DiagnosticProtocol_c::fillStructure(FunctionalitiesCharacteristics_t functionality,
                                         uint8_t version,
                                         const T& options)
{
  // fill structure
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);

  bool empty = true;
  for (uint8_t counter = 0; counter < options.getSizeInBytes(); ++counter)
  {
    if (options.getByte(counter) != 0)
      empty = false;
    functionality_description.options_bytes[counter] = options.getByte(counter);
  }
  // if no option is set, omit transmitting option bytes and set # of bytes 0
  functionality_description.number_of_option_bytes = ( empty ? 0 : options.getSizeInBytes());

  // add functionality
  return addFunctionality(functionality, functionality_description);
}

} // namespace __IsoAgLib
#endif
