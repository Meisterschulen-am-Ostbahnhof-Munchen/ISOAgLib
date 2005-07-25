/***************************************************************************
                          isoitem_c.h - object which represents an item
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
#ifndef ISO_ITEM_H
#define ISO_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/monitoritem_c.h"
#include "isoname_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  item class for ISO 11783 members monitor list to manage
  local (with address claim) and remote (statistic information)
  systems; utilizes MonitorItem_c for basic MonitorList
  management and ISOName_c for management of the 64bit NAME field
  @short Item with services needed for ISO11783 monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see MonitorItem
  @see ISOName
*/
class ISOItem_c : public MonitorItem_c  {
private:
  // private typedef alias names
public:
  /**
    constructor which can set optional all ident data
    @param ri32_time creation time of this item instance
    @param rc_gtp GETY_POS code of this item ((deviceClass << 3) | devClInst )
    @param rui8_nr number of this item
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ISOItem_c(int32_t ri32_time = 0, const GetyPos_c& rc_gtp = GetyPos_c::GetyPosUnspecified, uint8_t rui8_nr = 0xFE,
               IState_c::itemState_t rb_status = IState_c::IstateNull,
               uint16_t rui16_saEepromAdr = 0xFFFF, int riSingletonKey = 0 );
  /**
    copy constructor for ISOItem.
    The copy constructor checks if the source item is
    a master ( i.e. the pc_masterItem pointer points to this )
    -> it doesn't simply copy the pointer, but sets its
    own pointer also to the this-pointer of the new instance
    @param rrefc_src source ISOItem_c instance
  */
  ISOItem_c(const ISOItem_c& rrefc_src);
  /**
    assign constructor for ISOItem
    @param rrefc_src source ISOItem_c object
  */
  ISOItem_c& operator=(const ISOItem_c& rrefc_src);
  /**
    default destructor
  */
  ~ISOItem_c();

  /**
    deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const uint8_t* outputString() const {return c_gtp.getConstName().outputString();};

  /**
    get self config mode
    @return self configuration adress state
  */
  uint8_t selfConf() const {return c_gtp.getConstName().selfConf();};
  /**
    get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const {return c_gtp.getConstName().indGroup();};
  /**
    get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const {return c_gtp.getConstName().devClassInst();};
  /**
    get device class code
    @return:device class
  */
  uint8_t devClass() const {return c_gtp.getConstName().devClass();};
  /**
    get function code
    @return function code
  */
  uint8_t func() const {return c_gtp.getConstName().func();};
  /**
    get function instance code
    @return function instance code
  */
  uint8_t funcInst() const {return c_gtp.getConstName().funcInst();};
  /**
    get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const {return c_gtp.getConstName().ecuInst();};
  /**
    get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const {return c_gtp.getConstName().manufCode();};

  /**
    get serial number
    @return serial number
  */
  uint32_t serNo() const {return c_gtp.getConstName().serNo();};
  /**
    set the NAME data from 8 uint8_t string
    @param rpb_src pointer to 8byte source string
  */
  void inputString(const uint8_t* rpb_src) {c_gtp.set(rpb_src);};

