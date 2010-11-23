/*
  streamlinear_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef STREAMLINEAR_C_H
#define STREAMLINEAR_C_H


#include "stream_c.h"

// stl
#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


//  +X2C Class 755 : StreamLinear_c
class StreamLinear_c : public Stream_c
{

public:

  StreamLinear_c (const ReceiveStreamIdentifier_c& ac_rsi, uint32_t aui32_msgSize MULTITON_INST_PARAMETER_DEF_WITH_COMMA);

  virtual ~StreamLinear_c ();

  void immediateInitAfterConstruction() const {}


  void insert7Bytes(const uint8_t* pui8_data);

  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  void insertFirst6Bytes(const uint8_t* pui8_data);
  #endif

  //  Operation: getNextNotParsed
  //! Also moves one byte forward!
  inline uint8_t getNextNotParsed();

  //  Operation: getNotParsedSize
  inline uint32_t getNotParsedSize();

  //  Operation: getNotParsed
  inline uint8_t getNotParsed(uint32_t ui32_notParsedRelativeOffset);

  //  Operation: getLinearBuffer
  inline uint8_t* getLinearBuffer();

  inline bool eof() const;

protected:

private:
  STL_NAMESPACE::vector<uint8_t> mvecui8_buffer; // linearBuffer

  uint32_t mui32_parsedCnt;

}; // ~X2C



// //////////////////////////////// +X2C Operation 783 : getNextNotParsed
//! ATTENTION: Only use this function if it is assured using getNotParsedSize that there's something in the buffer!
inline uint8_t
StreamLinear_c::getNextNotParsed()
{ // ~X2C
  return mvecui8_buffer[mui32_parsedCnt++];
} // -X2C



inline uint8_t*
StreamLinear_c::getLinearBuffer()
{
  return &mvecui8_buffer.front();
}


// //////////////////////////////// +X2C Operation 838 : getNotParsedSize
inline uint32_t
StreamLinear_c::getNotParsedSize()
{ // ~X2C
  uint32_t ui32_bufSize = mvecui8_buffer.size();
  // if the buffer is larger than our totalMsgSize, clip at totalMsgSize.
  // this occurs as we always insert 7 bytes, even if the last 7byte packet should NOT been taken completely
  if (mui32_byteTotalSize < ui32_bufSize)
    return (mui32_byteTotalSize - mui32_parsedCnt);
  else
    return (ui32_bufSize - mui32_parsedCnt);
} // -X2C



// //////////////////////////////// +X2C Operation : getNotParsed
//! be sure to NOT read from an offset which overflows the unparsed buffer!
inline uint8_t
StreamLinear_c::getNotParsed (uint32_t ui32_notParsedRelativeOffset)
{
  return mvecui8_buffer[mui32_parsedCnt + ui32_notParsedRelativeOffset];
}



// //////////////////////////////// +X2C Operation 2878 : eof
inline bool
StreamLinear_c::eof() const
{ // ~X2C
  if (mui32_parsedCnt >= mui32_byteTotalSize)
    return true;
  else
    return false;
} // -X2C



} // end namespace __IsoAgLib
#endif // -X2C
