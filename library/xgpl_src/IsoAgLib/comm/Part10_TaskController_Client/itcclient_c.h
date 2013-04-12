/*
  itcclient_c.h: central managing instance for all process data

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

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
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>


namespace __IsoAgLib {
  class DevicePool_c;
}

namespace IsoAgLib {

class iTcClient_c : private __IsoAgLib::TcClient_c
{
public:
//#if defined(USE_ISO_TASKCONTROLLER_CLIENT)
//  iTcClientConnection_c& getTcClientConnection( iIdentItem_c& identItem )
//  {return static_cast<iTcClientConnection_c&>(TcClient_c::getTcClientConnection( static_cast<__IsoAgLib::IdentItem_c&>(identItem) )); }
//#endif

  /** register given object pool for uploading when possible. */
  iTcClientConnection_c* initAndRegister( iIdentItem_c& identItem, iProcDataHandler_c& procdata, iDevicePool_c& pool )
  { 
    return TcClient_c::initAndRegister( static_cast<__IsoAgLib::IdentItem_c&>(identItem), procdata, pool )->toInterfacePointer();
  }

  bool deregister( iIdentItem_c& identItem )
  { 
    return TcClient_c::deregister( static_cast<__IsoAgLib::IdentItem_c&>(identItem) );
  }

  bool isTcAvailable() const { return TcClient_c::isTcAvailable(); }

  void processChangeDesignator( iIdentItem_c& identItem, uint16_t objID, const char* newDesignator )
  {
    return TcClient_c::processChangeDesignator( static_cast<__IsoAgLib::IdentItem_c&>(identItem), objID, newDesignator );
  }

#ifdef USE_DATALOGGER
  bool isLoggerAvailable() const { return TcClient_c::isLoggerAvailable(); }
#endif

 private:
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iTcClient_c& getItcClientInstance( uint8_t instance );
  #else
  friend iTcClient_c& getItcClientInstance( void );
  #endif
  friend class iTcClientConnection_c;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  inline iTcClient_c& getItcClientInstance( uint8_t instance = 0 )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance(instance)); }
#else
  inline iTcClient_c& getItcClientInstance( void )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance()); }
#endif

}

#endif
