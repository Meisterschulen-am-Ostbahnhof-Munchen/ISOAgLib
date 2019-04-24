/*
  sendupload_c.h: class for managing the queued upload sendings

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
#ifndef SENDUPLOAD_H
#define SENDUPLOAD_H

#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <vector>

namespace IsoAgLib {
  class iVtObject_c;
}


namespace __IsoAgLib {

class vtObjectStringStreamer_c;


class SendUpload_c : public SendUploadBase_c
{
public:
  SendUpload_c()
    : SendUploadBase_c()
    , mc_streamer(NULL)
    , ppc_vtObjects (NULL)
    , ui16_numObjects (0)
  {}

  SendUpload_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
    : SendUploadBase_c( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8)
    , mc_streamer(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (rppc_vtObjects)
    , ui16_numObjects (aui16_numObjects)
  {}

  SendUpload_c (uint8_t* apui8_buffer, uint32_t bufferSize)
    : SendUploadBase_c (apui8_buffer, bufferSize)
    , mc_streamer(NULL)   /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (NULL)
    , ui16_numObjects (0)
  {}

  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9);
  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects);
#ifdef USE_VT_UNICODE_SUPPORT
  void set (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength, bool utf16 = false);
#else
  void set (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength);
#endif
  void set (uint8_t* apui8_buffer, uint32_t bufferSize);

  // special functions for streamer. Attention: Will allocate dynamic streamer object!
  void setStreamer(const char* apc_newValue, uint16_t a_ID, uint16_t aui16_strLenToSend);
  inline void unsetStreamer() { mc_streamer = NULL; };

  SendUpload_c (const SendUpload_c& r_source)
    : SendUploadBase_c(r_source)
    , mc_streamer(r_source.mc_streamer)
    , ppc_vtObjects (r_source.ppc_vtObjects)
    , ui16_numObjects (r_source.ui16_numObjects)
  {}

  ~SendUpload_c();

  const SendUpload_c& operator= (const SendUpload_c& r_source);

  /// Use either an MultiSendStreamer or a direct ui8-Buffer
  vtObjectStringStreamer_c* mc_streamer;

  IsoAgLib::iVtObject_c** ppc_vtObjects;
  uint16_t ui16_numObjects; // don't care for if "ppc_vtObjects==NULL"
};


} // __IsoAgLib

#endif
