/***************************************************************************
                          ican_io.h  -  Interface class for the internal
                                        library object CAN_IO
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

#ifndef ICAN_IO_H
#define ICAN_IO_H

//include the declaration of the internal object
#include "impl/canio_c.h"
#include <IsoAgLib/util/icanpkgext_c.h>
#include <IsoAgLib/util/icancustomer_c.h>
#include <IsoAgLib/util/icanpkg_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Main Object for CAN communication;
  act as interface for CANIO_c of the __IsoAgLib namespace. Manages
  dynamic amount of FilterBox_c which can have individual filter/mask settings
  (means no global mask needed).
  Inhertis virtual from Shared::Err_c to allow easy access to status checking functions.
  @author Dipl.-Inform. Achim Spangler
  */
class iCANIO_c : private __IsoAgLib::CANIO_c {
 private:
  // private typedef alias names
  typedef std::bad_alloc bad_alloc;
  typedef __IsoAgLib::CANCustomer_c CANCustomer_c;
  typedef __IsoAgLib::CANPkg_c CANPkg_c;
  typedef __IsoAgLib::Ident_c Ident_c;

 public:
  /**
    Initialize the CAN hardware, and instantiate one msg object for
     sending of messages. Do configuration for BUS number, sending bitrate,
    CAN ident length, minx/max hardware/BIOS Msg Obj numbers by parameters;
    called by specified constructor or external functions;
    wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

    If even the rui8_busNumber parameter has the default value 0xFF,
    then the configuration settings of a previous init call are not
    changed. In this case, the CAN BUS is only reset with the old settings.
    This is enabled by the default value 0xFF for rui8_busNumber, which is
    changed to DEFAULT_BUS_NUMBER for the first call of init() after the constructor.
    In all other cases, the special value 0xFF is indicator for empty parameter list.

    possible errors:
        * Err_c::range on undefined BUS,  msgOb_nr or sendPufferSize,
        * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
        * Err_c::busy on already used sending Msg-Obj
    @param rui8_busNumber optional number of the CAN bus
    @param rui16_bitrate optional bitrate (default by define in master_header.h)
    @param t_identLength optional length of the ident
           (S (11bit), E (29bit))
           (default by define in master_header.h)
    @param rui8_minObjNr optional minimum number for hardware CAN
           message object (important for sharing CAN controller with
         other tasks) (default by define in master_header.h)
    @param rui8_maxObjNr optional maximum number for hardware CAN
           message object (default by define in master_header.h)
    @return true -> correct initialisation without errors
  */
  inline bool init(uint8_t rui8_busNumber = 0xFF,
        uint16_t rui16_bitrate = DEFAULT_BITRATE,
        Ident_c::identType_t ren_identType = DEFAULT_CONFIG_IDENT_TYPE,
        uint8_t rui8_minObjNr = DEFAULT_MIN_OBJ_NR,
        uint8_t rui8_maxObjNr = DEFAULT_MAX_OBJ_NR
        ) {return CANIO_c::init(rui8_busNumber, rui16_bitrate,
          ren_identType, rui8_minObjNr, rui8_maxObjNr);};

  /**
    initiate processing of all received msg
    check all active MsgObj_c for received CAN msg and
    initiate their processing
    @return true -> all active CAN buffers are processed in time
  */
  bool processMsg() { return CANIO_c::processMsg();};
  /**
    deliver actual BUS load in baud
    @return baudrate in [baud] on used CAN BUS
  */
  int16_t getBusLoad() const {return CANIO_c::getBusLoad();};


  /**
    set the minimum delay in msec. between two sent CAN messages
    @param rui16_minDelay minimum time between two CAN messages [msec.]
  */
  void setSendpause(uint16_t rui16_minDelay) const { CANIO_c::setSendpause(rui16_minDelay);};

