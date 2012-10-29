/*
  proprietarymessagehandler_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessagehandler_c.h"
#include "proprietarymessageclient_c.h"
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib
{
  ProprietaryMessageHandler_c::ProprietaryMessageHandler_c()
    : m_customerA( *this )
    , m_customerB( *this )
  {
  }


  void
  ProprietaryMessageHandler_c::init()
  {
    isoaglib_assert (!initialized());
    setInitialized();
  }


  void
  ProprietaryMessageHandler_c::close()
  {
    isoaglib_assert (initialized());
    setClosed();
  }


  void ProprietaryMessageHandler_c::registerProprietaryMessage( ProprietaryMessageA_c& msg ) {
    isoaglib_assert(msg.m_ident);
    if( m_customerA.m_msgs.empty() ) {
      getCanInstance4Comm().insertFilter( m_customerA, m_customerA.m_filter, -1 );
      getMultiReceiveInstance4Comm().registerClientIso ( m_customerA,
                                                         msg.m_ident->isoName(),
                                                         m_customerA.m_filter.getFilter() >> 8,
                                                         m_customerA.m_filter.getMask() >> 8,
                                                         true /* also Broadcast */ );
    }
    m_customerA.m_msgs.push_front( &msg );
  }


  void ProprietaryMessageHandler_c::deregisterProprietaryMessage( ProprietaryMessageA_c& msg ) {

    for ( CanCustomerA_c::ProprietaryMessageAVectorIterator_t it = m_customerA.m_msgs.begin(); it != m_customerA.m_msgs.end(); ++it ) {
      if( *it == &msg ) {
        m_customerA.m_msgs.erase( it );
        break;
      }
    }

    if( m_customerA.m_msgs.empty() ) {
      getCanInstance4Comm().deleteFilter( m_customerA, m_customerA.m_filter );
      isoaglib_assert(msg.m_ident);
      getMultiReceiveInstance4Comm().deregisterClient ( m_customerA,
                                                        msg.m_ident->isoName(),
                                                        m_customerA.m_filter.getFilter() >> 8,
                                                        m_customerA.m_filter.getMask() >> 8 );
    }
  }


  void ProprietaryMessageHandler_c::registerProprietaryMessage( ProprietaryMessageB_c& msg ) {

    if( m_customerB.m_msgs.empty() ) {
      getCanInstance4Comm().insertFilter( m_customerB, m_customerA.m_filter, -1 );
      isoaglib_assert(msg.m_ident);
      getMultiReceiveInstance4Comm().registerClientIso ( m_customerB,
                                                         msg.m_ident->isoName(),
                                                         m_customerB.m_filter.getFilter() >> 8,
                                                         m_customerB.m_filter.getMask() >> 8,
                                                         true /* also Broadcast */ );
    }
    m_customerB.m_msgs.push_front( &msg );
  }


  void ProprietaryMessageHandler_c::deregisterProprietaryMessage( ProprietaryMessageB_c& msg ) {

    for ( CanCustomerB_c::ProprietaryMessageBVectorIterator_t it = m_customerB.m_msgs.begin(); it != m_customerB.m_msgs.end(); ++it ) {
      if( *it == &msg ) {
        m_customerB.m_msgs.erase( it );
        break;
      }
    }

    if( m_customerB.m_msgs.empty() ) {
      getCanInstance4Comm().deleteFilter( m_customerB, m_customerB.m_filter );
      isoaglib_assert(msg.m_ident);
      getMultiReceiveInstance4Comm().deregisterClient ( m_customerB,
                                                        msg.m_ident->isoName(),
                                                        m_customerB.m_filter.getFilter() >> 8,
                                                        m_customerB.m_filter.getMask() >> 8 );
    }
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerA_c::processMsg( const CanPkg_c& data )
  {
    CanPkgExt_c pkg( data, m_handler.getMultitonInst() );
    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return true;

    for ( ProprietaryMessageAVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( pkg.getMonitorItemForDA() != NULL ) && ( pkg.getMonitorItemForDA() != (*it)->m_ident->getIsoItem() ) )
        continue;

      if ( pkg.isoDp() != (*it)->m_dp )
        continue;

      if ( (*it)->m_remote.isSpecified() && ( pkg.getISONameForSA() != (*it)->m_remote ) )
        continue;

      (*it)->getDataReceive().clearVector();
      (*it)->getDataReceive().setDataStream( 0, pkg.getUint8DataConstPointer(), pkg.getLen() );
      // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
      //(*it)->processA( *static_cast<IsoAgLib::iIsoItem_c*>( pkg.getMonitorItemForSA() ) );
      (*it)->processA( *( (IsoAgLib::iIsoItem_c*)( pkg.getMonitorItemForSA() ) ) );
    }

    return true;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerA_c::reactOnStreamStart( const ReceiveStreamIdentifier_c &ident, uint32_t )
  {

    for ( ProprietaryMessageAVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {

      if ( ( ident.getDaIsoName().isSpecified() ) && ( ident.getDaIsoName() != (*it)->m_ident->isoName() ) )
        continue;

      if ( ( ident.getPgn() >> 16 ) != (*it)->m_dp ) // DP
        continue;

      if ( (*it)->m_remote.isSpecified() && ( ident.getSaIsoName() != (*it)->m_remote ) )
        continue;

      return true;
    }

    return false;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerA_c::processPartStreamDataChunk( Stream_c &apc_stream, bool, bool last )
  {
    if( ! last ) {
      return false;
    }

    // get the ident from the stream
    const ReceiveStreamIdentifier_c& ident = apc_stream.getIdent();


    ProprietaryMessageAVector_t msgs;

    for ( ProprietaryMessageAVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( ident.getDaIsoName().isSpecified() ) && ( ident.getDaIsoName() != (*it)->m_ident->isoName() ) )
        continue;

      if ( ( ident.getPgn() >> 16 ) != (*it)->m_dp ) // DP
        continue;

      if ( (*it)->m_remote.isSpecified() && ( ident.getSaIsoName() != (*it)->m_remote ) )
        continue;

      (*it)->getDataReceive().setIdent( (ident.getPgn() << 8) | ( ident.getDa() << 8) | ident.getSa() );
      (*it)->getDataReceive().clearVector();
      msgs.push_front( *it );
    }


    if( ! msgs.empty() ) {

      uint8_t db = apc_stream.getFirstByte();
      for ( unsigned cnt = 0; cnt < apc_stream.getByteTotalSize(); cnt++)
      {

        for( ProprietaryMessageAVectorIterator_t it = msgs.begin(); it != msgs.end(); ++it ) {
          (*it)->getDataReceive().setDataUi8( cnt, db );
        }
        db = apc_stream.getNextNotParsed();
      }

      for( ProprietaryMessageAVectorIterator_t it = msgs.begin(); it != msgs.end(); ++it ) {
        // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
        //(*it)->processA( *static_cast<IsoAgLib::iIsoItem_c*>( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
        (*it)->processA( * ( IsoAgLib::iIsoItem_c*)( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
      }
    }
    // don't keep the stream - we processed it right now!
    return false;

  }


  bool
  ProprietaryMessageHandler_c::CanCustomerB_c::processMsg( const CanPkg_c& data )
  {
    CanPkgExt_c pkg( data, m_handler.getMultitonInst() );
    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return true;

    for ( ProprietaryMessageBVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( pkg.isoDp() != (*it)->m_dp )
        continue;

      if ( (*it)->m_remote.isSpecified() && ( pkg.getISONameForSA() != (*it)->m_remote ) )
        continue;

      (*it)->getDataReceive().clearVector();
      (*it)->getDataReceive().setDataStream( 0, pkg.getUint8DataConstPointer(), pkg.getLen() );

      // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
      //(*it)->processB( *static_cast<IsoAgLib::iIsoItem_c*>( pkg.getMonitorItemForSA() ) );
      (*it)->processB( *( ( IsoAgLib::iIsoItem_c*)( pkg.getMonitorItemForSA() ) ) );
    }

    return true;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerB_c::reactOnStreamStart( const ReceiveStreamIdentifier_c &ident, uint32_t )
  {

    for ( ProprietaryMessageBVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( ident.getPgn() >> 16 ) != (*it)->m_dp ) // DP
        continue;

      if ( (*it)->m_remote.isSpecified() && ( ident.getSaIsoName() != (*it)->m_remote ) )
        continue;

      return true;
    }

    return false;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerB_c::processPartStreamDataChunk( Stream_c &apc_stream, bool, bool last )
  {
    if( ! last ) {
      return false;
    }

    // get the ident from the stream
    const ReceiveStreamIdentifier_c& ident = apc_stream.getIdent();


    ProprietaryMessageBVector_t msgs;

    for ( ProprietaryMessageBVectorIterator_t it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( ident.getPgn() >> 16 ) != (*it)->m_dp ) // DP
        continue;

      if ( (*it)->m_remote.isSpecified() && ( ident.getSaIsoName() != (*it)->m_remote ) )
        continue;

      (*it)->getDataReceive().setIdent( (ident.getPgn() << 8) | ( ident.getDa() << 8) | ident.getSa() );
      (*it)->getDataReceive().clearVector();
      msgs.push_front( *it );
    }


    if( ! msgs.empty() ) {

      uint8_t db = apc_stream.getFirstByte();
      for ( unsigned cnt = 0; cnt < apc_stream.getByteTotalSize(); cnt++)
      {

        for( ProprietaryMessageBVectorIterator_t it = msgs.begin(); it != msgs.end(); ++it ) {
          (*it)->getDataReceive().setDataUi8( cnt, db );
        }
        db = apc_stream.getNextNotParsed();
      }

      for( ProprietaryMessageBVectorIterator_t it = msgs.begin(); it != msgs.end(); ++it ) {
        // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
        //(*it)->processB( *static_cast<IsoAgLib::iIsoItem_c*>( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
        (*it)->processB( *( IsoAgLib::iIsoItem_c*)( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
      }
    }
    // don't keep the stream - we processed it right now!
    return false;

  }


  ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance(uint8_t aui8_instance)
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(ProprietaryMessageHandler_c, PRT_INSTANCE_CNT, aui8_instance);
  }

} // namespace __IsoAgLib
