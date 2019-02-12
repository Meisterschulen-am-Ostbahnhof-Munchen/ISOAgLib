/*
  tractorlighting_c.cpp: implements a lighting controller

  (C) Copyright 2017 - 2017 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorlighting_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>


namespace __IsoAgLib {

  TractorLighting_c &getTractorLightingInstance( unsigned instance )
  { // if > 1 singleton instance is used, no static reference can be used
    MACRO_MULTITON_GET_INSTANCE_BODY(TractorLighting_c, PRT_INSTANCE_CNT, instance);
  }


  void
  TractorLighting_c::init()
  {
    TractorCommonRx_c::init();

    m_ident = NULL;

    // init send values!
    mt_data.daytimeRunning       = IsoAgLib::IsoNotAvailable;
    mt_data.alternateHead        = IsoAgLib::IsoNotAvailable;
    mt_data.lowBeamHead          = IsoAgLib::IsoNotAvailable;
    mt_data.highBeamHead         = IsoAgLib::IsoNotAvailable;
    mt_data.frontFog             = IsoAgLib::IsoNotAvailable;
    mt_data.beacon               = IsoAgLib::IsoNotAvailable;
    mt_data.rightTurn            = IsoAgLib::IsoNotAvailable;
    mt_data.leftTurn             = IsoAgLib::IsoNotAvailable;

    mt_data.backUpLightAlarmHorn = IsoAgLib::IsoNotAvailable;
    mt_data.centerStop           = IsoAgLib::IsoNotAvailable;
    mt_data.rightStop            = IsoAgLib::IsoNotAvailable;
    mt_data.leftStop             = IsoAgLib::IsoNotAvailable;
    mt_data.implClearance        = IsoAgLib::IsoNotAvailable;
    mt_data.tracClearance        = IsoAgLib::IsoNotAvailable;
    mt_data.implMarker           = IsoAgLib::IsoNotAvailable;
    mt_data.tracMarker           = IsoAgLib::IsoNotAvailable;

    mt_data.rearFog              = IsoAgLib::IsoNotAvailable;
    mt_data.undersideWork        = IsoAgLib::IsoNotAvailable;
    mt_data.rearLowWork          = IsoAgLib::IsoNotAvailable;
    mt_data.rearHighWork         = IsoAgLib::IsoNotAvailable;
    mt_data.sideLowWork          = IsoAgLib::IsoNotAvailable;
    mt_data.sideHighWork         = IsoAgLib::IsoNotAvailable;
    mt_data.frontLowWork         = IsoAgLib::IsoNotAvailable;
    mt_data.frontHighWork        = IsoAgLib::IsoNotAvailable;

    mt_data.implOEMOpt2          = IsoAgLib::IsoNotAvailable;
    mt_data.implOEMOpt1          = IsoAgLib::IsoNotAvailable;
    mt_data.implRightForwardWork = IsoAgLib::IsoNotAvailable;
    mt_data.implLeftForwardWork  = IsoAgLib::IsoNotAvailable;
    mt_data.dataMsgReq           = IsoAgLib::IsoDontCare;
    mt_data.implRightFacingWork  = IsoAgLib::IsoNotAvailable;
    mt_data.implLeftFacingWork   = IsoAgLib::IsoNotAvailable;
    mt_data.implRearWork         = IsoAgLib::IsoNotAvailable;

    m_pgnRequest.init();
  }


  void
  TractorLighting_c::close()
  {
    m_pgnRequest.close();

    m_ident = NULL;

    TractorCommonRx_c::close();
  }


  bool
  TractorLighting_c::registerImplement( IdentItem_c& ident )
  {
    if( m_ident != NULL )
      return false;

    m_ident = &ident;

    return true;
  }

  
  void
  TractorLighting_c::setData( const IsoAgLib::iLighting_t& data )
  {
    mt_data = data;

    mt_data.dataMsgReq = IsoAgLib::IsoDontCare;
  }

  
  void
  TractorLighting_c::resetValues()
  {
    mt_cmd.daytimeRunning       = IsoAgLib::IsoNotAvailable;
    mt_cmd.alternateHead        = IsoAgLib::IsoNotAvailable;
    mt_cmd.lowBeamHead          = IsoAgLib::IsoNotAvailable;
    mt_cmd.highBeamHead         = IsoAgLib::IsoNotAvailable;
    mt_cmd.frontFog             = IsoAgLib::IsoNotAvailable;
    mt_cmd.beacon               = IsoAgLib::IsoNotAvailable;
    mt_cmd.rightTurn            = IsoAgLib::IsoNotAvailable;
    mt_cmd.leftTurn             = IsoAgLib::IsoNotAvailable;

    mt_cmd.backUpLightAlarmHorn = IsoAgLib::IsoNotAvailable;
    mt_cmd.centerStop           = IsoAgLib::IsoNotAvailable;
    mt_cmd.rightStop            = IsoAgLib::IsoNotAvailable;
    mt_cmd.leftStop             = IsoAgLib::IsoNotAvailable;
    mt_cmd.implClearance        = IsoAgLib::IsoNotAvailable;
    mt_cmd.tracClearance        = IsoAgLib::IsoNotAvailable;
    mt_cmd.implMarker           = IsoAgLib::IsoNotAvailable;
    mt_cmd.tracMarker           = IsoAgLib::IsoNotAvailable;

    mt_cmd.rearFog              = IsoAgLib::IsoNotAvailable;
    mt_cmd.undersideWork        = IsoAgLib::IsoNotAvailable;
    mt_cmd.rearLowWork          = IsoAgLib::IsoNotAvailable;
    mt_cmd.rearHighWork         = IsoAgLib::IsoNotAvailable;
    mt_cmd.sideLowWork          = IsoAgLib::IsoNotAvailable;
    mt_cmd.sideHighWork         = IsoAgLib::IsoNotAvailable;
    mt_cmd.frontLowWork         = IsoAgLib::IsoNotAvailable;
    mt_cmd.frontHighWork        = IsoAgLib::IsoNotAvailable;

    mt_cmd.implOEMOpt2          = IsoAgLib::IsoNotAvailable;
    mt_cmd.implOEMOpt1          = IsoAgLib::IsoNotAvailable;
    mt_cmd.implRightForwardWork = IsoAgLib::IsoNotAvailable;
    mt_cmd.implLeftForwardWork  = IsoAgLib::IsoNotAvailable;
    mt_cmd.dataMsgReq           = IsoAgLib::IsoDontCare;
    mt_cmd.implRightFacingWork  = IsoAgLib::IsoNotAvailable;
    mt_cmd.implLeftFacingWork   = IsoAgLib::IsoNotAvailable;
    mt_cmd.implRearWork         = IsoAgLib::IsoNotAvailable;
  };


  void
  TractorLighting_c::setValues( const CanPkgExt_c& pkg )
  {
    uint16_t ui16_temp = pkg.getUint16Data( 0 );
    mt_cmd.daytimeRunning       = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 );
    mt_cmd.alternateHead        = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 );
    mt_cmd.lowBeamHead          = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 );
    mt_cmd.highBeamHead         = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 );
    mt_cmd.frontFog             = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 );
    mt_cmd.beacon               = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 );
    mt_cmd.rightTurn            = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 );
    mt_cmd.leftTurn             = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 );
    ui16_temp = pkg.getUint16Data( 2 );
    mt_cmd.backUpLightAlarmHorn = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 );
    mt_cmd.centerStop           = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 );
    mt_cmd.rightStop            = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 );
    mt_cmd.leftStop             = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 );
    mt_cmd.implClearance        = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 );
    mt_cmd.tracClearance        = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 );
    mt_cmd.implMarker           = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 );
    mt_cmd.tracMarker           = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 );
    ui16_temp = pkg.getUint16Data( 4 );
    mt_cmd.rearFog              = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 );
    mt_cmd.undersideWork        = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 );
    mt_cmd.rearLowWork          = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 );
    mt_cmd.rearHighWork         = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 );
    mt_cmd.sideLowWork          = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 );
    mt_cmd.sideHighWork         = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 );
    mt_cmd.frontLowWork         = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 );
    mt_cmd.frontHighWork        = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 );
    ui16_temp = pkg.getUint16Data( 6 );
    mt_cmd.implOEMOpt2          = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 );
    mt_cmd.implOEMOpt1          = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 );
    mt_cmd.implRightForwardWork = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 );
    mt_cmd.implLeftForwardWork  = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 );
    mt_cmd.dataMsgReq           = IsoAgLib::IsoDataReq_t(    (ui16_temp >>  8) & 3 );
    mt_cmd.implRightFacingWork  = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 );
    mt_cmd.implLeftFacingWork   = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 );
    mt_cmd.implRearWork         = IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 );
  }


  void
  TractorLighting_c::sendData()
  {
    isoaglib_assert( m_ident != NULL );
    if( m_ident->getIsoItem() == NULL )
      return;
  
    CanPkgExt_c pkg;
    pkg.setMonitorItemForSA( m_ident->getIsoItem() );
    pkg.setIsoPri( 6 );
    pkg.setIsoPgn( LIGHTING_DATA_PGN );
    pkg.setLen( 8 );

    uint16_t ui16_temp =
      (mt_cmd.daytimeRunning <<  0) +
      (mt_cmd.alternateHead  <<  2) +
      (mt_cmd.lowBeamHead    <<  4) +
      (mt_cmd.highBeamHead   <<  6) +
      (mt_cmd.frontFog       <<  8) +
      (mt_cmd.beacon         << 10) +
      (mt_cmd.rightTurn      << 12) +
      (mt_cmd.leftTurn       << 14);
    pkg.setUint16Data(0, ui16_temp);

    ui16_temp =
      (mt_cmd.backUpLightAlarmHorn <<  0) +
      (mt_cmd.centerStop           <<  2) +
      (mt_cmd.rightStop            <<  4) +
      (mt_cmd.leftStop             <<  6) +
      (mt_cmd.implClearance        <<  8) +
      (mt_cmd.tracClearance        << 10) +
      (mt_cmd.implMarker           << 12) +
      (mt_cmd.tracMarker           << 14);
    pkg.setUint16Data(2, ui16_temp);

    ui16_temp =
      (mt_cmd.rearFog       <<  0) +
      (mt_cmd.undersideWork <<  2) +
      (mt_cmd.rearLowWork   <<  4) +
      (mt_cmd.rearHighWork  <<  6) +
      (mt_cmd.sideLowWork   <<  8) +
      (mt_cmd.sideHighWork  << 10) +
      (mt_cmd.frontLowWork  << 12) +
      (mt_cmd.frontHighWork << 14);
    pkg.setUint16Data(4, ui16_temp);

    ui16_temp =
      (mt_cmd.implOEMOpt2          <<  0) +
      (mt_cmd.implOEMOpt1          <<  2) +
      (mt_cmd.implRightForwardWork <<  4) +
      (mt_cmd.implLeftForwardWork  <<  6) +
      (0                           <<  8) +    //reserved field in lighting data
      (mt_cmd.implRightFacingWork  << 10) +
      (mt_cmd.implLeftFacingWork   << 12) +
      (mt_cmd.implRearWork         << 14);
    pkg.setUint16Data(6, ui16_temp);

    getIsoBusInstance4Comm() << pkg;
  }


  void
  TractorLighting_c::IsoRequestPgnHandlerProxy_c::init()
  {
    getIsoRequestPgnInstance( mrt_owner.getMultitonInst() ).registerPGN ( *this, LIGHTING_DATA_PGN );
  }

  void
  TractorLighting_c::IsoRequestPgnHandlerProxy_c::close()
  {
    getIsoRequestPgnInstance( mrt_owner.getMultitonInst() ).unregisterPGN ( *this, LIGHTING_DATA_PGN );
  }

  bool
  TractorLighting_c::IsoRequestPgnHandlerProxy_c::processMsgRequestPGN( uint32_t pgn, IsoItem_c *sender, IsoItem_c *receiver, ecutime_t time )
  {
    isoaglib_assert( pgn == LIGHTING_DATA_PGN );

    ( void )pgn;    
    ( void )sender;
    ( void )time;

    if( ( mrt_owner.m_ident != NULL ) && ( mrt_owner.m_ident->getIsoItem() != NULL ) )
    {
      if( ( receiver == NULL ) || ( receiver == mrt_owner.m_ident->getIsoItem() ) )
      {
        mrt_owner.sendData();
        return true;
      }
    }
    return false;
  }


} // __IsoAglib
