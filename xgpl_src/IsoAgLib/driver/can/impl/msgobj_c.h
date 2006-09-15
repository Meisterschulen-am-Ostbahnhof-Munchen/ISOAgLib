/***************************************************************************
                          msgobj_c.h  -  MsgObj_c encapsulate a hardware CAN Msg
                                        Object, to present a standard interaction
                                        with CAN for all platforms
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
#ifndef MSG_OBJ_H
#define MSG_OBJ_H

#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/can.h>

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && __GNUC__ >= 3
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

#include "filterbox_c.h"
#include "ident_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */
/** Each instances of this class handles one hardware CAN message object.
  While configuring the amount of instances can be higher than the dedicated.
  But before running the CAN system, the amount has to be shrinked by
  merging the instances with the smallest difference in filter bits.
  @short Encapsulate a BIOS MsgObj with forwarding received CAN msg to
    registered dedicated FilterBox_c instance.
  @author Dipl.-Inform. Achim Spangler
*/
class MsgObj_c {
private:
  // private typedef alias names
	#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<FilterBox_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator FilterRef;
	#else
  typedef STL_NAMESPACE::slist<FilterBox_c>::iterator FilterRef;
	#endif
public:
  /** default constructor for MsgObj_c which only init all member values defined start state */
  MsgObj_c();

  /** copy constructor for this class, which gets data from another MsgObj_c instance
    @param rrefc_src source MsgObj_c instance which should be cloned by this instance
  */
  MsgObj_c(const MsgObj_c& rrefc_src);

  /** destructor of MsgObj_c instance, which closes the hardware MsgObj_c */
  ~MsgObj_c();

  //+++++++++++++++++++++++++++++++++++
  // help functions for work on MsgObj_c
  //+++++++++++++++++++++++++++++++++++
  /** merge two msgObj instances rrefc_left and rrefc_right and tell with rb_extendedType
    if 11bit or 29 bit identifiers are used
    (uses BIOS function)

    possible errors:
    * Err_c::range BUS or MsgObj numbers out of allowed limits
    * Err_c::can_overflow amount of FilterBox_c of merged MsgObj_c is to big to store in one
        MsgObj_c (max is defined by FILTER_BOX_PER_MSG_OBJ in isoaglib_config.h)
    * Err_c::hwConfig BUS not initialized or ID can't be changed
    @param rrefc_right reference to MsgObj_c which should be merged into this instance
    @return true -> successful merged; false -> too many FilterBox_c refs for one MsgObj_c
  */
  bool merge(MsgObj_c& rrefc_right);

  /** close the t_data of this instance and close hardware CAN Msg Obj if it's open
    possible errors:
        * Err_c::range wrong BUS or MsgObj number stored in this instance
  */
  void close();

  /** close the correlated CAN object (uses BIOS function)
    possible errors:
        * Err_c::range wrong BUS or MsgObj number stored in this instance
  */
  void closeCan();

  /** test if a CAN msg object is open for this instance
    -> if the close function must be called in destructor
    @return true -> open
  */
  bool isOpen()const{return (bit_data.isOpen > 0)?true:false;}

  //+++++++++++++++++++++++++++++++++++
  // functions for configuring MsgObj_c
  //+++++++++++++++++++++++++++++++++++
  /** checks wether the filter of the given MsgObj_c is the same
    @param rrefc_other reference to MsgObj_c whoose filter should be compared with local filter
    @return true -> other MsgObj_c has same filter
  */
  inline bool operator==(const MsgObj_c& rrefc_other)const
    {return (rrefc_other.filter() == c_filter);}

  /** set t_filter of this MsgObj_c
    @param rrefc_val filter to set for this MsgObj_c
  */
  inline void setFilter(const Ident_c& rrefc_val){c_filter = rrefc_val;}

  /** get the t_filter of this MsgObj_c
    @return filter of this MsgObj_c instance
  */
  inline const Ident_c& filter() const {return c_filter;}

  /** get the common filter part of all merged FilterBox instances
    @return common filter of all FilterBoxes in this MsgObj_c instance
  */
  void commonFilterAfterMerge( Ident_c& rrefc_globalMask ) const;

  /** check if actual filter with specific filter type are equal to given combination
    @param rrefc_filter compared filter setting
  */
  bool equalFilter(const Ident_c& rrefc_filter) const
    {return (rrefc_filter == c_filter)?true:false;}

  /** configures the CAN hardware of given Msg Object (uses BIOS function)
    possible errors:
        * Err_c::hwConfig used CAN BUS wasn't configured properly
        * Err_c::range given BUS or MsgObj number not in allowed area
        * Err_c::busy wanted MsgObj already in use
        * Err_c::unspecified some other error
    @param rui8_busNumber BUS number, where this instance should act
    @param rui8_msgNr CAN hardware msg number for BIOS interaction
    @return true -> BIOS CAN object without errors configured
  */
  bool configCan(uint8_t rui8_busNumber, uint8_t rui8_msgNr);

  //+++++++++++++++++++++++++++++++++++++++
  // manage what to do with received t_data
  //+++++++++++++++++++++++++++++++++++++++
  /** insert pointer to FilterBox_c which receive
    CAN messages by this msgObj and reports success with true
    possible errors:
        * Err_c::can_overflow to many references to FilterBox_c instances for this MsgObj_c
    @param rrefc_box reference to FilterBox_c which should be inserted as possible processing instance of msg received by this instance
    @return true -> this reference could be stored in this MsgObj_c (limited amount)
  */
  bool insertFilterBox(FilterRef rrefc_box);

