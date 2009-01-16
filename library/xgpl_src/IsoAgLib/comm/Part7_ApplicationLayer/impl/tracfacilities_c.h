/***************************************************************************
               tracfacilities_c - working on facilities data;
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from BaseCommon_c derived for
                                  interaction with other IsoAgLib objects

                                  This class handles Part 7 - B.25.3
                                  Tractor facilities response message 0xFE09
                                  in both directions (configurable)
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Ing. Antoine Kandera
    email                : a.kandera@osb-ag:de
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
#ifndef TRACFACILITIES_C_H
#define TRACFACILITIES_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>

namespace __IsoAgLib {


class TracFacilities_c;
typedef SINGLETON_DERIVED(TracFacilities_c,BaseCommon_c) SingletonTracFacilities_c;
  /** stores, updates  and delivers all Facilities data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
class TracFacilities_c : public SingletonTracFacilities_c
{
 public:// Public methods

 /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
    void singletonInit();


  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual void init_base (const IsoName_c*, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement);
  /** config the TracFacilities_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

  /** destructor for TracFacilities_c which has nothing to do */
  virtual ~TracFacilities_c() { BaseCommon_c::close();}

  bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/

  /** set tecu class with class t_val
      @param t_val  type of tecu class
    */
  void setTractorClass(const IsoAgLib::IsoTecuClassFlag_t at_val);

  /** set t_facility facility with value t_val
      @param t_facility  type of facility to be set
      @param t_val       type of facility flag
    */
  void setFacitity(const IsoAgLib::IsoFacility_t at_facility, const IsoAgLib::IsoFacilityFlag_t at_val);

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get tecu class
      @return  type of tecu class
    */
  IsoAgLib::IsoTecuClassFlag_t getTractorClass() const {return IsoAgLib::IsoTecuClassFlag_t(arrui8_facilitiesBitData[0] >> 6);}

  /** get facility flag for t_facility
      @param at_facility  type of facility to be set
      @return  type of facility flag
    */
  IsoAgLib::IsoFacilityFlag_t getFacitity(const IsoAgLib::IsoFacility_t at_facility) const;

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

 private:
  // Private methods
  friend class SINGLETON_DERIVED(TracFacilities_c,BaseCommon_c);
  /** HIDDEN constructor for a TracFacilities_c object instance which can optional
      set the configuration for send/receive for a moving msg
      NEVER instantiate a variable of type TracFacilities_c within application
      only access TracFacilities_c via getTracFacilitiesInstance() or getTracFacilitiesInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  TracFacilities_c();

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  virtual void checkCreateReceiveFilter( );

  /** this is only called when sending ident is configured and it has already claimed an address
  @pre  function is only called in tractor mode
  @see  BaseCommon_c::timeEvent()
     */
  virtual bool timeEventTracMode();

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  virtual bool processMsg();

  /** send guidance data
  @see  TracFacilities_c::processMsgRequestPGN
  @see  CanIo_c::operator<<
   */
  void sendFacilities();

 private:
    // Private attributes
    /** struct of facilities data */
    uint8_t arrui8_facilitiesBitData[8];

    /** check if facilities already sent after address claim */
    bool mb_facilitiesOnInitSent;
};

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracFacilities_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracFacilities_c& getTracFacilitiesInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracFacilities_c singleton instance */
  TracFacilities_c& getTracFacilitiesInstance(void);
  #endif

}

#endif
