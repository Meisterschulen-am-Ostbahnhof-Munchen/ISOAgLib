/*
  vtobjectstringstreamer_c.h

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTSTRING_C_H
#define VTOBJECTSTRING_C_H

#include <IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h>
#include "vtobject_c.h"


namespace __IsoAgLib {

class MultiSendPkg_c;

/** helper class to avoid multiple inheritance*/
class vtObjectStringStreamer_c : public IsoAgLib::iMultiSendStreamer_c
{
 public:
  vtObjectStringStreamer_c(const char* apc_newValue, uint16_t a_ID, uint16_t aui16_strLenToSend)
    : iMultiSendStreamer_c()
    , mui16_vtObjectAId (a_ID)
    , mpc_stringToStream (apc_newValue)
    , mui16_strLenToSend (aui16_strLenToSend)
    , mui32_streamPosition (0)
    //marr_uploadBuffer
    , mui32_streamPositionStored (0)
  {}
  
  void setDataNextStreamPart(MultiSendPkg_c* mspData,
                             uint8_t bytes);

  void resetDataNextStreamPart();
  void saveDataNextStreamPart();
  void restoreDataNextStreamPart();

  uint32_t getStreamSize();
  uint8_t getFirstByte () { return 179; /* Command: "Command" --- Parameter: "Change String Value"; */ }

  const char* getStringToStream() { return mpc_stringToStream; }
  uint16_t getID() { return mui16_vtObjectAId; }

  void set5ByteCommandHeader(uint8_t* destinBuffer);

private:
  // ID from the connected __IsoAgLib::vtObject_c
  uint16_t mui16_vtObjectAId;
  // those both will be set before multisending is getting started.
  const char* mpc_stringToStream;
  uint16_t mui16_strLenToSend;

  /// TODOK following does not need to be here, only once per connection. maybe moved over, but first keep it here.
  uint32_t mui32_streamPosition;
  uint8_t marr_uploadBuffer [7+1]; /* "+1" for safety only */
  uint32_t mui32_streamPositionStored;
};

} // __IsoAgLib

#endif
