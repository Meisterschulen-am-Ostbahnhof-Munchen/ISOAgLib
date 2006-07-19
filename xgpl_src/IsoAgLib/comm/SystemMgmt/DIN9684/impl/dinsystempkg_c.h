/***************************************************************************
                          dinsystempkg_c.h - object for transformation of CAN
                                        uint8_t string to and from System
                                        data structure
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef DIN_SYSTEM_PKG_H
#define DIN_SYSTEM_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
// #include <cstring>
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/devkey_c.h>
#include "adrvect_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  DINSystemPkg_c transforms informations from CAN uint8_t string
  to and from data structure for Scheduler_c SystemMgmt_c messages
  @author Dipl.-Inform. Achim Spangler
*/

class DINSystemPkg_c : public CANPkgExt_c  {
public:
  /** default constructor */
  DINSystemPkg_c();
  /** default destructor */
  ~DINSystemPkg_c();

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */
  /**
    delivers value of A flag in identifier
    @return A flag of Scheduler_c SystemMgmt_c message
  */
  uint8_t a() const {return data.b_a;};
  /**
    deliver counter value of address claim message
    @return counter value of address claim message
  */
  uint8_t nr() const { return data.ui8_nr;};
  /**
    deliver DEV_KEY of sender
    @return DEV_KEY of sender
  */
  const DevKey_c& devKey() const { return c_devKey; };
  /**
    deliver VERW code of message
    @return VERW code of message
  */
  uint8_t verw() const {return data.b_verw;};
  /**
    deliver member or service number of sender (SEND)
    @return member or service number of sender (SEND)
  */
  uint8_t send() const {return data.b_send;};

  /**
    deliver member or service number of receiver (EMPF)
    @return member or service number of receiver (EMPF)
  */
  uint8_t empf() const {return data.b_empf;};
  /**
    deliver XXXX flag of message (part of STOP command)
    @return XXXX flag of message
  */
  uint8_t xxxx() const {return data.b_xxxx;};
  /**
    deliver M flag of message (part of state command)
    @return M flag of message
  */
  uint8_t m() const {return data.b_m;};
  /**
    deliver STA flag of message (part of state command)
    @return STA flag of message
  */
  uint8_t sta() const {return data.b_sta;};
  /**
    deliver TB flag of message (part of STOP message sent by Terminal)
    @return TB flag of message
  */
  uint8_t tb() const {return data.b_tb;};
  /**
    deliver NAE flag of message (part of STOP message sent by Terminal)
    @return NAE flag of message
  */
  uint8_t nae() const {return data.b_nae;};
  /**
    deliver member name string of message
    @return member name string of message
  */
  const uint8_t* name() const { return (pb_data + 1); };

  /** delivers value of SEND in identifier */
  /**
    deliver ADRESSBELEGVEKTO field
    @return ADRESSBELEGVEKTO field with '1' at all positions where number is used
  */
  AdrVect_c& adrvect() {return c_adrVect;};


  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */

  /**
    set value of A flag in identifier (address claim msg or not)
    @param rb_val value of A flag
  */
  void set_a(uint8_t rb_val){data.b_a = rb_val;};
  /**
    set counter of address claim message
    @param rb_val value of NR flag
  */
  void setNr(uint8_t rb_val) { data.ui8_nr = rb_val; };
  /**
    set value of DEV_KEY flag in identifier
    @param rc_devKey value of DEV_KEY flag
  */
  void setDevKey(const DevKey_c& rc_devKey){
    c_devKey = rc_devKey;
    setIdentType(Ident_c::StandardIdent);
  };
  /**
    set value of DEV_KEY flag in identifier with DIN 9684 format
    @param rb_val value of DEV_KEY flag
  */
  void setDinDevKey(uint8_t rui8_devKey){
    c_devKey.set( ( rui8_devKey >> 3 ), ( rui8_devKey & 0x7 ) );
    setIdentType(Ident_c::StandardIdent);
  };
  /**
    set value of VERW flag (selector for system msg type)
    @param rb_val value of VERW flag
  */
  void setVerw(uint8_t rb_val){data.b_verw = rb_val; data.b_a = 0; };
  /**
    set value of SEND flag (number of sender of msg)
    @param rb_val value of SEND flag
  */
  void setSend(uint8_t rb_val){data.b_send = rb_val; data.b_a = 0;};

  /**
    set value of EMPF flag (receiver of command msg)
    @param rb_val value of EMPF flag
  */
  void setEmpf(uint8_t rb_val){data.b_empf = rb_val;};
  /**
    set value of XXXX flag (part of stop command)
    @param rb_val value of XXXX flag
  */
  void setXxxx(uint8_t rb_val){data.b_xxxx = rb_val;};
  /**
    set value of M flag (part of state command)
    @param rb_val value of M flag
  */
  void set_m(uint8_t rb_val){data.b_m = rb_val;};
  /**
    set value of STA flag (part of state command)
    @param rb_val value of STA flag
  */
  void setSta(uint8_t rb_val){data.b_sta = rb_val;};
  /**
    set value of TB flag (part of system STOP command sent by terminal)
    @param rb_val value of TB flag
  */
  void setTb(uint8_t rb_val){data.b_tb = rb_val;};
  /**
    set value of NAE flag (part of system STOP command sent by terminal)
    @param rb_val value of NAE flag
  */
  void setNae(uint8_t rb_val){data.b_nae = rb_val;};
  /**
    set 7 uint8_t member name string
    @param rpb_val pointer to max. 7 uint8_t name array
  */
  void setName(const uint8_t* rName)
  {
    CNAMESPACE::memmove((pb_data+1),rName,7);
    pb_data[8]='\0';
    setLen(8);
  };

  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CANPkg_c or CANPkgExt
    @see CANPkg_c::operator=
    @see CANPkgExt_c::operator=
  */
  virtual void string2Flags();

  virtual bool isNetworkMgmt() const { return true; } ;
protected: // protected methods

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
  /** address vector (16 bit) with 1 on all positions
      with used number */
  AdrVect_c c_adrVect;
  /** DEV_KEY of sender */
  DevKey_c c_devKey;
  struct
  {
    /** VERW command subtype of system message */
    uint8_t b_verw : 4;
    /** send tnr of sender */
    uint8_t b_send : 4;
    /** some system commands have no adrVect,
        they have EMPF instead */
    uint8_t b_empf : 4;
    /** one signal is identified with XXXX */
    uint8_t b_xxxx : 4;
    /** one time there is a m flag */
    uint8_t b_m : 1;
    /** flag STA */
    uint8_t b_sta : 3;
    /** A flag of ident in system message */
    uint8_t b_a : 1;
    /** flag NAE */
    uint8_t b_nae : 3;
    /** flag TB */
    uint8_t b_tb : 2;
    /** counter for address claims */
    uint8_t ui8_nr : 2;
  } data;
};

}
#endif
