/*
  iprocdata.h: class with special (enum) types for ProcessData management

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

    enum AckResponse_t {
      AckNoErrors = 0x00, // 0 = no errors
      NackProcessDataCommandNotSupported = 0x01, // Bit 1 = 1 Process Data Command not supported
      NackInvalidElementNumber = 0x02, // Bit 2 = 1 Invalid Element Number
      NackDDINotSupportedByElement = 0x04, // Bit 3 = 1 DDI not supported by element
      NackTriggerMethodNotSupported = 0x08, // Bit 4 = 1 Trigger method not supported
      NackProcessDataNotSettable = 0x10, // Bit 5 = 1 Process Data not settable
      NackInvalidOrUnsupportedIntervalOrThreshold = 0x20, // Bit 6 = 1 Invalid or unsupported interval or threshold
      NackProcessDataValueNotConformToDdiDefinition = 0x40, // Bit 7 = 1 Process data value does not conform to DDI definition (TCv4+)
      NackProcessDataValueIsOutsideOperationalRange = 0x80, // Bit 8 = 1 Process data value is outside the operational range of this device (TCv4+)
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

    class iNackHandler_c
    {
    public:
      // errBitmask is a combination of the bits defined in "ProcData::AckResponse_t ackResponse".
      // connection can be either of type
      // - iProcDataConnection_c
      // - iTcClientConnection_c
      // as there's an intermediate base-class missing, only the raw void pointer is returned so you can
      // compare the raw pointers against your connections to figure out to which one it applied.
      // Global: Only possible in CF<->CF direct PD connections, not in a TcClientConnection.
      virtual void handleNack(int32_t ddi, uint16_t element, uint8_t errBitmask, bool global, const void *connection) = 0;
    };

  }

}

#endif
