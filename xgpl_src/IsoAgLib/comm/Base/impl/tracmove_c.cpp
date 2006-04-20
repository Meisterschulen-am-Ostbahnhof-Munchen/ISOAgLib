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
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/comm/Base/itracgeneral_c.h>
#include "tracmove_c.h"

//#define USE_RS232_FOR_DEBUG
#ifdef USE_RS232_FOR_DEBUG
  #include <supplementary_driver/driver/rs232/irs232io_c.h>
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

  /** config the TracMove_c object after init -> set pointer to devKey and
      config send/receive of a moving msg type
      @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracMove_c::config(const DevKey_c* rpc_devKey, const IsoAgLib::IdentMode_t rt_identMode)
  {
    //call config for handling which is base data independent
    BaseCommon_c::config(rpc_devKey, rt_identMode);

    // set the member msg value vars to NO_VAL codes
    i32_speedReal = i32_speedTheor = 0xFFFF;
    // set distance value to 0
    i32_lastDistReal = i32_lastDistTheor = 0;
    i32_distReal = i32_distTheor = 0;

    #ifdef USE_ISO_11783
    i32_lastMsgReceivedCmd = 0;
    t_operatorDirectionReversed = IsoAgLib::IsoNotAvailableReversed;
    t_startStopState = IsoAgLib::IsoNotAvailable;

    ui32_selectedDistance = 0;
    i32_selectedSpeed = 0xFFFF;
    t_selectedDirection = t_directionReal = t_directionTheor = IsoAgLib::IsoNotAvailableDirection;
    t_selectedDirectionCmd = IsoAgLib::IsoNotAvailableDirection;
    t_selectedSpeedSource = IsoAgLib::IsoNotAvailableSpeed;
    t_selectedSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;
    i16_selectedSpeedSetPointCmd = 0;
    i16_selectedSpeedSetPointLimit = 0;
    #endif
  };

  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracMove_c::checkCreateReceiveFilter()
  {
    SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();
    #ifdef USE_DIN_9684
    if ( ( !checkDinFilterCreated() ) && (c_systemMgmt.existActiveLocalDinMember() ) )
    { // check if needed receive filters for DIN are active
      setDinFilterCreated();
      // filter for base data 1
      c_can.insertFilter(*this, (0x7F << 4),(0x14 << 4), true);
    }
    #endif
    #ifdef USE_ISO_11783
    if ( ( !checkIsoFilterCreated() ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setIsoFilterCreated();
      // create FilterBox_c for PGN GROUND_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (GROUND_BASED_SPEED_DIST_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(GROUND_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN WHEEL_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (WHEEL_BASED_SPEED_DIST_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(WHEEL_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN SELECTED_SPEED_CMDClientBase, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (SELECTED_SPEED_CMD << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(SELECTED_SPEED_CMD) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN SELECTED_SPEED_MESSAGE, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (SELECTED_SPEED_MESSAGE << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(SELECTED_SPEED_MESSAGE) << 8), true, Ident_c::ExtendedIdent);
    }
    #endif
  }

  #ifdef USE_DIN_9684
  /** send a DIN9684 moving information PGN
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracMove_c::dinTimeEventTracMode()
  {
    CANIO_c& c_can = getCanInstance4Comm();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_send = getDinMonitorInstance4Comm().dinMemberDevKey(*getDevKey(), true).nr();

    if ( lastedTimeSinceUpdate() >= 100)
    { // send actual moving data
      setSelectedDataSourceDevKey( *getDevKey() );
      data().setIdent((0x14<<4 | b_send), __IsoAgLib::Ident_c::StandardIdent );
      data().setUint16Data(0, i32_speedReal);
      data().setUint16Data(2, i32_speedTheor);
      data().setUint16Data(4, i32_distReal);
      data().setUint16Data(6, i32_distTheor);
      data().setLen(8);

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      setUpdateTime( Scheduler_c::getLastTimeEventTrigger() );
    }
    if (Scheduler_c::getAvailableExecTime() == 0) return false;

    return true;
  }

  /** process a DIN9684 moving information PGN */
  bool TracMove_c::dinProcessMsg()
  { // a DIN9684 moving information msg received
    // store the devKey of the sender of moving data
    if (! getDinMonitorInstance4Comm().existDinMemberNr(data().dinSa()))
    { // the sender is not known -> ignore and block interpretation by other classes
      return true;
    }
    // the corresponding sender entry exist in the monitor list
    DevKey_c c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().dinSa()).devKey();

    // interprete data accordingto BABO
    switch (dataBabo())
    {
      case 4: //speed, dist
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempDevKey ) )
        { // sender is allowed to send
          // real speed
          setSpeedReal(data().getUint16Data( 0 ));
          // theor speed
          setSpeedTheor(data().getUint16Data( 2 ));
          // real dist -> react on 16bit int16_t overflow
          setOverflowSecure(i32_distReal, i32_lastDistReal, data().getUint16Data( 4 ));
          // theor dist -> react on 16bit int16_t overflow
          setOverflowSecure(i32_distTheor, i32_lastDistTheor, data().getUint16Data( 6 ));

          setSelectedDataSourceDevKey(c_tempDevKey);
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
          return false;
        }
        break;
    } // end switch
    return true;
  }
  #endif

  #ifdef USE_ISO_11783
  /**
    process a ISO11783 moving information PGN
  */
  bool TracMove_c::isoProcessMsg()
  {
    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    IsoAgLib::iTracGeneral_c& c_tracgeneral = IsoAgLib::getITracGeneralInstance();
    #endif
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    // store the devKey of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
    }

    #ifdef USE_RS232_FOR_DEBUG
    EXTERNAL_DEBUG_DEVICE << "c_tempDevKey: " <<  static_cast<const int>(c_tempDevKey.getDevClass() ) << "\n";
    EXTERNAL_DEBUG_DEVICE << "senderDevKey: " <<  static_cast<const int>(getSelectedDataSourceDevKey().getDevClass() ) << "\n";
    EXTERNAL_DEBUG_DEVICE << "PGN:          " << (data().isoPgn() & 0x1FFFF) << "\n";
    #endif

    switch (data().isoPgn() & 0x1FFFF)
    {
      case GROUND_BASED_SPEED_DIST_PGN:
      case WHEEL_BASED_SPEED_DIST_PGN:
      case SELECTED_SPEED_MESSAGE:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( ( checkParseReceived( c_tempDevKey ) ) )/*|| noch keine normale geschw empfangen */
        { // sender is allowed to send
          int32_t i32_tempSpeed = data().getUint16Data(0);
          switch (data().getUint8Data( 7 ) & 3 ) {
          case 0: // driving reverse
            if ( (i32_tempSpeed != ERROR_VAL_16S)
              && (i32_tempSpeed !=  0xFFFF) ) i32_tempSpeed *= -1;
            break;
          case 1: // driving forward
            break;
          case 2: // ERROR
            i32_tempSpeed = ERROR_VAL_16S;
            break;
          case 3: // not available
            i32_tempSpeed = NO_VAL_16S;
            break;
          }

          if (data().isoPgn() == GROUND_BASED_SPEED_DIST_PGN)
          { // real speed
            setSpeedReal(i32_tempSpeed);
            // real dist
            setDistReal( static_cast<int32_t>(data().getUint32Data( 2 ) ));
            t_directionReal = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7) & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            EXTERNAL_DEBUG_DEVICE << "PROCESS GROUND(65097): " <<  static_cast<const int>(c_tempDevKey.getDevClass() ) << "\n";
            #endif
          }
          else if (data().isoPgn() == WHEEL_BASED_SPEED_DIST_PGN)
          { // wheel based speed
            setSpeedTheor(i32_tempSpeed);
            // wheel based dist
            setDistTheor( static_cast<int32_t>(data().getUint32Data( 2 )) );
            #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
            // additionally scan for key switch and maximum power time
            c_tracgeneral.setKeySwitch(IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data( 7 ) >> 2 ) & 0x3 ));
            c_tracgeneral.setMaxPowerTime(data().getUint8Data( 6 ) );
            #endif
            t_operatorDirectionReversed = IsoAgLib::IsoOperatorDirectionFlag_t( ( data().getUint8Data(7) >> 6) & 0x3);
            t_startStopState = IsoAgLib::IsoActiveFlag_t( ( data().getUint8Data(7) >> 4) & 0x3);
            t_directionTheor = IsoAgLib::IsoDirectionFlag_t(data().getUint8Data(7)       & 0x3 );
            #ifdef USE_RS232_FOR_DEBUG
            EXTERNAL_DEBUG_DEVICE << "PROCESS WHEEL(65096): " <<  static_cast<const int>(c_tempDevKey.getDevClass() ) << "\n";
            #endif
          } else
          { //SELECTED_SPEED_MESSAGE
            i32_selectedSpeed = i32_tempSpeed;
            ui32_selectedDistance =                                         data().getUint16Data(2);
            t_selectedSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t(       ( (data().getUint8Data(7) >> 5) & 0x7) );
            t_selectedSpeedSource =      IsoAgLib::IsoSpeedSourceFlag_t( ( (data().getUint8Data(7) >> 2) & 0x7) );
            t_selectedDirection =        IsoAgLib::IsoDirectionFlag_t(   ( (data().getUint8Data(7) >> 0) & 0x3) );
          }
          setSelectedDataSourceDevKey(c_tempDevKey);
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
          return false;
        }
        break;
      case SELECTED_SPEED_CMD:
        if ( checkMode(IsoAgLib::IdentModeTractor) )
        {
          i16_selectedSpeedSetPointCmd =   data().getUint16Data(0);
          i16_selectedSpeedSetPointLimit = data().getUint16Data(2);
          t_selectedDirectionCmd = IsoAgLib::IsoDirectionFlag_t( (data().getUint8Data(7) & 0x3) );
          #ifdef USE_RS232_FOR_DEBUG
          EXTERNAL_DEBUG_DEVICE << "PROCESS SPEEDCMD(65104): " <<  static_cast<const int>(c_tempDevKey.getDevClass() ) << "\n";
          #endif
        }
        break;
    }
    return true;
  }

  /** send a ISO11783 moving information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracMove_c::isoTimeEventTracMode( )
  {
    if ( lastedTimeSinceUpdate()  >= 100 )
    { // it's time to send moving information
      isoSendMovingTracMode();
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** send a ISO11783 moving information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracMove_c::isoTimeEventImplMode( )
  {
    if ( lastedTimeSinceUpdateCmd()  >= 100 )
    { // it's time to send moving information
     isoSendMovingImplMode();
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

/** send moving data with ground&theor speed&dist */
  void TracMove_c::isoSendMovingImplMode( )
  { // send actual base1 data: ground/wheel based speed/dist
    if (!getIsoMonitorInstance4Comm().existIsoMemberDevKey(*getDevKey(), true)) return;

    CANIO_c& c_can = getCanInstance4Comm();

    #ifdef USE_RS232_FOR_DEBUG
    EXTERNAL_DEBUG_DEVICE << "SEND IMPL senderDevKey: " <<  static_cast<const int>(getSelectedDataSourceDevKey().getDevClass() ) << "\n";
    #endif

    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*getDevKey(), true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);
    data().setLen(8);

    data().setIsoPgn(SELECTED_SPEED_CMD);
    uint8_t ui8_temp = 0;
    data().setUint16Data(0, i16_selectedSpeedSetPointCmd);
    data().setUint16Data(2, i16_selectedSpeedSetPointLimit);
    ui8_temp |= t_selectedDirectionCmd;
    data().setUint16Data(7, ui8_temp);
    //reserved fields
    data().setUint16Data(4, 0);
    data().setUint8Data(6, 0);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    setUpdateTimeCmd( Scheduler_c::getLastTimeEventTrigger() );
  }

  /** send moving data with ground&theor speed&dist */
  void TracMove_c::isoSendMovingTracMode( )
  { // send actual base1 data: ground/wheel based speed/dist
    if (!getIsoMonitorInstance4Comm().existIsoMemberDevKey(*getDevKey(), true)) return;

    #if defined(USE_BASE) || defined(USE_TRACTOR_GENERAL)
    IsoAgLib::iTracGeneral_c& c_tracgeneral = IsoAgLib::getITracGeneralInstance();
    #endif
    CANIO_c& c_can = getCanInstance4Comm();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*getDevKey(), true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);
    data().setLen(8);

    #ifdef USE_RS232_FOR_DEBUG
    EXTERNAL_DEBUG_DEVICE << "SEND TRAC senderDevKey: " <<  static_cast<const int>(getSelectedDataSourceDevKey().getDevClass() ) << "\n";
    #endif

    setSelectedDataSourceDevKey(*getDevKey());

    data().setIsoPgn(GROUND_BASED_SPEED_DIST_PGN);
  #ifdef SYSTEM_PC_VC
    data().setUint16Data( 0, abs(i32_speedReal));
  #else
    uint16_t temp = CNAMESPACE::abs(i32_speedReal);
    data().setUint16Data(0, CNAMESPACE::abs(i32_speedReal));
  #endif
    data().setUint32Data(2 ,i32_distReal);
    uint8_t b_val8 = 0;
    switch (i32_speedReal) {
      case ERROR_VAL_16S:
        t_directionReal = IsoAgLib::IsoErrorDirection;
        break;
      case  0xFFFF:
        t_directionReal = IsoAgLib::IsoNotAvailableDirection;
        break;
      default:
        if (i32_speedReal < 0) t_directionReal = IsoAgLib::IsoReverse;
        else t_directionReal = IsoAgLib::IsoForward;
        break;
    }
    b_val8 |= t_directionReal;
    data().setUint8Data(7, b_val8);
    //reserved fields
    data().setUint8Data(6, 0);

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
    c_can << data();

    data().setIsoPgn(WHEEL_BASED_SPEED_DIST_PGN);
  #ifdef SYSTEM_PC_VC
    data().setUint16Data(0, abs(i32_speedTheor));
  #else
    data().setUint16Data(0, CNAMESPACE::abs(i32_speedTheor));
  #endif
    data().setUint32Data(2, i32_distTheor);

    b_val8 = 0;
    switch (i32_speedTheor) {
      case ERROR_VAL_16S:
        t_directionTheor = IsoAgLib::IsoErrorDirection;
        break;
      case 0xFFFF:
        t_directionTheor = IsoAgLib::IsoNotAvailableDirection;
        break;
      default:
        if (i32_speedTheor < 0)
          t_directionTheor = IsoAgLib::IsoReverse;
        else
          t_directionTheor = IsoAgLib::IsoForward;
        break;
    }
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

    // select best available speed and store which is actually the best available speed source
    if ( CNAMESPACE::abs(i32_speedReal) <= 0xFAFF)
    { //ground based speed
      t_selectedSpeedSource = IsoAgLib::IsoGroundBasedSpeed;
      i32_selectedSpeed = i32_speedReal;
    }
    else
    { //wheel based speed
      t_selectedSpeedSource = IsoAgLib::IsoWheelBasedSpeed;
      i32_selectedSpeed = i32_speedTheor;
    }

    //select the direction of the tractor and distance in reference to the selected speed source
    switch(t_selectedSpeedSource)
    {
      case IsoAgLib::IsoWheelBasedSpeed:
        t_selectedDirection = t_directionTheor;
        ui32_selectedDistance = i32_distTheor;
        break;
      case IsoAgLib::IsoGroundBasedSpeed:
        t_selectedDirection = t_directionReal;
        ui32_selectedDistance = i32_distReal;
        break;
      case IsoAgLib::IsoNavigationBasedSpeed:
        //t_selectedDirection = ;
        break;
      case IsoAgLib::IsoBlendedSpeed:
        //t_selectedDirection = ;
        break;
      case IsoAgLib::IsoSimulatedSpeed:
        //t_selectedDirection = ;
        break;
      case 7:
        t_selectedDirection = IsoAgLib::IsoNotAvailableDirection;
        break;
    }

    #ifdef SYSTEM_PC_VC
    data().setUint16Data(0, abs(i32_selectedSpeed));
    #else
    temp = CNAMESPACE::abs(i32_selectedSpeed);
    data().setUint16Data(0, CNAMESPACE::abs(i32_selectedSpeed));
    #endif
    data().setUint32Data(2, ui32_selectedDistance);
    ui8_temp |= (t_selectedSpeedLimitStatus << 5);
    ui8_temp |= (t_selectedSpeedSource      << 2);
    ui8_temp |= (t_selectedDirection        << 0);
    data().setUint8Data(7, ui8_temp);
    //reserved fields
    data().setUint8Data(6, 0);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    setUpdateTime( Scheduler_c::getLastTimeEventTrigger() );
  }
  #endif

  #ifdef USE_DIN_9684
  /** update distance values ; react on int16_t overflow
      @param reflVal to be updated value as int32_t variable
      @param refiVal to be updated value as 16bit int16_t variable
      @param rrefiNewVal new value given as reference to 16bit int
    */
  void TracMove_c::setOverflowSecure(int32_t& reflVal, int16_t& refiVal, const int16_t& rrefiNewVal)
  {
    // definition area [0..i32_maxVal] with overflow
    // 10000 of Fendt; 32767 of DIN
    const int32_t i32_maxDefFendt = 10000,
                  i32_maxDefDin = 32767;
    int32_t i32_newValFendt = reflVal,
            i32_newValDin = reflVal;
    int16_t i16_diff = rrefiNewVal - refiVal;

    // check if there was an overflow = diff is greater than half of def area (per sign side)
    #ifdef SYSTEM_PC_VC
    if ((abs(i16_diff) > i32_maxDefFendt/2) || (abs(i16_diff) > i32_maxDefDin/2))
    { // one of the overflow checks triggers
      if (abs(i16_diff) > i32_maxDefFendt/2)
    #else
    if ((CNAMESPACE::abs(i16_diff) > i32_maxDefFendt/2) || (CNAMESPACE::abs(i16_diff) > i32_maxDefDin/2))
    { // one of the overflow checks triggers
      if (CNAMESPACE::abs(i16_diff) > i32_maxDefFendt/2)
    #endif
      { // the old wrong fendt limit triggers
        if (rrefiNewVal > refiVal)
        { // dist decreased lower than 0 -> lower underflow
          i32_newValFendt -= refiVal; // max reducable before underflow
          i32_newValFendt -= (i32_maxDefFendt - rrefiNewVal); // decreased after underflow (jump to int16 max pos val)
        }
        else
        { // dist increased upper than 0x7FFF -> overflow
          i32_newValFendt += (i32_maxDefFendt - refiVal); // max decrease before overflow
          i32_newValFendt += rrefiNewVal; // decreased after overflow
        }
      }
      #ifdef SYSTEM_PC_VC
      if (abs(i16_diff) > i32_maxDefDin/2)
      #else
      if (CNAMESPACE::abs(i16_diff) > i32_maxDefDin/2)
      #endif
    { // the correct DIN limit triggers
        if (rrefiNewVal > refiVal)
        { // dist decreased lower than 0 -> lower underflow
          i32_newValDin -= refiVal; // max reducable before underflow
          i32_newValDin -= (i32_maxDefDin - rrefiNewVal); // decreased after underflow (jump to int16 max pos val)
        }
        else
        { // dist increased upper than 0x7FFF -> overflow
          i32_newValDin += (i32_maxDefDin - refiVal); // max decrease before overflow
          i32_newValDin += rrefiNewVal; // decreased after overflow
        }
      }
      // check, which new value should be used -> take the value which mean less distance to old val
      if (CNAMESPACE::labs(i32_newValFendt - reflVal) < CNAMESPACE::labs(i32_newValDin - reflVal))
      {
        reflVal = i32_newValFendt;
      }
      else
      {
        reflVal = i32_newValDin;
      }
    }
    else
    { // there's no overflow -> easy increment reflVal
      reflVal += i16_diff;
    }
    // store the given int16 val as last int16 val in refiVal
    refiVal = rrefiNewVal;
  };
  #endif

  /** set the real (radar measured) driven distance with int32_t val
      @param rreflVal value to store as real radar measured distance
    */
  void TracMove_c::setDistReal(const int32_t& rreflVal)
  {
    i32_distReal = rreflVal;
    // set the int16_t value regarding the overflow counting
    i32_lastDistReal = long2int(rreflVal);
  };

  /** set the theoretical (gear calculated) driven distance with int32_t val
      @param rreflVal value to store as theoretical (gear calculated) driven distance
    */
  void TracMove_c::setDistTheor(const int32_t& rreflVal)
  {
    i32_distTheor = rreflVal;
    // set the int16_t value regarding the overflow counting
    i32_lastDistTheor = long2int(rreflVal);
  };

  /** get int16_t overflowed val from long
      @param rreflVal value as int32_t (32bit) variable
      @return 16bit int16_t calculated with counting overflow from 32767 to (-32766)
    */
  int16_t TracMove_c::long2int(const int32_t& rreflVal)
  {
    // Version mit [0..10000] und Uberlauf a la Fendt Vario
    //int32_t i32_mod =  rreflVal % 10000;
    // Version mit [0..32767] und Uberlauf a la Scheduler_c DIN 9684
    int32_t i32_mod =  rreflVal % 32767;
    return i32_mod;
  };
} // End Namespace __IsoAglib
