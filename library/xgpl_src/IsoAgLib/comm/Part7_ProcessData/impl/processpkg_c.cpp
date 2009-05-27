/***************************************************************************
                          processpkg_c.cpp - data object for Process-Data
                                           messages
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include "proc_c.h"

namespace __IsoAgLib {


/** default constructor which has nothing to do */
ProcessPkg_c::ProcessPkg_c( int ai_singletonVecKey )
  : CanPkgExt_c( ai_singletonVecKey )
  , mi32_pdValue( 0 )
  , bit_data()
{
}

/** default constructor which has nothing to do */
ProcessPkg_c::~ProcessPkg_c(){
}

/**
  check if the 4byte value of the message has a special command of type
  proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  (extension to ISO specification)
  @param ren_checkCmd special command to check for
  @return true -> the given cmd was received
*/
bool ProcessPkg_c::isSpecCmd(proc_specCmd_t ren_checkCmd)const
{
  bool b_result = false;

  const int32_t ci32_test = mi32_pdValue;

  if (mc_processCmd.getCommand() == ProcessCmd_c::setValue &&
      mc_processCmd.checkIsSetpoint())
  { // setpoint value -> special commands are possible for exact, min, max, default setpopints
      if (((ren_checkCmd & setpointReleaseCmd) != 0)
       &&(ci32_test == static_cast<int32_t>(SETPOINT_RELEASE_COMMAND)))
      {
        b_result = true;
      }
      else
      {
        if (((ren_checkCmd & setpointErrCmd) != 0)
         && (ci32_test == static_cast<int32_t>(SETPOINT_ERROR_COMMAND))) b_result = true;
      }
  }

  if (mc_processCmd.getCommand() == ProcessCmd_c::setValue &&
      !mc_processCmd.checkIsSetpoint())
  { // measure value: conversion if: actual, min, max, integ, med
    // check for command values
    if (((ren_checkCmd & noVal_32s) != 0)
      &&(ci32_test == static_cast<int32_t>(NO_VAL_32S)))
    {
      b_result = true;
    }
    else
    {
      if (((ren_checkCmd & errVal_32s) != 0)
      && (ci32_test == static_cast<int32_t>(ERROR_VAL_32S))) b_result = true;
    }
  }
  return b_result;
}


/**
  set one of the special commands of type proc_specCmd_t:
  setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  with specified data type specifying flag in CAN data string
  @param ren_procSpecCmd special command to send
*/
void ProcessPkg_c::setData(proc_specCmd_t ren_procSpecCmd)
{
  switch (ren_procSpecCmd)
  {
    case setpointReleaseCmd:
      mi32_pdValue = SETPOINT_RELEASE_COMMAND;
      break;

    case setpointErrCmd:
      mi32_pdValue = SETPOINT_ERROR_COMMAND;
      break;

    case noVal_32s:
      mi32_pdValue = NO_VAL_32S;
      break;

    case errVal_32s:
      mi32_pdValue = ERROR_VAL_32S;
      break;
  }
}


/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CanPkg_c or CANPkgExt
  @see CanPkg_c::operator=
  @see CanPkgExt_c::operator=
*/
void ProcessPkg_c::string2Flags()
{
  set_Cmd(CanPkg_c::msc_data[0] & 0xf);
  uint16_t mui16_element = 0;
  mui16_element = uint16_t(CanPkg_c::msc_data[1]) << 4;
  mui16_element |= ((CanPkg_c::msc_data[0] & 0xF0)>>4);
  set_Element(mui16_element);

  uint16_t newDDI = 0;
  newDDI |= CanPkg_c::msc_data[3];
  newDDI = newDDI << 8;
  newDDI |= CanPkg_c::msc_data[2];
  set_DDI(newDDI);

  mi32_pdValue = CanPkg_c::msc_data.getInt32Data(4);
};

