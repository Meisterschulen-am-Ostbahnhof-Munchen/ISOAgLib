/***************************************************************************
                          filterbox_c.cpp  - FilterBox_c permits free definition
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
#include "filterbox_c.h"
#include "canio_c.h"
#include <IsoAgLib/util/impl/cancustomer_c.h>
/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
using namespace __IsoAgLib;

/**
  default constructor without parameter values for creating an instance
  with default start state (init variables)

   @exception badAlloc
*/
FilterBox_c::FilterBox_c()
  :
    c_filter(0, Ident_c::StandardIdent),
    c_mask(0, Ident_c::StandardIdent),
    c_additionalMask(~0, Ident_c::StandardIdent) // additional Mask set to "all-1s", so it defaults to "no additional mask".
{
  pc_customer = NULL;
};

/**
  constructor with parameter values setting specific start state with
  setting pointer to the root CANIO_c and to the according CANCustomer
  instance; even define specific mask and filter setting

  @param rpc_customer pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
  @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param ren_E select if FilterBox_c is used for standard 11bit or extended 29bit ident
  @param rpc_filterBox optional parameter for getting to filterboxes connected together into the same MsgObj!
   @exception badAlloc
*/
FilterBox_c::FilterBox_c(CANCustomer_c* rrpc_customer,
                                  MASK_TYPE rt_mask, MASK_TYPE rt_filter,
                                  Ident_c::identType_t ren_identType, FilterBox_c* rpc_filterBox)
  : c_filter(rt_filter, ren_identType),
    c_mask(rt_mask, ren_identType),
    c_additionalMask( (rpc_filterBox == NULL) ? (~0) : (~(rpc_filterBox->c_filter.ident() ^ rt_filter)), ren_identType)
{
      pc_customer = rrpc_customer;
}

/**
  copy constructor which uses data of another FilterBox_c instance

  @param rrefc_src reference to the source FilterBox_c instance for copying
   @exception badAlloc
*/
FilterBox_c::FilterBox_c(const FilterBox_c& rrefc_src)
  : c_filter(rrefc_src.c_filter),
    c_mask(rrefc_src.c_mask),
    c_additionalMask(rrefc_src.c_additionalMask)
{
  pc_customer = rrefc_src.pc_customer;
};

/**
  destructor of this FilterBox_c instance
*/
FilterBox_c::~FilterBox_c(){
}

/**
  copy values of rrefc_src FilterBox_c object to this instance

  possible errors:
      * badAlloc on not enough memory for copying puffed CAN msg from source

  @param rrefc_src FilterBox_c instance with data to assign to this instance
  @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
*/
FilterBox_c& FilterBox_c::operator=(const FilterBox_c& rrefc_src){
  if ( this != &rrefc_src)
  {
    // rrefc_src and self are different object instances
    pc_customer = rrefc_src.pc_customer;
    c_filter = rrefc_src.c_filter;
    c_mask = rrefc_src.c_mask;
    c_additionalMask = rrefc_src.c_additionalMask;
  }
  return *this;
}

/** clear the data of this instance */
void FilterBox_c::clearData()
{
  pc_customer = NULL;
  c_mask.set(0, DEFAULT_IDENT_TYPE);
  c_filter.set(0, DEFAULT_IDENT_TYPE);
  c_additionalMask.set(~0, DEFAULT_IDENT_TYPE);
};

/* *************************************** */
/* ******* filter/mask managing ********** */
/* *************************************** */

/**
  set the mask (t_mask) and filter (t_filter) of this FilterBox
  @param rpc_customer pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
  @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param ren_E select if FilterBox_c is used for standard 11bit or extended 29bit ident
*/
void FilterBox_c::set(const Ident_c& rrefc_mask,
    const Ident_c& rrefc_filter, CANCustomer_c *rpc_customer, FilterBox_c* rpc_filterBox)
{
  c_filter = rrefc_filter;
  c_mask = rrefc_mask;
  pc_customer = rpc_customer;
  if (rpc_filterBox == NULL)
    c_additionalMask.set (~0, c_mask.identType());
  else
    c_additionalMask.set (~(rpc_filterBox->c_filter.ident() ^ c_filter.ident()), c_mask.identType());
};


/* ************************************************** */
/* ***** insert/get/process puffered CANPkg_c ******** */
/* ************************************************** */

/**
  control the processing of a received message
  (MsgObj_c::processMsg inserted data directly in CANCustomer
   -> FilterBox_c::processMsg() initiates conversion of CAN string
      to data flags and starts processing in CANCustomer_c)

  possible errors:
      * precondition no valid CANCustomer_c (or derived) is registered
  @return true -> FilterBox_c was able to inform registered CANCustomer
*/
bool FilterBox_c::processMsg(){
  if (pc_customer != NULL)
  { // pointer to CANCustomer_c was set
    // call customer's processMsg function, to let it
    // process the received CAN msg
    pc_customer->dataBase().string2Flags();
    pc_customer->processMsg();
    return true;
  }
  else
  { // pointer to CANCustomer_c wasn't set
    // -> don't know who wants to process the msg
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::Can );
    return false;
  }
}
