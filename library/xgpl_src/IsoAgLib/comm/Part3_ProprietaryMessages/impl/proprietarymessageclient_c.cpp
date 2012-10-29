/*
  proprietarymessageclient_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessageclient_c.h"
#include "proprietarymessagehandler_c.h"


namespace __IsoAgLib
{
  bool ProprietaryMessageA_c::send() {

    isoaglib_assert(m_ident);

    if (getDataSend().getLen() <= 8)
    {
      CanPkgExt_c pkg;

      pkg.setIsoPri( 6 );
      pkg.setIsoDp( m_dp );
      pkg.setIsoPgn( PROPRIETARY_A_PGN );
      pkg.setISONameForDA( m_remote );
      pkg.setMonitorItemForSA( m_ident->getIsoItem() );
      pkg.setDataFromString ( getDataSend().getDataStream(), static_cast<uint8_t>( getDataSend().getLen() ) );
      getIsoBusInstance( m_ident->getMultitonInst() ) << pkg;
      return true;
    }
    else
    { /** multi-packet */
      /** variable should be evaluated */
      // const bool cb_couldStartMultiSend =
      // we could catch the information if the sending succeeded, but what to do with it anyway?
      return getMultiSendInstance( m_ident->getMultitonInst() ).sendIsoTarget(
         m_ident->isoName(),
         m_remote,
         getDataSend().getDataStream(0),
         getDataSend().getLen(),
         getDataSend().getIdent() >> 8,
         this );
    }
  }

  void ProprietaryMessageA_c::init(const IdentItem_c& ident, const IsoName_c& remote, uint8_t dp) {
    isoaglib_assert(NULL == m_ident);
    m_ident = &ident;
    m_remote = remote;
    m_dp = dp;
  }


  void ProprietaryMessageA_c::close() {
    isoaglib_assert(NULL != m_ident);
    m_ident = NULL;
    m_remote = IsoName_c::IsoNameUnspecified();
    m_dp = 0;
  }


  void ProprietaryMessageA_c::enableReception()
  {
    isoaglib_assert(NULL != m_ident);
    isoaglib_assert( ! m_isRegistered );
    getProprietaryMessageHandlerInstance( m_ident->getMultitonInst() ).registerProprietaryMessage( *this );
    m_isRegistered = true;
  }


  void ProprietaryMessageA_c::disableReception()
  {
    isoaglib_assert(NULL != m_ident);
    isoaglib_assert( m_isRegistered );
    getProprietaryMessageHandlerInstance( m_ident->getMultitonInst() ).deregisterProprietaryMessage( *this );
    m_isRegistered = false;
  }


  bool ProprietaryMessageB_c::send() {

    isoaglib_assert(m_ident);

    if (getDataSend().getLen() <= 8)
    {
      CanPkgExt_c pkg;

      pkg.setIsoPri( 6 );
      pkg.setIsoDp( m_dp );
      pkg.setIsoPgn( ( getDataSend().getIdent() << 8 ) & 0x00FFFF00  );
      pkg.setISONameForDA( m_remote );
      pkg.setMonitorItemForSA( m_ident->getIsoItem() );
      pkg.setDataFromString ( getDataSend().getDataStream(), static_cast<uint8_t>( getDataSend().getLen() ) );
      getIsoBusInstance( m_ident->getMultitonInst() ) << pkg;
      return true;
    }
    else
    { /** multi-packet */
      /** variable should be evaluated */
      // const bool cb_couldStartMultiSend =
      // we could catch the information if the sending succeeded, but what to do with it anyway?
      return getMultiSendInstance( m_ident->getMultitonInst() ).sendIsoTarget(
         m_ident->isoName(),
         m_remote,
         getDataSend().getDataStream(0),
         getDataSend().getLen(),
         getDataSend().getIdent() >> 8,
         this );
    }
  }

  void ProprietaryMessageB_c::init(const IdentItem_c& ident, const IsoName_c& remote, uint8_t dp) {
    isoaglib_assert(NULL == m_ident);
    m_ident = &ident;
    m_remote = remote;
    m_dp = dp;
  }


  void ProprietaryMessageB_c::close() {
    isoaglib_assert(NULL != m_ident);
    m_ident = NULL;
    m_remote = IsoName_c::IsoNameUnspecified();
    m_dp = 0;
  }


  void ProprietaryMessageB_c::enableReception()
  {
    isoaglib_assert(NULL != m_ident);
    isoaglib_assert( ! m_isRegistered );
    getProprietaryMessageHandlerInstance( m_ident->getMultitonInst() ).registerProprietaryMessage( *this );
    m_isRegistered = true;
  }


  void ProprietaryMessageB_c::disableReception()
  {
    isoaglib_assert(NULL != m_ident);
    isoaglib_assert( m_isRegistered );
    getProprietaryMessageHandlerInstance( m_ident->getMultitonInst() ).deregisterProprietaryMessage( *this );
    m_isRegistered = false;
  }

};
