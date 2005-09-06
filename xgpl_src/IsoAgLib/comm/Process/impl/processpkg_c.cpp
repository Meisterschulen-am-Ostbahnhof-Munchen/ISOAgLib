/***************************************************************************
                          processpkg_c.cpp - data object for Process-Data
                                           messages
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

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
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "processpkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

namespace __IsoAgLib {


/** default constructor which has nothing to do */
ProcessPkg_c::ProcessPkg_c( int ri_singletonVecKey ) : CANPkgExt_c( ri_singletonVecKey ) {
  c_specialTermGtp.setUnspecified();
  c_specialTermUseProcGtp.setUnspecified();
}
/** default constructor which has nothing to do */
ProcessPkg_c::~ProcessPkg_c(){
}

/**
  check if the 4byte value of the message has a special command of type
  proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  @param ren_checkCmd special command to check for
  @return true -> the given cmd was received
*/
bool ProcessPkg_c::isSpecCmd(proc_specCmd_t ren_checkCmd)const
{
  bool b_result = false;


  int32_t i32_test = *(static_cast<const int32_t*>(static_cast<const void*>(pb_procData)));
  // DIN: pd = 0 && mod < 4
  if (c_generalCommand.getCommand() == GeneralCommand_c::setValue &&
      c_generalCommand.checkIsSetpoint())
  { // setpoint value -> special commands are possible for exact, min, max, default setpopints
      if ((ren_checkCmd & setpointReleaseCmd != 0)
       &&(i32_test == static_cast<int32_t>(SETPOINT_RELEASE_COMMAND)))
      {
        b_result = true;
      }
      else
      {
        if ((ren_checkCmd & setpointErrCmd != 0)
         && (i32_test == static_cast<int32_t>(SETPOINT_ERROR_COMMAND))) b_result = true;
      }
  }

  // DIN: pd !=0 && mod < 5
  if (c_generalCommand.getCommand() == GeneralCommand_c::setValue &&
      !c_generalCommand.checkIsSetpoint())
  { // measure value: conversion if: actual, min, max, integ, med
    // check for command values
    if ((ren_checkCmd & noVal_32s != 0)
      &&(i32_test == static_cast<int32_t>(NO_VAL_32S)))
    {
      b_result = true;
    }
    else
    {
      if ((ren_checkCmd & errVal_32s != 0)
      && (i32_test == static_cast<int32_t>(ERROR_VAL_32S))) b_result = true;
    }
  }
  return b_result;
}


/**
  deliver data value as int32_t; the 4byte data of the message are
  accessed with the type defined by the format flags
  -> then it's assigned to the wanted type (e.g. if float value
  is received, this function access it as float and delivers the
  rounded value as int32_t)
  (4 uint8_t signed integer defined by int32_t in masterHeader)
  @return data value of message
*/
int32_t ProcessPkg_c::dataLong()const{
  int32_t i32_result;
  switch (valType())
  {
    case i32_val:
    case cmdVal:
      i32_result = *(static_cast<const int32_t*>(static_cast<const void*>(pb_procData)));
      break;
    case ui32_val:
      i32_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_procData)));
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      int2Float(pb_procData, ((float*)(&i32_result)));
      break;
#endif
  }
  return i32_result;
};
/**
  deliver data value as uint32_t; the 4byte data of the message are
  accessed with the type defined by the format flags
  -> then it's assigned to the wanted type (e.g. if float value
  is received, this function access it as float and delivers the
  rounded value as uint32_t)
  (4 uint8_t signed integer defined by uint32_t in masterHeader)
  @return data value of message
*/
uint32_t ProcessPkg_c::dataUlong()const{
  uint32_t ulResult;
  switch (valType())
  {
    case i32_val:
    case cmdVal:
      ulResult = *(static_cast<const int32_t*>(static_cast<const void*>(pb_procData)));
      break;
    case ui32_val:
      ulResult = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_procData)));
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      int2Float(pb_procData, ((float*)(&ulResult)));
      break;
