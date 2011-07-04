/*
  processwsmtaskmsghandler_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESS_WSM_TASK_MSG_HANDLER_C_H
#define PROCESS_WSM_TASK_MSG_HANDLER_C_H

#include <IsoAgLib/hal/hal_typedef.h>

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

namespace __IsoAgLib {


/**
*/
class ProcessWsmTaskMsgHandler_c
{
public:
  virtual ~ProcessWsmTaskMsgHandler_c() {};
  virtual void processWsmTaskMessage (uint8_t /* ui8_tcStatus */, const IsoName_c& /* acrc_isoName */) = 0;
};

}
#endif
