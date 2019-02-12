/*
  sendupload_c.h: class for managing the queued upload sendings

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "sendupload_c.h"
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectstring_c.h>


namespace __IsoAgLib {

const SendUpload_c& SendUpload_c::operator= (const SendUpload_c& r_source)
{
  SendUploadBase_c::operator=(r_source);

  if (mc_streamer != NULL)
  {
    delete mc_streamer;
  }
  mc_streamer = r_source.mc_streamer;
  ppc_vtObjects = r_source.ppc_vtObjects;
  ui16_numObjects = r_source.ui16_numObjects;
  return r_source;
}

SendUpload_c::~SendUpload_c()
{
  if (mc_streamer != NULL)
  {
    delete mc_streamer;
  }
}

void
SendUpload_c::setStreamer(const char* apc_newValue, uint16_t a_ID, uint16_t aui16_strLenToSend)
{
  isoaglib_assert(mc_streamer == NULL);

  ppc_vtObjects = NULL;

  mc_streamer = new vtObjectStringStreamer_c(apc_newValue, a_ID, aui16_strLenToSend);
}

void
SendUpload_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9)
{
  SendUploadBase_c::set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9);
  mc_streamer = NULL;   /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = NULL; 
  ui16_numObjects = 0;
}


void
SendUpload_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
{
  SendUploadBase_c::set( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8 );
  mc_streamer = NULL;  /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = rppc_vtObjects;
  ui16_numObjects = aui16_numObjects;
}

#ifdef STRIP_TRAILING_SPACES_FROM_STRING_UPLOADS
uint16_t
getStringLengthWithoutTrailingSpaces( const char* apc_string, uint16_t overrideSendLength, bool utf16 )
{
  uint16_t ret = (CNAMESPACE::strlen(apc_string) < overrideSendLength && !utf16) ? CNAMESPACE::strlen(apc_string) : overrideSendLength;

  if( !utf16 )
  {
    while( ( ret >= 1 ) && ( apc_string[ret-1] == ' ' ) )
      --ret;
  }
  else
  {
    while( ( ret >= 4 ) &&
       ( ( (uint8_t)apc_string[0] == (uint8_t)0xff && (uint8_t)apc_string[ret-1] == (uint8_t)0x00 && (uint8_t)apc_string[ret-2] == 0x20 )
      || ( (uint8_t)apc_string[0] == (uint8_t)0xfe && (uint8_t)apc_string[ret-1] == (uint8_t)0x20 && (uint8_t)apc_string[ret-2] == 0x00 ) ) )
      ret -= 2;
  }
  return ret;
}
#endif


void
#ifdef USE_VT_UNICODE_SUPPORT
SendUpload_c::set (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength, bool utf16)
{
#else
SendUpload_c::set (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength)
{
  const bool utf16 = false;
#endif

#ifdef STRIP_TRAILING_SPACES_FROM_STRING_UPLOADS
  uint16_t strLen = getStringLengthWithoutTrailingSpaces( apc_string, overrideSendLength, utf16 );
#else
  uint16_t strLen = uint16_t( (CNAMESPACE::strlen(apc_string) < overrideSendLength && !utf16) ? CNAMESPACE::strlen(apc_string) : overrideSendLength );
#endif

  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (((5+strLen) < 8) ? 8 : (5+strLen)); // DO NOT USED an UploadBuffer < 8 as ECU->VT ALWAYS has 8 BYTES!

  vec_uploadBuffer.push_back (179);
  vec_uploadBuffer.push_back (aui16_objId & 0xFF);
  vec_uploadBuffer.push_back (aui16_objId >> 8);
  vec_uploadBuffer.push_back (strLen & 0xFF);
  vec_uploadBuffer.push_back (strLen >> 8);
  int i=0;
  for (; i < strLen; i++) {
    vec_uploadBuffer.push_back (*apc_string);
    apc_string++;
  }
  for (; i < 3; i++) {
    // at least 3 bytes from the string have to be written, if not, fill with 0xFF, so the pkg-len is 8!
    vec_uploadBuffer.push_back (0xFF);
  }

  mc_streamer = NULL;  /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = NULL;
}


void
SendUpload_c::set (uint8_t* apui8_buffer, uint32_t bufferSize)
{
  SendUploadBase_c::set (apui8_buffer, bufferSize);
  mc_streamer = NULL;   /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = NULL;
}


} // __IsoAgLib