#endif
  }
  return ulResult;
};
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
/**
  deliver data value as float; the 4byte data of the message are
  accessed with the type defined by the format flags
  -> then it's assigned to the wanted type (e.g. if int32_t value
  is received, this function access it as int32_t and delivers the
  value as float)
  @return data value of message
*/
float ProcessPkg_c::dataFloat()const{
  float f_result;
  switch (valType())
  {
    case i32_val:
      f_result = *(static_cast<const int32_t*>(static_cast<const void*>(pb_procData)));
      break;
    case ui32_val:
      f_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_procData)));
      break;
    case float_val:
      int2Float(pb_procData, &f_result);
      break;
    case cmdVal:
      f_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_procData)));
      break;
  }
  return f_result;
};
#endif

/**
  set the 4byte data as int32_t val without any conversion
  for cmd's like measurement program increment, meas prog start/stop,
  etc.
  (4 uint8_t signed integer defined by int32_t in masterHeader)
  @param ri32_val new cmd value for message
  @param ren_procValType data type for message string (default i32_val)
*/
void ProcessPkg_c::setDataRawCmd(int32_t ri32_val, proc_valType_t ren_procValType)
{
  *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = ri32_val;

#ifndef USE_ISO_11783  // no need to do this switch case if new ISO part 10 is used. -bac
  switch (ren_procValType)
  {
    case i32_val:
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case ui32_val:
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      set_d(1);
      bit_data.b_valType = ren_procValType;
      break;
#endif
    case cmdVal: // only to avoid compiler warning
      break;
  }
#else
  bit_data.b_valType = ren_procValType;
#endif
}


/**
  set data value as with int32_t value parameter
  and with defined send value type and set data type flag
  (if parameter and send type are different,
   a conversion by assignment is performed; %e.g. int32_t val 3
   can be sent as float 3.0)
  @param ri32_val new data value for message
  @param ren_procValType data type for message string (default i32_val)
*/
void ProcessPkg_c::setData(int32_t ri32_val, proc_valType_t ren_procValType)
{
  switch (ren_procValType)
  {
    case i32_val:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = ri32_val;
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_procData))) = ri32_val;
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      float2Int( ((float *const)(&ri32_val)), pb_procData);
      set_d(1);
      bit_data.b_valType = ren_procValType;
      break;
#endif
    case cmdVal: // only to avoid compiler warning
      break;
  }
}

/**
  set data value as with uint32_t value parameter
  and with defined send value type and set data type flag
  (if parameter and send type are different,
   a conversion by assignment is performed; %e.g. int32_t val 3
   can be sent as float 3.0)
  @param ri32_val new data value for message
  @param ren_procValType data type for message string (default ui32_val)
*/
void ProcessPkg_c::setData(uint32_t ri32_val, proc_valType_t ren_procValType)
{
  switch (ren_procValType)
  {
    case i32_val:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = ri32_val;
      set_d(0);
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_procData))) = ri32_val;
      set_d(0);
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      float2Int( ((float *const)(&ri32_val)), pb_procData);
      set_d(1);
      break;
#endif
    case cmdVal: // only to avoid compiler warning
      break;
  }
  bit_data.b_valType = ren_procValType;
}
/**
  set one of the special commands of type proc_specCmd_t:
  setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  with specified data type specifying flag in CAN data string
  @param ren_procSpecCmd special command to send
  @param ren_procValType data type for message string (default ui32_val)
*/
void ProcessPkg_c::setData(proc_specCmd_t ren_procSpecCmd, proc_valType_t ren_procValType)
{
  switch (ren_procSpecCmd)
  {
    case setpointReleaseCmd:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = SETPOINT_RELEASE_COMMAND;
      break;
    case setpointErrCmd:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = SETPOINT_ERROR_COMMAND;
      break;
    case noVal_32s:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = NO_VAL_32S;
      break;
    case errVal_32s:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = ERROR_VAL_32S;
      break;
  }
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
  if (ren_procValType == float_val)  set_d(1);
  else
#endif
  set_d(0);

  bit_data.b_valType = ren_procValType;
}

#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
/**
  set data value as with float value parameter
  and with defined send value type and set data type flag
  (if parameter and send type are different,
   a conversion by assignment is performed; %e.g. int32_t val 3
   can be sent as float 3.0)
  @param ri32_val new data value for message
  @param ren_procValType data type for message string (default float_val)
*/
void ProcessPkg_c::setData(float rf_val, proc_valType_t ren_procValType)
{
  switch (ren_procValType)
  {
    case i32_val:
      *(static_cast<int32_t*>(static_cast<void*>(pb_procData))) = static_cast<int32_t>(rf_val);
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_procData))) = static_cast<uint32_t>(rf_val);
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case float_val:
      float2Int(&rf_val, pb_procData);
      set_d(1);
      bit_data.b_valType = ren_procValType;
      break;
    default:
      break;
  }
}
#endif



