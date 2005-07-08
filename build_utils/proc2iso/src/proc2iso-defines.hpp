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
#include <stdint.h>
#define stringLength 256


// Object Types
#define otDevice                  (0)
#define otDeviceElement           (1)
#define otDeviceProcessData       (2)
#define otDeviceProperty          (3)
#define otDeviceDinProcessData    (4)
#define otDeviceValuePresentation (5)
#define maxObjectTypes            (6) // +++ MAX +++ //object will not be inserted if (objType >= maxObjectTypes)


// Object May Be
#define ombDevice                   (uint64_t(1)<<0)
#define ombDeviceElement            (uint64_t(1)<<1)
#define ombDeviceProcessData        (uint64_t(1)<<2)
#define ombDeviceProperty           (uint64_t(1)<<3)
#define ombDeviceDinProcessData     (uint64_t(1)<<4)
#define ombDeviceValuePresentation  (uint64_t(1)<<5)

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
#define attrOffset (16)
#define attrScale (17)
#define attrNumber_of_decimals (18)
#define attrUnitdesignator (19)
#define attrSelf_conf (20)
#define attrIndustry_group (21)
#define attrFunction (22)
#define attrWanted_SA (23)
#define attrFunc_Inst (24)
#define attrECU_Inst (25)
#define attrStore_SA_at_EEPROM_address (26)
#define attrManufacturer_code (27)
#define attrDevice_class (28)
#define attrDevice_class_instance (29)
#define attrIdentity_number (30)
#define attrFeature_set (31)
#define attrPriority (32)
#define attrDevProgVarName (33)
#define attrProcProgVarName (34)
#define attrCumulative_value (35)

#define maxAttributeNames (36)


#define maxObjectTypesToCompare (maxObjectTypes)
char otCompTable [maxObjectTypesToCompare] [stringLength+1] = {
    "device",
    "deviceelement",
    "deviceprocessdata",
    "deviceproperty",
    "devicedinprocessdata",
    "devicevaluepresentation"
};

uint64_t omcTypeTable [maxObjectTypesToCompare] = {
    /* "device", */                  ombDeviceElement,
    /* "deviceelement" */            ombDeviceProcessData | ombDeviceProperty,
    /* "deviceprocessdata" */        ombDeviceDinProcessData | ombDeviceValuePresentation,
    /* "deviceproperty" */           ombDeviceValuePresentation,
    /* "devicedinprocessdata" */     0,
    /* "devicevaluepresentation" */  0

};

char otClassnameTable [maxObjectTypes] [stringLength+1] = {
    "Device",
    "DeviceElement",
    "DeviceProcessData",
    "DeviceProperty",
    "DeviceDinProcessData",
    "DeviceValuePresentation",
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
      "identity_number",
      "feature_set",
      "priority",
      "device_program_name",
      "proc_program_name",
      "cumulative_value"
};


#define maxTableID 6
char TableIDTable [maxTableID] [stringLength+1] = {
    "DVC",
    "DET",
    "DPD",
    "DPT",
    "DDP",
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
    "navigation_reference"
};

#define maxProperty 2
char PropertyTable [maxProperty] [stringLength+1] = {
    "defaultset",
    "setable"
};

#define maxTriggerMethods 5
char TriggerMethodTable [maxTriggerMethods] [stringLength+1] = {
    "time_interval",
    "distance_interval",
    "threshold_limits",
    "on_change",
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
    "SimpleSetPoint",
    "SimpleMeasureSetPoint"
};
