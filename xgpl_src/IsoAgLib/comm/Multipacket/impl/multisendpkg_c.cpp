/***************************************************************************
                          multisendpkg_c.cpp - data object for Process-Data
                                           messages
                             -------------------
    begin                : Fri Sep 28 2001
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisendpkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {


/** default constructor which has nothing to do */
MultiSendPkg_c::MultiSendPkg_c(){
  // set the standard uint8_t order as default
  en_byteOrder = LSB_MSB;
}
/** default constructor which has nothing to do */
MultiSendPkg_c::~MultiSendPkg_c(){
}

/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CANPkg_c or CANPkgExt
  @see CANPkg_c::operator=
  @see CANPkgExt_c::operator=
*/
void MultiSendPkg_c::string2Flags()
{
  #ifdef USE_ISO_11783
  if (identType() == Ident_c::ExtendedIdent)
  { // set pri, empf, send for convenience
    setEmpf(isoPs());
    setSend(isoSa());
    setIdentType(Ident_c::ExtendedIdent);
  }
  else
  #endif
  {
    setEmpf(((ident() >> 4) & 0xF));
    setSend(ident() & 0xF);
  }
};

/**
  overloaded virtual function to translate flag values to data string;
  needed for sending informations from this object via CANIO_c on CAN BUS,
  because CANIO_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CANPkg_c::getData
  @see CANPkgExt_c::getData
*/
void MultiSendPkg_c::flags2String()
{
};

/**
  deliver the message number in received CTS CAN pkg for IsoAgLib+ transfer
  (Fieldstar uses MSB_LSB uint8_t order)
  @param rb_pos position of two uint8_t integer in CAN string
  @return number of to be sent message
*/
uint16_t MultiSendPkg_c::getData_2ByteInteger(uint8_t rb_pos) const
{
  return (en_byteOrder == LSB_MSB)
    ?(data(rb_pos) + (data(rb_pos+1) << 8))
    :(data(rb_pos+1) + (data(rb_pos) << 8));
}
/**
  set the message number for sent CAN pkg for IsoAgLib+ transfer
  (Fieldstar uses MSB_LSB uint8_t order)
  @param rb_pos position of two uint8_t integer in CAN string
  @param rui16_val value to set
*/
void MultiSendPkg_c::setData_2ByteInteger(uint8_t rb_pos, uint16_t rui16_val)
{
  if (en_byteOrder == MSB_LSB)
  {
    pb_data[rb_pos] = (rui16_val >> 8);
    pb_data[rb_pos+1] = (rui16_val & 0xFF);
  }
  else
  {
    pb_data[rb_pos] = (rui16_val & 0xFF);
    pb_data[rb_pos+1] = (rui16_val >> 8);
  }
}

/**
  set the 7 uint8_t data part of transfer message
  @param rpb_source source data pointer
  @param ri32_pos uint8_t position in data string to start
  @param rb_partSize optional amount of bytes of data stream for actual pkg (default 7)
*/
void MultiSendPkg_c::setDataPart( HUGE_MEM uint8_t* rpb_source, int32_t ri32_pos, uint8_t rb_partSize)
{
  #ifdef USE_HUGE_MEM
  HUGE_MEM uint8_t* pb_source = rpb_source + ri32_pos;
  for (uint8_t ui8_ind = 0; ui8_ind < rb_partSize; ui8_ind++)
  {
    pb_data[ui8_ind+1] = pb_source[ui8_ind];
  }
  #else
  CNAMESPACE::memcpy(pb_data+1, rpb_source + ri32_pos, rb_partSize);
  #endif
  CNAMESPACE::memset(pb_data+1+rb_partSize,0xFF, (7-rb_partSize));
}

} // end of namespace __IsoAgLib
