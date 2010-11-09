/*
  messages.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef MESSAGES_H
#define MESSAGES_H
#include <logenvirons.h>

enum msgType_e
{
  msgTypePeriodic,
  msgTypeCommand,
  msgTypeResponse,
  msgTypeRTS,
  msgTypeCTS,
  msgTypeDATA,
  msgTypeDPO,
  msgTypeEOMACK,
  msgTypeCONNABORT
};

class Messages_c {
public:
  static size_t endOfModes();
  std::vector< uint64_t > &alives(
    size_t mode,
    uint8_t ui8_address);
  std::vector< msgType_e > &response(
      size_t mode,
      uint8_t ui8_address);
  int32_t alivePeriod(size_t mode) const;
  void setAlivePeriod(size_t mode, int32_t ai32_alivePeriod);
  char const *name(size_t mode) const;
  void setName(size_t mode, char const *pch_name);

private:
  enum { nofModes = 5 };
  struct msgType_s {
    std::vector<uint64_t> svec_alives[256];
    std::vector<msgType_e> svec_response[256];
    const char* name;
    int32_t alivePeriod; // ==0 => single Event - >0 ==> periodic Event - <0 ==> handshaking Event
    msgType_s();
  } marr_messages[nofModes];
};

/** Inline definitions. */

inline size_t Messages_c::endOfModes()
{
  return nofModes;
}

inline Messages_c::msgType_s::msgType_s() :
  svec_alives(),
  svec_response(),
  name(""),
  alivePeriod(0)
{
}

inline std::vector< uint64_t > &Messages_c::alives(
    size_t mode,
    uint8_t ui8_address)
{
  return marr_messages[mode].svec_alives[ui8_address];
}

inline std::vector< msgType_e > &Messages_c::response(
    size_t mode,
    uint8_t ui8_address)
{
  return marr_messages[mode].svec_response[ui8_address];
}

inline int32_t Messages_c::alivePeriod(size_t mode) const
{
  return marr_messages[mode].alivePeriod;
}

inline void Messages_c::setAlivePeriod(size_t mode, int32_t ai32_alivePeriod)
{
  marr_messages[mode].alivePeriod = ai32_alivePeriod;
}

inline char const *Messages_c::name(size_t mode) const
{
  return marr_messages[mode].name;
}

inline void Messages_c::setName(size_t mode, char const *pch_name)
{
  marr_messages[mode].name = pch_name;
}

#endif//ndef MESSAGES_H
