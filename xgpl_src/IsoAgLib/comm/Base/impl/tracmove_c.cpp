/***************************************************************************
                 tracmove_c.cpp - stores, updates and delivers all moving
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
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
  #include <IsoAgLib/comm/Base/impl/tracgeneral_c.h>
#endif
#include "tracmove_c.h"

#if ( (defined USE_BASE || defined USE_TIME_GPS) && defined NMEA_2000_FAST_PACKET)
  #include <IsoAgLib/comm/Base/impl/timeposgps_c.h>
#endif


using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAglib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracMove_c& getTracMoveInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracMove_c::instance( rui8_instance );
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracMove_c& getTracMoveInstance( void )
  {
    static TracMove_c& c_lbsTracMove = TracMove_c::instance();
    return c_lbsTracMove;
  };
  #endif

  /** config the TracMove_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracMove_c::config(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode)
  {
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config(rpc_isoName, rt_identMode) ) return false;

    // set distance value to 0
    ui32_distReal = ui32_distTheor = 0;

    // set the member msg value vars to NO_VAL codes
    i32_speedReal = i32_speedTheor = NO_VAL_16;

    t_operatorDirectionReversed = IsoAgLib::IsoNotAvailableReversed;
    t_startStopState = IsoAgLib::IsoNotAvailable;

    ui32_selectedDistance = 0xFFFFFFFF;
    i32_selectedSpeed = NO_VAL_16;
    t_selectedDirection = t_directionReal = t_directionTheor = IsoAgLib::IsoNotAvailableDirection;
    t_selectedSpeedSource = IsoAgLib::IsoNotAvailableSpeed;
    t_selectedSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;

    t_speedSource = IsoAgLib::NoSpeed;
    t_distDirecSource = IsoAgLib::NoDistDirec;
    ui32_lastUpdateTimeSpeed = 3000;
    ui32_lastUpdateTimeDistDirec = 3000;

    return true;
  };

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracMove_c::checkCreateReceiveFilter()
  {
    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN GROUND_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (GROUND_BASED_SPEED_DIST_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(GROUND_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN WHEEL_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (WHEEL_BASED_SPEED_DIST_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(WHEEL_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN SELECTED_SPEED_MESSAGE, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (SELECTED_SPEED_MESSAGE << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(SELECTED_SPEED_MESSAGE) << 8), true, Ident_c::ExtendedIdent);
    }
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CANPkgExt_c::resolveSendingInformation()
    */
  bool TracMove_c::processMsg()
  {
    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    TracGeneral_c& c_tracgeneral = getTracGeneralInstance4Comm();
    #endif

    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    ISOName_c c_tempISOName( ISOName_c::ISONameUnspecified );

    // there is no need to check if sender exist in the monitor list because this is already done
    // in CANPkgExt_c -> resolveSendingInformation
    c_tempISOName = data().getISONameForSA();

    #ifdef USE_RS232_FOR_DEBUG
    INTERNAL_DEBUG_DEVICE << "c_tempISOName: " <<  static_cast<const int>(c_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "senderISOName: " <<  static_cast<const int>(getSelectedDataSourceISOName().devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "PGN:          " << (data().isoPgn() & 0x1FFFF) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    switch (data().isoPgn() & 0x1FFFF)
    {
      case GROUND_BASED_SPEED_DIST_PGN:
      case WHEEL_BASED_SPEED_DIST_PGN:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( ( checkParseReceived( c_tempISOName ) ) )/*|| noch keine normale geschw empfangen */
        { // sender is allowed to send
          int32_t i32_tempSpeed = data().getUint16Data(0);

          if ( ( (data().getUint8Data( 7 ) & 3 ) == IsoAgLib::IsoReverse) && (labs(i32_tempSpeed) <= 0xFAFF) )
            //if direction is in error state this has no effect to the speed;
            //if direction is not available it is assumed that the speed is positive
            i32_tempSpeed *= -1; //driving reverse;


          if (data().isoPgn() == GROUND_BASED_SPEED_DIST_PGN)
          { // real speed
            setSpeedReal(i32_tempSpeed);
            // real dist
            setDistReal( data().getUint32Data( 2 ) );
            t_directionReal = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7) & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            INTERNAL_DEBUG_DEVICE << "PROCESS GROUND(65097): " <<  static_cast<const int>(c_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            uint32_t tempTime = (Scheduler_c::getLastTimeEventTrigger() - ui32_lastUpdateTimeSpeed);

            //decide if ground based speed is actually the best available speed
            if ( t_speedSource <= IsoAgLib::GroundBasedSpeed && labs(i32_speedReal) <= 0xFAFF
                || (tempTime >= TIMEOUT_SENDING_NODE && tempTime < 4000)
               )
            {
              updateSpeed(IsoAgLib::GroundBasedSpeed);
            }
            //if selected speed is ground based but the speed has no valid value fall back to wheel based speed
            if (t_speedSource == IsoAgLib::GroundBasedSpeed && labs(i32_speedReal) > 0xFAFF)
              t_speedSource = IsoAgLib::WheelBasedSpeed;

            tempTime = (Scheduler_c::getLastTimeEventTrigger() - ui32_lastUpdateTimeDistDirec);
            //if ground based dist and direction is actually the best available
            if ( t_distDirecSource <= IsoAgLib::GroundBasedDistDirec && ui32_distReal <= 0xFAFFFFFF
                || (tempTime >= TIMEOUT_SENDING_NODE && tempTime < 4000)
               )
            {
              updateDistanceDirection(IsoAgLib::GroundBasedDistDirec);
            } else
              t_distDirecSource = IsoAgLib::WheelBasedDistDirec;

          }
          else
          { // wheel based speed
            setSpeedTheor(i32_tempSpeed);
            // wheel based dist
            setDistTheor( data().getUint32Data( 2 ) );
            #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
            // additionally scan for key switch and maximum power time
            c_tracgeneral.setKeySwitch(IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data( 7 ) >> 2 ) & 0x3 ));
            c_tracgeneral.setMaxPowerTime(data().getUint8Data( 6 ) );
            #endif
            t_operatorDirectionReversed = IsoAgLib::IsoOperatorDirectionFlag_t( ( data().getUint8Data(7) >> 6) & 0x3);
            t_startStopState = IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data(7) >> 4) & 0x3);
            t_directionTheor = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7)       & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            INTERNAL_DEBUG_DEVICE << "PROCESS WHEEL(65096): " <<  static_cast<const int>(c_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            if (t_speedSource <= IsoAgLib::WheelBasedSpeed
                || ( (Scheduler_c::getLastTimeEventTrigger() - ui32_lastUpdateTimeSpeed) >= TIMEOUT_SENDING_NODE)
               )
            {
              updateSpeed(IsoAgLib::WheelBasedSpeed);
            }
            if (t_distDirecSource <= IsoAgLib::WheelBasedDistDirec
                || ( (Scheduler_c::getLastTimeEventTrigger() - ui32_lastUpdateTimeDistDirec) >= TIMEOUT_SENDING_NODE)
               )
            {
              updateDistanceDirection(IsoAgLib::WheelBasedDistDirec);
            }
          }
          setSelectedDataSourceISOName(c_tempISOName);
          setUpdateTime( ci32_now );
        }
        else
        { // there is a sender conflict
          getLibErrInstance().registerError( LibErr_c::BaseSenderConflict, LibErr_c::Base );
          return false;
        }
        break;
      case SELECTED_SPEED_MESSAGE:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( ( checkParseReceived( c_tempISOName ) ) )
        {
          t_selectedSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( ( (data().getUint8Data(7) >> 5) & 0x7) );
          t_selectedDirection        = IsoAgLib::IsoDirectionFlag_t( data().getUint8Data(7) & 0x3);

          setSelectedDataSourceISOName(c_tempISOName);
          setUpdateTime( ci32_now );

          if (data().getUint16Data(0) <= 0xFAFF) //valid selected speed?
          {
            i32_selectedSpeed = data().getUint16Data(0);
            t_selectedSpeedSource =  IsoAgLib::IsoSpeedSourceFlag_t( ( (data().getUint8Data(7) >> 2) & 0x7) );
            updateSpeed(IsoAgLib::SelectedSpeed);
            if (t_selectedDirection == IsoAgLib::IsoReverse)
              i32_selectedSpeed *= -1; //driving reverse
          }
          else //fall back to ground based speed
            t_speedSource = IsoAgLib::GroundBasedSpeed;

          if (data().getUint32Data(2) <= 0xFAFFFFFF) //valid selected distance?
          {
            ui32_selectedDistance = data().getUint32Data(2);
            t_selectedDirection = IsoAgLib::IsoDirectionFlag_t(   ( (data().getUint8Data(7) >> 0) & 0x3) );
            updateDistanceDirection(IsoAgLib::SelectedDistDirec);
          } else //fall back to ground based direction and distance
            t_distDirecSource = IsoAgLib::GroundBasedDistDirec;

        } else
        { // there is a sender conflict
          getLibErrInstance().registerError( LibErr_c::BaseSenderConflict, LibErr_c::Base );
          return false;
        }
        break;
    }
    return true;
  }

  void TracMove_c::updateSpeed(IsoAgLib::SpeedSource_t t_speedSrc)
  {
    #if ( (defined USE_BASE || defined USE_TIME_GPS) && defined NMEA_2000_FAST_PACKET)
    TimePosGPS_c& c_timeposgps = getTimePosGpsInstance4Comm();
    #endif
    switch(t_speedSrc)
    {
      case IsoAgLib::SelectedSpeed:
        t_speedSource = IsoAgLib::SelectedSpeed; //nothing more to do because variables are already set
        break;
      #if ( (defined USE_BASE || defined USE_TIME_GPS) && defined NMEA_2000_FAST_PACKET)
      case IsoAgLib::GpsBasedSpeed:
        t_speedSource = IsoAgLib::GpsBasedSpeed;
        t_selectedSpeedSource = IsoAgLib::IsoNavigationBasedSpeed;
        i32_selectedSpeed = c_timeposgps.getGpsSpeedCmSec();
        break;
      #endif
      case IsoAgLib::GroundBasedSpeed:
        t_speedSource = IsoAgLib::GroundBasedSpeed;
        t_selectedSpeedSource = IsoAgLib::IsoGroundBasedSpeed;
        i32_selectedSpeed = i32_speedReal;
        break;
      case IsoAgLib::WheelBasedSpeed:
        t_speedSource = IsoAgLib::WheelBasedSpeed;
        t_selectedSpeedSource = IsoAgLib::IsoWheelBasedSpeed;
        i32_selectedSpeed = i32_speedTheor;
        break;
      default:
        t_speedSource = IsoAgLib::NoSpeed;
        t_selectedSpeedSource = IsoAgLib::IsoNotAvailableSpeed;
        break;
    }
    // update time
    ui32_lastUpdateTimeSpeed = Scheduler_c::getLastTimeEventTrigger();
  }

  void TracMove_c::updateDistanceDirection(IsoAgLib::DistanceDirectionSource_t t_distanceSrc)
  {
    //select the direction of the tractor and distance in reference to the selected speed source
    switch(t_distanceSrc)
    {
      case IsoAgLib::SelectedDistDirec:
        t_distDirecSource = IsoAgLib::SelectedDistDirec; //nothing more to do because variables are already set
        break;
      case IsoAgLib::GroundBasedDistDirec:
        t_distDirecSource = IsoAgLib::GroundBasedDistDirec;
        ui32_selectedDistance = ui32_distReal;
        t_selectedDirection = t_directionReal;
        break;
      case IsoAgLib::WheelBasedDistDirec:
        t_distDirecSource = IsoAgLib::WheelBasedDistDirec;
        ui32_selectedDistance = ui32_distTheor;
        t_selectedDirection = t_directionTheor;
        break;
      default:
        t_distDirecSource = IsoAgLib::NoDistDirec;
        break;
    }
    // update time
    ui32_lastUpdateTimeDistDirec = Scheduler_c::getLastTimeEventTrigger();
  }

  /** send a ISO11783 moving information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracMove_c::timeEventTracMode( )
  {
    if ( lastedTimeSinceUpdate()  >= 100 )
    { // it's time to send moving information
      sendMovingTracMode();
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** send moving data with ground&theor speed&dist
      @pre  function is only called in tractor mode and only from timeEventTracMode
      @see  CANIO_c::operator<<
    */
  void TracMove_c::sendMovingTracMode( )
  { // there is no need to check for address claim because this is already done in the timeEvent
    // function of base class BaseCommon_c

    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    TracGeneral_c& c_tracgeneral = getTracGeneralInstance4Comm();
    #endif
    CANIO_c& c_can = getCanInstance4Comm();

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    #ifdef USE_RS232_FOR_DEBUG
    INTERNAL_DEBUG_DEVICE << "getISOName: " <<  static_cast<const int>(getISOName()->devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "SEND TRAC senderISOName: " <<  static_cast<const int>(getSelectedDataSourceISOName().devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    setSelectedDataSourceISOName(*getISOName());

    data().setIsoPgn(GROUND_BASED_SPEED_DIST_PGN);
  #ifdef SYSTEM_PC_VC
    data().setUint16Data( 0, labs(i32_speedReal));
  #else
    uint16_t temp = CNAMESPACE::labs(i32_speedReal);
    data().setUint16Data(0, CNAMESPACE::labs(i32_speedReal));
  #endif
    data().setUint32Data(2 ,ui32_distReal);
    uint8_t b_val8 = 0;
    b_val8 |= t_directionReal;
    data().setUint8Data(7, b_val8);
    //reserved fields
    data().setUint8Data(6, 0xFF);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    data().setIsoPgn(WHEEL_BASED_SPEED_DIST_PGN);
  #ifdef SYSTEM_PC_VC
    data().setUint16Data(0, labs(i32_speedTheor));
  #else
    data().setUint16Data(0, CNAMESPACE::labs(i32_speedTheor));
  #endif
    data().setUint32Data(2, ui32_distTheor);

    b_val8 = 0;
    //data().setUint8Data(7, b_val8);
    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    // additionally scan for key switch and maximum power time
    data().setUint8Data(6, c_tracgeneral.maxPowerTime() );
    b_val8 |= (c_tracgeneral.keySwitch() << 2);
    #endif
    b_val8 |= (t_operatorDirectionReversed << 6);
    b_val8 |= (t_startStopState << 4);
    b_val8 |= t_directionTheor;
    data().setUint8Data(7, b_val8);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    data().setIsoPgn(SELECTED_SPEED_MESSAGE);
    uint8_t ui8_temp = 0;

    #ifdef SYSTEM_PC_VC
    data().setUint16Data(0, labs(i32_selectedSpeed));
    #else
    temp = CNAMESPACE::labs(i32_selectedSpeed);
    data().setUint16Data(0, CNAMESPACE::labs(i32_selectedSpeed));
    #endif
    data().setUint32Data(2, ui32_selectedDistance);
    ui8_temp |= (t_selectedSpeedLimitStatus << 5);
    ui8_temp |= (t_selectedSpeedSource      << 2);
    ui8_temp |= (t_selectedDirection        << 0);
    data().setUint8Data(7, ui8_temp);
    //reserved fields
    data().setUint8Data(6, 0xFF);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    setUpdateTime( Scheduler_c::getLastTimeEventTrigger() );
  }

} // End Namespace __IsoAglib
