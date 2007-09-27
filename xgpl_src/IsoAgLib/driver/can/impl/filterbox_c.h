/***************************************************************************
                          filterbox_c.h  - Filter_Box_c permits free definition
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
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/can.h>

#include "ident_c.h"
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */
/** Virtual Msg Box where special Filter and Mask can be defined.
  CanIo_c handles free amount of instances of this class.
  Every MsgObj_c has 1 to m appointed instances of this class.
  Each FilterBox_c has exact one CanCustomer_c, which is sored on creation
  of a FilterBox_c instance, who registered for processing of received msg.
  @short FilterBox_c can use individual filter and mask
    for selecting received CAN telegrams and controls their processing.

  @author Dipl.-Inform. Achim Spangler
*/
typedef enum
{
  IdleState = 0xff,
  InactiveState = 0xfe
} filterBoxState_t;

class FilterBox_c {
public:
  struct CustomerLen_s
  {
    CustomerLen_s (CanCustomer_c* rpc_customer, int8_t ri8_dlcForce) : pc_customer(rpc_customer), i8_dlcForce (ri8_dlcForce) {}

    CanCustomer_c* pc_customer;
    int8_t i8_dlcForce; // 0..8: DLC must exactly be 0..8.   < 0 (-1): DLC doesn't care! (default!)
  };

  /** default constructor without parameter values for creating an instance
    with default start state (init variables)

     @exception badAlloc
  */
  FilterBox_c();

  /** constructor with parameter values setting specific start state with
    setting pointer to the root CanIo_c and to the according CANCustomer
    instance; even define specific mask and filter setting

    @param rpc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
    @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param ren_identType select if FilterBox_c is used for standard 11bit or extended 29bit ident
    @param rpc_filterBox optional parameter for getting to filterboxes connected together into the same MsgObj!
     @exception badAlloc
  FilterBox_c(CanCustomer_c* rpc_customer,
              MASK_TYPE rt_mask, MASK_TYPE rt_filter,
              Ident_c::identType_t ren_identType = Ident_c::StandardIdent, FilterBox_c* rpc_filterBox = NULL);
  */

  /** copy constructor which uses data of another FilterBox_c instance
    @param rrefc_src reference to the source FilterBox_c instance for copying
     @exception badAlloc
  */
  FilterBox_c(const FilterBox_c& rrefc_src);

  /** destructor of this FilterBox_c instance */
  ~FilterBox_c();

  /** copy values of rrefc_src FilterBox_c object to this instance
    possible errors:
        * Err_c::badAlloc on not enough memory for copying puffed CAN msg from source

    @param rrefc_src FilterBox_c instance with data to assign to this instance
    @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
  */
  FilterBox_c& operator=(const FilterBox_c& rrefc_src);

  /** clear the data of this instance */
  void clearData();

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** close the BIOS filterbox object of this instance and close hardware CAN filterbox object */
  void closeHAL();

  /** check if this filterBox_c instance is really in use
      @return true -> filterBox is not in use
  */
  bool isIdle() {return (ui8_busNumber == IdleState && ui8_filterBoxNr == IdleState && vec_customer.empty());}
  #endif
  /** store new can customer with same filter and mask
      @param pc_cancustomer  new can customer
    */
  void insertCustomer(CanCustomer_c* pc_cancustomer, int8_t ri8_len) {vec_customer.push_back(CustomerLen_s(pc_cancustomer, ri8_len));}

  /** configures the CAN hardware of given FilterBox (uses BIOS function with EXTENDED_HAL)

    possible errors:
        * hwConfig used CAN BUS wans't configured properly
        * range given BUS or FilterBox number not in allowed area
        * hwBusy wanted FilterBox already in use
        * unspecified some other error
    @param rui8_busNumber BUS number, where this instance should act
    @param rui8_FilterBoxNr CAN hardware msg number for BIOS interaction
    @return true -> BIOS CAN object without errors configured
  */
  bool configCan(uint8_t rui8_busNumber, uint8_t rui8_FilterBoxNr);

  /* *************************************** */
  /* ******* filter/mask managing ********** */
  /* *************************************** */

