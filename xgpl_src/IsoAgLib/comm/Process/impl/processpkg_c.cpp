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
  check if the 4byte value of the message has a convertable value (e.g. is no
  special command)
  ((check for special values: SETPOINT_RELEASE_COMMAND, SETPOINT_ERROR_COMMAND,
  NO_VAL_32S, ERROR_VAL_32S))
  @return true -> conversions of message value are safe
*/
bool ProcessPkg_c::isConvertableVal()const
{
  bool b_result = false;
  int32_t i32_test = *(static_cast<const int32_t*>(static_cast<const void*>(pb_data)));
  if (pd() == 0)
  { // setpoint value -> special command
    if ((mod()==0)||(mod()==2)||(mod()==3))
    { // conversion is needed
      if ((i32_test != static_cast<int32_t>(SETPOINT_RELEASE_COMMAND))
       && (i32_test != static_cast<int32_t>(SETPOINT_ERROR_COMMAND))) b_result = true;
    }
  }
  else
  { // measure value: conversion if: actual, min, max, integ, med
    if (mod() < 5)
    { // conversion is needed
      if ((i32_test != static_cast<int32_t>(NO_VAL_32S))
       && (i32_test != static_cast<int32_t>(ERROR_VAL_32S))) b_result = true;
    }
  }
  return b_result;
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


  int32_t i32_test = *(static_cast<const int32_t*>(static_cast<const void*>(pb_data)));
  if (pd() == 0)
  { // setpoint value -> special commands are possible for exact, min, max setpopints
    if (mod() < 4)
    { // check for command values
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
  }
  else
  { // measure value: conversion if: actual, min, max, integ, med
    if (mod() < 5)
    { // check for command values
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
      i32_result = *(static_cast<const int32_t*>(static_cast<const void*>(pb_data)));
      break;
    case ui32_val:
      i32_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_data)));
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      int2Float(pb_data, ((float*)(&i32_result)));
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
      ulResult = *(static_cast<const int32_t*>(static_cast<const void*>(pb_data)));
      break;
    case ui32_val:
      ulResult = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_data)));
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      int2Float(pb_data, ((float*)(&ulResult)));
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
      f_result = *(static_cast<const int32_t*>(static_cast<const void*>(pb_data)));
      break;
    case ui32_val:
      f_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_data)));
      break;
    case float_val:
      int2Float(pb_data, &f_result);
      break;
    case cmdVal:
      f_result = *(static_cast<const uint32_t*>(static_cast<const void*>(pb_data)));
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
  *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = ri32_val;

#ifndef ISO_TASK_CONTROLLER    // no need to do this switch case if new ISO part 10 is used. -bac
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
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = ri32_val;
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_data))) = ri32_val;
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      float2Int( ((float *const)(&ri32_val)), pb_data);
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
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = ri32_val;
      set_d(0);
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_data))) = ri32_val;
      set_d(0);
      break;
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    case float_val:
      float2Int( ((float *const)(&ri32_val)), pb_data);
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
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = SETPOINT_RELEASE_COMMAND;
      break;
    case setpointErrCmd:
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = SETPOINT_ERROR_COMMAND;
      break;
    case noVal_32s:
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = NO_VAL_32S;
      break;
    case errVal_32s:
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = ERROR_VAL_32S;
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
      *(static_cast<int32_t*>(static_cast<void*>(pb_data))) = static_cast<int32_t>(rf_val);
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case ui32_val:
      *(static_cast<uint32_t*>(static_cast<void*>(pb_data))) = static_cast<uint32_t>(rf_val);
      set_d(0);
      bit_data.b_valType = ren_procValType;
      break;
    case float_val:
      float2Int(&rf_val, pb_data);
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
 #ifdef USE_DIN_9684
  #ifdef USE_ISO_11783
  if (identType() == Ident_c::StandardIdent)
  #endif
  {
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
    setGtp( GetyPos_c(((CANPkg_c::pb_data[2] >> 3) & 0xF), (CANPkg_c::pb_data[2] & 0x7) ) );

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
  }
 #endif // USE_DIN_9684


#ifndef ISO_TASK_CONTROLLER         // The following is the original code for old part 10 -bac

 #ifdef USE_ISO_11783
  #ifdef USE_DIN_9684
  else
  #endif
  { // set pri, empf, send for convenience
    setPri(2); // signal target message
    setEmpf(isoPs());
    setSend(isoSa());
    setIdentType(Ident_c::ExtendedIdent);

    setLis(0); // ISO doesn't support LIS code -> set to default 0

    bit_data.b_valType = static_cast<proc_valType_t>((CANPkg_c::pb_data[0] >> 5) & 0x3);
    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
    if (bit_data.b_valType == float_val) set_d(1);
    else
    #endif
    set_d(0);

    setGtp( GetyPos_c(((CANPkg_c::pb_data[2] >> 4) & 0xF), (CANPkg_c::pb_data[2] & 0xF) ) );

    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
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
  }
  #endif // ISO

  setPd(((CANPkg_c::pb_data[0] >> 3) & 0x3));
  setMod((CANPkg_c::pb_data[0] & 0x7));

  setZaehlnum(CANPkg_c::pb_data[1]);

  setWert(CANPkg_c::pb_data[3] >> 4);
  setInst((CANPkg_c::pb_data[3] & 0xF));


  // some DIN systems place wrong GETY in data string -> correct GETY, for all requests to GETY of receiver
  // and correct GETY for all messages from special registered terminal
  if ( (pc_monitorEmpf != NULL) // receiver must be found in monitor list
    && (pc_monitorSend != NULL) // sender must be found in monitor list
    && ( gtp() != GetyPos_c( 0, 0 ) ) // Proc is not of special GetyPos 0,0 case
    && ( ( ( pd() & 0x2 ) != 0 ) // first alternative: it's a request
    ||   ( ( c_specialTermGtp ==  pc_monitorSend->gtp() ) && (c_specialTermGtp.isSpecified()) ) // or it's from known special terminal
       )
     )
  { // sender is special case terminal -> change GETY_POS for data part from terminal GETY_POS to local of empf
    setGtp(pc_monitorEmpf->gtp());
  }