/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CANPkg_c or CANPkgExt
  @see CANPkg_c::operator=
  @see CANPkgExt_c::operator=
*/
void ProcessPkg_c::string2Flags()
{
#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
  if (identType() == Ident_c::StandardIdent)
#endif
  {
#ifdef USE_DIN_9684

    setPri(((ident() >> 8) & 0xF));
    setEmpf(((ident() >> 4) & 0xF));
    setSend(ident() & 0xF);

    setLis(CANPkg_c::pb_data[0] >> 5);

    set_d(CANPkg_c::pb_data[2] >> 7);
    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    if (d()==0)
    #endif
      bit_data.b_valType = i32_val;
    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    else bit_data.b_valType = float_val;
    #endif
    setGety((CANPkg_c::pb_data[2] >> 3) & 0xF);
    setPos(CANPkg_c::pb_data[2] & 0x7);

    DINMonitor_c &c_din_monitor = getDinMonitorInstance4Comm();
    // now set pc_monitorSend and pc_monitorEmpf
    if (((pri() == 5) || (pri() == 2)) && (c_din_monitor.existDinMemberNr(empf())))
    { // DIN targeted process msg with empf as defined DIN member
      pc_monitorEmpf = static_cast<MonitorItem_c*>(&(c_din_monitor.dinMemberNr(empf())));
    }
    else
    { // either no target process msg or empf no defined member
      pc_monitorEmpf = NULL;
    }
    if (c_din_monitor.existDinMemberNr(send()))
    { // sender SEND registered as
      pc_monitorSend = static_cast<MonitorItem_c*>(&(c_din_monitor.dinMemberNr(send())));
    }
    else
    { // send is no defined member
      pc_monitorSend = NULL;
    }

    setPd(((CANPkg_c::pb_data[0] >> 3) & 0x3));
    setMod((CANPkg_c::pb_data[0] & 0x7));

    setZaehlnum(CANPkg_c::pb_data[1]);

    setWert(CANPkg_c::pb_data[3] >> 4);
    setInst((CANPkg_c::pb_data[3] & 0xF));

    // some DIN systems place wrong GETY in data string -> correct GETY, for all requests to GETY of receiver
    // and correct GETY for all messages from special registered terminal
    if ( (pc_monitorEmpf != NULL) // receiver must be found in monitor list
         && (pc_monitorSend != NULL) // sender must be found in monitor list
         && ( gety() != 0 ) // Proc is not of special GetyPos 0,0 case
         && ( pos() != 0 ) // Proc is not of special GetyPos 0,0 case
         && ( ( ( pd() & 0x2 ) != 0 ) // first alternative: it's a request
              ||   ( ( c_specialTermGtp ==  pc_monitorSend->gtp() ) && (c_specialTermGtp.isSpecified()) ) // or it's from known special terminal
              )
         )
    { // sender is special case terminal -> change GETY_POS for data part from terminal GETY_POS to local of empf
      setGety((pc_monitorEmpf->gtp()).getGety());
      setPos((pc_monitorEmpf->gtp()).getPos());
    }
#endif //DIN

  }

#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
  else
#endif
  {
#ifdef USE_ISO_11783

    // New Part 10 code to go here -bac
    // set pri, empf, send for convenience
    setPri(2); // signal target message
    setEmpf(isoPs());
    setSend(isoSa());
    setIdentType(Ident_c::ExtendedIdent);

    setLis(0); // ISO doesn't support LIS code -> set to default 0

    // bit_data.b_valType = static_cast<proc_valType_t>((CANPkg_c::pb_data[0] >> 5) & 0x3);

    // Not sure if this is needed at this point. May need the GPS portion but not the Float Data Type stuff since this is not really used in Part 10 now. -bac
    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    if (bit_data.b_valType == float_val) set_d(1);
    else
    #endif
    set_d(0);

    //Need to replace this call with the getpos from the monitor item. Getypos no longer encapsulated in the message data itself
    //See new line added below that uses c_isoMonitor. -bac
    //setGtp( GetyPos_c(((CANPkg_c::pb_data[2] >> 4) & 0xF), (CANPkg_c::pb_data[2] & 0xF) ) );

    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();

    // gtp in ProcessPkg_c is no longer used in ISO (for DIN the message contains the empf gtp!)
    //setGtp(c_isoMonitor.isoMemberNr(send()).gtp());  // Get the gety and pos (Device Class, Device Class Instance -bac

    // now set pc_monitorSend and pc_monitorEmpf
    if ((pri() == 2) && (c_isoMonitor.existIsoMemberNr(empf())))
    { // ISO targeted process msg with empf as defined ISO member
      pc_monitorEmpf = static_cast<MonitorItem_c*>(&(c_isoMonitor.isoMemberNr(empf())));
    }
    else
    { // either no target process msg or empf no defined member
      pc_monitorEmpf = NULL;
    }
    if (c_isoMonitor.existIsoMemberNr(send()))
    { // sender SEND registered as
      pc_monitorSend = static_cast<MonitorItem_c*>(&(c_isoMonitor.isoMemberNr(send())));
    }
    else
    { // send is no defined member
      pc_monitorSend = NULL;
    }

    set_Cmd(CANPkg_c::pb_data[0] & 0xf);
    uint16_t element = 0;
     element = uint16_t(CANPkg_c::pb_data[1]) << 4;
    element |= ((CANPkg_c::pb_data[0] & 0xF0)>>4);
    set_Element(element);

    uint16_t newDDI = 0;
    newDDI |= CANPkg_c::pb_data[3];
    newDDI = newDDI << 8;
    newDDI |= CANPkg_c::pb_data[2];
    set_DDI(newDDI);

#endif  // USE_ISO_11783
  }

  CNAMESPACE::memmove(pb_procData, (CANPkg_c::pb_data+4), 4);
};

