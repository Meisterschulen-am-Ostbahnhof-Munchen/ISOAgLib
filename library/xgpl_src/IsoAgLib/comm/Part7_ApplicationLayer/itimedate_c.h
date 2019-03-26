/*
  itimedate_c.h: Handling of Time/Date information from the ISOBUS.

  (C) Copyright 2015 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITIMEDATE_C_H
#define ITIMEDATE_C_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "impl/timedate_c.h"


namespace IsoAgLib {


class iTimeDate_c : private __IsoAgLib::TimeDate_c
{
public:
  /**
    @param apc_ident pointer to the variable of the responsible member instance. If NULL the module cannot send requests!
    @return true -> configuration was successfull
  */
  bool config ( iIdentItem_c* ident ) { return TimeDate_c::config_base( ident, IsoAgLib::IdentModeImplement, 0 ); }

  // @return true => Request was sent on the bus. false => IdentItem not properly claimed!
  bool requestUpdate() { return TimeDate_c::requestUpdate(); }

  const iTimezone_s &getTimezone() const { return TimeDate_c::getTimezone(); }
  const iDateTime_s &get( IsoAgLib::TimeStandard_t ts ) const { return TimeDate_c::get( ts ); }

  ecutime_t lastUpdateTime() const { return TimeDate_c::lastUpdateTime(); }

  /** register an event handler that gets called for any incoming PGN.
      Please look into the implementation to see for which PGNs it is
      actually called.
      Note: Double registration will be allowed, whereas deregistration
            will remove all occurances. */
  void registerMsgEventHandler( iMsgEventHandler_c &handler ) { TimeDate_c::registerMsgEventHandler( handler ); }

  /** deregister all event handlers matching the parameter
      @param arc_msgEventHandler Reference to an implementation of the
                                 handler class of type iMsgEventHandler_c */
  void deregisterMsgEventHandler( iMsgEventHandler_c &handler ) { TimeDate_c::deregisterMsgEventHandler( handler ); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
  friend iTimeDate_c& getITimeDateInstance();
#endif
  friend iTimeDate_c& getITimeDateInstance( unsigned instance );
};

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTimeDate_c& getITimeDateInstance()
  { return static_cast<iTimeDate_c&>(__IsoAgLib::getTimeDateInstance( 0 )); }
#endif
  inline iTimeDate_c& getITimeDateInstance( unsigned instance )
  { return static_cast<iTimeDate_c&>(__IsoAgLib::getTimeDateInstance( instance )); }

}
#endif
