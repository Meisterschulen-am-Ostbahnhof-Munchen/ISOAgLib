/***************************************************************************
                          filter_box.h  - Filter_Box_c permits free definition
                                          of mask/filter for receiving CAN
                                          telegrams by a CANCustomer
                                          (or derived) object
                             -------------------
    begin                : Fri Jul 30 1999
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
#ifndef FILTER_BOX_H
#define FILTER_BOX_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/hal/can.h>

#include "ident_c.h"
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */

/**
  Virtual Msg Box where special Filter and Mask can be defined.
  CANIO_c handles free amount of instances of this class.
  Every MsgObj_c has 1 to m appointed instances of this class.
  Each FilterBox_c has exact one CANCustomer_c, which is sored on creation
  of a FilterBox_c instance, who registered for processing of received msg.
  @short FilterBox_c can use individual filter and mask
    for selecting received CAN telegrams and controls their processing.

  @author Dipl.-Inform. Achim Spangler
*/
class FilterBox_c {
public:
  /**
    default constructor without parameter values for creating an instance
    with default start state (init variables)

     @exception badAlloc
  */
  FilterBox_c();

  /**
    constructor with parameter values setting specific start state with
    setting pointer to the root CANIO_c and to the according CANCustomer
    instance; even define specific mask and filter setting

    @param rpc_customer pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
    @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param ren_identType select if FilterBox_c is used for standard 11bit or extended 29bit ident
     @exception badAlloc
  */
  FilterBox_c(CANCustomer_c* rpc_customer,
             MASK_TYPE rt_mask, MASK_TYPE rt_filter,
             Ident_c::identType_t ren_identType = Ident_c::StandardIdent);

  /**
    copy constructor which uses data of another FilterBox_c instance

    @param rrefc_src reference to the source FilterBox_c instance for copying
     @exception badAlloc
  */
  FilterBox_c(const FilterBox_c& rrefc_src);

  /** destructor of this FilterBox_c instance */
  ~FilterBox_c();

  /**
    copy values of rrefc_src FilterBox_c object to this instance

    possible errors:
        * Err_c::badAlloc on not enough memory for copying puffed CAN msg from source

    @param rrefc_src FilterBox_c instance with data to assign to this instance
    @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
  */
  FilterBox_c& operator=(const FilterBox_c& rrefc_src);

  /** clear the data of this instance */
  void clearData();

  /* *************************************** */
  /* ******* filter/mask managing ********** */
  /* *************************************** */

  /**
    set the mask (t_mask) and filter (t_filter) of this FilterBox
    @param rpc_customer pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
    @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param ren_identType select if FilterBox_c is used for standard 11bit or extended 29bit ident
  */
  void set(const Ident_c& rrefc_mask, const Ident_c& rrefc_filter,
             CANCustomer_c *_customer = NULL);

  /**
    check if ID from a CAN msg matches this FilterBox
    @param rt_ident CAN ident of received msg
    @return true -> CAN ident fits to local filter/mask definition
  */
  inline bool matchMsgId(MASK_TYPE rt_ident, Ident_c::identType_t rt_type )
    {return ( (c_mask.masked(rt_ident) == c_filter.ident()) && (rt_type==identType()) );};

  /**
    checks, if FilterBox_c definition given by rc_mask and rc_filter is the same
    @param rc_mask mask to use for comparison
    @param rc_filter filter to use for comparison
    @return true -> given mask and filter are same as the local defs
  */
  bool equalFilterMask(const Ident_c& rc_mask, const Ident_c& rc_filter)
    {return ((c_mask == rc_mask) && (c_filter == rc_filter));};
  /**
    deliver the type of the FilterBox_c ident
  */
  Ident_c::identType_t identType() const {return c_filter.identType();};
  /**
    deliver const reference to mask Ident
    @return const reference to mask Ident_c instance
  */
  const Ident_c& mask() const {return c_mask;};
  /**
    deliver const reference to filter Ident
    @return const reference to filter Ident_c instance
  */
  const Ident_c& filter() const {return c_filter;};


  /* ************************************************** */
  /* ***** insert/get/process puffered CANPkg_c ******** */
  /* ************************************************** */

  /**
    deliver a pointer to the CANPkg_c of the customer, so that
    MsgObj_c::processMsg can control the insertion of received
    data
    @return pointer to CANPkg_c of CANCustomer
  */
  CANPkgExt_c* customersCanPkg() const { return &(pc_customer->dataBase());};

  /**
    control the processing of a received message
    (MsgObj_c::processMsg inserted data directly in CANCustomer
     -> FilterBox_c::processMsg() initiates conversion of CAN string
        to data flags and starts processing in CANCustomer_c)

    possible errors:
        * Err_c::precondition no valid CANCustomer_c (or derived) is registered
    @return true -> FilterBox_c was able to inform registered CANCustomer
  */
  bool processMsg();
private:
// Private attributes
  /** c_filter for this FilterBox_c insance */
  Ident_c c_filter;
  /** c_mask for this FilterBox_c instance */
  Ident_c c_mask;
  /** pointer to pc_customer CANCustomer_c which works with the received CAN data */
  CANCustomer_c* pc_customer;
};
}
#endif
