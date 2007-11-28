/***************************************************************************
                          processpkg_c.h - data object for Process-Data
                                          messages
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef PROCESS_PKG_H
#define PROCESS_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isoname_c.h>

#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isomonitor_c.h>
#include "generalcommand_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include "../elementddi_s.h"
#include <list>

namespace __IsoAgLib {

/**
  data object for Process-Data messages
  transforms flag formated information to/from
  CAN uint8_t string
  @author Dipl.-Inform. Achim Spangler
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
    deliver EMPF of process msg (receiving member number)
    @return EMPF value of message
  */
  uint8_t empf()const{return bit_data.b_empf;}

  /**
    deliver SEND of process msg (sending member number)
    @return SEND value of message
  */
  uint8_t send()const{return bit_data.b_send;}

  /**
    deliver D of process msg (0 -> integer; 1 -> float)
    @return D value of message
  */
  uint8_t d()const{return bit_data.b_d;}

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
    deliver the data type of the value in the message
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const{return static_cast<proc_valType_t>(bit_data.b_valType);}

  /**
    check if the 4byte value of the message has a special command of type
    proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    @param ren_checkCmd special command to check for
    @return true -> the given cmd was received
  */
  bool isSpecCmd(proc_specCmd_t ren_checkCmd)const;

  /**
    deliver the 4byte data as int32_t val without any conversion
    for cmd's like measurement program increment, meas prog start/stop,
    etc.
    (4 uint8_t signed integer defined by int32_t in masterHeader)
    @return data value of message
  */
  int32_t dataRawCmdLong()const {return mc_flex4Data.getInt32Data();}

  /**
    deliver data value as int32_t; the 4byte data of the message are
    accessed with the type defined by the format flags
    -> then it's assigned to the wanted type (e.g. if float value
    is received, this function access it as float and delivers the
    rounded value as int32_t)
    (4 uint8_t signed integer defined by int32_t in masterHeader)
    @return data value of message
  */
  int32_t dataLong()const;

  /**
    deliver data value as uint32_t; the 4byte data of the message are
    accessed with the type defined by the format flags
    -> then it's assigned to the wanted type (e.g. if float value
    is received, this function access it as float and delivers the
    rounded value as uint32_t)
    (4 uint8_t signed integer defined by uint32_t in masterHeader)
    @return data value of message
  */
  uint32_t dataUlong()const;

#if defined(USE_FLOAT_DATA_TYPE)
  /**
    deliver data value as float; the 4byte data of the message are
    accessed with the type defined by the format flags
    -> then it's assigned to the wanted type (e.g. if int32_t value
    is received, this function access it as int32_t and delivers the
    value as float)
    @return data value of message
  */
  float dataFloat()const;