  /** set the mask (t_mask) and filter (t_filter) of this FilterBox
    @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param rpc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
    @param ri8_dlcForce force the DLC to be exactly this long (0 to 8 bytes). use -1 for NO FORCING and accepting any length can-pkg
    @param ren_identType select if FilterBox_c is used for standard 11bit or extended 29bit ident
  */
  void set (const Ident_c& rrefc_mask,
            const Ident_c& rrefc_filter,
            CanCustomer_c *rpc_customer = NULL,
            int8_t ri8_dlcForce = -1,
            FilterBox_c* rpc_filterBox = NULL);

  /** check if ID from a CAN msg matches this FilterBox
    @param rt_ident CAN ident of received msg
    @return true -> CAN ident fits to local filter/mask definition
  */
  inline bool matchMsgId(MASK_TYPE rt_ident, Ident_c::identType_t rt_type )
  { return ( (c_mask.masked(rt_ident) == c_mask.masked(c_filter)) && (rt_type == identType()) );}

  /** checks, if FilterBox_c definition given by rc_mask and rc_filter is the same
    @param rc_mask mask to use for comparison
    @param rc_filter filter to use for comparison
    @return true -> given mask and filter are same as the local defs
  */
  bool equalFilterMask(const Ident_c& rc_mask, const Ident_c& rc_filter) const
    {return ((c_mask == rc_mask) && (c_filter == rc_filter));}

  /** checks, if Filter_Box_c has already stored given customer
      @param rref_customer  customer to compare
      @return               true -> customer already stored
    */
	bool equalCustomer( const __IsoAgLib::CanCustomer_c& rref_customer ) const;

  /** delete CanCustomer_c instance from array
      @param  rref_customer  CANCustomer to delete
      @return                true -> no more cancustomers exist, whole filterbox can be deleted
    */
  bool deleteFilter(const __IsoAgLib::CanCustomer_c& rref_customer);

  /** deliver the type of the FilterBox_c ident */
  Ident_c::identType_t identType() const {return c_filter.identType();}

  /** deliver const reference to mask Ident
    @return const reference to mask Ident_c instance
  */
  const Ident_c& mask() const {return c_mask;}

  /** deliver const reference to additionalMask Ident
    @return const reference to additionalMask Ident_c instance
  */
  const Ident_c& additionalMask() const {return c_additionalMask;}

  /** deliver const reference to filter Ident
    @return const reference to filter Ident_c instance
  */
  const Ident_c& filter() const {return c_filter;}

  #ifdef DEBUG_CAN_BUFFER_FILLING
  /** some debug messages */
  void doDebug(uint8_t rui8_busNumber);
  #endif

  /* ************************************************** */
  /* ***** insert/get/process puffered CanPkg_c ******** */
  /* ************************************************** */

  /** control the processing of a received message
    (MsgObj_c::processMsg inserted data directly in CANCustomer
     -> FilterBox_c::processMsg() initiates conversion of CAN string
        to data flags and starts processing in CanCustomer_c)

    possible errors:
        * Err_c::precondition no valid CanCustomer_c (or derived) is registered
    @return true -> FilterBox_c was able to inform registered CANCustomer
  */
  bool processMsg();

private:
// Private attributes
  /** c_filter for this FilterBox_c insance */
  Ident_c c_filter;

  /** c_mask for this FilterBox_c instance */
  Ident_c c_mask;

  /** c_additionalMask for this FilterBox_c insance (used for intentionally merging objects to one MsgObj! */
  Ident_c c_additionalMask;

  /**vector of pointer to pc_customer CanCustomer_c which works with the received CAN data */
  STL_NAMESPACE::vector<CustomerLen_s> vec_customer;

  /** number of message object */
  uint8_t ui8_filterBoxNr; //use like ui8_msgObjNr from msgobj_c class

  /** BUS Number for systems with more than one BUS */
  uint8_t ui8_busNumber;

#if ((defined(USE_ISO_11783)) && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
  /** we have either compiled for ISO, OR there is at least one internal / proprietary CAN channel */
  bool b_performIsobusResolve;
  #endif
};
}
#endif
