/***************************************************************************
                 tracpto_c.h  - working on Base Data Msg Type 2;
                                stores, updates  and delivers all base data
                                informations from CANCustomer_c derived for
                                CAN sending and receiving interaction;
                                from ElementBase_c derived for
                                interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef TRACPTO_C_H
#define TRACPTO_C_H

#include <IsoAgLib/typedef.h>
#include "../ibasetypes.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>

#include "basepkg_c.h"

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  class TracPTO_c;
  typedef SINGLETON_DERIVED(TracPTO_c,ElementBase_c) SingletonTracPto_c;
  /** working on Base Data Msg Type 2;
      stores, updates and delivers all base data informations;
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracPTO_c : public SingletonTracPto_c{
  public:// Public methods
    /* ********************************************* */
    /** \name Management Functions for class TracPTO_c  */
    /*@{*/

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances, to receive
        the needed CAN msg with base msg type 1,2 and calendar
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
      */
    void init(const DevKey_c* rpc_devKey = NULL, IsoAgLib::BaseDataGroup_t rt_mySendSelection = IsoAgLib::BaseDataNothing);

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close(void);

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends base msg if configured in the needed rates
        possible errors:
          * dependant error in CANIO_c on CAN send problems
        @see CANPkg_c::getData
        @see CANPkgExt_c::getData
        @see CANIO_c::operator<<
        @return true -> all planned activities performed in allowed time
      */
    bool timeEvent(void);
    /** check if filter boxes shall be created - create only ISO or DIN filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    void checkCreateReceiveFilter(void);

    /** config the Base_c object after init -> set pointer to devKey and
        config send/receive of different base msg types
        @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_mySendSelection optional Bitmask of base data to send (default send nothing)
      */
    void config(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection);
    /** Retrieve the last update time of the specified information type
        @param rt_mySendSelection optional Bitmask of base data to send (default send nothing)
      */
    int32_t lastUpdate(IsoAgLib::BaseDataGroup_t rt_mySendSelection) const;

    /** destructor for Base_c which has nothing to do */
    virtual ~TracPTO_c() { close();};
    /** process received base msg and store updated value for later reading access;
        called by FilterBox_c::processMsg after receiving a msg
        possible errors:
          * Err_c::lbsBaseSenderConflict base msg recevied from different member than before
        @see CANIO_c::processMsg
        @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
      */
    virtual bool processMsg();
    /*@}*/

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** set rear PTO
        @param ri16_val value to store as the speed of the rear PTO
      */
    void setPtoRear(int16_t ri16_val){i16_ptoRear = ri16_val;};
    /** set front PTO
        @param ri16_val value to store as the speed of the front PTO
      */
    void setPtoFront(int16_t ri16_val){i16_ptoFront = ri16_val;};

    #ifdef USE_ISO_11783
    /** set explicit information whether front PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoFrontEngaged(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPtoEngaged = rt_val;};
    /** set explicit information whether rear PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoRearEngaged(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPtoEngaged = rt_val;};
    /** set std RPM of front PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoFront1000(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPto1000 = rt_val;};
    /** set std RPM of rear PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoRear1000(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPto1000 = rt_val;};
    /** set economy mode of front PTO
      * @param rt_val IsoActive -> front PTO is in economy mode
      */
    void setPtoFrontEconomy(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPtoEconomy = rt_val;};
    /** set economy mode of rear PTO
      * @param rt_val IsoActive -> rear PTO is in economy mode
      */
    void setPtoRearEconomy(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPtoEconomy = rt_val;};
    #endif
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get rear pto
        @return actual rpm speed of rear PTO
      */
    int16_t ptoRear() const { return i16_ptoRear;};
    /** get front pto
        @return actual rpm speed of front PTO
      */
    int16_t ptoFront() const { return i16_ptoFront;};

    /** deliver the devKey of the sender of the base data
        possible errors:
          * Err_c::range rui8_typeNr doesn't match valid base msg type number
        @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2 | BaseDataCalendar
        @return DEV_KEY code of member who is sending the intereested base msg type
      */
    const DevKey_c& senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp);

    #ifdef USE_ISO_11783
    /** deliver explicit information whether front PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEngaged() const { return t_frontPtoEngaged;};
    /** deliver explicit information whether rear PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEngaged() const { return t_rearPtoEngaged;};
    /** deliver std RPM of front PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoFront1000() const { return t_frontPto1000;};
    /** deliver std RPM of rear PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoRear1000() const { return t_rearPto1000;};
    /** deliver economy mode of front PTO
      * @return IsoActive -> front PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEconomy() const { return t_frontPtoEconomy;};
    /** deliver economy mode of rear PTO
      * @return IsoActive -> rear PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEconomy() const { return t_rearPtoEconomy;};
    #endif
    /*@}*/

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracPTO_c,ElementBase_c);
    /**
      HIDDEN constructor for a Base_c object instance which can optional
      set the configuration for send/receive for base msg type 1,2
      and calendar
      NEVER instantiate a variable of type Base_c within application
      only access Base_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
      @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
    TracPTO_c() {};
    /**
      initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
    void singletonInit();
    /**
      deliver reference to data pkg
      @return reference to the member BasePkg_c, which encapsulates the CAN send structure
    */
    BasePkg_c& data(){return c_data;};
    /**
      deliver reference to data pkg as reference to CANPkgExt_c
      to implement the base virtual function correct
    */
    CANPkgExt_c& dataBase();

    /** check if a received message should be parsed */
    bool checkParseReceived(const DevKey_c& rrefc_currentSender, const DevKey_c& rrefc_activeSender, IsoAgLib::BaseDataGroup_t rt_selfSend ) const;

    #ifdef USE_DIN_9684
    /** send a DIN9684 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
    bool dinTimeEvent( void );
    /** process a DIN9684 base information PGN */
    bool dinProcessMsg();
    #endif
    #if defined(USE_ISO_11783)
    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    bool isoTimeEvent( void );
    /** send Base2 Group data with hitch and PTO state */
    void isoSendBase2Update( void );
    /** process a ISO11783 base information PGN */
    bool isoProcessMsg();
    #endif

  private:
    // Private attributes
    /** temp data where received data is put */
    BasePkg_c c_data;
    /** last time of base_2 msg [msec] */
    int32_t i32_lastBase2;

    /** pto rear */
    int16_t i16_ptoRear;
    /** pto front */
    int16_t i16_ptoFront;

    /** devKey which act as sender of base msg */
    const DevKey_c* pc_devKey;

    /** bitmask with selection of all base data types to send */
    IsoAgLib::BaseDataGroup_t t_mySendSelection;
    /** DEVKEY of base2 sender */
    DevKey_c c_sendBase2DevKey;

    #ifdef USE_DIN_9684
    /** flag to detect, if receive filters for DIN are created */
    bool b_dinFilterCreated;
    #endif
    #ifdef USE_ISO_11783
    /// General
    /** flag to detect, if receive filters for ISO are created */
    bool b_isoFilterCreated;
    /** front PTO engaged */
    IsoAgLib::IsoActiveFlag_t t_frontPtoEngaged;
    /** rear PTO engaged */
    IsoAgLib::IsoActiveFlag_t t_rearPtoEngaged;
    /** mode of front PTO: true == 1000 n/min; false == 540 n/min */
    IsoAgLib::IsoActiveFlag_t t_frontPto1000;
    /** mode of rear PTO: true == 1000 n/min; false == 540 n/min */
    IsoAgLib::IsoActiveFlag_t t_rearPto1000;
    /** economy mode of front PTO */
    IsoAgLib::IsoActiveFlag_t t_frontPtoEconomy;
    /** economy mode of rear PTO */
    IsoAgLib::IsoActiveFlag_t t_rearPtoEconomy;
    #endif
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracPTO_c& getTracPtoInstance(uint8_t rui8_instance = 0);
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracPTO_c& getTracPtoInstance(void);
  #endif
}
#endif