#endif

  /**
    deliver data value as single uint8_t from position ab_pos
    @return uint8_t of wanted data position of message
  */
  uint8_t data(uint8_t ab_pos)const{return mc_flex4Data.getUint8Data(ab_pos);}

  /**
    check if SEND member number is valid (e.g. there has claimed address member in
    monitor list registered)
    @return true -> there has claimed address member with number SEND
  */
  bool existMemberSend() const {return (mpc_monitorSend != NULL)?true:false;}

  /**
    check if EMPF member number is valid (e.g. there has claimed address member in
    monitor list registered)
    @return true -> there has claimed address member with number EMPF
  */
  bool existMemberEmpf() const {return (mpc_monitorEmpf != NULL)?true:false;}

  /**
    deliver reference to IsoItem_c of EMPF member (IsoItem_c is base class for IsoItem_c)
    (check with existMemberEmpf before access to not defined item)

    @return reference to IsoItem_c of member which is addressed by EMPF
     @exception containerElementNonexistant
  */
  IsoItem_c& memberEmpf()const;

  /**
    deliver reference to IsoItem_c of SEND member (IsoItem_c is base class for IsoItem_c)
    (check with existMemberSend before access to not defined item)

    @return reference to IsoItem_c of member which is addressed by SEND
     @exception containerElementNonexistant
  */
  IsoItem_c& memberSend()const;


  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */
  /**
    set value EMPF of process msg
    @param ab_val new EMPF value for message
  */
  void setEmpf(uint8_t ab_val) {
    bit_data.b_empf = ab_val;
    setIdentType(Ident_c::StandardIdent);
  }

  /**
    set value SEND of process msg
    @param ab_val new SEND value for message
  */
  void setSend(uint8_t ab_val) {
    bit_data.b_send = ab_val;
    setIdentType(Ident_c::StandardIdent);
  }

  /**
    set value D of process msg
    @param ab_val new D value for message
  */
  void set_d(uint8_t ab_val){bit_data.b_d = ab_val;}

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
    set the 4byte data as int32_t val without any conversion
    for cmd's like measurement program increment, meas prog start/stop,
    etc.
    (4 uint8_t signed integer defined by int32_t in masterHeader)
    @param ai32_val new cmd value for message
    @param ren_procValType data type for message string (default i32_val)
  */
  void setDataRawCmd(int32_t ai32_val, proc_valType_t ren_procValType = i32_val);

  /**
    set data value as with int32_t value parameter
    and with defined send value type and set data type flag
    (if parameter and send type are different,
     a conversion by assignment is performed; %e.g. int32_t val 3
     can be sent as float 3.0)
    @param ai32_val new data value for message
    @param ren_procValType data type for message string (default i32_val)
  */
  void setData(int32_t ai32_val, proc_valType_t ren_procValType = i32_val);

  /**
    set data value as with uint32_t value parameter
    and with defined send value type and set data type flag
    (if parameter and send type are different,
     a conversion by assignment is performed; %e.g. int32_t val 3
     can be sent as float 3.0)
    @param ai32_val new data value for message
    @param ren_procValType data type for message string (default ui32_val)
  */
  void setData(uint32_t ai32_val, proc_valType_t ren_procValType = ui32_val);

  /**
    set one of the special commands of type proc_specCmd_t:
    setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    with specified data type specifying flag in CAN data string
    @param ren_procSpecCmd special command to send
    @param ren_procValType data type for message string (default ui32_val)
  */
  void setData(proc_specCmd_t ren_procSpecCmd, proc_valType_t ren_procValType = i32_val);

#if defined(USE_FLOAT_DATA_TYPE)
  /**
    set data value as with float value parameter
    and with defined send value type and set data type flag
    (if parameter and send type are different,
     a conversion by assignment is performed; %e.g. int32_t val 3
     can be sent as float 3.0)
    @param ai32_val new data value for message
    @param ren_procValType data type for message string (default float_val)
  */
  void setData(float af_val, proc_valType_t ren_procValType = float_val);
#endif

  /**
    set value data value as single uint8_t on position ab_pos
    @param ab_pos position of written uint8_t in data string
    @param ab_val uint8_t data value
  */
  void setData(uint8_t ab_pos, uint8_t ab_val){mc_flex4Data.setUint8Data(ab_pos, ab_val);}

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

  /**
    some LBS+ terminals wants process data interaction for syncronisation of
    terminal mask with ISOName of terminal even for local process data
    @param ac_isoName ISOName of terminal, for which the ISOName of data is converted
    @param aui8_useProcISOName ISOName for process data (optional, default to terminal isoName)
  */
  void useTermISONameForLocalProc(const IsoName_c& ac_isoName, const IsoName_c& ac_useProcISOName = IsoName_c::IsoNameUnspecified());

  /** stores the command in generalized form */
  GeneralCommand_c mc_generalCommand;

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
  Flexible4ByteString_c mc_flex4Data;

  struct _bit_data {
    /** EMPF forheader */
    uint16_t b_empf : 8;
    /** SEND for header */
    uint16_t b_send : 8;

    /** PD for data */
    uint16_t b_pd : 2;

    /** MOD for data */
    uint16_t b_mod : 3;
    /** D for data */
    uint16_t b_d : 1;

    /** decide about used val type: int32_t, uint32_t, float, cmd */
    uint16_t b_valType : 2;

    uint16_t ui8_Command : 4;
    uint16_t ui16_Element : 12;
    uint16_t ui16_DDI : 16;
  } bit_data;

  /** pointer to monitor list item of sender "SEND" (NULL if not claimed address) */
  IsoItem_c* mpc_monitorSend;

  /** pointer to monitor list item of receiver "EMPF" (NULL if not claimed address) */
  IsoItem_c* mpc_monitorEmpf;
  /**
    some terminal wants to use ISOName of terminal even for local process
    data for communication on CAN BUS (default 0xFF for off)
  */
  IsoName_c mc_specialTermISOName;

  /**
    some terminal wants to use ISOName of terminal even for local process
    data for communication on CAN BUS (default 0xFF for off)
  */
  IsoName_c mc_specialTermUseProcISOName;

};

}
#endif
