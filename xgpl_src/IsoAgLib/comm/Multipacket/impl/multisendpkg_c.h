/***************************************************************************
                          multisendpkg_c.h - data object for multi message
                                             data transfer
                             -------------------
    begin                : Fri Sep 28 2001
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
#ifndef MULTI_SEND_PKG_H
#define MULTI_SEND_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>

#ifdef USE_ISO_11783
  #include <IsoAgLib/driver/can/impl/ident_c.h>
#endif

namespace __IsoAgLib {

typedef enum { MSB_LSB = 1, LSB_MSB = 2} byteOrder_t;

/**
  data object for multi message transfer
  transforms flag formated information to/from
  CAN uint8_t string
  @author Dipl.-Inform. Achim Spangler
*/
class MultiSendPkg_c : public CANPkgExt_c  {
private:
  // private typedef alias names
public:
  /** default constructor which has nothing to do */
  MultiSendPkg_c();
  /** default constructor which has nothing to do */
  ~MultiSendPkg_c();
  /**
    assign operator to insert informations from one CANPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CANPkgExt_c::operator=
    @see CANPkgExt_c::getData
    @param rrefc_right reference to the source CANPkg_c on the right
    @return reference to the source CANPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  */
  virtual const CANPkg_c& operator=(const CANPkg_c& rrefc_right);

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */

  /**
    deliver PRI of process msg (message type)
    @return PRI value of message
  */
  uint8_t pri()const{return ((ident() >> 8) & 0xF);};
  /**
    deliver EMPF of process msg (receiving member number)
    @return EMPF value of message
  */
  uint8_t empf()const{return bitfield.b_empf;};
  /**
    deliver SEND of process msg (sending member number)
    @return SEND value of message
  */
  uint8_t send()const{return bitfield.b_send;};
  /**
    deliver the message number in received CTS CAN pkg for IsoAgLib+ transfer
    (Fieldstar uses MSB_LSB uint8_t order)
    @param rui8_pos position of two uint8_t integer in CAN string
    @return number of to be sent message
  */
  uint16_t getData_2ByteInteger(uint8_t rui8_pos) const;
  /**
    set the uint8_t order for two uint8_t integer values, as %e.g. Fieldstar
    Terminals use the unusual order MSB_LSB
    @param ren_byteOrder wanted uint8_t order {LSB_MSB, MSB_LSB}
  */
  void setByteOrder(byteOrder_t ren_byteOrder){en_byteOrder = ren_byteOrder;};
  /**
    set the message number for sent CAN pkg for IsoAgLib+ transfer
    (Fieldstar uses MSB_LSB uint8_t order)
    @param rui8_pos position of two uint8_t integer in CAN string
    @param rui16_msgNr message number to set
  */
  void setData_2ByteInteger(uint8_t rui8_pos, uint16_t rui16_msgNr);
  /**
    set the 7 uint8_t data part of transfer message
    @param rpb_source source data pointer
    @param ri32_pos uint8_t position in data string to start
    @param rb_partSize optional amount of bytes of data stream for actual pkg (default 7)
  */
  void setDataPart(const HUGE_MEM uint8_t* rpb_source, int32_t ri32_pos, uint8_t rb_partSize = 7);
  /**
    set the 7 uint8_t data part of transfer message
    @param rpb_source source data pointer
    @param ri32_pos uint8_t position in data string to start
    @param rb_partSize optional amount of bytes of data stream for actual pkg (default 7)
   */
  void setDataPart(const std::vector<uint8_t>& refc_vecSource, int32_t ri32_pos, uint8_t rb_partSize = 7);
  #if defined(NMEA_2000_FAST_PACKET)
  /**
    set the 7 uint8_t data part of transfer message
    @param rpb_source source data pointer
    @param ri32_pos uint8_t position in data string to start
    @param rb_partSize optional amount of bytes of data stream for actual pkg (default 7)
   */
  void setFastPacketDataPart(const HUGE_MEM uint8_t* rpb_source, int32_t ri32_pos, uint8_t rb_partSize = 7, uint8_t rui8_offset = 0);
  /**
    set the 7 uint8_t data part of transfer message
    @param rpb_source source data pointer
    @param ri32_pos uint8_t position in data string to start
    @param rb_partSize optional amount of bytes of data stream for actual pkg (default 7)
   */
  void setFastPacketDataPart(const std::vector<uint8_t>& refc_vecSource, int32_t ri32_pos, uint8_t rb_partSize = 7, uint8_t rui8_offset = 0);
  #endif
  /**
    set value EMPF of process msg
    @param rb_val new EMPF value for message
  */
  void setEmpf(uint8_t rb_val) {
    bitfield.b_empf = rb_val;
    setIdentType(Ident_c::StandardIdent);
  };

  /**
    set value SEND of process msg
    @param rb_val new SEND value for message
  */
  void setSend(uint8_t rb_val) {
    bitfield.b_send = rb_val;
    setIdentType(Ident_c::StandardIdent);
  };

  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CANPkg_c or CANPkgExt
    @see CANPkg_c::operator=
    @see CANPkgExt_c::operator=
  */
  virtual void string2Flags();
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
  struct {
    /** EMPF forheader */
    uint8_t b_empf : 8;
    /** SEND for header */
    uint8_t b_send : 8;
  } bitfield;
  /** uint8_t order for two uint8_t integers as Fieldstar orders them MSB_LSB */
  byteOrder_t en_byteOrder;
};

}
#endif
