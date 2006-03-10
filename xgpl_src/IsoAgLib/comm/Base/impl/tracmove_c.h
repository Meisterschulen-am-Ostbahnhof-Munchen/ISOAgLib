/***************************************************************************
                 tracmove_c.h -   stores, updates and delivers all moving
                                  data information from CANCustomer_c
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

#include "../ibasetypes.h"
#include <IsoAgLib/comm/Base/impl/basecommon_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

  class TracMove_c;
  typedef SINGLETON_DERIVED(TracMove_c,BaseCommon_c) SingletonTracMove_c;
  /** stores, updates  and delivers all moving data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracMove_c : public SingletonTracMove_c{
  public:// Public methods
    /** config the TracMove_c object after init -> set pointer to devKey and
        config send/receive of a moving msg type
        @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    void config(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode);

    /** check if filter boxes shall be created - create only ISO or DIN filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    void checkCreateReceiveFilter( );

    /** destructor for TracMove_c which has nothing to do */
    virtual ~TracMove_c() { BaseCommon_c::close();};

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

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracMove_c,BaseCommon_c);
    /** HIDDEN constructor for a TracMove_c object instance which can optional
        set the configuration for send/receive for a moving msg
        NEVER instantiate a variable of type TracMove_c within application
        only access TracMove_c via getTracMoveInstance() or getTracMoveInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
        @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rc_sendMovingDevKey optional Bitmask of base data to send ( default send nothing )
      */
    TracMove_c() {};

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

    #ifdef USE_DIN_9684
    /** send a DIN9684 moving information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    bool dinTimeEventTracMode( );
    /** process a DIN9684 moving information PGN */
    bool dinProcessMsg();
    #endif

    #if defined(USE_ISO_11783)
    /** send a ISO11783 moving information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    bool isoTimeEventTracMode( );
    /** send moving data with ground&theor speed&dist */
  //  void isoSendMovingUpdate( );
    /** process a ISO11783 moving information PGN */
    bool isoProcessMsg();
     /** send moving data with ground&theor speed&dist */
    void isoSendMovingUpdate( );
    #endif

  private:
    // Private attributes
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
