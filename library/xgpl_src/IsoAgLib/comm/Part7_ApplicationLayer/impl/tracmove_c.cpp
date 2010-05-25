/*
  tracmove_c.cpp: stores, updates and delivers all moving data
    information from CanCustomer_c derived for CAN sending and
    receiving interaction; from BaseCommon_c derived for interaction
    with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracgeneral_c.h>
#endif
#include "tracmove_c.h"

#include <IsoAgLib/util/impl/util_funcs.h>

#if ( (defined USE_BASE || defined USE_TIME_GPS) && defined ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.h>
#endif
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

using namespace std;

#define TIMEOUT_SPEED_LOST 3000
#define MISSING_SPEED         0

namespace __IsoAgLib { // Begin Namespace __IsoAglib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracMove_c& getTracMoveInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracMove_c::instance( aui8_instance );
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracMove_c& getTracMoveInstance( void )
  {
    static TracMove_c& c_lbsTracMove = TracMove_c::instance();
    return c_lbsTracMove;
  };
  #endif

/** initialize directly after the singleton instance is created.
  this is called from singleton.h and should NOT be called from the user again.
  users please use init(...) instead.
*/
void TracMove_c::singletonInit()
{ // singletonInit is called, AFTER the initializing instance() function has assigned a suitable
  // singleton vec key - this key value is NOT available at construction time!!!
  BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY);
}


  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracMove_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

  /** config the TracMove_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracMove_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    ///Set time Period for Scheduler_c
    if (at_identMode == IsoAgLib::IdentModeTractor)
    { // SEND data with short period
      setTimePeriod( (uint16_t) 100);
    }
    else
    { // check with long period for timeout after loss of sending node
      setTimePeriod( (uint16_t) 1000 );
    }

    // un-/register to PGN
    if (t_oldMode == at_identMode)
      ; // no change, still the same mode
    else if (at_identMode == IsoAgLib::IdentModeTractor) {
      // a change from Implement mode to Tractor mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, LANGUAGE_PGN
      RegisterPgn_s s_register = getRegisterPgn();
      if (canSendGroundBasedSpeedDist())
        s_register(GROUND_BASED_SPEED_DIST_PGN);
      if (canSendWheelBasedSpeedDist())
        s_register(WHEEL_BASED_SPEED_DIST_PGN);
      if (canSendSelectedSpeed())
        s_register(SELECTED_SPEED_MESSAGE);
    } else {
      // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      UnregisterPgn_s s_unregister = getUnregisterPgn();
      if (canSendGroundBasedSpeedDist())
        s_unregister(GROUND_BASED_SPEED_DIST_PGN);
      if (canSendWheelBasedSpeedDist())
        s_unregister(WHEEL_BASED_SPEED_DIST_PGN);
      if (canSendSelectedSpeed())
        s_unregister(SELECTED_SPEED_MESSAGE);
    }

    // set distance value to NO_VAL codes
    mui32_distReal = mui32_distTheor = NO_VAL_32;

    // set the member msg value vars to NO_VAL codes
    mi32_speedReal = mi32_speedTheor = NO_VAL_32S;

    mt_operatorDirectionReversed = IsoAgLib::IsoNotAvailableReversed;
    mt_startStopState = IsoAgLib::IsoNotAvailable;

    mui32_selectedDistance = 0xFFFFFFFF;
    mi32_selectedSpeed = NO_VAL_32S;
    mt_selectedDirection = mt_directionReal = mt_directionTheor = IsoAgLib::IsoNotAvailableDirection;
    mt_selectedSpeedSource = IsoAgLib::IsoNotAvailableSpeed;
    mt_selectedSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;

    mt_speedSource = IsoAgLib::NoSpeed;
    mt_distDirecSource = IsoAgLib::NoDistDirec;
    mui32_lastUpdateTimeSpeed = 3000;
    mui32_lastUpdateTimeDistDirec = 3000;

    return true;
  };

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false
    */
  bool TracMove_c::isSelectedSpeedUsable() const
  {
    if ( isSelectedSpeedMissing() ) return false;
    else if ( isSelectedSpeedErroneous() ) return false;
    else return true;
  }

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false
    */
  bool TracMove_c::isRealSpeedUsable() const
  {
    if ( isRealSpeedMissing() ) return false;
    else if ( isRealSpeedErroneous() ) return false;
    else return true;
  }

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false
    */
  bool TracMove_c::isTheorSpeedUsable() const
  {
    if ( isTheorSpeedMissing() ) return false;
    else if ( isTheorSpeedErroneous() ) return false;
    else return true;
  }

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracMove_c::checkCreateReceiveFilter()
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN GROUND_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (GROUND_BASED_SPEED_DIST_PGN << 8)
      c_can.insertStandardIsoFilter(*this,GROUND_BASED_SPEED_DIST_PGN,false);

      // create FilterBox_c for PGN WHEEL_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (WHEEL_BASED_SPEED_DIST_PGN << 8)
      c_can.insertStandardIsoFilter(*this,WHEEL_BASED_SPEED_DIST_PGN,false);

      // create FilterBox_c for PGN SELECTED_SPEED_MESSAGE, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (SELECTED_SPEED_MESSAGE << 8)
      c_can.insertStandardIsoFilter(*this,SELECTED_SPEED_MESSAGE,true);

    }
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracMove_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    TracGeneral_c& c_tracgeneral = getTracGeneralInstance4Comm();
    #endif

    const int32_t ci32_now = data().time();

    #ifdef USE_RS232_FOR_DEBUG
    INTERNAL_DEBUG_DEVICE << "c_tempISOName: " <<  static_cast<const int>(rcc_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "senderISOName: " <<  static_cast<const int>(getSelectedDataSourceISOName().devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "PGN:          " << (data().isoPgn() /*& 0x1FFFF*/) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    switch (data().isoPgn() /*& 0x3FFFF*/) // don't need to &0x3FFFF as this is the whole PGN...
    {
      case GROUND_BASED_SPEED_DIST_PGN:
      case WHEEL_BASED_SPEED_DIST_PGN:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( ( checkParseReceived( rcc_tempISOName ) ) )/*|| noch keine normale geschw empfangen */
        { // sender is allowed to send
          int32_t i32_tempSpeed = data().getUint16Data(0);
          bool b_usableSpeed = true;
          uint32_t ui32_tempDist = data().getUint32Data( 2 );

          // handle special values
          switch (data().getUint16Data(0))
          {
            case NO_VAL_16: // missing signal --> match to NO_VAL_32S
              i32_tempSpeed = NO_VAL_32S;
              b_usableSpeed = false;
              break;
            case ERROR_VAL_16: // erroneous signal --> match to ERROR_VAL_32S
              i32_tempSpeed = ERROR_VAL_32S;
              b_usableSpeed = false;
              break;
            default: // handle too high value as erroneous signal
              if ( i32_tempSpeed > 0xFAFF ) {
                i32_tempSpeed = ERROR_VAL_32S;
                b_usableSpeed = false;
              }
              break;
          }

          // if distance value is outside valid area, the special error state flag-value ERROR_VAL_32 should be used
          // but if NO_VAL_32 indicates missing information, this information should NOT be overwritten
          if ( ( ui32_tempDist > 0xFAFFFFFF ) && ( ui32_tempDist != NO_VAL_32 ) ) ui32_tempDist = ERROR_VAL_32;

          if ( ( (data().getUint8Data( 7 ) & 3 ) == IsoAgLib::IsoReverse) && (b_usableSpeed) )
            //if direction is in error state this has no effect to the speed;
            //if direction is not available it is assumed that the speed is positive
            i32_tempSpeed *= -1; //driving reverse;


          const uint32_t testTimeOutdatedSpeed = (data().time() - mui32_lastUpdateTimeSpeed);
          const uint32_t testTimeOutdatedDist  = (data().time() - mui32_lastUpdateTimeDistDirec);
          if (data().isoPgn() == GROUND_BASED_SPEED_DIST_PGN)
          { // real speed
            setSpeedReal(i32_tempSpeed);
            // real dist
            setDistReal( ui32_tempDist );
            mt_directionReal = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7) & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            INTERNAL_DEBUG_DEVICE << "PROCESS GROUND(65097): " <<  static_cast<const int>(rcc_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif

            //decide if ground based speed is actually the best available speed
            if ( ( b_usableSpeed ) &&
                 ( ( mt_speedSource <= IsoAgLib::GroundBasedSpeed )
                || ( testTimeOutdatedSpeed >= TIMEOUT_SENDING_NODE && testTimeOutdatedSpeed < 4000 )
                 )
               )
            { // speed information is usable and the current selected speed is at least not better or outdated
              updateSpeed(IsoAgLib::GroundBasedSpeed);
            }

            //if ground based dist and direction is actually the best available
            if ( ( mui32_distReal <= 0xFAFFFFFF ) &&
                 ( ( mt_distDirecSource <= IsoAgLib::GroundBasedDistDirec )
                || ( testTimeOutdatedDist >= TIMEOUT_SENDING_NODE && testTimeOutdatedDist < 4000 )
                 )
               )
            { // distance information is usable and the current selected distance is at least not better or outdated
              updateDistanceDirection(IsoAgLib::GroundBasedDistDirec);
            }

          }
          else
          { // wheel based speed
            setSpeedTheor(i32_tempSpeed);
            // wheel based dist
            setDistTheor( ui32_tempDist );
            #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
            // additionally scan for key switch and maximum power time
            c_tracgeneral.setKeySwitch(IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data( 7 ) >> 2 ) & 0x3 ));
            c_tracgeneral.setMaxPowerTime(data().getUint8Data( 6 ) );
            #endif
            mt_operatorDirectionReversed = IsoAgLib::IsoOperatorDirectionFlag_t( ( data().getUint8Data(7) >> 6) & 0x3);
            mt_startStopState = IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data(7) >> 4) & 0x3);
            mt_directionTheor = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7)       & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            INTERNAL_DEBUG_DEVICE << "PROCESS WHEEL(65096): " <<  static_cast<const int>(rcc_tempISOName.devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            if ( ( b_usableSpeed ) &&
                 ( ( mt_speedSource <= IsoAgLib::WheelBasedSpeed )
                || ( testTimeOutdatedSpeed >= TIMEOUT_SENDING_NODE && testTimeOutdatedSpeed < 4000 )
                 )
               )
            { // speed information is usable and the current selected speed is at least not better or outdated
              updateSpeed(IsoAgLib::WheelBasedSpeed);
            }
            if ( ( mui32_distReal <= 0xFAFFFFFF ) &&
                 ( ( mt_distDirecSource <= IsoAgLib::WheelBasedDistDirec )
                || ( testTimeOutdatedDist >= TIMEOUT_SENDING_NODE && testTimeOutdatedDist < 4000 )
                 )
               )
            { // distance information is usable and the current selected distance is at least not better or outdated
              updateDistanceDirection(IsoAgLib::WheelBasedDistDirec);
            }
          }
          setUpdateTime( ci32_now );
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false;
        }
        break;
      case SELECTED_SPEED_MESSAGE:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender

        // first check whether the received message indicates a valid speed source - and ignore the message totally,
        // if speedSource == IsoNotAvailableSpeed
        const IsoAgLib::IsoSpeedSourceFlag_t t_testSpeedSource = IsoAgLib::IsoSpeedSourceFlag_t( ( (data().getUint8Data(7) >> 2) & 0x7) );

        if ( ( ( checkParseReceived( rcc_tempISOName ) ) ) && (t_testSpeedSource != IsoAgLib::IsoNotAvailableSpeed))
        {
          mt_selectedSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( ( (data().getUint8Data(7) >> 5) & 0x7) );
          mt_selectedDirection        = IsoAgLib::IsoDirectionFlag_t( data().getUint8Data(7) & 0x3);

          setSelectedDataSourceISOName(rcc_tempISOName);
          setUpdateTime( ci32_now );

          if (data().getUint16Data(0) <= 0xFAFF) //valid selected speed?
          {
            mi32_selectedSpeed = data().getUint16Data(0);
            mt_selectedSpeedSource =  t_testSpeedSource;
            updateSpeed(IsoAgLib::SelectedSpeed);
            if (mt_selectedDirection == IsoAgLib::IsoReverse)
              mi32_selectedSpeed *= -1; //driving reverse
          }
          else //fall back to ground based speed
            mt_speedSource = IsoAgLib::GroundBasedSpeed;

          if (data().getUint32Data(2) <= 0xFAFFFFFF) //valid selected distance?
          {
            mui32_selectedDistance = data().getUint32Data(2);
            mt_distDirecSource = IsoAgLib::SelectedDistDirec;
            mt_selectedDirection = IsoAgLib::IsoDirectionFlag_t(   ( (data().getUint8Data(7) >> 0) & 0x3) );
            updateDistanceDirection(IsoAgLib::SelectedDistDirec);
          } else //fall back to ground based direction and distance
            mt_distDirecSource = IsoAgLib::GroundBasedDistDirec;

        } else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false;
        }
        break;
    }
    return true;
  }

  void TracMove_c::updateSpeed(IsoAgLib::SpeedSource_t t_speedSrc)
  {
    #if ( (defined USE_BASE || defined USE_TIME_GPS) && defined ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
    TimePosGps_c& c_timeposgps = getTimePosGpsInstance4Comm();
    #endif
    switch(t_speedSrc)
    {
      case IsoAgLib::SelectedSpeed:
        mt_speedSource = IsoAgLib::SelectedSpeed; //nothing more to do because variables are already set
        break;
      #if ( (defined USE_BASE || defined USE_TIME_GPS) && defined ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
      case IsoAgLib::GpsBasedSpeed:
        mt_speedSource = IsoAgLib::GpsBasedSpeed;
        mt_selectedSpeedSource = IsoAgLib::IsoNavigationBasedSpeed;
        mi32_selectedSpeed = c_timeposgps.getGpsSpeedCmSec()*10; // Selected Speed is in mm/s
        break;
      #endif
      case IsoAgLib::GroundBasedSpeed:
        mt_speedSource = IsoAgLib::GroundBasedSpeed;
        mt_selectedSpeedSource = IsoAgLib::IsoGroundBasedSpeed;
        mi32_selectedSpeed = mi32_speedReal;
        break;
      case IsoAgLib::WheelBasedSpeed:
        mt_speedSource = IsoAgLib::WheelBasedSpeed;
        mt_selectedSpeedSource = IsoAgLib::IsoWheelBasedSpeed;
        mi32_selectedSpeed = mi32_speedTheor;
        break;
      default:
        mt_speedSource = IsoAgLib::NoSpeed;
        mt_selectedSpeedSource = IsoAgLib::IsoNotAvailableSpeed;
        break;
    }
    // update time
    mui32_lastUpdateTimeSpeed = data().time();
  }

  void TracMove_c::updateDistanceDirection(IsoAgLib::DistanceDirectionSource_t t_distanceSrc)
  {
    //select the direction of the tractor and distance in reference to the selected speed source
    switch(t_distanceSrc)
    {
      case IsoAgLib::SelectedDistDirec:
        mt_distDirecSource = IsoAgLib::SelectedDistDirec; //nothing more to do because variables are already set
        break;
      case IsoAgLib::GroundBasedDistDirec:
        mt_distDirecSource = IsoAgLib::GroundBasedDistDirec;
        mui32_selectedDistance = mui32_distReal;
        mt_selectedDirection = mt_directionReal;
        break;
      case IsoAgLib::WheelBasedDistDirec:
        mt_distDirecSource = IsoAgLib::WheelBasedDistDirec;
        mui32_selectedDistance = mui32_distTheor;
        mt_selectedDirection = mt_directionTheor;
        break;
      default:
        mt_distDirecSource = IsoAgLib::NoDistDirec;
        break;
    }
    /// update time -> System Time caused by call of interface class
    mui32_lastUpdateTimeDistDirec = System_c::getTime();
  }

  /** Detect stop of Speed update from tractor
      @see  BaseCommon_c::timeEvent()
    */
  bool TracMove_c::timeEventImplMode()
  {
    const int32_t ci32_now = getLastRetriggerTime();
    // checking for timeout of speed update
    if ( ( (ci32_now - mui32_lastUpdateTimeSpeed)  >= TIMEOUT_SPEED_LOST || getSelectedDataSourceISONameConst().isUnspecified()  )
      && ( !isSelectedSpeedMissing() ) )
    { // TECU stoppped its Speed and doesn't send speed updates - as defined by ISO 11783
      // --> switch value of selected speed to ZERO
      mi32_selectedSpeed = NO_VAL_32S;
      setSelectedSpeed( mi32_selectedSpeed );
    }
    if ( ( (ci32_now - mui32_lastUpdateTimeSpeed)  >= TIMEOUT_SPEED_LOST || getSelectedDataSourceISONameConst().isUnspecified()  )
      && ( !isRealSpeedMissing() ) )
    { // TECU stoppped its Speed and doesn't send speed updates - as defined by ISO 11783
      // --> switch value of selected speed to ZERO
      mi32_speedReal = NO_VAL_32S;
      setSpeedReal( mi32_speedReal );
    }
    if ( ( (ci32_now - mui32_lastUpdateTimeSpeed)  >= TIMEOUT_SPEED_LOST || getSelectedDataSourceISONameConst().isUnspecified()  )
      && ( !isTheorSpeedMissing() ) )
    { // TECU stoppped its Speed and doesn't send speed updates - as defined by ISO 11783
      // --> switch value of selected speed to ZERO
      mi32_speedTheor = NO_VAL_32S;
      setSpeedTheor( mi32_speedTheor );
    }
    return true;
  }

  /** send a ISO11783 moving information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracMove_c::timeEventTracMode( )
  { ///Timeperiod of 100ms is set in ::config
    sendMovingTracMode();
    return true;
  }

  void TracMove_c::prepareSending()
  {
    // there is no need to check for address claim because this is already done in the timeEvent
    // function of base class BaseCommon_c

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    #ifdef USE_RS232_FOR_DEBUG
    INTERNAL_DEBUG_DEVICE << "getISOName: " <<  static_cast<const int>(getISOName()->devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "SEND TRAC senderISOName: " <<  static_cast<const int>(getSelectedDataSourceISOName().devClass() ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    setSelectedDataSourceISOName(*getISOName());
  }

  TracMove_c::SendMessage_e TracMove_c::sendGroundBasedSpeedDist()
  {
    if (!canSendGroundBasedSpeedDist())
      return MessageNotSent;
    prepareSending();
    data().setIsoPgn(GROUND_BASED_SPEED_DIST_PGN);
    data().setUint16Data( 0, __IsoAgLib::abs(mi32_speedReal));
    data().setUint32Data(2, mui32_distReal);

    uint8_t b_val8 = 0;
    b_val8 |= mt_directionReal;
    data().setUint8Data(7, b_val8);
    //reserved fields
    data().setUint8Data(6, 0xFF);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
    return MessageSent;
  }

  TracMove_c::SendMessage_e TracMove_c::sendWheelBasedSpeedDist()
  {
    if (!canSendWheelBasedSpeedDist())
      return MessageNotSent;
    prepareSending();
    data().setIsoPgn(WHEEL_BASED_SPEED_DIST_PGN);
    data().setUint16Data(0, __IsoAgLib::abs(mi32_speedTheor));
    data().setUint32Data(2, mui32_distTheor);

    uint8_t b_val8 = 0;
    //data().setUint8Data(7, b_val8);
#if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    TracGeneral_c& c_tracgeneral = getTracGeneralInstance4Comm();
    // additionally scan for key switch and maximum power time
    data().setUint8Data(6, c_tracgeneral.maxPowerTime() );
    b_val8 |= (c_tracgeneral.keySwitch() << 2);
#endif
    b_val8 |= (mt_operatorDirectionReversed << 6);
    b_val8 |= (mt_startStopState << 4);
    b_val8 |= mt_directionTheor;
    data().setUint8Data(7, b_val8);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
    return MessageSent;
  }

  TracMove_c::SendMessage_e TracMove_c::sendSelectedSpeed()
  {
    if (!canSendSelectedSpeed())
      return MessageNotSent;
    prepareSending();
    data().setIsoPgn(SELECTED_SPEED_MESSAGE);
    uint8_t ui8_temp = 0;

    data().setUint16Data(0, __IsoAgLib::abs(mi32_selectedSpeed));
    data().setUint32Data(2, mui32_selectedDistance);
    ui8_temp |= (mt_selectedSpeedLimitStatus << 5);
    ui8_temp |= (mt_selectedSpeedSource      << 2);
    ui8_temp |= (mt_selectedDirection        << 0);
    data().setUint8Data(7, ui8_temp);
    //reserved fields
    data().setUint8Data(6, 0xFF);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
    return MessageSent;
  }
  
  TracMove_c::SendMessage_e TracMove_c::sendEngineSpeed()
  {
    if (!canSendEngineSpeed())
      return MessageNotSent;
    
    data().setIsoPgn(ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE);

    // TODO unimplemented Engine Torque mode
    data().setUint16Data(0, 0xFF);
    // TODO Driver's demand engine - Percentage torque
    data().setUint8Data(1, 0xFF);
    // TODO Actual Engine - Percentage torque
    data().setUint8Data(2, 0xFF);
    // Engine speed
    data().setUint16Data(3, mui16_engineSpeed);
    // Source adresse of controlling deice for engine.
    data().setUint8Data(5, 0xFF);
    // Engine starter mode
    data().setUint8Data(6, 0xFF);
    // Engine demand
    data().setUint8Data(7, 0xFF);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
    return MessageSent;
  }

  /** send moving data with ground&theor speed&dist
      @pre  function is only called in tractor mode and only from timeEventTracMode
      @see  CanIo_c::operator<<
    */
  void TracMove_c::sendMovingTracMode( )
  {
    (void)sendGroundBasedSpeedDist();
    (void)sendWheelBasedSpeedDist();
    (void)sendSelectedSpeed();
  (void)sendEngineSpeed();
  }


///  Used for Debugging Tasks in Scheduler_c
const char*
TracMove_c::getTaskName() const
{   return "TracMove_c"; }

bool TracMove_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
{
  // check if we are allowed to send a request for pgn
  if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) )
    return false;

  bool b_processed = true;
  switch (aui32_pgn) {
  default:
    b_processed = false;
    break;
  case GROUND_BASED_SPEED_DIST_PGN:
    sendGroundBasedSpeedDist();
    break;
  case WHEEL_BASED_SPEED_DIST_PGN:
    sendWheelBasedSpeedDist();
    break;
  case SELECTED_SPEED_MESSAGE:
    sendSelectedSpeed();
    break;
  }
  return b_processed;
}

} // End Namespace __IsoAglib