  /**
    deliver the numbers which can be placed at the moment in the send puffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return number of msgs which fit into send buffer
  */
  uint8_t sendCanFreecnt(Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  {return CANIO_c::sendCanFreecnt(ren_identType);};
  /**
    clear the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  */
  void sendCanClearbuf(Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  {CANIO_c::sendCanClearbuf(ren_identType);};

  /**
    test if a FilterBox_c definition already exist
    (version expecial for standard ident, chosen at compile time)
    @param rui32_mask individual mask for this filter box
    @param rui32_filter individual filter
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(uint16_t rui32_mask, uint16_t rui32_filter,
    Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE,
      ArrFilterBox::iterator* rpc_iter = NULL)
  { return CANIO_c::existFilter(rui32_mask, rui32_filter,
    ren_identType, rpc_iter);};
  /**
    test if a FilterBox_c definition already exist
    (version expecial for extended ident, chosen at compile time)
    @param rui32_mask individual mask for this filter box
    @param rui32_filter individual filter
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(uint32_t rui32_mask, uint32_t rui32_filter,
      Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE,
      ArrFilterBox::iterator* rpc_iter = NULL)
  {return CANIO_c::existFilter(rui32_mask, rui32_filter, ren_identType, rpc_iter);};
  /**
    test if a FilterBox_c definition already exist
    (version with comper items as Ident_c class instances, chosen by compiler)
    @param rc_compMask individual mask for this filter box
    @param rc_compFilter individual filter
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(const Ident_c& rc_compMask, const Ident_c& rc_compFilter,
      ArrFilterBox::iterator* rpc_iter = NULL)
  {return CANIO_c::existFilter(rc_compMask, rc_compFilter, rpc_iter);};

  /**
    create a Filter Box with specified rt_mask/rt_filter
    on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
    rb_reconfigImmediate == true -> useful for
    avoiding unneeded reconfiguration during
    sequence of FilterBox_c insertions;
    by rref_customer iCANIO_c (FilterBox_c) can start direct processing
    of received data in dedicated customer object (no search);
    uses BIOS functions

    possible errors:
        * Err_c::badAlloc on not enough memory for new FilterBox
          instance or for new configured MsgObj_c's
    @see IsoAgLib::iCANCustomer
    @param rref_customer reference to IsoAgLib::iCANCustomer_c which needs
           filtered messages (-> on received msg call
           rref_customer.processMsg())
    @param rt_mask individual mask for this filter box
    @param rt_filter individual filter
    @param rb_reconfigImmediate true -> all Filter objects are reconfigured
           to according CAN hardware MsgObj after creating this filter
    @param rt_identType ident type of the created ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> inserting and if wanted reconfiguration are
          performed without errors
    @exception badAlloc
  */
  inline bool insertFilter(IsoAgLib::iCANCustomer_c& rref_customer, MASK_TYPE rt_mask,
       MASK_TYPE rt_filter, bool rb_reconfigImmediate = true,
                     const Ident_c::identType_t rt_identType = DEFAULT_IDENT_TYPE)
  {return CANIO_c::insertFilter
      (static_cast<CANCustomer_c&>(rref_customer),
       rt_mask, rt_filter, rb_reconfigImmediate, rt_identType);
  };
  /**
    reconfigure the MsgObj after insert/delete of FilterBox
  */
  bool reconfigureMsgObj() {return CANIO_c::reconfigureMsgObj();};

  /**
    delete a FilerBox definition
    @param rui32_mask individual mask for this filter box
    @param rui32_filter individual filter
    @param rt_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
    @return true -> FilterBox_c found and deleted
  */
  bool deleteFilter(MASK_TYPE rui32_mask, MASK_TYPE rui32_filter,
      const Ident_c::identType_t rt_identType = DEFAULT_IDENT_TYPE)
  {return CANIO_c::deleteFilter(rui32_mask, rui32_filter, rt_identType);};



  /**
    function for sending data out of iCANPkg
    if send puffer is full a local loop waits till puffer has enough space
    (every 100ms the watchdog is triggered, to avoid watchdog reset)

    possible errors:
        * Err_c::hwConfig on wrong configured CAN obj,
          not init BUS or no configured send obj
        * Err_c::range on undef BUS or BIOS send obj nr
        * Err_c::can_warn on physical CAN-BUS problems
        * Err_c::can_off on physical CAN-BUS off state
    @see iCANPkg_c
    @param rrefc_src iCANPkg_c which holds the to be sent data
    @return reference to this iCANIO_c instance ==>
          needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
  */
  iCANIO_c& operator<<(iCANPkg_c& rrefc_src)
    {return static_cast<iCANIO_c&>(CANIO_c::operator<<(static_cast<iCANPkg_c&>(rrefc_src)));};
  /**
    function for sending data out of iCANPkg
    if send puffer is full a local loop waits till puffer has enough space
    (every 100ms the watchdog is triggered, to avoid watchdog reset)

    possible errors:
        * Err_c::hwConfig on wrong configured CAN obj,
          not init BUS or no configured send obj
        * Err_c::range on undef BUS or BIOS send obj nr
        * Err_c::can_warn on physical CAN-BUS problems
        * Err_c::can_off on physical CAN-BUS off state
    @see iCANPkg_c
    @param rrefc_src iCANPkg_c which holds the to be sent data
    @return reference to this iCANIO_c instance ==>
          needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
  */
  iCANIO_c& operator<<(iCANPkgExt_c& rrefc_src)
    {return static_cast<iCANIO_c&>(CANIO_c::operator<<(static_cast<__IsoAgLib::CANPkgExt_c&>(rrefc_src)));};

 private:
  /** allow getIcanInstance() access to shielded base class.
      otherwise __IsoAgLib::getCanInstance() wouldn't be accepted by compiler
    */
  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  friend iCANIO_c& getIcanInstance( uint8_t rui8_instance );
  #else
  friend iCANIO_c& getIcanInstance( void );
  #endif
};
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique iCANIO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iCANIO_c& getIcanInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iCANIO_c&>(__IsoAgLib::getCanInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique DINMonitor_c singleton instance */
  inline iCANIO_c& getIcanInstance( void )
  { return static_cast<iCANIO_c&>(__IsoAgLib::getCanInstance());};
#endif
}
#endif
