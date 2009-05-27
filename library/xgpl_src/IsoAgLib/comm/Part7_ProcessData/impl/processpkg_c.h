/***************************************************************************
                          processpkg_c.h - data object for Process-Data
                                          messages
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef PROCESS_PKG_H
#define PROCESS_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>

#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "processcmd_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include "../elementddi_s.h"

namespace __IsoAgLib {

/**
  data object for Process-Data messages
  transforms flag formated information to/from
  CAN uint8_t string
  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inf. Martin Wodok
*/
class ProcessPkg_c : public CanPkgExt_c  {
private:
public:
  /** default constructor which has nothing to do */
  ProcessPkg_c( int ai_singletonVecKey = 0 );
  /** default constructor which has nothing to do */
  ~ProcessPkg_c();

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */

   /**
    deliver Cmd of process msg
    @return Command value of message
  */
  uint8_t cmd()const{return bit_data.ui8_Command;}

  /**
    deliver Element Number of process msg
    @return Element Number value of message
  */
  uint16_t element()const{return bit_data.ui16_Element;}

    /**
    deliver DDI of process msg
    @return Data Dictionary Identifier value of message
  */
  uint16_t DDI()const{return bit_data.ui16_DDI;}

  /**
    check if the 4byte value of the message has a special command of type
    proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    @param ren_checkCmd special command to check for
    @return true -> the given cmd was received
  */
  bool isSpecCmd(proc_specCmd_t ren_checkCmd)const;

  /**
    deliver data value as int32_t
    @return data value of message
  */
  int32_t getValue() const { return mi32_pdValue; }

  /**
    @return pointer to IsoItem_c of Receiver
  */
  IsoItem_c* receiverItem() { return getMonitorItemForDA(); }

  /**
    @return pointer to IsoItem_c of Sender
  */
  IsoItem_c* senderItem() { return getMonitorItemForSA(); }


  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */
   /**
    set command of process msg
    @param ab_cmd command value of process data message
  */
  void set_Cmd(uint8_t ab_cmd){bit_data.ui8_Command = ab_cmd;}

   /**
    set DDI of process msg
    @param ab_DDI Data dictionary Itendifier
  */
  void set_DDI(uint16_t ab_DDI){bit_data.ui16_DDI = ab_DDI;}

   /**
    set Element Number of Process Data Message
    @param ab_Element command value of process data message
  */
  void set_Element(uint16_t ab_Element){bit_data.ui16_Element = ab_Element;}

  /**
    set data value
    @param ai32_val new data value for message
  */
  void setData (int32_t ai32_val) { mi32_pdValue = ai32_val; }

  /**
    set one of the special commands of type proc_specCmd_t:
    setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    with specified data type specifying flag in CAN data string
    @param ren_procSpecCmd special command to send
  */
  void setData (proc_specCmd_t ren_procSpecCmd);

  /**
    extract data from ISO commands and save it to member class
    @param rl_elementDDI
  */
  bool resolveCommandTypeForISO(const IsoAgLib::ElementDdi_s& rl_elementDDI);

  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CanPkg_c or CANPkgExt
    @see CanPkg_c::operator=
    @see CanPkgExt_c::operator=
  */
  virtual void string2Flags();

  /** stores the command in generalized form */
  ProcessCmd_c mc_processCmd;

private: // Private methods

  /**
    overloaded virtual function to translate flag values to data string;
    needed for sending informations from this object via CanIo_c on CAN BUS,
    because CanIo_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CanPkg_c::getData
    @see CanPkgExt_c::getData
  */
  virtual void flags2String();

private: // Private attributes
  int32_t mi32_pdValue;

  struct _bit_data {
    _bit_data() : ui8_Command(0), ui16_Element(0), ui16_DDI(0) {}
    uint16_t ui8_Command : 4;
    uint16_t ui16_Element : 12;
    uint16_t ui16_DDI : 16;
  } bit_data;
};

}
#endif
