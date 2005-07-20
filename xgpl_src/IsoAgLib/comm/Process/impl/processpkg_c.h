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
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include "generalcommand_c.h"

#ifdef USE_ISO_11783
  #include <IsoAgLib/driver/can/impl/ident_c.h>
#endif

namespace __IsoAgLib {

/**
  data object for Process-Data messages
  transforms flag formated information to/from
  CAN uint8_t string
  @author Dipl.-Inform. Achim Spangler
*/
class ProcessPkg_c : public CANPkgExt_c  {
private:
public:
  /** default constructor which has nothing to do */
  ProcessPkg_c( int ri_singletonVecKey = 0 );
  /** default constructor which has nothing to do */
  ~ProcessPkg_c();

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */

  /**
    deliver PRI of process msg (message type)
    @return PRI value of message
  */
  uint8_t pri()const{return bit_data.ui8_pri;};
  /**
    deliver EMPF of process msg (receiving member number)
    @return EMPF value of message
  */
  uint8_t empf()const{return bit_data.b_empf;};
  /**
    deliver SEND of process msg (sending member number)
    @return SEND value of message
  */
  uint8_t send()const{return bit_data.b_send;};

  /**
    deliver LIS of process msg (list number of Process-Data)
    @return LIS value of message
  */
  uint8_t lis()const{return bit_data.ui8_lis;};
  /**
    deliver GETY of process msg (machine type specific number of Proces-Data table)
    @return GETY value of message
  */
  uint8_t gety()const{return bit_data.c_gtp.getGety();};
  /**
    deliver WERT of process msg (row of Process-Data table)
    @return  value of message
  */
  uint8_t wert()const{return bit_data.ui8_wert;};
  /**
    deliver INST of process msg (column of Process-Data table)
    @return  value of message
  */
  uint8_t inst()const{return bit_data.ui8_inst;};

  /**
    deliver PD of process msg (PD flag for subtype/action of Process-Data)
    @return PD value of message
  */
  uint8_t pd()const{return bit_data.b_pd;};
  /**
    deliver MOD of process msg (MOD flag for subtype of Process-Data)
    @return MOD value of message
  */
  uint8_t mod()const{return bit_data.b_mod;};
  /**
    deliver ZAEHLNUM of process msg (specify working width part corresponding to this msg)
    @return ZAEHLNUM value of message
  */
  uint8_t zaehlnum()const{return bit_data.ui8_zaehlnum;};
  /**
    deliver D of process msg (0 -> integer; 1 -> float)
    @return D value of message
  */
  uint8_t d()const{return bit_data.b_d;};

#ifdef USE_ISO_11783
   /**
    deliver Cmd of process msg 
    @return Command value of message
  */
  uint8_t cmd()const{return bit_data.ui8_Command;};

  /**
    deliver Element Number of process msg 
    @return Element Number value of message
  */
  uint16_t element()const{return bit_data.ui16_Element;};

    /**
    deliver DDI of process msg 
    @return Data Dictionary Identifier value of message
  */
  uint16_t DDI()const{return bit_data.ui16_DDI;};
#endif
  /**
    deliver the data type of the value in the message
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const{return static_cast<proc_valType_t>(bit_data.b_valType);};

  /**
    check if the 4byte value of the message has a special command of type
    proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    @param ren_checkCmd special command to check for
    @return true -> the given cmd was received
  */
  bool isSpecCmd(proc_specCmd_t ren_checkCmd)const;

  /**
    deliver POS of process msg (mounting position; important for more active members of same GETY)
    @return POS value of message
  */
  uint8_t pos()const{return bit_data.c_gtp.getPos();};
#ifdef USE_DIN_9684  
  /**
    deliver gety and pos as gtp
    only used for DIN messages, ISO: gtp not in message
    @return GETY_POS value of message
  */
  GetyPos_c gtp()const{return bit_data.c_gtp;};
#endif

  /**
    deliver the 4byte data as int32_t val without any conversion
    for cmd's like measurement program increment, meas prog start/stop,
    etc.
    (4 uint8_t signed integer defined by int32_t in masterHeader)
    @return data value of message
  */
  int32_t dataRawCmdLong()const
    {return *((int32_t*)pb_data);};
//    {return *(static_cast<int32_t*>(static_cast<void*>(pb_data)));};
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
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
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
    deliver data value as single uint8_t from position rb_pos
    @return uint8_t of wanted data position of message
  */
  uint8_t data(uint8_t rb_pos)const{return pb_data[rb_pos];};

