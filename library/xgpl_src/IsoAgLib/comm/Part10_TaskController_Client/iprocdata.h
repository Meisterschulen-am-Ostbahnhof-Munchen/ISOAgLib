/*
  iprocdata.h: class with special (enum) types for ProcessData management

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCDATA_H
#define IPROCDATA_H

#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>
#include <list>


namespace IsoAgLib
{
  class iIsoItem_c;


  namespace ProcData
  {
    static const int ServerTypes = 3;
    enum ServerType_t {
      ServerTypeTaskController,
      ServerTypeDataLogger,
      ServerTypeProprietary };

    enum MeasurementCommand_t { 
      MeasurementCommandTimeProp =          0x4,
      MeasurementCommandDistProp =          0x5,
      MeasurementCommandMinimumThreshold =  0x6,
      MeasurementCommandMaximumThreshold =  0x7,
      MeasurementCommandOnChange =          0x8 };

    enum CommandType_t {
      TechnicalData                         = 0x0,
      DeviceDescriptor                      = 0x1,
      RequestValue                          = 0x2,
      Value                                 = 0x3,
      MeasurementTimeValueStart             = 0x4,
      MeasurementDistanceValueStart         = 0x5,
      MeasurementMinimumThresholdValueStart = 0x6,
      MeasurementMaximumThresholdValueStart = 0x7,
      MeasurementChangeThresholdValueStart  = 0x8,
      ControlAssignment                     = 0x9,
      SetValueAndAcknowledge                = 0xa,
      ReservedB                             = 0xb,
      ReservedC                             = 0xc,
      ProcessDataAcknowledge                = 0xd,
      TaskControllerStatus                  = 0xe,
      ClientTaskOrNotApplicable             = 0xf,
    };

    enum SpecialDDI_t {
      PgnBasedDataDDI       = 0xDFFE,
      DefaultDataLoggingDDI = 0xDFFF,
      ProprietaryDDIfirst   = 0xE000,
      ProprietaryDDIlast    = 0xFFFE,
      ReservedDDI           = 0xFFFF };

    enum DeviceObjectType_t { // exact order as needed in array lookup
      ObjectTypeDVC,
      ObjectTypeDET,
      ObjectTypeDPD,
      ObjectTypeDPT,
      ObjectTypeDVP };

    enum DeviceElementType_t {
      ElementTypeDevice = 1,
      ElementTypeFunction,
      ElementTypeBin,
      ElementTypeSection,
      ElementTypeUnit,
      ElementTypeConnector,
      ElementTypeNavigationReference };

    enum TriggerMethod_t { 
      TimeInterval = 0,
      DistInterval = 1,
      ThresholdLimit = 2, 
      OnChange = 3,
      Total = 4 };

    struct TriggerMethod_s {
      typedef TriggerMethod_t enum_type;
      enum { number_of_bits = 5 };
    };

    typedef BitFieldWrapperRight_c<TriggerMethod_s> Methods_t;

    enum Property_t {
      Defaultset = 0,
      Settable = 1,
      ControlSource = 2 };

    struct Property_s {
      typedef Property_t enum_type;
      enum { number_of_bits = 3 };
    };

    typedef BitFieldWrapperRight_c<Property_s> Properties_t;

    struct ClientCapabilities_s {
      uint8_t versionNr;

      bool hasTcBas;
      bool hasTcGeo;
      bool hasTcSc;

      unsigned numBoom;
      unsigned numSection;
      unsigned numBin;
    };

    struct ServerCapabilities_s {
      uint8_t versionNr;

      bool hasTcBas;
      bool hasTcGeoWithPbc;
      bool hasTcGeoWithoutPbc;
      bool hasPeerControl;
      bool hasTcSc;

      unsigned numBoom;
      unsigned numSection;
      unsigned numBin;
    };

    struct ConnectionCapabilities_s {
      ConnectionCapabilities_s()
        : versionNr(0)
        , hasPeerControl(false)
      {}

      uint8_t versionNr;
      bool hasPeerControl;
    };
}

}

#endif
