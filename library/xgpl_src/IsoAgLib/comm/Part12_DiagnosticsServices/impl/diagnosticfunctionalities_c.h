/*
  diagnosticfunctionalities_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTICFUNCTIONALITIES_C_H
#define DIAGNOSTICFUNCTIONALITIES_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>
#include "diagnosticstypes.h"

#include <map>

#define MAX_OPTION_BYTES 2


namespace __IsoAgLib
{

class IdentItem_c;


/**
  This class implements the handling of ISO 11783-12 B.9 Control function functionalities
  @author Antoine Kandera
  @author Martin Wodok
*/
class DiagnosticFunctionalities_c : public IsoRequestPgnHandler_c
{
public:
  struct Functionality_s {
    uint8_t generation;
    uint8_t number_of_option_bytes;           // TBD : replace with vector<uint8_t> ?
    uint8_t options_bytes[MAX_OPTION_BYTES];  // TBD : replace with vector<uint8_t> ?

    Functionality_s();
  };
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s> FunctionalitiesArr;
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s>::iterator FunctionalitiesArrIter;
  typedef STL_NAMESPACE::map<FunctionalitiesCharacteristics_t,Functionality_s>::const_iterator FunctionalitiesArrCIter;
  typedef STL_NAMESPACE::pair<FunctionalitiesCharacteristics_t,Functionality_s> FunctionalitiesArrPair;

public:
  DiagnosticFunctionalities_c( IdentItem_c& );
  ~DiagnosticFunctionalities_c();

  void init();
  void close();

  bool addFunctionalitiesMinimumControlFunction      (uint8_t version, const MinimumControlFunctionOptionsBitMask_t& options);
  bool addFunctionalitiesUniversalTerminal           (bool implement, uint8_t version, const UniversalTerminalOptionsBitMask_t& options);
  bool addFunctionalitiesAuxO                        (bool function,  uint8_t version, const AuxOOptionsBitMask_t& options);
  bool addFunctionalitiesAuxN                        (bool function,  uint8_t version, const AuxNOptionsBitMask_t& options);
  bool addFunctionalitiesTaskControllerBasic         (bool implement, uint8_t version, const TaskControllerBasicOptionsBitMask_t& options);
  bool addFunctionalitiesTaskControllerGeo           (bool implement, uint8_t version, uint8_t numberOfChannels, const TaskControllerGeoOptionsBitMask_t& options);
  bool addFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections);
  bool addFunctionalitiesDataLogger                  (bool implement, uint8_t version, const DataLoggerOptionsBitMask_t& options);
  bool addFunctionalitiesBasicTractorECU             (bool implement, uint8_t version, const BasicTractorECUOptionsBitMask_t& options);
  bool addFunctionalitiesStopAllImplementOperations  (bool implement, uint8_t version, const StopAllImplementOperationsOptionsBitMask_t& options);
#if 0
// not supported by ISO yet
  bool addFunctionalitiesAdvanceTractorECU           (bool implement, uint8_t version, const AdvanceTractorECUOptionsBitMask_t& options);
  bool addFunctionalitiesSequenceControl             (bool implement, uint8_t version, const SequenceControlOptionsBitMask_t& options);
  bool addFunctionalitiesFileServer                  (bool implement, uint8_t version, const FileServerOptionsBitMask_t& options);

  bool addFunctionalitiesDiagnosticTool();
  bool addFunctionalitiesDiagnosticECU();
#endif
    
  bool remFunctionalities(FunctionalitiesCharacteristics_t);

  const FunctionalitiesArr& getRefFunctionalitiesArr() const { return m_functionalities; }

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

  bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/, ecutime_t );

  void sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,int32_t ai32_pgn);

  void updatePackageIfNeeded();

  bool addFunctionality(FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description);

  uint8_t getGeneration(FunctionalitiesCharacteristics_t functionality, uint8_t version) const;
  
  template <class T> bool fillStructure(FunctionalitiesCharacteristics_t functionality,
                                        uint8_t generation,
                                        const T& options);

private: // attributes
  IdentItem_c& mrc_identItem;

  MultiSendEventHandlerProxy_c mt_multiSendEventHandler;

  uint8_t *m_currentFunctionalities;
  uint16_t mui16_arrayLength;

  FunctionalitiesArr m_functionalities;
  bool m_functionalitiesDirty;

private:
  // not copyable
  DiagnosticFunctionalities_c(const DiagnosticFunctionalities_c&);
  DiagnosticFunctionalities_c& operator=(const DiagnosticFunctionalities_c&); 
};

template <class T>
bool DiagnosticFunctionalities_c::fillStructure(FunctionalitiesCharacteristics_t functionality,
                                         uint8_t version,
                                         const T& options)
{
  // fill structure in Little Endian!
  Functionality_s functionality_description;
  functionality_description.generation = getGeneration(functionality, version);

  functionality_description.number_of_option_bytes = 0;
  const uint8_t byteSize = options.getSizeInBytes();
  for (uint8_t counter = 0; counter < byteSize; ++counter)
  {
    const uint8_t curByte = options.getByte(byteSize - counter - 1);
    functionality_description.options_bytes[counter] = curByte;

    // From A.12: If a functionality has option bytes, all trailing zero option bytes
    //            shall be omitted and not counted in the number of option bytes.
    if (curByte != 0)
      functionality_description.number_of_option_bytes = counter+1;
  }

  // add functionality
  return addFunctionality(functionality, functionality_description);
}

} // __IsoAgLib

#endif
