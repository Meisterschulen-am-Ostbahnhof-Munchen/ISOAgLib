/*
  proprietarymessagehandler_c.cpp

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessagehandler_c.h"
#include "proprietarymessageclient_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>


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


  // Register A / B
  /////////////////

  void ProprietaryMessageHandler_c::registerProprietaryMessage( ProprietaryMessageA_c& msg )
  {
    isoaglib_assert( initialized() ); // most likely module was not configured!

    m_customerA.m_msgs.push_front( &msg );

    m_customerA.registerFilter( getMultitonInst(), msg.ident()->isoName() );
  }


  void ProprietaryMessageHandler_c::registerProprietaryMessage( ProprietaryMessageB_c& msg, uint8_t ps )
  {
    isoaglib_assert(initialized()); // most likely module was not configured!

    if( m_customerB.m_msgs[ ps ] == NULL )
    {
      m_customerB.m_msgs[ ps ] = new CanCustomerB_c::MsgList();
    }

    m_customerB.m_msgs[ ps ]->push_front( &msg );

    m_customerB.registerFilter( getMultitonInst(), msg.ident()->isoName() );
  }


  // Deregister A / B
  ///////////////////

  void ProprietaryMessageHandler_c::deregisterProprietaryMessage( ProprietaryMessageA_c& msg )
  {
    for ( CanCustomerA_c::MsgIterator it = m_customerA.m_msgs.begin(); it != m_customerA.m_msgs.end(); ++it )
    {
      if( *it == &msg )
      {
        m_customerA.m_msgs.erase( it );
        break;
      }
    }

    m_customerA.deregisterFilter( getMultitonInst(), msg.ident()->isoName() );
  }


  void ProprietaryMessageHandler_c::deregisterProprietaryMessage( ProprietaryMessageB_c& msg, uint8_t ps )
  {
    isoaglib_assert( m_customerB.m_msgs[ ps ] != NULL );

    for( CanCustomerB_c::MsgIterator it = m_customerB.m_msgs[ ps ]->begin(); it != m_customerB.m_msgs[ ps ]->end(); ++it )
    {
      if( *it == &msg )
      {
        m_customerB.m_msgs[ ps ]->erase( it );

        if( m_customerB.m_msgs[ ps ]->empty() )
        {
          delete m_customerB.m_msgs[ ps ];
          m_customerB.m_msgs[ ps ] = NULL;
        }
        break;
      }
    }

    m_customerB.deregisterFilter( getMultitonInst(), msg.ident()->isoName() );
  }


  // CanCustomerAB_c/CanCustomerA_c/CanCustomerB_c
  ////////////////////////////////////////////////

  void
  ProprietaryMessageHandler_c::CanCustomerAB_c::registerFilter( int multitonInst, const IsoName_c& identName )
  {
    if( mmap_registeredMsgs.empty() )
    {
      getCanInstance( multitonInst ).insertFilter( *this, m_filter, -1 );
    }

    if( mmap_registeredMsgs.find( identName ) != mmap_registeredMsgs.end() )
    {
      mmap_registeredMsgs[ identName ] += 1;
    }
    else
    {
      mmap_registeredMsgs[ identName ] = 1;

      getMultiReceiveInstance( multitonInst ).registerClientIso(
          *this, identName, m_filter.getFilter() >> 8, m_filter.getMask() >> 8, true );
    }
  }


  void
  ProprietaryMessageHandler_c::CanCustomerAB_c::deregisterFilter( int multitonInst, const IsoName_c& identName )
  {
    if( mmap_registeredMsgs[ identName ] > 1 )
    {
      mmap_registeredMsgs[ identName ] -= 1;
    }
    else
    {
      mmap_registeredMsgs.erase( identName );

      getMultiReceiveInstance( multitonInst ).deregisterClient(
          *this, identName, m_filter.getFilter() >> 8, m_filter.getMask() >> 8 );
    }

    if( mmap_registeredMsgs.empty() )
    {
      getCanInstance( multitonInst ).deleteFilter( *this, m_filter );
    }
  }



  void 
  ProprietaryMessageHandler_c::CanCustomerA_c::processMsg( const CanPkg_c& data )
  {
    CanPkgExt_c pkg( data, m_handler.getMultitonInst() );
    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return;

    for ( MsgIterator it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( pkg.getMonitorItemForDA() != NULL ) && ( pkg.getMonitorItemForDA() != (*it)->ident()->getIsoItem() ) )
        continue;

      if ( pkg.isoDp() != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( pkg.getISONameForSA() != (*it)->remote() ) )
        continue;

      (*it)->getDataReceive().clearVector();
      (*it)->getDataReceive().setDataStream( 0, pkg.getUint8DataConstPointer(), pkg.getLen() );
      // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
      //(*it)->processA( *static_cast<IsoAgLib::iIsoItem_c*>( pkg.getMonitorItemForSA() ) );
      (*it)->processA( *( (IsoAgLib::iIsoItem_c*)( pkg.getMonitorItemForSA() ) ),
                       (NULL == pkg.getMonitorItemForDA()) /* a_broadcast */);
    }
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerA_c::reactOnStreamStart( const ReceiveStreamIdentifier_c &ident, uint32_t len )
  {
    // currently only supports up to 0xFFFE byte, because most internal setters are uint16_t only
    if ( len >= 0xFFFF )
      return false;

    for ( MsgIterator it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( ident.getDaIsoName().isSpecified() ) && ( ident.getDaIsoName() != (*it)->ident()->isoName() ) )
        continue;

      if ( ( ident.getPgn() >> 16 ) != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( ident.getSaIsoName() != (*it)->remote() ) )
        continue;

      return true;
    }

    return false;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerA_c::processPartStreamDataChunk( Stream_c &apc_stream, bool, bool last )
  {
    if( ! last )
      return false;

    MsgList msgs;

    const ReceiveStreamIdentifier_c& ident = apc_stream.getIdent();
    for ( MsgIterator it = m_msgs.begin(); it != m_msgs.end(); ++it )
    {
      if ( ( ident.getDaIsoName().isSpecified() ) && ( ident.getDaIsoName() != (*it)->ident()->isoName() ) )
        continue;

      if ( ( ident.getPgn() >> 16 ) != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( ident.getSaIsoName() != (*it)->remote() ) )
        continue;

      (*it)->getDataReceive().clearVector();
      msgs.push_front( *it );
    }

    if( ! msgs.empty() )
    {
      uint16_t cnt=0;

      if (apc_stream.getIdent().getDa() == 0xFF)
      { // was BAM, where the first byte was not set already!
        // let all be read by loop below, no special handling for first byte
      }
      else
      { // directed, first byte was filled out, get it in a special way!
          uint8_t db = apc_stream.getFirstByte();
          for (; cnt < 1; ++cnt)
          {
              for (MsgIterator it = msgs.begin(); it != msgs.end(); ++it) {
                  (*it)->getDataReceive().setDataUi8(cnt, db);
              }
          }
      }

      for (; cnt < apc_stream.getByteTotalSize(); ++cnt)
      {
        uint8_t db = apc_stream.getNextNotParsed();
        for( MsgIterator it = msgs.begin(); it != msgs.end(); ++it ) {
          (*it)->getDataReceive().setDataUi8( cnt, db );
        }
      }

      for( MsgIterator it = msgs.begin(); it != msgs.end(); ++it ) {
        // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
        //(*it)->processA( *static_cast<IsoAgLib::iIsoItem_c*>( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
        (*it)->processA( * ( IsoAgLib::iIsoItem_c*)( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ),
                         (ident.getDaIsoName().isUnspecified()) /* a_broadcast */);
      }
    }

    // don't keep the stream - we processed it right now!
    return false;
  }
  

  void 
  ProprietaryMessageHandler_c::CanCustomerB_c::processMsg( const CanPkg_c& data )
  {
    CanPkgExt_c pkg( data, m_handler.getMultitonInst() );
    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return;

    const uint8_t ps = pkg.isoPs();
    if( m_msgs[ ps ] == NULL )
      return;

    for ( MsgIterator it = m_msgs[ ps ]->begin(); it != m_msgs[ ps ]->end(); ++it )
    {
      if ( pkg.isoDp() != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( pkg.getISONameForSA() != (*it)->remote() ) )
        continue;

      (*it)->getDataReceive().clearVector();
      (*it)->getDataReceive().setDataStream( 0, pkg.getUint8DataConstPointer(), pkg.getLen() );

      // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
      //(*it)->processB( *static_cast<IsoAgLib::iIsoItem_c*>( pkg.getMonitorItemForSA() ) );
      (*it)->processB( *( ( IsoAgLib::iIsoItem_c*)( pkg.getMonitorItemForSA() ) ), ps );
    }
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerB_c::reactOnStreamStart( const ReceiveStreamIdentifier_c &ident, uint32_t len )
  {
    // currently only supports up to 0xFFFE byte, because most internal setters are uint16_t only
    if (len >= 0xFFFF)
      return false;

    const uint8_t ps = uint8_t( ident.getPgn() & 0xFF );
    const uint8_t dp = uint8_t( ident.getPgn() >> 16 );

    if( m_msgs[ ps ] == NULL )
      return false;

    for ( MsgIterator it = m_msgs[ ps ]->begin(); it != m_msgs[ ps ]->end(); ++it )
    {
      if ( dp != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( ident.getSaIsoName() != (*it)->remote() ) )
        continue;

      return true;
    }

    return false;
  }


  bool
  ProprietaryMessageHandler_c::CanCustomerB_c::processPartStreamDataChunk( Stream_c &apc_stream, bool, bool last )
  {
    if( ! last )
      return false;

    const ReceiveStreamIdentifier_c& ident = apc_stream.getIdent();
    const uint8_t ps = uint8_t( ident.getPgn() & 0xFF );
    const uint8_t dp = uint8_t( ident.getPgn() >> 16 );

    if( m_msgs[ ps ] == NULL )
      return false;

    MsgList msgs;

    for ( MsgIterator it = m_msgs[ ps ]->begin(); it != m_msgs[ ps ]->end(); ++it )
    {
      if ( dp != (*it)->dp() )
        continue;

      if ( (*it)->remote().isSpecified() && ( ident.getSaIsoName() != (*it)->remote() ) )
        continue;

      (*it)->getDataReceive().clearVector();
      msgs.push_front( *it );
    }


    if( ! msgs.empty() )
    {
      uint16_t cnt = 0;

      if (apc_stream.getIdent().getDa() == 0xFF)
      { // was BAM, where the first byte was not set already!
        // let all be read by loop below, no special handling for first byte
      }
      else
      { // directed, first byte was filled out, get it in a special way!
          uint8_t db = apc_stream.getFirstByte();
          for (; cnt < 1; ++cnt)
          {
              for (MsgIterator it = msgs.begin(); it != msgs.end(); ++it) {
                  (*it)->getDataReceive().setDataUi8(cnt, db);
              }
          }
      }

      for( ; cnt < apc_stream.getByteTotalSize(); ++cnt )
      {
        uint8_t db = apc_stream.getNextNotParsed();
        for( MsgIterator it = msgs.begin(); it != msgs.end(); ++it ) {
          (*it)->getDataReceive().setDataUi8( cnt, db );
        }
      }

      for( MsgIterator it = msgs.begin(); it != msgs.end(); ++it ) {
        // the static cast is prettier that a old C cast, but the Hightec GCC 3.4.6 is quite picky
        //(*it)->processB( *static_cast<IsoAgLib::iIsoItem_c*>( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ) );
        (*it)->processB( *( IsoAgLib::iIsoItem_c*)( getIsoMonitorInstance( m_handler.getMultitonInst() ).isoMemberNrFast( ident.getSa() ) ), ps );
      }
    }

    // don't keep the stream - we processed it right now!
    return false;
  }


  ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance( int instance )
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(ProprietaryMessageHandler_c, PRT_INSTANCE_CNT, instance );
  }

} // namespace __IsoAgLib