  /**
    check if SEND member number is valid (e.g. there has claimed address member in
    monitor list registered)
    @return true -> there has claimed address member with number SEND
  */
  bool existMemberSend() const {return (pc_monitorSend != NULL)?true:false;};
  /**
    check if EMPF member number is valid (e.g. there has claimed address member in
    monitor list registered)
    @return true -> there has claimed address member with number EMPF
  */
  bool existMemberEmpf() const {return (pc_monitorEmpf != NULL)?true:false;};
  /**
    deliver reference to MonitorItem_c of EMPF member (MonitorItem_c is base class for both
    DINItem_c and ISOItem_c)
    (check with existMemberEmpf before access to not defined item)

    @return reference to MonitorItem_c of member which is addressed by EMPF
     @exception containerElementNonexistant
  */
  MonitorItem_c& memberEmpf()const;
  /**
    deliver reference to MonitorItem_c of SEND member (MonitorItem_c is base class for both
    DINItem_c and ISOItem_c)
    (check with existMemberSend before access to not defined item)

    @return reference to MonitorItem_c of member which is addressed by SEND
     @exception containerElementNonexistant
  */
  MonitorItem_c& memberSend()const;
      

  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */
  /**
    set value PRI of process msg
    @param rb_val new PRI value for message
  */
  void setPri(uint8_t rb_val) {
    bit_data.ui8_pri = rb_val; 
    setIdentType(Ident_c::StandardIdent);
  };
  /**
    set value EMPF of process msg
    @param rb_val new EMPF value for message
  */
  void setEmpf(uint8_t rb_val) {
    bit_data.b_empf = rb_val;
    setIdentType(Ident_c::StandardIdent);
  };

  /**
    set value SEND of process msg
    @param rb_val new SEND value for message
  */
  void setSend(uint8_t rb_val) {
    bit_data.b_send = rb_val;
    setIdentType(Ident_c::StandardIdent);
  };

  /**
    set value LIS of process msg
    @param rb_val new LIS value for message
  */
  void setLis(uint8_t rb_val){bit_data.ui8_lis = rb_val;};
  /**
    set value GETY of process msg
    @param rb_val new GETY value for message
  */
  void setGety(uint8_t rb_val){bit_data.c_gtp.setGety(rb_val);};
  /**
    set value WERT of process msg
    @param rb_val new WERT value for message
  */
  void setWert(uint8_t rb_val){bit_data.ui8_wert = rb_val;};
  /**
    set value INST of process msg
    @param rb_val new INST value for message
  */
  void setInst(uint8_t rb_val){bit_data.ui8_inst = rb_val;};
  /**
    set value PD of process msg
    @param rb_val new PD value for message
  */
  void setPd(uint8_t rb_val){bit_data.b_pd = rb_val;};
  /**
    set value MOD of process msg
    @param rb_val new MOD value for message
  */
  void setMod(uint8_t rb_val){bit_data.b_mod = rb_val;};
  /**
    set value ZAEHLNUM of process msg
    @param rb_val new ZAEHLNUM value for message
  */
  void setZaehlnum(uint8_t rb_val){bit_data.ui8_zaehlnum = rb_val;};
  /**
    set value D of process msg
    @param rb_val new D value for message
  */
  void set_d(uint8_t rb_val){bit_data.b_d = rb_val;};
#ifdef USE_ISO_11783
   /**
    set command of process msg
    @param rb_cmd command value of process data message
  */
  void set_Cmd(uint8_t rb_cmd){bit_data.ui8_Command = rb_cmd;};

   /**
    set DDI of process msg
    @param rb_DDI Data dictionary Itendifier
  */
  void set_DDI(uint16_t rb_DDI){bit_data.ui16_DDI = rb_DDI;};

   /**
    set Element Number of Process Data Message
    @param rb_Element command value of process data message
  */
  void set_Element(uint16_t rb_Element){bit_data.ui16_Element = rb_Element;};

#endif
  
  /**
    set value POS of process msg
    @param rb_val new POS value for message
  */
  void setPos(uint8_t rb_val){bit_data.c_gtp.setPos(rb_val);};

#ifdef USE_DIN_9684
  /**
    set values gety and pos from gtp
    only used for DIN messages, ISO: gtp not in message
    @param rc_val new GETY_POS value for message
  */
  void setGtp(GetyPos_c rc_val){bit_data.c_gtp = rc_val;};
#endif

