/*
  iprocess_c.h: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENT_H
#define ITCCLIENT_H

#include "impl/tcclient_c.h"
#include "iprocdata.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/idevpropertyhandler_c.h>

// Begin Namespace IsoAgLib::iTcClient_c
namespace IsoAgLib {


class iTcClient_c : private __IsoAgLib::TcClient_c  {
public:

#if defined(USE_ISO_TASKCONTROLLER_CLIENT)
  iDevPropertyHandler_c& getDevPropertyHandlerInstance( void )
  {return static_cast<iDevPropertyHandler_c&>(TcClient_c::getDevPropertyHandlerInstance());};
#endif

  /** set the pointer to the handler class (used for callback when TC status message is processed)
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcDataHandler( iProcDataHandler_c *apc_procDataHandler )
   { TcClient_c::setProcDataHandler( apc_procDataHandler ); } ;

 private:
  /** allow getITcClientInstance() access to shielded base class.
      otherwise __IsoAgLib::getTcClientInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iTcClient_c& getITcClientInstance( uint8_t aui8_instance );
  #else
  friend iTcClient_c& getITcClientInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique TcClient_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTcClient_c& getITcClientInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique TcClient_c singleton instance */
  inline iTcClient_c& getITcClientInstance( void )
  { return static_cast<iTcClient_c&>(__IsoAgLib::getTcClientInstance());};
#endif

} // end of namespace IsoAgLib::iTcClient_c
#endif
