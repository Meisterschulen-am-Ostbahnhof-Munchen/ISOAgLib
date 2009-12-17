/*
  istream_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISTREAM_C_H
#define ISTREAM_C_H

// IsoAgLib
#include <supplementary_driver/driver/datastreams/streaminput_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>



namespace __IsoAgLib
{ // forward declarations
  class MultiReceive_c;
  class Stream_c;
}



// Begin Namespace IsoAgLib
namespace IsoAgLib {



/**
  struct for stream identification
*/
class ReceiveStreamIdentifier_c {
public:
  ReceiveStreamIdentifier_c (uint32_t aui32_pgn, uint8_t aui8_da, const iIsoName_c& acrc_daIsoName,
                                                 uint8_t aui8_sa, const iIsoName_c& acrc_saIsoName)
                          : ui32_pgn (aui32_pgn)
                          , ui8_da (aui8_da)
                          , ui8_sa (aui8_sa)
                          , c_daIsoName (acrc_daIsoName)
                          , c_saIsoName (acrc_saIsoName) {}

  ReceiveStreamIdentifier_c (const ReceiveStreamIdentifier_c& ac_rsi)
                          : ui32_pgn (ac_rsi.ui32_pgn)
                          , ui8_da (ac_rsi.ui8_da)
                          , ui8_sa (ac_rsi.ui8_sa)
                          , c_daIsoName (ac_rsi.c_daIsoName)
                          , c_saIsoName (ac_rsi.c_saIsoName) {}

  bool operator == (const ReceiveStreamIdentifier_c& ac_rsi) const
  {
    return (ac_rsi.ui8_sa == ui8_sa) && (ac_rsi.ui8_da == ui8_da) && (ac_rsi.ui32_pgn == ui32_pgn);
  }


  bool matchSaDa(uint8_t sa, uint8_t da) const
  {
    return (sa == ui8_sa) && (da == ui8_da);
  }


  /** test if the client's da/pgn match this RCI, so the incoming stream is to be handled by this client.
      the client also HAS to handle Broadcasts, so in case of (da==0xFF) he's also responsible!
    */
  bool matchDaPgnWithMask(uint8_t aui8_da, uint32_t aui32_pgn, uint32_t aui32_pgnMask) const
  {
    return ((aui8_da == 0xFF) || (aui8_da == ui8_da)) && ((aui32_pgn & aui32_pgnMask) == (ui32_pgn & aui32_pgnMask));
  }


  uint32_t          getPgn()       const { return ui32_pgn; }
  const iIsoName_c& getDaIsoName() const { return c_daIsoName; }
  const iIsoName_c& getSaIsoName() const { return c_saIsoName; }

  // public for direct access to the CACHED addresses.
  //! Be aware that SA/DA can CHANGE while the system is running, so
  //! better use the IsoName normally instead. Only use the SA if you
  //! keep your own SA list also up-to-date with the SA-Claim-Handlers!
  uint8_t getDa() const { return ui8_da; }
  uint8_t getSa() const { return ui8_sa; }

// declaring/defining the following methods mutable as
// A) they're only updating the cached SA/DA and
// B) only MultiReceive is friend and can do this!
private:
  void setDa (uint8_t aui8_da) const { ui8_da = aui8_da; }
  void setSa (uint8_t aui8_sa) const { ui8_sa = aui8_sa; }

  friend class __IsoAgLib::MultiReceive_c;
  friend class __IsoAgLib::Stream_c;

private:

  /** PGN of the stream */
  uint32_t ui32_pgn;

  /** Destination Address - mutable as it's only a CACHE for the c_daIsoName! */
  mutable uint8_t ui8_da;

  /** Source Address - mutable as it's only a CACHE for the c_daIsoName! */
  mutable uint8_t ui8_sa;

  /** Destination ISOName */
  iIsoName_c c_daIsoName;

  /** Source ISOName */
  iIsoName_c c_saIsoName;
};



class iStream_c : public StreamInput_c
{

public:

  iStream_c() : b_streamInvalid(false) {};

  virtual uint32_t getNotParsedSize()=0;

  virtual uint8_t getNextNotParsed()=0;

  virtual uint8_t getNotParsed(uint32_t ui32_notParsedRelativeOffset)=0;

  virtual uint32_t getByteTotalSize() const=0;

  virtual uint32_t getByteAlreadyReceived() const=0;

  virtual const ReceiveStreamIdentifier_c& getIdent()=0;

  virtual uint8_t getFirstByte() const=0;

  virtual void setStreamFinishedJustKept()=0;

  void setStreamInvalid()       { b_streamInvalid = true; };
  bool getStreamInvalid() const { return b_streamInvalid; };


protected:

  // Has this stream been marked invalid while parsing?
  bool b_streamInvalid;
};

} // end namespace IsoAgLib

#endif // ISTREAM_C_H

