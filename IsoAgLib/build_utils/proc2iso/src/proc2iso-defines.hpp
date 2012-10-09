/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 ***************************************************************************/
#ifndef SYSTEM_PC_VC
	#include <stdint.h>
#else
	typedef unsigned char uint8_t;
	typedef signed char int8_t;
	typedef unsigned short uint16_t;
	typedef short int16_t;
	typedef unsigned int uint32_t;
	typedef int int32_t;
	typedef unsigned long long int uint64_t;
	typedef long int long int64_t;
#endif

#define stringLength 256


// Object Types
#define otDevice                       (0)
#define otDeviceElement                (1)
#define otDeviceProcessData            (2)
#define otDeviceProperty               (3)
#define otDeviceDinProcessData         (4)
#define otDeviceProcessDataCombination (5)
#define otDeviceValuePresentation      (6)
#define maxObjectTypes                 (7) // +++ MAX +++ //object will not be inserted if (objType >= maxObjectTypes)


// Object May Be
#define ombDevice                       (uint64_t(1)<<0)
#define ombDeviceElement                (uint64_t(1)<<1)
#define ombDeviceProcessData            (uint64_t(1)<<2)
#define ombDeviceProperty               (uint64_t(1)<<3)
#define ombDeviceDinProcessData         (uint64_t(1)<<4)
#define ombDeviceProcessDataCombination (uint64_t(1)<<5)
#define ombDeviceValuePresentation      (uint64_t(1)<<6)

// Attributes
#define attrDesignator (0)
#define attrSoftware_version (1)
#define attrWorkingset_mastername (2)
#define attrSerial_number (3)
#define attrStructure_label (4)
#define attrLocalization_label (5)
#define attrElement_type (6)
#define attrElement_number (7)
#define attrParent_name (8)
#define attrDdi (9)
#define attrProperties (10)
#define attrTrigger_methods (11)
#define attrDevice_value_presentation_name (12)
#define attrProperty_value (13)
#define attrLis (14)
#define attrWert_inst (15)
#define attrZaehl_num (16)
#define attrOffset (17)
#define attrScale (18)
#define attrNumber_of_decimals (19)
#define attrUnitdesignator (20)
#define attrSelf_conf (21)
#define attrIndustry_group (22)
#define attrFunction (23)
#define attrWanted_SA (24)
#define attrFunc_Inst (25)
#define attrECU_Inst (26)
#define attrStore_SA_at_EEPROM_address (27)
#define attrManufacturer_code (28)
#define attrDevice_class (29)
#define attrDevice_class_instance (30)
#define attrWS_identity_number (31)
#define attrFeature_set (32)
#define attrPriority (33)
#define attrDevProgVarName (34)
#define attrProcProgVarName (35)
#define attrCumulative_value (36)
#define attrCommand_type (37)
#define attrSetpoint (38)

#define maxAttributeNames (39)


#define maxObjectTypesToCompare (maxObjectTypes)
char otCompTable [maxObjectTypesToCompare] [stringLength+1] = {
    "device",
    "deviceelement",
    "deviceprocessdata",
    "deviceproperty",
    "devicedinprocessdata",
    "deviceprocessdatacombination",
    "devicevaluepresentation"
};

uint64_t omcTypeTable [maxObjectTypesToCompare] = {
    /* "device", */                       ombDeviceElement | ombDeviceValuePresentation,
    /* "deviceelement" */                 ombDeviceProcessData | ombDeviceProperty,
    /* "deviceprocessdata" */             ombDeviceDinProcessData | ombDeviceProcessDataCombination,
    /* "deviceproperty" */                0,
    /* "devicedinprocessdata" */          0,
    /* "deviceprocessdatacombination" */  0,
    /* "devicevaluepresentation" */       0
};

char otClassnameTable [maxObjectTypes] [stringLength+1] = {
    "Device",
    "DeviceElement",
    "DeviceProcessData",
    "DeviceProperty",
    "DeviceDinProcessData",
    "DeviceProcessDataCombination",
    "DeviceValuePresentation"
};


char attrNameTable [maxAttributeNames] [stringLength+1] = {
      "designator",
      "software_version",
      "workingset_mastername",
      "serialnumber",
      "structure_label",
      "localization_label",
      "element_type",
      "element_number",
      "parent_name",
      "ddi",
      "properties",
      "trigger_methods",
      "device_value_presentation_name",
      "property_value",
      "lis",
      "wert_inst",
      "zaehl_num",
      "offset",
      "scale",
      "number_of_decimals",
      "unit_designator",
      "self_configurable_address",
      "industry_group",
      "function",
      "wanted_sa",
      "function_instance",
      "ecu_instance",
      "store_sa_at_eeprom_address",
      "manufacturer_code",
      "device_class",
      "device_class_instance",
      "ws_identity_number",
      "feature_set",
      "priority",
      "device_program_name",
      "proc_program_name",
      "cumulative_value",
      "command_type",
      "is_setpoint"
};

#define maxTableID 7
char TableIDTable [maxTableID] [stringLength+1] = {
    "DVC",
    "DET",
    "DPD",
    "DPT",
    "DDP",
    "DPC",
    "DVP"
};

#define maxDeviceElementType 7
char DeviceElementTypeTable [maxDeviceElementType] [stringLength+1] = {
    "device",
    "function",
    "bin",
    "section",
    "unit",
    "connector",
    "navigationreference"
};

#define maxProperty 2
char PropertyTable [maxProperty] [stringLength+1] = {
    "defaultset",
    "setable"
};

#define maxTriggerMethods 5
char TriggerMethodTable [maxTriggerMethods] [stringLength+1] = {
    "timeinterval",
    "distanceinterval",
    "thresholdlimits",
    "onchange",
    "counter"
};

#define maxTruthTable 2
char truthTable [maxTruthTable] [stringLength+1] = {
    "yes",
    "true"
};

#define maxFalseTable 2
char falseTable [maxFalseTable] [stringLength+1] = {
    "no",
    "false"
};

#define maxFeatureSet 3
char featureSetTable [maxFeatureSet] [stringLength+1] = {
    "SimpleMeasure",
    "SimpleSetpoint",
    "SimpleSetpointSimpleMeasure"
};

#define maxCommandTypes 4
char CommandTypeTable [maxCommandTypes] [stringLength+1] = {
    "exact",
    "default",
    "min",
    "max"
};