#else
    // New Part 10 code to go here -bac

   // set pri, empf, send for convenience
    setPri(2); // signal target message
    setEmpf(isoPs());
    setSend(isoSa());
    setIdentType(Ident_c::ExtendedIdent);

    setLis(0); // ISO doesn't support LIS code -> set to default 0

   //  bit_data.b_valType = static_cast<proc_valType_t>((CANPkg_c::pb_data[0] >> 5) & 0x3);

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
    setGtp(c_isoMonitor.isoMemberNr(send()).gtp());  // Get the gety and pos (Device Class, Device Class Instance -bac

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

#endif  // end of new part 10 code

  CNAMESPACE::memmove(pb_data, (CANPkg_c::pb_data+4), 4);
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
  #ifdef USE_ISO_11783
  if (identType() == Ident_c::StandardIdent)
  {
  #endif
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

    // if c_specialTermGtp != GetyPos_c(0xF, 0xF) dependent on gtp receiver of the GETY_POS of this data should be changed
    // to remote gtp
    CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | gtp().getCombinedDin();
    #if 0
    if ( (c_specialTermGtp.isSpecified())
      && (c_specialTermGtp == getDinMonitorInstance4Comm().dinMemberNr(empf()).gtp())
      && ( ( (wert() == 0) && (inst() >= 0xC) ) || ( (wert() == 0xF) &&  ( (inst() == 3) || (inst() == 4) ) ) )
       )
    { // sender is special case terminal -> change GETY_POS for data part to terminal GETY_POS to local from empf
      CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | (c_specialTermUseProcGtp.getCombinedDin());
    }
    else
    { // standard case
      CANPkg_c::pb_data[2] = ((d() & 0x1) << 7) | gtp().getCombinedDin();
    }
    #endif
#ifndef ISO_TASK_CONTROLLER
  #ifdef USE_ISO_11783

  }
  else
  { // format for ISO -> set int32_t or float
      CANPkg_c::pb_data[0] = (valType()) | ((pd() & 0x3) << 3) | (mod() & 0x7);
	#if 0
    // if ui8_termGtpForLocalProc is specified dependent on gtp receiver of the GETY_POS of this data should be changed
    // to remote gtp
    if ( (c_specialTermGtp.isSpecified())
      && (c_specialTermGtp == getIsoMonitorInstance4Comm().isoMemberNr(empf()).gtp())
      && ( ( (wert() == 0) && (inst() >= 0xC) ) || ( (wert() == 0xF) &&  ( (inst() == 3) || (inst() == 4) ) ) )
       )
    { // sender is special case terminal -> change GETY_POS for data part to terminal GETY_POS to local from empf
      CANPkg_c::pb_data[2] = c_specialTermUseProcGtp.getCombinedIso();
    }
    else
    { // standard case
      CANPkg_c::pb_data[2] = gtp().getCombinedIso();
    }
	#else
	CANPkg_c::pb_data[2] = gtp().getCombinedIso();
	#endif
  }
  // for ISO the ident is directly read and written
  #endif

  CANPkg_c::pb_data[1] = zaehlnum();

  CANPkg_c::pb_data[3] = (wert() << 4) | (inst() & 0xF);

  CNAMESPACE::memmove((CANPkg_c::pb_data+4), pb_data, 4);

  if ((pd() >> 1) == 1)
  { // request has only 4 bytes
    setLen(4);
  }
  else
  { // value send with 8 bytes
    setLen(8);
  }
#else   // code added by Brad Cox to format the message to conform to the latest part 10 Specification. -bac
 }
  else
  {
    CANPkg_c::pb_data[0] = (cmd() & 0xf) | ( (element() & 0xf) << 4);
	CANPkg_c::pb_data[1] = element() >> 4;
    CANPkg_c::pb_data[2] = DDI() & 0x00FF ;
    CANPkg_c::pb_data[3] = (DDI()& 0xFF00) >> 8 ;
  }
  // for ISO the ident is directly read and written


  CNAMESPACE::memmove((CANPkg_c::pb_data+4), pb_data, 4);

  setLen(8);

#endif  // End of ISO_TASK_CONTROLLER
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
void ProcessPkg_c::useTermGtpForLocalProc(GetyPos_c rc_gtp, GetyPos_c rc_useProcGtp)
{
  c_specialTermGtp = rc_gtp;
  if (rc_useProcGtp.isSpecified())c_specialTermUseProcGtp = rc_useProcGtp;
  else c_specialTermUseProcGtp = rc_gtp;
}


} // end of namespace __IsoAgLib
