/***************************************************************************
                          idinserviceitem_c.h  - object which represents an item
                                             in a service monitor list
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
#ifndef ISERVICE_ITEM_H
#define ISERVICE_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/dinserviceitem_c.h"
#include <IsoAgLib/util/igetypos_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  item class for service lists, which can manage
  adress vector, element status, number, GETY_POS, local/remote
  @short Item with services needed for service monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see iMemberItem
*/
class iDINServiceItem_c : private __IsoAgLib::DINServiceItem_c {
public:
  // Public methods
  /**
    deliver the GETY_POS code of this item
    @return GETY_POS code
  */
  iGetyPos_c gtp()const{return static_cast<iGetyPos_c>(DINServiceItem_c::gtp());};
  /**
    deliver the GETY code alone (derived from gtp)
    @return GETY code
  */
  uint8_t gety()const{return DINServiceItem_c::gety();};
  /**
    deliver the POS code alone (derived from gtp)
    @return POS code
  */
  uint8_t pos()const{return DINServiceItem_c::pos();};
  /**
    deliver the number/adress of this item
    @return number
  */
  uint8_t nr(){return DINServiceItem_c::nr();};
  /**
    calculates time between now and last set of i32_lastTime;
    if called with time parameter, the difference to this is calculated,
    otherwise the system time is retreived and used
    @param ri32_time optional time to use for calculating the difference time to last update (default use actual System_c::getTime() )
    @return lasted time between last update and the compare time [msec.]
  */
  int32_t lastedTime() {return DINServiceItem_c::lastedTime();};
  /**
    check if specific state is set
    @param ren_itemState state information to check
    @return true -> the given state is set
  */
  bool itemState(itemState_t ren_itemState) const
    {return DINServiceItem_c::itemState(ren_itemState);};
    
  /**
    deliver the state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  const itemState_t itemState() const
    {return DINServiceItem_c::itemState();};
};

}
#endif
