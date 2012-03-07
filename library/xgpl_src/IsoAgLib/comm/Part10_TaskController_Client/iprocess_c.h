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
#ifndef IPROCESS_H
#define IPROCESS_H

#include "impl/process_c.h"
#include "proc_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/idevpropertyhandler_c.h>

// Begin Namespace IsoAgLib::iProcess_c
namespace IsoAgLib {


class iProcess_c : private __IsoAgLib::Process_c  {
public:

#if defined(USE_ISO_TASKCONTROLLER_CLIENT)
  iDevPropertyHandler_c& getDevPropertyHandlerInstance( void )
  {return static_cast<iDevPropertyHandler_c&>(Process_c::getDevPropertyHandlerInstance());};
#endif

  void registerWsmTaskMsgHandler(__IsoAgLib::ProcessWsmTaskMsgHandler_c* apc_processWsmTaskMsgHandler)
  { Process_c::registerWsmTaskMsgHandler(apc_processWsmTaskMsgHandler); }

  /** set the pointer to the handler class (used for callback when TC status message is processed)
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { Process_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); } ;

 private:
  /** allow getIProcessInstance() access to shielded base class.
      otherwise __IsoAgLib::getProcessInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iProcess_c& getIProcessInstance( uint8_t aui8_instance );
  #else
  friend iProcess_c& getIProcessInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iProcess_c& getIProcessInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  inline iProcess_c& getIProcessInstance( void )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance());};
#endif

} // end of namespace IsoAgLib::iProcess_c
#endif
