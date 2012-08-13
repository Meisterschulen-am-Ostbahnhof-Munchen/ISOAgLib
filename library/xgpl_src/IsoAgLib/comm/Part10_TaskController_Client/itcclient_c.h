/*
  itcclient_c.h: central managing instance for all process data

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENT_C_H
#define ITCCLIENT_C_H

#include "impl/tcclient_c.h"
#include "iprocdata.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/idevpropertyhandler_c.h>


namespace IsoAgLib {

class iTcClient_c : private __IsoAgLib::TcClient_c
{
public:
#if defined(USE_ISO_TASKCONTROLLER_CLIENT)
  iDevPropertyHandler_c& getDevPropertyHandlerInstance( void )
  {return static_cast<iDevPropertyHandler_c&>(TcClient_c::getDevPropertyHandlerInstance());};
#endif

  void setProcDataHandler( iProcDataHandler_c *apc_procDataHandler )
 { TcClient_c::setProcDataHandler( apc_procDataHandler ); } ;

 private:
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iTcClient_c& getItcClientInstance( uint8_t aui8_instance );
  #else
  friend iTcClient_c& getItcClientInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  inline iTcClient_c& getItcClientInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance(aui8_instance));};
#else
  inline iTcClient_c& getItcClientInstance( void )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance());};
#endif

}

#endif