/**
  overloaded virtual function to translate flag values to data string;
  needed for sending informations from this object via CANIO_c on CAN BUS,
  because CANIO_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CANPkg_c::getData
  @see CANPkgExt_c::getData
*/
void ProcessPkg_c::flags2String()
{

  if (identType() == Ident_c::StandardIdent)
  {

#ifdef USE_DIN_9684
    uint8_t ui8_mod = 0;
    uint8_t ui8_pd = 0;

    switch (c_generalCommand.getCommand()) {
      case GeneralCommand_c::requestValue:
      case GeneralCommand_c::setValue:
         if (c_generalCommand.checkIsSetpoint()) {
            // setpoint
            switch (c_generalCommand.getValueGroup()) {
               case GeneralCommand_c::exactValue: ui8_mod = 0; break;
               case GeneralCommand_c::minValue:   ui8_mod = 2; break;
               case GeneralCommand_c::maxValue:   ui8_mod = 3; break;
            }
         } else {
           // measure
           ui8_pd |= 1;
            switch (c_generalCommand.getValueGroup()) {
               case GeneralCommand_c::exactValue: ui8_mod = 0; break;
               case GeneralCommand_c::minValue:   ui8_mod = 1; break;
               case GeneralCommand_c::maxValue:   ui8_mod = 2; break;
               case GeneralCommand_c::integValue: ui8_mod = 3; break;
               case GeneralCommand_c::medValue:   ui8_mod = 4; break;
            }
         }
         break;

      case GeneralCommand_c::measurementTimeValue:            ui8_pd = 0; ui8_mod = 4; break;
      case GeneralCommand_c::measurementDistanceValue:        ui8_pd = 0; ui8_mod = 4; break;
      case GeneralCommand_c::measurementChangeThresholdValue: ui8_pd = 0; ui8_mod = 4; break;

      case GeneralCommand_c::measurementStart:
      case GeneralCommand_c::measurementStop:
      case GeneralCommand_c::measurementReset:
        ui8_pd = 0; ui8_mod = 6;
        break;
      default: ;
    }

    if (c_generalCommand.checkIsRequest())
       ui8_pd |= 1 << 1;

    setPd(ui8_pd);
    setMod(ui8_mod);

    // build ident dependent on PRI
    if (pri() == 1)
    { // base process msg
      setIdent((((pri() & 0x7) << 8) | (2 << 6) |((pd() & 0x3) << 4) | (send() & 0xF)), Ident_c::StandardIdent);
    }
    else
    { // target process msg
      setIdent((((pri() & 0x7) << 8) |((empf() & 0xF) << 4) | (send() & 0xF)), Ident_c::StandardIdent);
    }
    CANPkg_c::pb_data[0] = ((lis() & 0x7) << 5) |((pd() & 0x3) << 3) | (mod() & 0x7);

    // if c_specialTermGtp != GetyPos_c::GetyPosUnspecified dependent on gtp receiver of the GETY_POS of this data should be changed
    // to remote gtp
    CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | ( gtp().getGety() << 3 ) | ( gtp().getPos() & 0x7 );
    #if 0
    if ( (c_specialTermGtp.isSpecified())
      && (c_specialTermGtp == getDinMonitorInstance4Comm().dinMemberNr(empf()).gtp())
      && ( ( (wert() == 0) && (inst() >= 0xC) ) || ( (wert() == 0xF) &&  ( (inst() == 3) || (inst() == 4) ) ) )
       )
    { // sender is special case terminal -> change GETY_POS for data part to terminal GETY_POS to local from empf
      CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | ( gtp().getGety() << 3 ) | ( gtp().getPos() & 0x7 );
    }
    else
    { // standard case
      CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | ( gtp().getGety() << 3 ) | ( gtp().getPos() & 0x7 );
    }
    #endif

    CANPkg_c::pb_data[1] = zaehlnum();
    CANPkg_c::pb_data[3] = (wert() << 4) | (inst() & 0xF);

    CNAMESPACE::memmove((CANPkg_c::pb_data+4), pb_procData, 4);

    if ((pd() >> 1) == 1)
    { // request has only 4 bytes
      setLen(4);
    }
    else
    { // value send with 8 bytes
      setLen(8);
    }
#endif // USE_DIN_9684

  }
  else
  {  // code added by Brad Cox to format the message to conform to the latest part 10 Specification. -bac

#ifdef USE_ISO_11783
    uint8_t ui8_cmd;
    switch (c_generalCommand.getCommand()) {
      case GeneralCommand_c::requestConfiguration:                  ui8_cmd = 0; break;
      case GeneralCommand_c::configurationResponse:                 ui8_cmd = 1; break;
      case GeneralCommand_c::requestValue:                          ui8_cmd = 2; break;
      case GeneralCommand_c::setValue:                              ui8_cmd = 3; break;
      case GeneralCommand_c::measurementTimeValueStart:             ui8_cmd = 4; break;
      case GeneralCommand_c::measurementDistanceValueStart:         ui8_cmd = 5; break;
      case GeneralCommand_c::measurementMinimumThresholdValueStart: ui8_cmd = 6; break;
      case GeneralCommand_c::measurementMaximumThresholdValueStart: ui8_cmd = 7; break;
      case GeneralCommand_c::measurementChangeThresholdValueStart:  ui8_cmd = 8; break;
      case GeneralCommand_c::nack:                                  ui8_cmd = 0xd; break;
      case GeneralCommand_c::taskControllerStatus:                  ui8_cmd = 0xe; break;
      case GeneralCommand_c::workingsetMasterMaintenance:           ui8_cmd = 0xf; break;
      // map reset command to setValue command
      case GeneralCommand_c::measurementReset:                      ui8_cmd = 3; break;
      default: ui8_cmd = 0xFF;
    }

    CANPkg_c::pb_data[0] = (ui8_cmd & 0xf) | ( (element() & 0xf) << 4);
    CANPkg_c::pb_data[1] = element() >> 4;
    CANPkg_c::pb_data[2] = DDI() & 0x00FF ;
    CANPkg_c::pb_data[3] = (DDI()& 0xFF00) >> 8 ;
    // for ISO the ident is directly read and written

    CNAMESPACE::memmove((CANPkg_c::pb_data+4), pb_procData, 4);

    setLen(8);
#endif  // USE_ISO_11783
  }
};

