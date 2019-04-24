/*
  tractor_c.cpp: 

  (C) Copyright 2016 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractor_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>


namespace __IsoAgLib {

  Tractor_c &getTractorInstance( unsigned instance )
  { // if > 1 singleton instance is used, no static reference can be used
    MACRO_MULTITON_GET_INSTANCE_BODY(Tractor_c, PRT_INSTANCE_CNT, instance);
  }


  void
  Tractor_c::init()
  {
    TractorCommonRx_c::init();

    m_ident = NULL;

    mt_maintainEcuPower      = IsoAgLib::IsoNotAvailable;
    mt_maintainActuatorPower = IsoAgLib::IsoNotAvailable;
    
    m_implTransportFlag   = IsoAgLib::IsoNotAvailableTransport;
    m_implParkFlag        = IsoAgLib::IsoNotAvailablePark;
    m_implReadyToWorkFlag = IsoAgLib::IsoNotAvailableWork;
    m_implInWorkFlag      = IsoAgLib::IsoNotAvailableInWork;

    m_pgnRequest.init();
  }


  void
  Tractor_c::close()
  {
    m_pgnRequest.close();

    m_ident = NULL;

    TractorCommonRx_c::close();
  }


  bool
  Tractor_c::registerImplement( IdentItem_c& ident )
  {
    if( m_ident != NULL )
      return false;

    m_ident = &ident;

    m_ident->getDiagnosticFunctionalities().addFunctionalitiesBasicTractorECU(
        true /* implement */,
        1 /* version */,
        BasicTractorECUOptionsBitMask_t() );
    
    return true;
  }


  void
  Tractor_c::resetValues()
  {
    mui16_speed    = NO_VAL_16;
    mui32_distance = NO_VAL_32;
       
    mui8_maxPowerTime = NO_VAL_8;
    
    mt_operatorDirection = IsoAgLib::IsoNotAvailableReversed;
    mt_masterSwitch      = IsoAgLib::IsoNotAvailable;
    mt_keySwitch         = IsoAgLib::IsoNotAvailable;
    mt_direction         = IsoAgLib::IsoNotAvailableDirection;
  };


  void
  Tractor_c::setValues( const CanPkgExt_c& pkg )
  {
    mui16_speed    = pkg.getUint16Data( 0 );
    mui32_distance = pkg.getUint32Data( 2 );

    mui8_maxPowerTime = pkg.getUint8Data( 6 );

    mt_operatorDirection = IsoAgLib::IsoOperatorDirectionFlag_t( ( pkg.getUint8Data( 7 ) >> 6 ) & 0x03 );
    mt_masterSwitch      = IsoAgLib::IsoActiveFlag_t           ( ( pkg.getUint8Data( 7 ) >> 4 ) & 0x03 );
    mt_keySwitch         = IsoAgLib::IsoActiveFlag_t           ( ( pkg.getUint8Data( 7 ) >> 2 ) & 0x03 );
    mt_direction         = IsoAgLib::IsoDirectionFlag_t        (   pkg.getUint8Data( 7 )        & 0x03 );
  }


  void
  Tractor_c::sendMaintainPower()
  {
    isoaglib_assert( m_ident != NULL );

    CanPkgExt_c pkg;
    pkg.setMonitorItemForSA( m_ident->getIsoItem() );
    pkg.setIsoPri( 6 );
    pkg.setIsoPgn( MAINTAIN_POWER_REQUEST_PGN );
    pkg.setUint8Data(0, ( mt_maintainEcuPower      << 6) |
                        ( mt_maintainActuatorPower << 4) );
    pkg.setUint8Data(1, ( m_implTransportFlag   << 6) |
                        ( m_implParkFlag        << 4) |
                        ( m_implReadyToWorkFlag << 2) |
                        ( m_implInWorkFlag          ) );
    pkg.setUint16Data(2, 0xFFFFU);
    pkg.setUint32Data(4, 0xFFFFFFFFUL);
    pkg.setLen(8);
    getIsoBusInstance4Comm() << pkg;
  }


  void
  Tractor_c::maintainPowerAndSendImplStates( IsoAgLib::IsoActiveFlag_t ecuPower, IsoAgLib::IsoActiveFlag_t actuatorPower )
  {
    isoaglib_assert( m_ident != NULL );

    mt_maintainEcuPower      = ecuPower;
    mt_maintainActuatorPower = actuatorPower;

    sendMaintainPower();
  }

  void
  Tractor_c::IsoRequestPgnHandlerProxy_c::init()
  {
    getIsoRequestPgnInstance( mrt_owner.getMultitonInst() ).registerPGN ( *this, MAINTAIN_POWER_REQUEST_PGN );
  }

  void
  Tractor_c::IsoRequestPgnHandlerProxy_c::close()
  {
    getIsoRequestPgnInstance( mrt_owner.getMultitonInst() ).unregisterPGN ( *this, MAINTAIN_POWER_REQUEST_PGN );
  }

  bool
  Tractor_c::IsoRequestPgnHandlerProxy_c::processMsgRequestPGN( uint32_t pgn, IsoItem_c *sender, IsoItem_c *receiver, ecutime_t time )
  {
    isoaglib_assert( pgn == MAINTAIN_POWER_REQUEST_PGN );
    
    ( void )pgn;    
    ( void )sender;
    ( void )time;

    if( ( mrt_owner.m_ident != NULL ) && ( mrt_owner.m_ident->getIsoItem() != NULL ) )
    {
      if( ( receiver == NULL ) || ( receiver == mrt_owner.m_ident->getIsoItem() ) )
      {
        mrt_owner.sendMaintainPower();
        return true;
      }
    }
    return false;
  }


} // __IsoAglib
