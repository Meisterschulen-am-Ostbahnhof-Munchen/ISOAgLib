/*
  idiagnosticprotocoltypes.h: types for Diagnostic Protocol Message

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDIAGNOSTICPROTOCOL_TYPES_H
#define IDIAGNOSTICPROTOCOL_TYPES_H

#include <IsoAgLib/hal/hal_typedef.h>

namespace IsoAgLib {

enum EcuDiagnosticProtocolIdentification_t
{
  ProtocolId_OnlyISO11783Level1Diagnostics = 0,
  ProtocolId_J1939_73,
  ProtocolId_ISO14230,
  ProtocolId_ISO15765_3,
  ProtocolId_ISO11783Level2,
  ProtocolId_ReservedForISO = 8
};

struct EcuDiagnosticProtocolIdentification_s
{
  typedef EcuDiagnosticProtocolIdentification_t enum_type;
  enum { number_of_bits = ProtocolId_ReservedForISO };
};
typedef BitFieldWrapper_c<EcuDiagnosticProtocolIdentification_s> EcuDiagnosticProtocolIdentificationBitMask_t;

enum FunctionalitiesCharacteristics_t
{
  NoFunctionalitiesReported = 0,  // TBD : PURPOSE OF NoFunctionalitiesReported ? REMOVE FROM ENUM AND START WITH VirtualTerminal = 1 ?
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
  Functionality_ReservedForISO = 0xFF
};

// VT
enum VirtualTerminalOptions_t
{
  VirtualTerminal_NoOptions = 0,          // TBD : PURPOSE OF NoOptions ? REMOVE FROM ENUM AND START WITH VALID OPTION = 1 ?
  VirtualTerminal_ProportionalFonts,
  VirtualTerminal_UserLayoutScreens,
  VirtualTerminal_ColourMap,
  VirtualTerminal_GraphicsContext,
  VirtualTerminal_ReservedForISO = 8
};

struct VirtualTerminalOptions_s
{
  typedef VirtualTerminalOptions_t enum_type;
  enum { number_of_bits = VirtualTerminal_ReservedForISO };
};
typedef BitFieldWrapper_c<VirtualTerminalOptions_s> VirtualTerminalOptionsBitMask_t;

// Aux 1
enum AuxControlType1Options_t
{
  AuxControlType1_NoOptions = 0,          // TBD : PURPOSE OF NoOptions ? REMOVE FROM ENUM AND START WITH VALID OPTION = 1 ?
  AuxControlType1_SupportsType0Function,
  AuxControlType1_SupportsType1Function,
  AuxControlType1_SupportsType2Function,
  AuxControlType1_ReservedForISO = 8
};

struct AuxControlType1Options_s
{
  typedef AuxControlType1Options_t enum_type;
  enum { number_of_bits = AuxControlType1_ReservedForISO };
};
typedef BitFieldWrapper_c<AuxControlType1Options_s> AuxControlType1OptionsBitMask_t;

// Aux 2
enum AuxControlType2Options_t
{
  AuxControlType2_NoOptions = 0,          // TBD : PURPOSE OF NoOptions ? REMOVE FROM ENUM AND START WITH VALID OPTION = 1 ?
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

struct AuxControlType2Options_s
{
  typedef AuxControlType2Options_t enum_type;
  enum { number_of_bits = AuxControlType2_ReservedForISO };
};
typedef BitFieldWrapper_c<AuxControlType2Options_s> AuxControlType2OptionsBitMask_t;

// TC Basic
enum TaskControllerBasicOptions_t          // TBD : NO OPTIONS SPECIFIED YET ? REMOVE ENUM ?
{
  TaskControllerBasic_NoOptions = 0,
  TaskControllerBasic_ReservedForISO = 8
};

struct TaskControllerBasicOptions_s
{
  typedef TaskControllerBasicOptions_t enum_type;
  enum { number_of_bits = TaskControllerBasic_ReservedForISO };
};
typedef BitFieldWrapper_c<TaskControllerBasicOptions_s> TaskControllerBasicOptionsBitMask_t;

// TC Geo
enum TaskControllerGeoOptions_t          // TBD : NO OPTIONS SPECIFIED YET ? REMOVE ENUM ?
{
  TaskControllerGeo_NoOptions = 0,
  TaskControllerGeo_ReservedForISO = 8
};

struct TaskControllerGeoOptions_s
{
  typedef TaskControllerGeoOptions_t enum_type;
  enum { number_of_bits = TaskControllerGeo_ReservedForISO };
};
typedef BitFieldWrapper_c<TaskControllerGeoOptions_s> TaskControllerGeoOptionsBitMask_t;

// Basic TECU
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

struct BasicTractorECUOptions_s
{
  typedef BasicTractorECUOptions_t enum_type;
  enum { number_of_bits = BasicTractorECU_ReservedForISO };
};
typedef BitFieldWrapper_c<BasicTractorECUOptions_s> BasicTractorECUOptionsBitMask_t;

// Advance TECU
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

struct AdvanceTractorECUOptions_s
{
  typedef AdvanceTractorECUOptions_t enum_type;
  enum { number_of_bits = AdvanceTractorECU_ReservedForISO };
};
typedef BitFieldWrapper_c<AdvanceTractorECUOptions_s> AdvanceTractorECUOptionsBitMask_t;

// SC
enum SequenceControlOptions_t          // TBD : NO OPTIONS SPECIFIED YET ? REMOVE ENUM ?
{
  SequenceControl_NoOptions = 0,
  SequenceControl_ReservedForISO = 8
};

struct SequenceControlOptions_s
{
  typedef SequenceControlOptions_t enum_type;
  enum { number_of_bits = SequenceControl_ReservedForISO };
};
typedef BitFieldWrapper_c<SequenceControlOptions_s> SequenceControlOptionsBitMask_t;

// Stop implements
enum StopAllImplementOptions_t          // TBD : NO OPTIONS SPECIFIED YET ? REMOVE ENUM ?
{
  StopAllImplement_NoOptions = 0,
  StopAllImplement_ReservedForISO = 8
};

struct StopAllImplementOptions_s
{
  typedef StopAllImplementOptions_t enum_type;
  enum { number_of_bits = StopAllImplement_ReservedForISO };
};
typedef BitFieldWrapper_c<StopAllImplementOptions_s> StopAllImplementOptionsBitMask_t;

// FS
enum FileServerOptions_t          // TBD : NO OPTIONS SPECIFIED YET ? REMOVE ENUM ?
{
  FileServer_NoOptions = 0,
  FileServer_ReservedForISO = 8
};

struct FileServerOptions_s
{
  typedef FileServerOptions_t enum_type;
  enum { number_of_bits = FileServer_ReservedForISO };
};
typedef BitFieldWrapper_c<FileServerOptions_s> FileServerOptionsBitMask_t;

} // end namespace IsoAgLib


#endif

