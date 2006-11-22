/***************************************************************************
                 tracmove_c.h -   stores, updates and delivers all moving
                                  data information from CANCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from BaseCommon_c derived for
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

  class TracMove_c : public SingletonTracMove_c
  {
  public:// Public methods

    /** config the TracMove_c object after init -> set pointer to isoName and
        config send/receive of a moving msg type
        @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode);

    /** update selected speed with actually best available speed
        @param t_speedSrc  from which source is the speed available
      */
    void updateSpeed(IsoAgLib::SpeedSource_t t_speedSrc);

    /** update distance and direction with the actually best available distance and direction
        @param t_distanceSrc  from which source is the distance and direction available
      */
    void updateDistanceDirection(IsoAgLib::DistanceDirectionSource_t t_distanceSrc);

    /** destructor for TracMove_c which has nothing to do */
    virtual ~TracMove_c() { BaseCommon_c::close();}

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/
    /** set the theoretical (gear calculated) driven distance
        @param rreflVal value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(const uint32_t& rreflVal) { ui32_distTheor = rreflVal; }

    /** set the real (radar measured) driven distance
        @param rreflVal value to store as real radar measured distance
      */
    void setDistReal(const uint32_t& rreflVal) { ui32_distReal = rreflVal; }

    /** set the value of real speed (measured by radar)
        @param ri16_val value to store as real radar measured speed
      */
    void setSpeedReal(const int32_t& ri32_val) {i32_speedReal = ri32_val;}

    /** set the value of theoretical speed (calculated from gear)
        @param ri16_val value to store as theoretical gear calculated speed
      */
    void setSpeedTheor(const int32_t& ri32_val) {i32_speedTheor = ri32_val;}

    /** set measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
        @return  direction of travel
      */
    void setDirectionTheor(IsoAgLib::IsoDirectionFlag_t t_val) {t_directionTheor = t_val;}

    /** set measured signal indicating either forward or reverse as the real (radar measured) direction of travel
        @return  direction of travel
      */
    void setDirectionReal(IsoAgLib::IsoDirectionFlag_t t_val) {t_directionReal = t_val;}

    /** set parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
        @param rt_val  indicates direction (IsoInactive = not reversed; IsoActive = reversed)
      */
    void setOperatorDirectionReversed(const IsoAgLib::IsoOperatorDirectionFlag_t rt_val) { t_operatorDirectionReversed = rt_val;}

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    void setStartStopState(const IsoAgLib::IsoActiveFlag_t rt_val) {t_startStopState = rt_val;}

    /** set actual distance traveled by the machine based on the value of selected machine speed
        @param i32_val  actual distance
      */
    void setSelectedDistance(const uint32_t& i32_val) {ui32_selectedDistance = i32_val;}

    /** set current direction of travel of the machine
        @param t_val  current direction of travel
      */
    void setSelectedDirection(IsoAgLib::IsoDirectionFlag_t t_val) {t_selectedDirection = t_val;}

    /** get current value of the speed as determined from a number of sources by the machine
        @param i32_val  current value of speed
      */
    void setSelectedSpeed(const int32_t& i32_val)  {i32_selectedSpeed = i32_val;}

    /** set speed source that is currently being reported in the machine speed parameter
        @param t_val  actual speed source
      */
    void setSelectedSpeedSource(IsoAgLib::IsoSpeedSourceFlag_t t_val) {t_selectedSpeedSource = t_val;}

    /** present limit status of selected speed
        @param t_val  limit status
      */
    void setSelectedSpeedLimitStatus(const IsoAgLib::IsoLimitFlag_t t_val) {t_selectedSpeedLimitStatus = t_val;}
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/
    /** get the real driven distance with int16_t val
        @return actual radar measured driven distance value
      */
    uint32_t distReal() const { return ui32_distReal;}

    /** get the real driven distance with int16_t val
        @return actual gear calculated driven distance value
      */
    uint32_t distTheor() const { return ui32_distTheor;}

    /** get the value of real speed (measured by radar)
        @return actual radar measured speed value
      */
    int32_t speedReal() const { return i32_speedReal;}

    /** get the value of theoretical speed (calculated from gear)
        @return theoretical gear calculated speed value
      */
    int32_t speedTheor() const { return i32_speedTheor;}

    /** get measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
        @return  direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t directionTheor() {return t_directionTheor;}

    /** get measured signal indicating either forward or reverse as the real (radar measured) direction of travel
        @return  direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t directionReal() {return t_directionReal;}

    /** get parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
        @return indicates direction (IsoInactive = not reversed; IsoActive = reversed)
      */
    IsoAgLib::IsoOperatorDirectionFlag_t operatorDirectionReversed()const { return t_operatorDirectionReversed;}

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    IsoAgLib::IsoActiveFlag_t startStopState() const {return t_startStopState;}

    /** get actual distance traveled by the machine based on the value of selected machine speed
        @return  actual distance traveled
      */
    uint32_t selectedDistance() const {return ui32_selectedDistance;}

    /** get current direction of travel of the machine
        @return  current direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t selectedDirection() const {return t_selectedDirection;}

    /** get current value of the speed as determined from a number of sources by the machine
        @return  current value of speed
      */
    int32_t selectedSpeed() const {return i32_selectedSpeed;}

    /** present limit status of selected speed
        @return  limit status
      */
    IsoAgLib::IsoLimitFlag_t selectedSpeedLimitStatus() const {return t_selectedSpeedLimitStatus;}

    /** get speed source that is currently being reported in the machine speed parameter
        @return  speed source that is currently being reported
      */
    IsoAgLib::IsoSpeedSourceFlag_t selectedSpeedSource() const {return t_selectedSpeedSource;}


  virtual const char* getTaskName() const;

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracMove_c,BaseCommon_c);
    /** HIDDEN constructor for a TracMove_c object instance which can optional
        set the configuration for send/receive for a moving msg
        NEVER instantiate a variable of type TracMove_c within application
        only access TracMove_c via getTracMoveInstance() or getTracMoveInstance( int riLbsBusNr ) in case more than one BUS is used for IsoAgLib
      */
    TracMove_c() {}

    /** check if filter boxes shall be created - create only filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter( );

    /** send a ISO11783 moving information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in tractor mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventTracMode();

    /** process a ISO11783 base information PGN
        @pre  sender of message is existent in monitor list
        @see  CANPkgExt_c::resolveSendingInformation()
      */
    virtual bool processMsg();

    /** send moving data with ground&theor speed&dist
        @see  CANIO_c::operator<<
      */
    void sendMovingTracMode( );

  private:
    // Private attributes
    /** actually selected distance and direction source */
    IsoAgLib::DistanceDirectionSource_t t_distDirecSource;

    /** last time when selected speed data was processed */
    uint32_t ui32_lastUpdateTimeSpeed;

    /** last time when direction and distance data was processed */
    uint32_t ui32_lastUpdateTimeDistDirec;


    /************ DISTANCE **************/

    /** real distance as int32_t value (cumulates 16bit overflows) */
    uint32_t ui32_distReal;

    /** theoretical distance as int32_t value (cumulates 16bit overflows)*/
    uint32_t ui32_distTheor;

    /** last 16bit real distance to cope with 16bit overflows */
    //uint16_t ui32_lastDistReal;

    /** last 16bit theoretical distance to cope with 16bit overflows */
    //uint16_t ui32_lastDistTheor;

    /** actual distance traveled by the machine based on the value of selected machine speed */
    uint32_t ui32_selectedDistance;

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    IsoAgLib::IsoActiveFlag_t t_startStopState;


    /************ DIRECTION *************/
    /** parameter indicates whetcher the reported direction is reversed from the perspective of the operator */
    IsoAgLib::IsoOperatorDirectionFlag_t t_operatorDirectionReversed;

    /** measured signal indicating either forward or reverse as the direction of travel */
    IsoAgLib::IsoDirectionFlag_t t_directionTheor;

    /** measured signal indicating either forward or reverse as the direction of travel */
    IsoAgLib::IsoDirectionFlag_t t_directionReal;

    /** indicates the current direction of travel of the machine */
    IsoAgLib::IsoDirectionFlag_t t_selectedDirection;


    /************ SPEED ***************/
    /** actually selected speed source */
    IsoAgLib::SpeedSource_t t_speedSource;

    /** real speed */
    int32_t i32_speedReal;

    /** theoretical speed */
    int32_t i32_speedTheor;

    /** current value of the speed as determined from a number of sources by the machine */
    int32_t i32_selectedSpeed;

    /** present limit status of selected speed */
    IsoAgLib::IsoLimitFlag_t t_selectedSpeedLimitStatus;

    /** indicates the speed source that is currently being reported in the machine speed parameter */
    IsoAgLib::IsoSpeedSourceFlag_t t_selectedSpeedSource;
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
