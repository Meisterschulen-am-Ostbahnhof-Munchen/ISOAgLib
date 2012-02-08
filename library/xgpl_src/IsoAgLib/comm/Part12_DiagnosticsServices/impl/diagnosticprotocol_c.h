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

#include <map>

namespace IsoAgLib
{

enum FunctionalitiesCharacteristics_t
{
  NoFunctionalitiesReported = 0,
  VirtualTerminal,
  VirtualTerminalWorkingSet,
  AuxiliaryControlType1Inputs,
  AuxiliaryControlType1Functions,
  AuxiliaryControlType2Inputs,
  AuxiliaryControlType2Functions,
  TaskControllerBasic,
  TaskControllerBasicWorkingSet,
  TaskControllerGeo,
  TaskControllerGeoWorkingSet,
  TaskControllerSectionControl,
  TaskControllerSectionControlWorkingSet,
  BasicTractorECU,
  BasicTractorECUImplementSet,
  AdvanceTractorECU,
  AdvanceTractorECUImplementSet,
  FileServer,
  FileServerClient,
  SequenceControlServer,
  SequenceControlClient,
  StopAllImplementOperationsInput,
  StopAllImplementOperationsImplementSet,
  DiagnosticTool,
  DiagnosticECU,
  ReservedForISO = 0xFF
};

enum VirtualTerminalOptions_t
{
  VirtualTerminal_NoOptions = 0,
  VirtualTerminal_ProportionalFonts,
  VirtualTerminal_UserLayoutScreens,
  VirtualTerminal_ColourMap,
  VirtualTerminal_GraphicsContext,
  VirtualTerminal_ReservedForISO = 8
};

enum AuxControlType1Options_t
{
  AuxControlType1_NoOptions = 0,
  AuxControlType1_SupportsType0Function,
  AuxControlType1_SupportsType1Function,
  AuxControlType1_SupportsType2Function,
  AuxControlType1_ReservedForISO = 8
};

enum AuxControlType2Options_t
{
  AuxControlType2_NoOptions = 0,
  AuxControlType2_SupportsType0Function,
  AuxControlType2_SupportsType1Function,
  AuxControlType2_SupportsType2Function,
  AuxControlType2_SupportsType3Function,
  AuxControlType2_SupportsType4Function,
  AuxControlType2_SupportsType5Function,
  AuxControlType2_SupportsType6Function,
  AuxControlType2_SupportsType7Function,
  AuxControlType2_SupportsType8Function,
  AuxControlType2_SupportsType9Function,
  AuxControlType2_SupportsType10Function,
  AuxControlType2_SupportsType11Function,
  AuxControlType2_SupportsType12Function,
  AuxControlType2_SupportsType13Function,
  AuxControlType2_SupportsType14Function,
  AuxControlType2_ReservedForISO = 16
};

enum TaskControllerBasicOptions_t
{
  TaskControllerBasic_NoOptions = 0,
  TaskControllerBasic_ReservedForISO = 8
};

enum TaskControllerGeoOptions_t
{
  TaskControllerGeo_NoOptions = 0,
  TaskControllerGeo_ReservedForISO = 8
};

enum BasicTractorECUOptions_t
{
  BasicTractorECU_TECUNotMeetingCompleteClass1Requirements = 0,
  BasicTractorECU_Class1NoOptions,
  BasicTractorECU_Class2NoOptions,
  BasicTractorECU_ClassRequiredLighting,
  BasicTractorECU_NavigationOption,
  BasicTractorECU_FrontHitchOption,
  BasicTractorECU_GuidanceOption,
  BasicTractorECU_ReservedForISO = 8
};

enum AdvanceTractorECUOptions_t
{
  AdvanceTractorECU_TECUNotMeetingCompleteClass3Requirements = 0,
  AdvanceTractorECU_Class3NoOptions,
  AdvanceTractorECU_FullLightingMessageSet,
  AdvanceTractorECU_NavigationOption,
  AdvanceTractorECU_FrontHitchOption,
  AdvanceTractorECU_GuidanceOption,
  AdvanceTractorECU_PowertrainOption,
  AdvanceTractorECU_MotionInitiationOption,
  AdvanceTractorECU_ReservedForISO = 8
};

enum SequenceControlOptions_t
{
  SequenceControl_NoOptions = 0,
  SequenceControl_ReservedForISO = 8
};

enum StopAllImplementOptions_t
{
  StopAllImplement_NoOptions = 0,
  StopAllImplement_ReservedForISO = 8
};

enum FileServerOptions_t
{
  FileServer_NoOptions = 0,
  FileServer_ReservedForISO = 8
};

} // namespace IsoAgLib


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

  typedef std::vector<IsoAgLib::VirtualTerminalOptions_t> ArrVirtualTerminalOptions;
  typedef std::vector<IsoAgLib::AuxControlType1Options_t> ArrAuxControlType1Options;
  typedef std::vector<IsoAgLib::AuxControlType2Options_t> ArrAuxControlType2Options;
  typedef std::vector<IsoAgLib::TaskControllerBasicOptions_t> ArrTaskControllerBasicOptions;
  typedef std::vector<IsoAgLib::TaskControllerGeoOptions_t> ArrTaskControllerGeoOptions;
  //typedef std::vector<IsoAgLib::TaskControllerSectionControlOptions_t> ArrTaskControllerSectionControlOptions;
  typedef std::vector<IsoAgLib::BasicTractorECUOptions_t> ArrBasicTractorECUOptions;
  typedef std::vector<IsoAgLib::AdvanceTractorECUOptions_t> ArrAdvanceTractorECUOptions;
  typedef std::vector<IsoAgLib::SequenceControlOptions_t> ArrSequenceControlOptions;
  typedef std::vector<IsoAgLib::StopAllImplementOptions_t> ArrStopAllImplementOptions;
  typedef std::vector<IsoAgLib::FileServerOptions_t> ArrFileServerOptions;

