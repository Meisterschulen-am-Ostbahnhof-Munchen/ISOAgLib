/*
  multisendeventhandler.h: transfer of a stream of multi message data

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
#ifndef MULTI_SEND_EVENT_HANDLER_H
#define MULTI_SEND_EVENT_HANDLER_H

#include <IsoAgLib/isoaglib_config.h>


namespace __IsoAgLib {

class SendStream_c;

class MultiSendEventHandler_c {
public:
  MultiSendEventHandler_c() {}
  virtual ~MultiSendEventHandler_c() {}

  /** call back function called when the TP is finished
    @param sendStream stream that has just finished (success or abort)
  */
  virtual void reactOnStateChange(const SendStream_c& sendStream) = 0;
};

} // __IsoAgLib
#endif
