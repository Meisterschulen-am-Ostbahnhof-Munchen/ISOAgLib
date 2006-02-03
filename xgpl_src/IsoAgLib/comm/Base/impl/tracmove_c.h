/***************************************************************************
                 tracmove_c.h - working on Base Data Msg Type 1;
                                  stores, updates  and delivers all base
                                  data informations from CANCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
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
#ifndef TRACMOVE_C_H
#define TRACMOVE_C_H

#include <IsoAgLib/typedef.h>
#include "../ibasetypes.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>

#include "basepkg_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

  class TracMove_c;
  typedef SINGLETON_DERIVED(TracMove_c,ElementBase_c) SingletonTracMove_c;
  /** working on Base Data Msg Type 1;
      stores, updates  and delivers all base data informations;
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracMove_c : public SingletonTracMove_c{
  public:// Public methods
    /* ********************************************* */
    /** \name Management Functions for class TracMove_c  */
    /*@{*/

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances, to receive
        the needed CAN msg with base msg type 1,2 and calendar
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
      */
    void init(const DevKey_c* rpc_devKey = NULL, IsoAgLib::BaseDataGroup_t rt_mySendSelection = IsoAgLib::BaseDataNothing );

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( void );

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
    bool timeEvent( void );
    /** check if filter boxes shall be created - create only ISO or DIN filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    void checkCreateReceiveFilter( void );

    /** config the TracMove_c object after init -> set pointer to devKey and
        config send/receive of different base msg types
        @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
      */
    void config(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection);
    /** Retrieve the last update time of the specified information type
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
    int32_t lastedTimeSinceUpdate( IsoAgLib::BaseDataGroup_t rt_mySendSelection ) const;
    /** Retrieve the time of last update */
    int32_t lastUpdateTime( IsoAgLib::BaseDataGroup_t rt_mySendSelection ) const;

    /** destructor for TracMove_c which has nothing to do */
    virtual ~TracMove_c() { close();};
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

    /** set the value of real speed (measured by radar)
        @param ri16_val value to store as real radar measured speed
      */
    void setSpeedReal(int16_t ri16_val) {i16_speedReal = ri16_val;};
    /** set the value of theoretical speed (calculated from gear)
        @param ri16_val value to store as theoretical gear calculated speed
      */
    void setSpeedTheor(int16_t ri16_val) {i16_speedTheor = ri16_val;};
    /** set the real (radar measured) driven distance with int16_t val
        @param ri16_val value to store as real radar measured distance
      */
    void setDistReal(int16_t ri16_val)
    { // use function to detect and handle 16bit integer overflows
      setOverflowSecure(i32_distReal, i16_lastDistReal, ri16_val);
    };
    /** set the real (radar measured) driven distance with int32_t val
        @param rreflVal value to store as real radar measured distance
      */
    void setDistReal(const int32_t& rreflVal);
    /** set the theoretical (gear calculated) driven distance with int16_t val
        @param ri16_val value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(int16_t ri16_val)
    { // use function to detect and handle 16bit integer overflows
      setOverflowSecure(i32_distTheor, i16_lastDistTheor, ri16_val);
    };
    /** set the theoretical (gear calculated) driven distance with int32_t val
        @param rreflVal value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(const int32_t& rreflVal);
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get the value of real speed (measured by radar)
        @return actual radar measured speed value
      */
    int16_t speedReal() const { return i16_speedReal;};
    /** get the value of theoretical speed (calculated from gear)
        @return theoretical gear calculated speed value
      */
    int16_t speedTheor() const { return i16_speedTheor;};

    /** get the real driven distance with int16_t val
        @return actual radar measured driven distance value
      */
    int32_t distReal() const { return i32_distReal;};
    /** get the real driven distance with int16_t val
        @return actual gear calculated driven distance value
      */
    int32_t distTheor() const { return i32_distTheor;};

    /** deliver the devKey of the sender of the base data
        possible errors:
          * Err_c::range rui8_typeNr doesn't match valid base msg type number
        @param rt_typeGrp base msg type no of interest: BaseDataGroup2 | BaseDataGroup3 | BaseDataGroupGps | BaseDataCalendar
        @return DEV_KEY code of member who is sending the intereested base msg type
      */
    const DevKey_c& senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp) const;

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracMove_c,ElementBase_c);
    /** HIDDEN constructor for a Base_c object instance which can optional
        set the configuration for send/receive for base msg type 1
        NEVER instantiate a variable of type TracMove_c within application
        only access Base_c via getTracMoveInstance() or getTracMoveInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
        @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
      */
    TracMove_c() {};
    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
    void singletonInit();
    /** deliver reference to data pkg
        @return reference to the member BasePkg_c, which encapsulates the CAN send structure
      */
    BasePkg_c& data(){return c_data;};
    /** deliver reference to data pkg as reference to CANPkgExt_c
        to implement the base virtual function correct
      */
    CANPkgExt_c& dataBase();

    /** update distance values ; react on int16_t overflow
        @param reflVal to be updated value as int32_t variable
        @param refiVal to be updated value as 16bit int16_t variable
        @param rrefiNewVal new value given as reference to 16bit int
      */
    static void setOverflowSecure(int32_t& reflVal, int16_t& refiVal, const int16_t& rrefiNewVal);

    /** get int16_t overflowed val from long
        @param rreflVal value as int32_t (32bit) variable
        @return 16bit int16_t calculated with counting overflow from 32767 to (-32766)
      */
    static int16_t long2int(const int32_t& rreflVal);

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
    /** send Base1 Group data with ground&theor speed&dist */
    void isoSendBase1Update( void );
    /** process a ISO11783 base information PGN */
    bool isoProcessMsg();
    #endif

  private:
    // Private attributes
    /** temp data where received data is put */
    BasePkg_c c_data;
    /** last time of base_1 msg [msec] */
    int32_t i32_lastBase1;
    /** last time of base_2 msg [msec] */

    /** distance */
    /** real distance as int32_t value (cumulates 16bit overflows) */
    int32_t i32_distReal;
    /** theoretical distance as int32_t value (cumulates 16bit overflows)*/
    int32_t i32_distTheor;
    /** last 16bit real distance to cope with 16bit overflows */
    int16_t i16_lastDistReal;
    /** last 16bit theoretical distance to cope with 16bit overflows */
    int16_t i16_lastDistTheor;

    /** speed */
    /** real speed */
    int16_t i16_speedReal;
    /** theoretical speed */
    int16_t i16_speedTheor;

    /** devKey which act as sender of base msg */
    const DevKey_c* pc_devKey;

    /** bitmask with selection of all base data types to send */
    IsoAgLib::BaseDataGroup_t t_mySendSelection;
    /** DEVKEY of base1 sender */
    DevKey_c c_sendBase1DevKey;

    #ifdef USE_DIN_9684
    /** flag to detect, if receive filters for DIN are created */
    bool b_dinFilterCreated;
    #endif
    #ifdef USE_ISO_11783
    /// General
    /** flag to detect, if receive filters for ISO are created */
    bool b_isoFilterCreated;
    #endif
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracMove_c& getTracMoveInstance(uint8_t rui8_instance = 0);
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracMove_c& getTracMoveInstance(void);
  #endif
}
#endif