public:
  DiagnosticProtocol_c (IdentItem_c&);
  virtual ~DiagnosticProtocol_c();

  void init();
  void close();

  virtual bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/, int32_t );

  bool addAefFunctionalitiesVirtualTerminal(bool implement, uint8_t version, const ArrVirtualTerminalOptions& options);
  bool addAefFunctionalitiesAuxControlType1(bool implement, uint8_t version, const ArrAuxControlType1Options& options);
  bool addAefFunctionalitiesAuxControlType2(bool implement, uint8_t version, const ArrAuxControlType2Options& options);
  bool addAefFunctionalitiesTaskControllerBasic(bool implement, uint8_t version, const ArrTaskControllerBasicOptions& options);
  bool addAefFunctionalitiesTaskControllerGeo(bool implement, uint8_t version, const ArrTaskControllerGeoOptions& options);
  bool addAefFunctionalitiesTaskControllerSectionControl(bool implement, uint8_t version, uint8_t numberOfBooms, uint8_t numberOfSections);
  bool addAefFunctionalitiesBasicTractorECU(bool implement, uint8_t version, const ArrBasicTractorECUOptions& options);
  bool addAefFunctionalitiesAdvanceTractorECU(bool implement, uint8_t version, const ArrAdvanceTractorECUOptions& options);
  bool addAefFunctionalitiesSequenceControl(bool implement, uint8_t version, const ArrSequenceControlOptions& options);
  bool addAefFunctionalitiesStopAllImplement(bool implement, uint8_t version, const ArrStopAllImplementOptions& options);
  bool addAefFunctionalitiesFileServer(bool implement, uint8_t version, const ArrFileServerOptions& options);

  bool addAefFunctionalitiesDiagnosticTool();
  bool addAefFunctionalitiesDiagnosticECU();

  bool remAefFunctionalities(IsoAgLib::FunctionalitiesCharacteristics_t);

  void setUseAEFSpecifications(bool useAEF) { mb_useAefSpecifications = useAEF; mb_arrIsDirty = true; }

private:
  void sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,int32_t ai32_pgn);

  void updatePackage();

  bool addFunctionality(IsoAgLib::FunctionalitiesCharacteristics_t functionality, const Functionality_s& functionality_description);

private: // attributes
  IdentItem_c& mrc_identItem;

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

} // namespace __IsoAgLib
#endif
