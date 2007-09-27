/***************************************************************************
                          icancustomer_c.h  -  header file for basic object
                                            for customer relationship to one
                                            or more FilterBox_c (-> CAN IO) instances
                             -------------------
    begin                : Tue Aug 3 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef ICAN_CUSTOMER_H
#define ICAN_CUSTOMER_H

#include "impl/cancustomer_c.h"
#include <IsoAgLib/util/icanpkgext_c.h>

namespace __IsoAgLib
{ // forward declarations
  class ProprietaryMessageHandler_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Base class for CAN customer classes
  -> FilterBox_c calls member function
    processMsg() to initiate processing of new received CAN telegram,
    which suits the configured filter/mask combination
  ==> all CAN handling object, which uses the services of FilterBox_c, CAN_IO
      should be derived from this class
  @see __IsoAgLib::CAN_IO
  @see __IsoAgLib::FilterBox
  *@author Dipl.-Inform. Achim Spangler
  */
class iCanCustomer_c : private __IsoAgLib::CanCustomer_c
{
public:
  /**
    virtual function which delivers a pointer to the iCANCustomer
    specific iCanPkgExt_c instance
  */
  virtual iCanPkgExt_c& iDataBase()=0;

private:
  /**
    virtual function which delivers a pointer to the CANCustomer
    specific CanPkgExt_c instance
  */
  virtual __IsoAgLib::CanPkgExt_c& dataBase() { return static_cast<__IsoAgLib::CanPkgExt_c&>(iDataBase()); }

  friend class iCanIo_c;
  friend class iIsoFilter_s;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iCanCustomer_c iCANCustomer_c;

} // namespace
#endif