/**
  deliver reference to MonitorItem_c of EMPF member (MonitorItem_c is base class for both
  DINItem_c and ISOItem_c)
  (check with existMemberEmpf before access to not defined item)

  @return reference to MonitorItem_c of member which is addressed by EMPF
  @exception containerElementNonexistant
*/
MonitorItem_c& ProcessPkg_c::memberEmpf() const
{ // check if pc_monitorEmpf is set
  if (pc_monitorEmpf == NULL)
  { // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    // only dummy to deliver something
    return getSystemMgmtInstance4Comm().memberNr(empf());
  }
  else
  { // o.k. -> return item
    return *pc_monitorEmpf;
  }
}
/**
  deliver reference to MonitorItem_c of SEND member (MonitorItem_c is base class for both
  DINItem_c and ISOItem_c)
  (check with existMemberSend before access to not defined item)

  @return reference to MonitorItem_c of member which is addressed by SEND
  @exception containerElementNonexistant
*/
MonitorItem_c& ProcessPkg_c::memberSend() const
{ // check if pc_monitorSend is set
  if (pc_monitorSend == NULL)
  { // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    // only dummy to deliver something
    return getSystemMgmtInstance4Comm().memberNr(send());
  }
  else
  { // o.k. -> return item
    return *pc_monitorSend;
  }
}
/**
  some LBS+ terminals wants process data interaction for syncronisation of
  terminal mask with GETY_POS of terminal even for local process data
  @param rc_gtp GETY_POS of terminal, for which the GETY_POS of data is converted
  @param rc_useProcGtp GTP for process data (optional, default to terminal gtp)
*/
void ProcessPkg_c::useTermGtpForLocalProc(const GetyPos_c& rc_gtp, const GetyPos_c& rc_useProcGtp)
{
  c_specialTermGtp = rc_gtp;
  if (rc_useProcGtp.isSpecified())c_specialTermUseProcGtp = rc_useProcGtp;
  else c_specialTermUseProcGtp = rc_gtp;
}