  /**
    set self config mode
    @param rb_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool rb_selfConf) {c_gtp.getName().setSelfConf(rb_selfConf);};
  /**
    set industry group code
    @param rui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t rui8_indGroup) {c_gtp.getName().setIndGroup(rui8_indGroup);};
  /**
    set device class instance number
    @param rb_devClassInst instance number of ECU with same devClass
          in the network (comparable to POS in DIN9684)
  */
  void setDevClassInst(uint8_t rb_devClassInst) {c_gtp.getName().setDevClassInst(rb_devClassInst);};
  /**
    set device class code
    @param rb_devClass device class of ECU (equivalent to GETY in DIN)
  */
  void setDevClass(uint8_t rb_devClass) {c_gtp.getName().setDevClass(rb_devClass);};
  /**
    set function code
    @param rb_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t rb_func) {c_gtp.getName().setFunc(rb_func);};
  /**
    set function instance code
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t rb_funcInst) {c_gtp.getName().setFuncInst(rb_funcInst);};
  /**
    set ECU instance code
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t rb_ecuInst) {c_gtp.getName().setEcuInst(rb_ecuInst);};
  /**
    set manufactor code
    @param rui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t rui16_manufCode) {c_gtp.getName().setManufCode(rui16_manufCode);};

  /**
    set serial number (Identity Number)
    @param rui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t rui32_serNo) {c_gtp.getName().setSerNo(rui32_serNo);};

  /**
    set all element data with one call
    @param ri32_time creation time of this item instance
    @param rc_gtp GETY_POS code of this item ((deviceClass << 3) | devClInst )
    @param rui8_nr number of this item
    @param rb_status state of this ident (off, claimed address, ...)
    @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time, const GetyPos_c& rc_gtp, uint8_t rui8_nr,
           itemState_t ren_status = IState_c::Active,
           uint16_t rui16_saEepromAdr = 0xFFFF, int riSingletonKey = 0 );

  #if 0
  /**
    set all element data with one call
    @param ri32_time creation time of this item instance
    @param rc_gtp GETY_POS code of this item ((deviceClass << 3) | devClInst )
    @param rui8_nr number of this item
    @param rb_selfConf true -> the item has a self configurable source adress
    @param rui8_indGroup industry group code (2 for agriculture)
    @param rb_func function code (25 = network interconnect)
    @param rui16_manufCode manufactor code
    @param rui32_serNo serial no specific for one ECU of one manufactor
    @param ren_status state of this ident (off, claimed address, ...) (default: off)
    @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
    @param rb_funcInst counter for devices with same function (default 0)
    @param rb_ecuInst counter for ECU with same function and function instance (default 0)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time, GetyPos_c rc_gtp, uint8_t rui8_nr,
          bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
          uint32_t rui32_serNo, itemState_t ren_status = IState_c::Active,
          uint16_t rui16_saEepromAdr = 0xFFFF, uint8_t rb_funcInst = 0,
          uint8_t rb_ecuInst = 0, int riSingletonKey = 0 );
  #endif

  /**
    set GETY_POS code of this item
    @param rc_gtp GETY_POS
  */
  void setGtp(const GetyPos_c& rc_gtp)
  {c_gtp = rc_gtp;};
  /**
    deliver GETY_POS code of this item
    @return GETY_POS
  */
  const GetyPos_c& gtp() const {return MonitorItem_c::gtp();};

  /**
    deliver name
    @return pointer to the name uint8_t string (7byte)
  */
  virtual const uint8_t* name() const;
  /**
    check if the name field is empty (no name received)
    @return true -> no name received
  */
  virtual bool isEmptyName() const;
  /**
    deliver name as pure ASCII string
    @param pc_name string where ASCII string is inserted
    @param rui8_maxLen max length for name
  */
  virtual void getPureAsciiName(int8_t *pc_asciiName, uint8_t rui8_maxLen);
  /**
    periodically time evented actions:
      * find free SA or check if last SA is available
      * send adress claim

    possible errors:
      * dependant error in CANIO_c during send
    @return true -> all planned time event activitie performed
  */
  bool timeEvent( void );
  /**
    process received CAN pkg to update data and react if needed
    * update settings for remote members (e.g. change of SA)
    * react on adress claims or request for adress claims for local items
    @return true -> a reaction on the received/processed msg was sent
  */
  bool processMsg();

  // returns NULL if standalone, SELF if it is master itself, or the master ISOItem otherwise.
  ISOItem_c* getMaster () const;

  // check if this item is a master ( i.e. the master pointer points to itself )
  bool isMaster () const { return ( this == pc_masterItem )?true:false;};

  /** check if item has claimed address */
  bool isClaimedAddress() const { return wsClaimedAddress; };

  // attach to a master
  void setMaster ( ISOItem_c* rpc_masterItem );

protected:
// Protected Methods

private: // private methods
  /**
    set eeprom adress and read SA from there
  */
  void readEepromSa();
  /**
    write actual SA to the given EEPROM adress
  */
  void writeEepromSa();
  /**
    calculate random wait time between 0 and 153msec. from NAME and time
    @return wait offset in msec. [0..153]
  */
  uint8_t calc_randomWait();

private:
  /** EEPROM adress to store actual SA to use for next run */
  uint16_t ui16_saEepromAdr;

  /**
    pointer to the master ISOItem_c (if == this, then i'm master myself)
    NULL if not part of a master/slave setup
  */
  ISOItem_c* pc_masterItem;

  #ifdef USE_ISO_TERMINAL
  int8_t slavesToClaimAddress;

  bool b_oldVtState;
  #endif

protected:
  bool wsClaimedAddress;
};

}
#endif
