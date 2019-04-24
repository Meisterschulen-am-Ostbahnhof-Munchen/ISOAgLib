/*
  proprietarymessagehandler_c.h - Currently only supports up to 0xFFFE byte,
  because most internal setters are uint16_t only!

  (C) Copyright 2009 - 2019 by OSB AG

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

#ifndef PROPRIETARYMESSAGEHANDLER_H
#define PROPRIETARYMESSAGEHANDLER_H

#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/can/imaskfilter_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <list>
#include <map>


namespace __IsoAgLib
{

  class ProprietaryMessageA_c;
  class ProprietaryMessageB_c;


  class ProprietaryMessageHandler_c : public Subsystem_c
  {
    MACRO_MULTITON_CONTRIBUTION();
  public:
    ProprietaryMessageHandler_c();

    virtual ~ProprietaryMessageHandler_c() {}

    void init();
    void close();

    void registerProprietaryMessage( ProprietaryMessageA_c& msg );
    void registerProprietaryMessage( ProprietaryMessageB_c& msg, uint8_t ps );

    void deregisterProprietaryMessage( ProprietaryMessageA_c& msg );
    void deregisterProprietaryMessage( ProprietaryMessageB_c& msg, uint8_t ps );

  private:

    class CanCustomerAB_c : public CanCustomer_c
    {
    private:
      CanCustomerAB_c(const CanCustomerAB_c&); // not copyable!
      CanCustomerAB_c& operator=(const CanCustomerAB_c&);

    public:
      CanCustomerAB_c( ProprietaryMessageHandler_c& handler, IsoAgLib::iMaskFilterType_c filter ) : m_handler( handler ), m_filter( filter ), mmap_registeredMsgs() {}
      virtual ~CanCustomerAB_c() {}

      void   registerFilter( int multitonInst, const IsoName_c& identName );
      void deregisterFilter( int multitonInst, const IsoName_c& identName );

    private:
      virtual void reactOnAbort( Stream_c & ) {}
      virtual void notificationOnMultiReceiveError( ReceiveStreamIdentifier_c const &, uint8_t, bool ) {}

    protected:
      ProprietaryMessageHandler_c& m_handler;
      const IsoAgLib::iMaskFilterType_c m_filter;

    private:
      STL_NAMESPACE::map<IsoName_c,unsigned> mmap_registeredMsgs; // how many registered for a given NAME.
    };

    class CanCustomerA_c : public CanCustomerAB_c
    {
    public:
      CanCustomerA_c( ProprietaryMessageHandler_c& handler ) : CanCustomerAB_c( handler, IsoAgLib::iMaskFilterType_c( 0x00FF0000, PROPRIETARY_A_PGN << 8, IsoAgLib::iIdent_c::ExtendedIdent ) ), m_msgs() {} // A1 and A2
      virtual ~CanCustomerA_c() {}

      typedef STL_NAMESPACE::list<ProprietaryMessageA_c*> MsgList;
      typedef STL_NAMESPACE::list<ProprietaryMessageA_c*>::iterator MsgIterator;

      MsgList m_msgs;

    private:
      virtual void processMsg( const CanPkg_c& arc_data );

      virtual bool reactOnStreamStart( const ReceiveStreamIdentifier_c& ident, uint32_t len );
      virtual bool processPartStreamDataChunk( Stream_c &apc_stream, bool first, bool last );
    };

    class CanCustomerB_c : public CanCustomerAB_c
    {
    public:
      CanCustomerB_c( ProprietaryMessageHandler_c& handler ) : CanCustomerAB_c( handler, IsoAgLib::iMaskFilterType_c( 0x00FF0000, PROPRIETARY_B_PGN << 8, IsoAgLib::iIdent_c::ExtendedIdent ) ) // B1 and B2
      { CNAMESPACE::memset( m_msgs, 0x00, sizeof( m_msgs ) ); }

      virtual ~CanCustomerB_c() {}

      typedef STL_NAMESPACE::list<ProprietaryMessageB_c*> MsgList;
      typedef STL_NAMESPACE::list<ProprietaryMessageB_c*>::iterator MsgIterator;

      MsgList* m_msgs[256];

    private:
      virtual void processMsg( const CanPkg_c& arc_data );

      virtual bool reactOnStreamStart( const ReceiveStreamIdentifier_c& ident, uint32_t len );
      virtual bool processPartStreamDataChunk( Stream_c &apc_stream, bool first, bool last );
    };

  private:
    CanCustomerA_c m_customerA;
    CanCustomerB_c m_customerB;

    friend ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance(int instance);
  };

  ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance(int instance);

}
#endif