/**
  extract data from DIN/ISO commands and save it to member class
*/
bool ProcessPkg_c::resolveCommandType()
{

  bool b_isSetpoint = false;
  bool b_isRequest = false;
  GeneralCommand_c::ValueGroup_t en_valueGroup;
  GeneralCommand_c::CommandType_t en_command = GeneralCommand_c::noCommand;

  if ( identType() == Ident_c::StandardIdent) {

#ifdef USE_DIN_9684
    // DIN command

    // make first decision: requestValue / setValue
    en_command = ( pd() & 0x2 ) ? GeneralCommand_c::requestValue : GeneralCommand_c::setValue;

    if (((pd() & 0x1) == 0) && (mod() < 4)) {
      // pd {00, 10} with mod {000,001,010} are setpoint messages
      b_isSetpoint = true;

      switch (mod()) {
        case 0x0:
          en_valueGroup = GeneralCommand_c::exactValue;
          break;
        case 0x2:
          en_valueGroup = GeneralCommand_c::minValue;
          break;
        case 0x3:
          en_valueGroup = GeneralCommand_c::maxValue;
          break;
      }
    }

    if (((pd() & 0x1) == 1) && (mod() < 5)) {
      // measure

      switch (mod()) {
        case 0x0:
          en_valueGroup = GeneralCommand_c::exactValue;
          break;
        case 0x1:
          en_valueGroup = GeneralCommand_c::minValue;
          break;
        case 0x2:
          en_valueGroup = GeneralCommand_c::maxValue;
          break;
        case 0x3:
          en_valueGroup = GeneralCommand_c::integValue;
          break;
        case 0x4:
          en_valueGroup = GeneralCommand_c::medValue;
          break;
      }
    }

    if (pd() == 0) {
      // measure prog commands
      if (mod() == 4) {
        if (dataLong() >= 0)
          en_command = GeneralCommand_c::measurementDistanceValue;
        else
          en_command = GeneralCommand_c::measurementTimeValue;
      }
      if (mod() == 5) {
        en_command = GeneralCommand_c::noCommand;
        if (dataLong() > 0)
          en_command = GeneralCommand_c::measurementChangeThresholdValue;
      }
      if (mod() == 6) {
        en_command = GeneralCommand_c::noCommand;
        if ( (dataLong() & 0xF) == 0)
          en_command = GeneralCommand_c::measurementStop;
        if ( ((dataLong() & 0xF) > 0) && ((dataLong() & 0xF) < 7) )
          en_command = GeneralCommand_c::measurementStart;
        if ( (dataLong() & 0xF) == 0x8)
          en_command = GeneralCommand_c::measurementReset;
      }
    }

    if (existMemberEmpf() && (gety() == memberEmpf().gtp().getGety())) {
       // we are local
       // from MeasureProgLocal_c::processMsg and ProcDataLocalBase_c::processProg())
       if ((pd() == 1) && (dataRawCmdLong() == 0) )
       {
          // write - accept only write actions to local data only if this is reset try
          // (not standard conformant, but practised)
          // setValue command for measure value is treated as reset (DIN: new values are 0, ISO: new value is value from message
          en_command = GeneralCommand_c::setValue;
      }
    }

    if (pd() & 0x2) {
      // pd {10, 11} are requests
      b_isRequest = true;
    }
#endif

  } else {

#ifdef USE_ISO_11783

    // ISO command
    switch (cmd()) {
      case 0x00:
        en_command = GeneralCommand_c::requestConfiguration;
        break;
      case 0x01:
        en_command = GeneralCommand_c::configurationResponse;
        break;
      case 0x02:
        en_command = GeneralCommand_c::requestValue;
        b_isRequest = true;
        break;
      case 0x03:
        en_command = GeneralCommand_c::setValue;
        break;
      case 0x04:
        en_command = GeneralCommand_c::measurementTimeValueStart;
        break;
      case 0x05:
        en_command = GeneralCommand_c::measurementDistanceValueStart;
        break;
      case 0x06:
        en_command = GeneralCommand_c::measurementMinimumThresholdValueStart;
        break;
      case 0x07:
        en_command = GeneralCommand_c::measurementMaximumThresholdValueStart;
        break;
      case 0x08:
        en_command = GeneralCommand_c::measurementChangeThresholdValueStart;
        break;
      case 0x0d:
        en_command = GeneralCommand_c::nack;
        break;
      case 0x0e:
        en_command = GeneralCommand_c::taskControllerStatus;
        break;
      case 0x0f:
        en_command = GeneralCommand_c::workingsetMasterMaintenance;
        break;
    }

    // decide setpoint/measurement

   b_isSetpoint = true;
    if ( DDI() < 74 ) {
      if ( DDI() % 5 == 2 ) {
        // actual value with DDI 2, 7, 12, 17, ...: measure
        b_isSetpoint = false;
        en_valueGroup = GeneralCommand_c::exactValue;
      }

      if ( DDI() % 5 == 1 )
        en_valueGroup = GeneralCommand_c::exactValue;
      if ( DDI() % 5 == 3 )
        en_valueGroup = GeneralCommand_c::defaultValue;
      if ( DDI() % 5 == 4 )
        en_valueGroup = GeneralCommand_c::minValue;
      if ( DDI() % 5 == 0 )
        en_valueGroup = GeneralCommand_c::maxValue;

    }
    else if ( DDI() < 80 ) {
      if ( ( DDI() - 74 ) % 3 > 0 ) {
        // actual value with DDI 75, 76, 78, 79 ...: measure
        b_isSetpoint = false;
        en_valueGroup = GeneralCommand_c::exactValue;
      }

      if ( ( DDI() - 74 ) % 3 == 0 )
        en_valueGroup = GeneralCommand_c::exactValue;
      if ( ( DDI() - 74 ) % 3 == 2 )
        en_valueGroup = GeneralCommand_c::maxValue;
    }
    else if ( DDI() <= 100 ) {
      b_isSetpoint = false;
      en_valueGroup = GeneralCommand_c::exactValue;
    }
    else if ( DDI() < 116 ) {
      if ( DDI() % 5 == 2 ) {
        // actual value with DDI 2, 7, 12, 17, ...: measure
        b_isSetpoint = false;
        en_valueGroup = GeneralCommand_c::exactValue;
      }

      if ( DDI() % 5 == 1 )
        en_valueGroup = GeneralCommand_c::exactValue;
      if ( DDI() % 5 == 3 )
        en_valueGroup = GeneralCommand_c::defaultValue;
      if ( DDI() % 5 == 4 )
        en_valueGroup = GeneralCommand_c::minValue;
      if ( DDI() % 5 == 0 )
        en_valueGroup = GeneralCommand_c::maxValue;

    }
    else if ( DDI() < 124 ) {
      b_isSetpoint = false;
      en_valueGroup = GeneralCommand_c::exactValue;
    }
    else if ( DDI() < 134 ) {
      en_valueGroup = GeneralCommand_c::exactValue;
    }
    else {
      b_isSetpoint = false;
      en_valueGroup = GeneralCommand_c::exactValue;
    }

#endif

  }

  if (en_command != GeneralCommand_c::noCommand) {
    c_generalCommand.setValues(b_isSetpoint, b_isRequest, en_valueGroup, en_command);
    return true;
  } else
    return false;

};

} // end of namespace __IsoAgLib
