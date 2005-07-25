/***************************************************************************
                          isosystempkg_c.h - object for transformation of CAN
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
#ifndef ISO_SYSTEM_PKG_H
#define ISO_SYSTEM_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>
#include "isoname_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  ISOSystemPkg_c transforms informations from CAN uint8_t string
  to and from data structure for Scheduler_c SystemMgmt_c messages
  @author Dipl.-Inform. Achim Spangler
*/

class ISOSystemPkg_c : public CANPkgExt_c  {
public:
  /** default constructor */
  ISOSystemPkg_c();
  /** default destructor */
  ~ISOSystemPkg_c();
  /**
    ==> OBSOLETE, because now all can-pkg-data is STATIC!
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    assign operator to insert informations from one CANPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CANPkgExt_c::operator=
    @see CANPkgExt_c::getData
    @param rrefc_right reference to the source CANPkg_c on the right
    @return reference to the source CANPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  virtual const CANPkg_c& operator=(const CANPkg_c& rrefc_right) { return CANPkg_c::operator=(rrefc_right);};
  */

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */
  /**
    deliver counter value of address claim message
    @return counter value of address claim message
  */
  uint8_t nr() const { return isoSa(); };
  /**
    deliver GETY_POS of sender
    @return GETY_POS of sender
  */
  const GetyPos_c& gtp() const
  { // read on extended ident ISO msg the GTP from NAME setting
    return c_isoGtp;
  };
  /**
    deliver member name string of message
    @return member name string of message
  */
  const uint8_t* name() const
  { // deliver NAME from pb_data[0] on
    return pb_data;
  };

  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */

  /**
    set counter of address claim message
    @param rb_val value of NR flag
  */
  void setNr(uint8_t rb_val) { setIsoSa(rb_val); };
  /**
    set 7 uint8_t member name string
    @param rpb_val pointer to max. 7 uint8_t name array
  */
  void setName(const uint8_t* rName)
  { // deliver NAME from pb_data[0] on
    CNAMESPACE::memmove(pb_data,rName,8);
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
  private:
    GetyPos_c c_isoGtp;
};

}
#endif
