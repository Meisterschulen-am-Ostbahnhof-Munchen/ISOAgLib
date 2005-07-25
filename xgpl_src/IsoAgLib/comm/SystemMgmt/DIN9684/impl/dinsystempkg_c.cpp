/***************************************************************************
                          dinsystempkg_c.cpp - object for transformation of CAN
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
#include "dinsystempkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {

/** default constructor */
DINSystemPkg_c::DINSystemPkg_c(){
}
/** default destructor */
DINSystemPkg_c::~DINSystemPkg_c(){
}

/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CANPkg_c or CANPkgExt
  @see CANPkg_c::operator=
  @see CANPkgExt_c::operator=
*/
void DINSystemPkg_c::string2Flags()
{
  setDinGtp(ident() & 0x7F);
  set_a((ident() & 0x80) >> 7);
  if (a() == 1)
  { // address claim telegram
    setNr(pb_data[0]);
  }
  else
  {
    setVerw(pb_data[0] >> 4);
    setSend(pb_data[0] & 0xF);
    switch (verw())
    {
      case 8:  // name request
        break;
      case 9: // has name
        setName(pb_data + 1);
        break;
#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
      case 10: case 11:   // system or member stop
        setEmpf(pb_data[1] >> 4);
        setXxxx(pb_data[1] & 0xF);
        break;
      case 12: // member state
        setEmpf(pb_data[1] >> 4);
        set_m((pb_data[1] >> 3) & 0x1);
        setSta(pb_data[1] & 0x7);
        break;
      case 15:
        setTb(pb_data[3] >> 6);
        setNae(pb_data[3] & 0x7);
        break;
#endif
      default: // has adrVect
        c_adrVect.set(pb_data+1);
        break;
    }
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
void DINSystemPkg_c::flags2String()
{
  if (a() == 1)
  {  // address claim telegramm
    pb_data[0] = nr();
    setLen(1);
    setIdent( MASK_TYPE( (a() << 7) | ( gtp().getGety() << 3 ) | ( gtp().getPos() & 0x7 ) ), Ident_c::StandardIdent); // PRI system is always 0
  }
  else
  {
    pb_data[0] = ((verw() << 4)| (send()));
    setIdent( MASK_TYPE( ( gtp().getGety() << 3 ) | ( gtp().getPos() & 0x7 ) ), Ident_c::StandardIdent); // PRI system is always 0
    switch (verw())
    {
      case 8: // request name
        setLen(1);
        break;
      case 9: // has name
        CNAMESPACE::memmove(pb_data+1, name(), 7);
        setLen(8);
        break;
#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
      case 10:
        pb_data[1] = (0xF0 | xxxx());
        setLen(2);
        break;
      case 11:   // system or member stop
        pb_data[1] = ((empf() << 4) | xxxx());
        setLen(2);
        break;
      case 12: // member state
        pb_data[1] = ((empf() << 4) | (m() << 3) | sta());
        setLen(2);
        break;
      case 15:
        pb_data[1] = pb_data[2] = 0xFF;
        pb_data[3] = ((tb() << 6)|(nae() ));
        setLen(4);
        break;
#endif
      default: // in [0,7] --> has adrVect
        c_adrVect.getCanString(pb_data+1);
        setLen(3);
        break;
    }
  }
};
} // end of namespace __IsoAgLib
