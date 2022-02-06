/*
  vtobjectstringstreamer_c.cpp

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
#include "vtobjectstring_c.h"
#include "../ivtobjectstring_c.h"

#include <IsoAgLib/comm/Part3_DataLink/impl/multisendpkg_c.h>



namespace __IsoAgLib {

// Operation : setDataNextStreamPart
//! @param mspData:
//! @param bytes:
void
vtObjectStringStreamer_c::setDataNextStreamPart(__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes)
{
  uint8_t bytesDone=0;
  if (mui32_streamPosition == 0) {
    set5ByteCommandHeader(marr_uploadBuffer);
    bytesDone = 5;

    mui32_streamPosition += 5;
  }

  while (bytesDone < bytes) {
    marr_uploadBuffer [bytesDone] = mpc_stringToStream [mui32_streamPosition-5];
    bytesDone++;
    mui32_streamPosition++;
  }

  mspData->setDataPart (marr_uploadBuffer, 0, bytes);
}


void
vtObjectStringStreamer_c::set5ByteCommandHeader(uint8_t* destinBuffer)
{
  destinBuffer [0] = getFirstByte(); // 179 == Command: "Command" --- Parameter: "Change String Value"
  destinBuffer [1] = getID () & 0xFF;
  destinBuffer [2] = getID () >> 8;
  destinBuffer [3] = mui16_strLenToSend & 0xFF;
  destinBuffer [4] = mui16_strLenToSend >> 8;
}

// Operation : resetDataNextStreamPart
void
vtObjectStringStreamer_c::resetDataNextStreamPart()
{
  mui32_streamPosition = 0;
}


// Operation : saveDataNextStreamPart
void
vtObjectStringStreamer_c::saveDataNextStreamPart()
{
  mui32_streamPositionStored = mui32_streamPosition;
}


// Operation : restoreDataNextStreamPart
void
vtObjectStringStreamer_c::restoreDataNextStreamPart()
{
  mui32_streamPosition = mui32_streamPositionStored;
}

// Operation : getStreamSize
uint32_t
vtObjectStringStreamer_c::getStreamSize()
{
  return 5+mui16_strLenToSend;
}

    vtObjectStringStreamer_c::vtObjectStringStreamer_c(const char *apc_newValue, uint16_t a_ID,
                                                       uint16_t aui16_strLenToSend)
            : iMultiSendStreamer_c()
            , mui16_vtObjectAId (a_ID)
            , mpc_stringToStream (apc_newValue)
            , mui16_strLenToSend (aui16_strLenToSend)
            , mui32_streamPosition (0)
            //marr_uploadBuffer
            , mui32_streamPositionStored (0)
    {}

    const char *vtObjectStringStreamer_c::getStringToStream() { return mpc_stringToStream; }

    uint16_t vtObjectStringStreamer_c::getID() { return mui16_vtObjectAId; }

    uint8_t vtObjectStringStreamer_c::getFirstByte() { return 179; /* Command: "Command" --- Parameter: "Change String Value"; */ }
} // end namespace __IsoAgLib