  /**
    set the 4byte data as int32_t val without any conversion
    for cmd's like measurement program increment, meas prog start/stop,
    etc.
    (4 uint8_t signed integer defined by int32_t in masterHeader)
    @param ri32_val new cmd value for message
    @param ren_procValType data type for message string (default i32_val)
  */
  void setDataRawCmd(int32_t ri32_val, proc_valType_t ren_procValType = i32_val);
  /**
    set data value as with int32_t value parameter
    and with defined send value type and set data type flag
    (if parameter and send type are different,
     a conversion by assignment is performed; %e.g. int32_t val 3
     can be sent as float 3.0)
    @param ri32_val new data value for message
    @param ren_procValType data type for message string (default i32_val)
  */
  void setData(int32_t ri32_val, proc_valType_t ren_procValType = i32_val);
  /**
    set data value as with uint32_t value parameter
    and with defined send value type and set data type flag
    (if parameter and send type are different,
     a conversion by assignment is performed; %e.g. int32_t val 3
     can be sent as float 3.0)
    @param ri32_val new data value for message
    @param ren_procValType data type for message string (default ui32_val)
  */
  void setData(uint32_t ri32_val, proc_valType_t ren_procValType = ui32_val);
  /**
    set one of the special commands of type proc_specCmd_t:
    setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    with specified data type specifying flag in CAN data string
    @param ren_procSpecCmd special command to send
    @param ren_procValType data type for message string (default ui32_val)
  */
  void setData(proc_specCmd_t ren_procSpecCmd, proc_valType_t ren_procValType = i32_val);
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
  void setData(float rf_val, proc_valType_t ren_procValType = float_val);
#endif
  /**
    set value data value as single uint8_t on position rb_pos
    @param rb_pos position of written uint8_t in data string
    @param rb_val uint8_t data value
  */
  void setData(uint8_t rb_pos, uint8_t rb_val){pb_data[rb_pos] = rb_val;};
  /**
    extract data from DIN/ISO commands and save it to member class
  */
  bool resolveCommandType();
  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CANPkg_c or CANPkgExt
    @see CANPkg_c::operator=
    @see CANPkgExt_c::operator=
  */
  virtual void string2Flags();
  /**
    some LBS+ terminals wants process data interaction for syncronisation of
    terminal mask with GETY_POS of terminal even for local process data
    @param rc_gtp GETY_POS of terminal, for which the GETY_POS of data is converted
    @param rui8_useProcGtp GTP for process data (optional, default to terminal gtp)
  */
  void useTermGtpForLocalProc(GetyPos_c rc_gtp, GetyPos_c rc_useProcGtp = GetyPos_c(0xF, 0xF));
  
  /** stores the command in generalized form */  
  GeneralCommand_c c_generalCommand;
  
private: // Private methods
  
  /**
    overloaded virtual function to translate flag values to data string;
    needed for sending informations from this object via CANIO_c on CAN BUS,
    because CANIO_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
  */
  virtual void flags2String();

private: // Private attributes
  /** data value as uint8_t string */
  uint8_t pb_data[5];
  struct _bit_data {
    /** ZAEHLNUM for data */
    uint16_t ui8_zaehlnum : 8;
    /** EMPF forheader */
    uint16_t b_empf : 8;
    /** SEND for header */
    uint16_t b_send : 8;

    /** PRI for header */
    uint16_t ui8_pri : 3;
    /** LIS for data*/
    uint16_t ui8_lis : 3;
    /** PD for data */
    uint16_t b_pd : 2;

    /** GetyPos for data */
    GetyPos_c c_gtp;
    
    /** MOD for data */
    uint16_t b_mod : 3;
    /** D for data */
    uint16_t b_d : 1;

    /** WERT for data */
    uint16_t ui8_wert : 4;
    /** INST for data */
    uint16_t ui8_inst : 4;
    /** decide about used val type: int32_t, uint32_t, float, cmd */
    uint16_t b_valType : 2;

#ifdef USE_ISO_11783
    uint16_t ui8_Command : 4;
    uint16_t ui16_Element : 12;
    uint16_t ui16_DDI : 16;
#endif
  } bit_data;
  /** pointer to monitor list item of sender "SEND" (NULL if not claimed address) */
  MonitorItem_c* pc_monitorSend;
  /** pointer to monitor list item of receiver "EMPF" (NULL if not claimed address) */
  MonitorItem_c* pc_monitorEmpf;
  /**
    some terminal wants to use GETY_POS of terminal even for local process
    data for communication on CAN BUS (default 0xFF for off)
  */
  GetyPos_c c_specialTermGtp;
  /**
    some terminal wants to use GETY_POS of terminal even for local process
    data for communication on CAN BUS (default 0xFF for off)
  */
  GetyPos_c c_specialTermUseProcGtp;
 
};

}
#endif
