/*
  idiagnosticprotocoltypes.h: types for Diagnostic Protocol Message

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTICPROTOCOL_TYPES_H
#define DIAGNOSTICPROTOCOL_TYPES_H

#include <IsoAgLib/isoaglib_config.h>

namespace __IsoAgLib {

enum EcuDiagnosticProtocolIdentification_t
{
  ProtocolId_J1939_73 = 0,
  ProtocolId_ISO14230,
  ProtocolId_ISO15765_3,
  ProtocolId_ISO11783Level2,
  ProtocolId_BITSIZE = 8
};

struct EcuDiagnosticProtocolIdentification_s
{
  typedef EcuDiagnosticProtocolIdentification_t enum_type;
  enum { number_of_bits = ProtocolId_BITSIZE };
};
typedef BitFieldWrapperRight_c<EcuDiagnosticProtocolIdentification_s> EcuDiagnosticProtocolIdentificationBitMask_t;

enum FunctionalitiesCharacteristics_t
{
  //NoFunctionalitiesReported = 0,
  VirtualTerminal = 1,
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
  VirtualTerminal_ProportionalFonts = 0,
  VirtualTerminal_UserLayoutScreens,
  VirtualTerminal_ColourMap,
  VirtualTerminal_GraphicsContext,
  VirtualTerminal_BITSIZE = 8
};

struct VirtualTerminalOptions_s
{
  typedef VirtualTerminalOptions_t enum_type;
  enum { number_of_bits = VirtualTerminal_BITSIZE };
};
typedef BitFieldWrapperRight_c<VirtualTerminalOptions_s> VirtualTerminalOptionsBitMask_t;

// Aux 1
enum AuxControlType1Options_t
{
  AuxControlType1_SupportsType0Function = 0,
  AuxControlType1_SupportsType1Function,
  AuxControlType1_SupportsType2Function,
  AuxControlType1_BITSIZE = 8
};

struct AuxControlType1Options_s
{
  typedef AuxControlType1Options_t enum_type;
  enum { number_of_bits = AuxControlType1_BITSIZE };
};
typedef BitFieldWrapperRight_c<AuxControlType1Options_s> AuxControlType1OptionsBitMask_t;

// Aux 2
enum AuxControlType2Options_t
{
  AuxControlType2_SupportsType0Function = 0,
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
  AuxControlType2_BITSIZE = 16
};

struct AuxControlType2Options_s
{
  typedef AuxControlType2Options_t enum_type;
  enum { number_of_bits = AuxControlType2_BITSIZE };
};
typedef BitFieldWrapperRight_c<AuxControlType2Options_s> AuxControlType2OptionsBitMask_t;

// TC Basic
enum TaskControllerBasicOptions_t
{
  TaskControllerBasic_BITSIZE = 0
};

struct TaskControllerBasicOptions_s
{
  typedef TaskControllerBasicOptions_t enum_type;
  enum { number_of_bits = TaskControllerBasic_BITSIZE };
};
typedef BitFieldWrapperRight_c<TaskControllerBasicOptions_s> TaskControllerBasicOptionsBitMask_t;

// TC Geo
enum TaskControllerGeoOptions_t
{
  TaskControllerGeo_BITSIZE = 0
};

struct TaskControllerGeoOptions_s
{
  typedef TaskControllerGeoOptions_t enum_type;
  enum { number_of_bits = TaskControllerGeo_BITSIZE };
};
typedef BitFieldWrapperRight_c<TaskControllerGeoOptions_s> TaskControllerGeoOptionsBitMask_t;

// Basic TECU
enum BasicTractorECUOptions_t
{
  BasicTractorECU_Class1NoOptions = 0,
  BasicTractorECU_Class2NoOptions,
  BasicTractorECU_ClassRequiredLighting,
  BasicTractorECU_NavigationOption,
  BasicTractorECU_FrontHitchOption,
  BasicTractorECU_GuidanceOption,
  BasicTractorECU_BITSIZE = 8
};

struct BasicTractorECUOptions_s
{
  typedef BasicTractorECUOptions_t enum_type;
  enum { number_of_bits = BasicTractorECU_BITSIZE };
};
typedef BitFieldWrapperRight_c<BasicTractorECUOptions_s> BasicTractorECUOptionsBitMask_t;

// Advance TECU
enum AdvanceTractorECUOptions_t
{
  AdvanceTractorECU_Class3NoOptions = 0,
  AdvanceTractorECU_FullLightingMessageSet,
  AdvanceTractorECU_NavigationOption,
  AdvanceTractorECU_FrontHitchOption,
  AdvanceTractorECU_GuidanceOption,
  AdvanceTractorECU_PowertrainOption,
  AdvanceTractorECU_MotionInitiationOption,
  AdvanceTractorECU_BITSIZE = 8
};

struct AdvanceTractorECUOptions_s
{
  typedef AdvanceTractorECUOptions_t enum_type;
  enum { number_of_bits = AdvanceTractorECU_BITSIZE };
};
typedef BitFieldWrapperRight_c<AdvanceTractorECUOptions_s> AdvanceTractorECUOptionsBitMask_t;

// SC
enum SequenceControlOptions_t
{
  SequenceControl_BITSIZE = 0
};

struct SequenceControlOptions_s
{
  typedef SequenceControlOptions_t enum_type;
  enum { number_of_bits = SequenceControl_BITSIZE };
};
typedef BitFieldWrapperRight_c<SequenceControlOptions_s> SequenceControlOptionsBitMask_t;

// Stop implements
enum StopAllImplementOptions_t
{
  StopAllImplement_BITSIZE = 0
};

struct StopAllImplementOptions_s
{
  typedef StopAllImplementOptions_t enum_type;
  enum { number_of_bits = StopAllImplement_BITSIZE };
};
typedef BitFieldWrapperRight_c<StopAllImplementOptions_s> StopAllImplementOptionsBitMask_t;

// FS
enum FileServerOptions_t
{
  FileServer_BITSIZE = 0
};

struct FileServerOptions_s
{
  typedef FileServerOptions_t enum_type;
  enum { number_of_bits = FileServer_BITSIZE };
};
typedef BitFieldWrapperRight_c<FileServerOptions_s> FileServerOptionsBitMask_t;

} // end namespace IsoAgLib


#endif

