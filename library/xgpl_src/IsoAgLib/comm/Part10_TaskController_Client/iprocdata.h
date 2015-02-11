/*
  iprocdata.h: class with special (enum) types for ProcessData management

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

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
    enum RemoteType_t {
      RemoteTypeTaskController,
      RemoteTypeDataLogger,
      RemoteTypeProprietary };

    enum MeasurementCommand_t { 
      MeasurementCommandTimeProp =          0x4,
      MeasurementCommandDistProp =          0x5,
      MeasurementCommandMinimumThreshold =  0x6,
      MeasurementCommandMaximumThreshold =  0x7,
      MeasurementCommandOnChange =          0x8 };

    enum CommandType_t {
      requestConfiguration                  = 0x0,
      configurationResponse                 = 0x1,
      requestValue                          = 0x2,
      setValue                              = 0x3,
      measurementTimeValueStart             = 0x4,
      measurementDistanceValueStart         = 0x5,
      measurementMinimumThresholdValueStart = 0x6,
      measurementMaximumThresholdValueStart = 0x7,
      measurementChangeThresholdValueStart  = 0x8,
      commandReserved1                      = 0x9,
      commandReserved2                      = 0xa,
      commandReserved3                      = 0xb,
      commandReserved4                      = 0xc,
      nack                                  = 0xd,
      taskControllerStatus                  = 0xe,
      workingsetMasterMaintenance           = 0xf,

      CommandUndefined                      = 0x10
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
  
    typedef STL_NAMESPACE::list< STL_NAMESPACE::pair<const IsoAgLib::iIsoItem_c*, RemoteType_t> > ServerList;

  }

}

#endif
