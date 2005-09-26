/***************************************************************************
                          dinserviceitem_c.h - object which represents an item
                                           in a service monitor list
                             -------------------
    begin                : Fri Apr 07 2000
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
#ifndef SERVICE_ITEM_H
#define SERVICE_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/monitoritem_c.h"
#include "adrvect_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  item class for service lists, which can manage
  adress vector, element status, number, DEV_KEY, local/remote
  @short Item with services needed for service monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see MemberItem
*/
class DINServiceItem_c : public MonitorItem_c  {
private:
public:
  /**
    constructor which can set optional all ident data
    @param ri32_time creation time of this item instance
    @param rc_devKey DEV_KEY code of this item
    @param rui8_nr number of this item (for real services equal to devClass)
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param rui16_adrVect ADRESSBELVEKT used by this item
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  DINServiceItem_c(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xF,
               uint8_t rb_status = 0, uint16_t rui16_adrVect = 0, int ri_singletonVecKey = 0);
  /**
    copy constructor for ServiceItem
    @param rrefc_src source DINServiceItem_c instance
  */
  DINServiceItem_c(const DINServiceItem_c& rrefc_src);
  /**
    assign constructor for ServiceItem
    @param rrefc_src source DINServiceItem_c object
  */
  DINServiceItem_c& operator=(const DINServiceItem_c& rrefc_src);
  /**
    default destructor
  */
  virtual ~DINServiceItem_c();

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
    set all element data with one call
    @param ri32_time creation time of this item instance
    @param rc_devKey DEV_KEY code of this item
    @param rui8_nr number of this item (for real services equal to devClass)
    @param ren_status status information of this ident (IState_c::Off, IState_c::Active, ...) (default: IState_c::Active)
    @param rui16_adrVect ADRESSBELVEKT used by this item
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xF,
           itemState_t ren_status = IState_c::Active, uint16_t rui16_adrVect = 0, int ri_singletonVecKey = 0);


  /**
    periodically time evented actions: send alive if needed

    possible errors:
      * dependant error in CANIO_c during send
    @return true -> all planned time event activitie performed
  */
  bool timeEvent( void );
  /**
    process received CAN pkg to update data and react if needed
    reacts on: member/service alive, member/service error notification,
               member stop command, state indication
    reactions of DINItem_c: adress claim, alive, name request, name msg,
                              status command,
    reactions delegated to DINServiceItem_c: status information, nromal alive,
                                        error information
    @return true -> a reaction on the received/processed msg was sent
  */
  bool processMsg();


  /**
    deliver status in STA format
    @return 0 == OFF, 1 == STANDBY, 2 == ACTIVE
  */
  uint8_t sta();


  /**
    deliver reference to c_adrvect
    @return reference to the ADRESSBELEGVEKT of this ServiceItem
  */
  AdrVect_c& adrvect(){return c_adrvect;};


protected:
// Protected Methods

   /**
    get alive verw code by function, which answers different for this class (service)
     and derived class MonitorItem_c (member)
    @return VERW code dependent on state IState_c::Member or IState_c::Service
  */
   uint8_t getAliveVerw(){return (itemState(IState_c::Member)? 1:4);};

// Protected attributes
  /** adress usage vector with 1 for every used adress */
  AdrVect_c c_adrvect;
};

}
#endif
