/***************************************************************************
                          dinserviceitem_c.cpp - object which represents an item
                                            in a service monitor list
                             -------------------
    begin                : Fri Apr 07 2000
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
#include "dinserviceitem_c.h"
#include "dinmonitor_c.h"
#include "dinsystempkg_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

namespace __IsoAgLib {


/**
  constructor which can set optional all ident data
  @param ri32_time creation time of this item instance
  @param rc_gtp GETY_POS code of this item
  @param rui8_nr number of this item (for real services equal to gety)
  @param rb_status state of this ident (off, claimed address, ...) (default: off)
  @param rui16_adrVect ADRESSBELVEKT used by this item
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
DINServiceItem_c::DINServiceItem_c(int32_t ri32_time, GetyPos_c rc_gtp, uint8_t rui8_nr, uint8_t rb_status, uint16_t rui16_adrVect, int ri_singletonVecKey)
  : MonitorItem_c(ri32_time, rc_gtp, rui8_nr, (IState_c::itemState_t)rb_status, ri_singletonVecKey ), c_adrvect(rui16_adrVect)
{}

/**
  copy constructor for ServiceItem
  @param rrefc_src source DINServiceItem_c instance
*/
DINServiceItem_c::DINServiceItem_c(const DINServiceItem_c& rrefc_src) : MonitorItem_c(rrefc_src), c_adrvect(rrefc_src.c_adrvect)
{}

/**
  assign constructor for ServiceItem
  @param rrefc_src source DINServiceItem_c object
*/
DINServiceItem_c& DINServiceItem_c::operator=(const DINServiceItem_c& rrefc_src)
{
  MonitorItem_c::operator=(rrefc_src);
  c_adrvect = rrefc_src.c_adrvect;
  return *this;
}

/**
  default destructor
*/
DINServiceItem_c::~DINServiceItem_c(){
}

/**
  deliver name
  @return pointer to the name uint8_t string (7byte)
*/
const uint8_t* DINServiceItem_c::name() const {
  return (const uint8_t*)("\0\0\0\0\0\0\0\0");
}
/**
  check if the name field is empty (no name received)
  @return true -> no name received
*/
bool DINServiceItem_c::isEmptyName() const {
  return false;
}

/**
  deliver name as pure ASCII string
  @param pc_name string where ASCII string is inserted
  @param rui8_maxLen max length for name
*/
void DINServiceItem_c::getPureAsciiName(int8_t *pc_asciiName, uint8_t rui8_maxLen) {
  pc_asciiName[0] = '\0';
  // just to make compiler happy -> as this function overloads base class variant, which is also
  // used for DIN 9684, where the length is needed
  if ( rui8_maxLen > 1 )pc_asciiName[1] = '\0';
}

/**
  set all element data with one call
  @param rpc_lbs pointer to the central Scheduler_c instance
  @param ri32_time creation time of this item instance
  @param rc_gtp GETY_POS code of this item
  @param rui8_nr number of this item (for real services equal to gety)
  @param ren_status status information of this ident (IState_c::Off, IState_c::Active, ...) (default: IState_c::Active)
  @param rui16_adrVect ADRESSBELVEKT used by this item
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void DINServiceItem_c::set(int32_t ri32_time, GetyPos_c rc_gtp, uint8_t rui8_nr,
        itemState_t ren_status, uint16_t rui16_adrVect,  int ri_singletonVecKey )
{
   MonitorItem_c::set(ri32_time, rc_gtp, rui8_nr, ren_status, ri_singletonVecKey);
   c_adrvect = rui16_adrVect;
};


/**
  periodically time evented actions: send alive if needed

  possible errors:
    * dependant error in CANIO_c during send
  @return true -> all planned time event activitie performed
*/
bool DINServiceItem_c::timeEvent( void )
{
  CANIO_c& c_can = getCanInstance4Comm();
  DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

   uint8_t b_aliveVerw = getAliveVerw();
   // check if alive message must be sent
   if (checkUpdateTime(1000))
   { // last alive msg is >= 1000msec. ago -> send alive
     DINSystemPkg_c& c_pkg = c_din_monitor.data();
     // fill data in SystemPkg
     c_pkg.setGtp(gtp());
     // if item is in error state, send errorAlive
     // dependent on the original aliveVerw, the errorVerw can be
     // built by setting the bits of 0x3
     if (itemState(IState_c::Error)) b_aliveVerw |= 0x3;
     c_pkg.setVerw(b_aliveVerw);
     c_pkg.setSend(nr());
     c_adrvect = c_din_monitor.adrvect();
     c_pkg.adrvect() = c_adrvect;
     // start send
    c_can << c_pkg;
  }
  return true;
}


/**
  process received CAN pkg to update data and react if needed
  reacts on: member/service alive, member/service error notification,
             member stop command, state indication
  @return true -> a reaction on the received/processed msg was sent
*/
bool DINServiceItem_c::processMsg(){
  DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();
  bool result = false;
  switch (c_pkg.verw())
  {
    case 1: case 4:
      // member or service alive
      updateTime(c_pkg.time());
      setNr(c_pkg.send()); // update nr if possibly changed
      setItemState(IState_c::ClaimedAddress); // was alive of member or service
      clearItemState(IState_c::Error); // clear possible error flag
      c_adrvect =  c_pkg.adrvect();
      result = true;
      break;
    case 3: case 7:
      // member or service error notification
      setItemState(IState_c::Error); //set state to error
      result = true;
      updateTime(c_pkg.time());
      c_adrvect =  c_pkg.adrvect();
      break;
#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
    case 11:
      // member stop command
      if (c_pkg.xxxx() == 0xF) setItemState(IState_c::Stop); // to stop
      else clearItemState(IState_c::Stop);
      result = true;
      updateTime(c_pkg.time());
      break;
    case 12:
      if (c_pkg.m() == 1)
      { // indication of state
        switch (c_pkg.sta())
        {
          case 0:
            setItemState(IState_c::Off);
            break;
          case 1:
            setItemState(IState_c::Standby);
            break;
          case 2:
            setItemState(IState_c::Active);
            break;
        }
      }
      result = true;
      updateTime(c_pkg.time());
      break;
#endif
    default:
      break;
  }
  return result;
};

/**
  deliver status in STA format
  @return 0 == OFF, 1 == STANDBY, 2 == ACTIVE
*/
uint8_t DINServiceItem_c::sta()
{
  uint8_t b_result = 0; // OFF as default
  if (itemState(IState_c::Standby)) b_result = 1;
  else if (itemState(IState_c::Active)) b_result = 2;

  return b_result;
};

} // end of namespace __IsoAgLib