  /** delete pointer to a FilterBox_c and move following pointers one position forward
    possible errors:
        * Err_c::elNonexistant to be deleted FilterBox_c reference not registered for this MsgObj_c
    @param rrefc_box reference to FilterBox_c which should be deleted from reference array
    @return true -> given FilterBox_c was deleted from local reference array
  */
  bool deleteFilterBox(FilterRef rrefc_box);

  /** deliver count of contained FilterBox_c pointers
    @return count of references to FilterBox_c instances
  */
  uint8_t cnt_filterBox()const
  {
    #if 0
    return bit_data.cnt_filterBox;
    #else
    return arrPfilterBox.size();
    #endif
  }

#if 0
  /** delivers amount of insertable arrPfilterBox pointers
    @return amount of references to FilterBox_c which could be inserted into/handled by this MsgObj_c
  */
  uint8_t getFilterBoxCapacity() { return (FILTER_BOX_PER_MSG_OBJ - cnt_filterBox()); }
#endif

  /** start processing a received CAN msg
    (called by interrupt function)  (uses BIOS function)

      * Err_c::hwConfig MsgObj wans't configured properly
      * Err_c::can_warn CAN is in WARN state
      * Err_c::can_off CAN is in OFF state
      * Err_c::range since config of this MsgObj some action changed the
            BUS or MsgObj number to incorrect values
      * Err_c::can_overflow receive puffer overflow during receive
          (not likely because processing is called by receive IRQ)
      * dependant error if insertion in registered FilterBox_c causes error
    @param rui8_busNumber BUS number to check for received msg
		@param rb_forceProcessAll true -> process all CAN messages in HAL buffer, even if timeEvent would force stop
															( this important to process all buffered messages before FilterBoxes are reconfigured )
    @return number of received messages
  */
  uint8_t processMsg(uint8_t rui8_busNumber, bool rb_forceProcessAll = false );

	/** lock the corresponding hardware MsgObj to avoid receiving further CAN messages.
		This important for handling of the LastMsgObj, as it should only receive messages
		during process of CANIO_c::reconfigureMsgObj() - but as the messages shall be processed
		within normal CANIO_c::processMsg(), nu furhter messages shall be placed in the receive queue
		of the BIOS/OS. A immediate process wihtin execution of CANIO_c::reconfigureMsgObj() can cause
		deadlocks when the reconfig is initiated as a result of:
		-# Singleton_c::instance() -> Singleton_c::init()
		-# -> partialClass_c::init()
		-# -> CANIO_c::reconfigureMsgObj()
		-# -> MsgObj_c::processMsg()
		-# -> partialClass_c::processMsg()
		-# -> trigger update/reaction by Singleton_c::update() !!! undefined result, as Singleton_c::instance()
					has not yet finished, so that this type of circular access on the same Singleton_c::instance()
					is blocked by returning the signal value 0x1

		Thus CANIO_c::reconfigureMsgObj() locks the lastMessageObject at the end, so that the buffer content is
		simply conserved until normal CANIO_c::processMsg() is called.
	*/
	void lock( bool rb_lock = true );

	/** check if this given MsgObj_c is locked */
	bool isLocked() const { return bit_data.isLocked;}

private:
  // Private attributes
  /** array of pointer to appointed arrPfilterBox instances */
  std::vector<FilterRef> arrPfilterBox;

//  FilterRef arrPfilterBox[FILTER_BOX_PER_MSG_OBJ];
  /** Ident_c filter for this msgObj */
  Ident_c c_filter;

  /** msgObjNr */
  struct {
	  uint16_t ui8_msgObjNr  : 7;
		uint16_t isLocked      : 1;
    #if 0
    uint16_t cnt_filterBox : 4;
    #endif
    uint16_t isOpen        : 1;
    uint16_t busNumber     : 3;
  } bit_data;
private:
// Private methods
  #if 0
  /** set counter for appointed arrPfilterBox instances
    @param rb_val wanted counter for appointed arrPfilterBox instances
  */
  void setCntFilterBox(uint8_t rb_val) { bit_data.cnt_filterBox = rb_val; }
  #endif

  /** set if a CAN msg object is open for this instance
    -> if the close function must be called in destructor
    @param rb_state wanted state
  */
  void setIsOpen(bool rb_state){bit_data.isOpen = (rb_state)?1:0;}

  /** deliver Bus Number of this MsgObj_c
    @return bus number
  */
  uint8_t busNumber()const{return bit_data.busNumber;}

  /** set Bus Number of this MsgObj_c
    @param rb_val wanted bus number
  */
  void setBusNumber(uint8_t rb_val){bit_data.busNumber = (rb_val<8)?rb_val:0;}

  /** deliver msgObj number for BIOS interaction
    @return msgObj number
  */
  uint8_t msgObjNr()const{return bit_data.ui8_msgObjNr;}

  /** set msgObj number for BIOS interaction
    @param rb_val wanted msgObj number
  */
  void setMsgObjNr(uint8_t rb_val){bit_data.ui8_msgObjNr = rb_val;}

  /** verify given BUS number and MsgObj number, if they are within allowed
    limits (defined in isoaglib_config.h)
    if called withoutparameter values (default -1) the actual configured are
    checked -> if these are incorrect Err_c::range is set
    (mostly used by MsgObj_c to verify itself)
    @param rc_busNr number of the BUS (default the actual stored number is checked)
    @param rc_msgobjNr number of the MsgObj (default the actual stored number is checked)
    @return true -> values are correct
  */
  bool verifyBusMsgobjNr(int8_t rc_busNr = -1, int8_t rc_msgobjNr = -1);
};
} // end of __IsoAgLib namespace
#endif
