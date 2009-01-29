/***************************************************************************
                          processcmd_c.h - extracts general data from ISO
                                               process messages
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2005-07-08 16:35:57 +0200 (Fr, 08 Jul 2005) $
    $LastChangedRevision: 1092 $
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
#ifndef PROCESSCMD_H
#define PROCESSCMD_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  extracts process command info from ISO process messages
*/
class ProcessCmd_c {
public:

  /** enum for specification of defined setpoint types */
  enum ValueGroup_t { noValue = 0, exactValue = 1, minValue = 2, maxValue = 4, defaultValue = 8 };

  /** enum for general commands */
  enum CommandType_t {
    noCommand                             = 0,
    requestConfiguration                  = 0x01,
    configurationResponse                 = 0x02,
    setValue                              = 0x03,
    requestValue                          = 0x04,
    taskControllerStatus                  = 0x05,
    workingsetMasterMaintenance           = 0x06,
    nack                                  = 0x09,
    measurementStop                       = 0x07,

    // measurement commands: command & 0x10 != 0
    measurementReset                      = 0x10,
    // ISO measurement
    measurementTimeValueStart             = 0x15,
    measurementDistanceValueStart         = 0x16,
    measurementMaximumThresholdValueStart = 0x17,
    measurementMinimumThresholdValueStart = 0x18,
    measurementChangeThresholdValueStart  = 0x19

  };

  /** constructor */
  ProcessCmd_c();

  /** read access for isSetpoint */
  bool checkIsSetpoint() const { return mb_isSetpoint; };

  /** read access for isRequest */
  bool checkIsRequest() const { return mb_isRequest; };

  /** read access for men_valueGroup */
  ValueGroup_t getValueGroup() const { return men_valueGroup; };

  /** read access for men_command */
  CommandType_t getCommand() const { return men_command; };

  /** set values, called in ProcessPkg_c::resolveCommand() */
  void setValues(bool mb_isSetpoint, bool mb_isRequest, ValueGroup_t men_valueGroup,
                 CommandType_t men_command);

private:

  bool mb_isSetpoint;
  bool mb_isRequest;

  /** command affects min, max, exact or default value */
  ValueGroup_t men_valueGroup;

  /** current command */
  CommandType_t men_command;

};

}
#endif