/**
  overloaded virtual function to translate flag values to data string;
  needed for sending informations from this object via CanIo_c on CAN BUS,
  because CanIo_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CanPkg_c::getData
  @see CanPkgExt_c::getData
*/
void ProcessPkg_c::flags2String()
{
  uint8_t ui8_cmd;
  switch (mc_processCmd.getCommand()) {
    case ProcessCmd_c::requestConfiguration:                  ui8_cmd = 0; break;
    case ProcessCmd_c::configurationResponse:                 ui8_cmd = 1; break;
    case ProcessCmd_c::requestValue:                          ui8_cmd = 2; break;
    case ProcessCmd_c::setValue:                              ui8_cmd = 3; break;
    case ProcessCmd_c::measurementTimeValueStart:             ui8_cmd = 4; break;
    case ProcessCmd_c::measurementDistanceValueStart:         ui8_cmd = 5; break;
    case ProcessCmd_c::measurementMinimumThresholdValueStart: ui8_cmd = 6; break;
    case ProcessCmd_c::measurementMaximumThresholdValueStart: ui8_cmd = 7; break;
    case ProcessCmd_c::measurementChangeThresholdValueStart:  ui8_cmd = 8; break;
    case ProcessCmd_c::nack:                                  ui8_cmd = 0xd; break;
    case ProcessCmd_c::taskControllerStatus:                  ui8_cmd = 0xe; break;
    case ProcessCmd_c::workingsetMasterMaintenance:           ui8_cmd = 0xf; break;
    // map reset command to setValue command
    case ProcessCmd_c::measurementReset:                      ui8_cmd = 3; break;
    default: ui8_cmd = 0xFF;
  }

  CanPkg_c::msc_data.setUint8Data(0, (ui8_cmd & 0xf) | ( (element() & 0xf) << 4) );
  CanPkg_c::msc_data.setUint8Data(1, element() >> 4 );
  CanPkg_c::msc_data.setUint8Data(2, DDI() & 0x00FF );
  CanPkg_c::msc_data.setUint8Data(3, (DDI()& 0xFF00) >> 8 );
  // for ISO the ident is directly read and written

  CanPkg_c::msc_data.setInt32Data(4, mi32_pdValue);

  setLen(8);
};

/**
  extract data from ISO commands and save it to member class
  @param rl_elementDDI
*/
bool ProcessPkg_c::resolveCommandTypeForISO(const IsoAgLib::ElementDdi_s& rl_elementDDI)
{
  bool mb_isSetpoint = false;
  bool mb_isRequest = false;
  ProcessCmd_c::ValueGroup_t men_valueGroup = ProcessCmd_c::noValue;
  ProcessCmd_c::CommandType_t men_command = ProcessCmd_c::noCommand;

  if ( identType() != Ident_c::StandardIdent) {
    // ISO command
    switch (cmd()) {
      case 0x00:
        men_command = ProcessCmd_c::requestConfiguration;
        break;
      case 0x01:
        men_command = ProcessCmd_c::configurationResponse;
        break;
      case 0x02:
        men_command = ProcessCmd_c::requestValue;
        mb_isRequest = true;
        break;
      case 0x03:
        men_command = ProcessCmd_c::setValue;
        break;
      case 0x04:
        men_command = ProcessCmd_c::measurementTimeValueStart;
        break;
      case 0x05:
        men_command = ProcessCmd_c::measurementDistanceValueStart;
        break;
      case 0x06:
        men_command = ProcessCmd_c::measurementMinimumThresholdValueStart;
        break;
      case 0x07:
        men_command = ProcessCmd_c::measurementMaximumThresholdValueStart;
        break;
      case 0x08:
        men_command = ProcessCmd_c::measurementChangeThresholdValueStart;
        break;
      case 0x0d:
        men_command = ProcessCmd_c::nack;
        break;
      case 0x0e:
        men_command = ProcessCmd_c::taskControllerStatus;
        break;
      case 0x0f:
        men_command = ProcessCmd_c::workingsetMasterMaintenance;
        break;
    }
  }

  if ( rl_elementDDI.ui16_DDI == DDI() )
  {
    mb_isSetpoint = rl_elementDDI.b_isSetpoint;
    men_valueGroup = rl_elementDDI.en_valueGroup;
  }

  if (men_command != ProcessCmd_c::noCommand)
  {
    mc_processCmd.setValues(mb_isSetpoint, mb_isRequest, men_valueGroup, men_command);
    return true;
  }
  else return false;
}

} // end of namespace __IsoAgLib
