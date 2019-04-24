/*
  tractorlighting_c.h: implements a lighting controller

  (C) Copyright 2017 - 2019 by OSB AG

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
#ifndef TRACTORLIGHTING_C_H
#define TRACTORLIGHTING_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {

  class IdentItem_c;


  class TractorLighting_c : public TractorCommonRx_c
  {
  public:

    void init();
    void close();

    //! Register implement for LIGHTING_DATA messages,
    //! @return success - only one implement can be registered! (currently!)
    bool registerImplement( IdentItem_c& ident );

    const IsoAgLib::iLighting_t& getCommand();
    const IsoAgLib::iLighting_t& getData();

    void setData( const IsoAgLib::iLighting_t& );
    void sendData();

  private:
    TractorLighting_c() : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_NONE, LIGHTING_COMMAND_PGN ), m_pgnRequest( *this ) { /* all done at init() */ }

    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

    class IsoRequestPgnHandlerProxy_c : public IsoRequestPgnHandler_c {
    public:
      typedef TractorLighting_c Owner_t;

      IsoRequestPgnHandlerProxy_c( Owner_t &owner ) : mrt_owner( owner ) {}
      virtual ~IsoRequestPgnHandlerProxy_c() {}

      void init();
      void close();

    private:
      virtual bool processMsgRequestPGN( uint32_t pgn, IsoItem_c *sender, IsoItem_c *receiver, ecutime_t time );

      IsoRequestPgnHandlerProxy_c(IsoRequestPgnHandlerProxy_c const &); // not copyable!
      IsoRequestPgnHandlerProxy_c &operator=(IsoRequestPgnHandlerProxy_c const &); // not assignable!

      Owner_t &mrt_owner;
    };

  private:
    /// Reception
    IsoAgLib::iLighting_t mt_cmd;

    // Sending
    IsoAgLib::iLighting_t mt_data;
    IdentItem_c* m_ident;

    IsoRequestPgnHandlerProxy_c m_pgnRequest;

    friend TractorLighting_c &getTractorLightingInstance( unsigned instance );
  };


  inline
  const IsoAgLib::iLighting_t&
  TractorLighting_c::getCommand()
  {
    return mt_cmd;
  }

  
  inline
  const IsoAgLib::iLighting_t&
  TractorLighting_c::getData()
  {
    return mt_data;
  }



  TractorLighting_c &getTractorLightingInstance( unsigned instance );


}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
