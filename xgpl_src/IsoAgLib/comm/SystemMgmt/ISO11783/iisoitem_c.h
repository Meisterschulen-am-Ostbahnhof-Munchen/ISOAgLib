/***************************************************************************
                          iiso_item.h - object which represents an item
                                           in a iso monitor list
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef IISO_ITEM_H
#define IISO_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isoitem_c.h"
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>

namespace __IsoAgLib
{ // forward declarations (for friends)
  class ProprietaryMessageHandler_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** item class for ISO 11783 members monitor list to manage
  local (with address claim) and remote (statistic information)
  systems; utilizes IsoItem_c for basic MonitorList
  management and IsoName_c for management of the 64bit NAME field
  @short Item with services needed for ISO11783 monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see iMonitorItem
*/
class iIsoItem_c : private __IsoAgLib::IsoItem_c {
private:
  friend class iIdentItem_c;
  friend class iSystemMgmt_c;
  friend class iIsoMonitor_c;
  friend class iCanPkgExt_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;

public:
  /** deliver the ISOName code of this item
    @return ISOName code
  */
  const iIsoName_c& isoName()const{return IsoItem_c::isoName().toConstIisoName_c();}
  /** deliver the number/adress of this item
    @return number
  */
  uint8_t nr() const {return IsoItem_c::nr();}

  /** deliver name
    @return pointer to the name uint8_t string (7byte)
  */
  virtual const uint8_t* name() const { return IsoItem_c::name();}

  /** get self config mode
    @return self configuration adress state
  */
  uint8_t selfConf() const {return IsoItem_c::selfConf();}

  /** get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const {return IsoItem_c::indGroup();}

  /** get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const {return IsoItem_c::devClassInst();}

  /** get device class code
    @return:device class
  */
  uint8_t devClass() const {return IsoItem_c::devClass();}

  /** get function code
    @return function code
  */
  uint8_t func() const {return IsoItem_c::func();}

  /** get function instance code
    @return function instance code
  */
  uint8_t funcInst() const {return IsoItem_c::funcInst();}

  /** get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const {return IsoItem_c::ecuInst();}

  /** get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const {return IsoItem_c::manufCode();}

  /** get serial number
    @return serial number
  */
  uint32_t serNo() const {return IsoItem_c::serNo();}

  /** check if specific state is set
    @param ren_itemState state information to check
    @return true -> the given state is set
  */
  bool itemState(itemState_t ren_itemState) const {return IsoItem_c::itemState(ren_itemState);}

  /** deliver the state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  const itemState_t itemState() const {return IsoItem_c::itemState();}

  /** set the state of a local memberItem and send optional the new state, if
    Item::off, Item::standby or Item::active was set
    @param ren_itemState wante new state (independent old values aren't changed)
    @return resulting state
  */
  itemState_t setItemState(itemState_t ren_itemState) {return IsoItem_c::setItemState(ren_itemState, false);}

  /** set self config mode
    @param rb_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool rb_selfConf) {IsoItem_c::setSelfConf(rb_selfConf);}

  /** set industry group code
    @param rui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t rui8_indGroup) {IsoItem_c::setIndGroup(rui8_indGroup);}

  /** set device class instance number
    @param rui8_devClassInst instance number of ECU with same devClass in the network
  */
  void setDevClassInst(uint8_t rui8_devClassInst) {IsoItem_c::setDevClassInst(rui8_devClassInst);}

  /** set device class code
    @param rui8_devClass device class of ECU
  */
  void setDevClass(uint8_t rui8_devClass) {IsoItem_c::setDevClass(rui8_devClass);}

  /** set function code
    @param rb_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t rb_func) {IsoItem_c::setFunc(rb_func);}

  /** set function instance code
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t rb_funcInst) {IsoItem_c::setFuncInst(rb_funcInst);}

  /** set ECU instance code
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t rb_ecuInst) {IsoItem_c::setEcuInst(rb_ecuInst);}

  /** set manufactor code
    @param rui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t rui16_manufCode) {IsoItem_c::setManufCode(rui16_manufCode);}

  /** set serial number (Identity Number)
    @param rui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t rui32_serNo) {IsoItem_c::setSerNo(rui32_serNo);}


#ifdef USE_WORKING_SET
  /** check if item has announced its working-set description
      normally just use for WorkingSet-Master!
      if called for anything else, it is simply returned if this item has successfully claimed its address! */
  bool isWsAnnounced() const { return IsoItem_c::isClaimedAndWsAnnounced(); }
#endif
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iIsoItem_c iISOItem_c;

}

#endif
