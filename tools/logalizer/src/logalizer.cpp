/*
  logalizer.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <logenvirons.h>
#include <transfercollection.h>
#include <alivecollection.h>
#include <dataframe.h>
#include <inputstream.h>
#include <string>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>

namespace
{
typedef std::pair< int, PtrDataFrame_t > ParseLogLine_t(std::string const &acr_line);
typedef void Interprete_t(PtrDataFrame_t);


struct Main_s {
  size_t mt_sizeMultipacketWrap; // default will be set when parsing parameters
  TransferCollection_c mc_trans;
  ParseLogLine_t *pt_parseLogLine;
  AliveCollection_c m_alive;
  Main_s();
};

inline Main_s::Main_s() :
  mt_sizeMultipacketWrap(0),
  mc_trans(),
  pt_parseLogLine(0),
  m_alive()
{
}

struct Main_s gs_main;
} //namespace

Interprete_t interpretePgnData;
Interprete_t interpretePgnsFs2Cl;
Interprete_t dump;
void decodeErrorCode(uint8_t);
void decodeAttributes(uint8_t);
void decodeDate(uint16_t);
void decodeTime(uint16_t);

void exit_with_usage(const char* progname)
{
  std::cout << "Usage: (1) " << progname << " <logFile> [logType] [wrapMultipacket]" << std::endl << std::endl;
  std::cout << "logFile:  filepath or - (dash, means standard input rather than a real file)"<<std::endl;
  std::cout << "logTypes: 0 -> can_server [DEFAULT]"<<std::endl;
  std::cout << "          1 -> rte"<<std::endl;
  std::cout << "          2 -> CANMon"<<std::endl;
  std::cout << "          3 -> CANoe ASCII (.asc)"<<std::endl;
  std::cout << "          4 -> A1ASCII"<<std::endl;
  std::cout << "          5 -> PCANView"<<std::endl;
  std::cout << "          6 -> JohnDeere"<<std::endl;
  std::cout << "          7 -> rte2"<<std::endl;
  std::cout << "          8 -> JRF (.jrf)"<<std::endl;
  std::cout << std::endl;
  std::cout << "wrapMultipacket: Number of data-bytes to display per line. Defaults to 32." << std::endl;
  std::cout << std::endl;
#ifdef WIN32
  std::cout << "can_server: '104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0'"<<std::endl;
  std::cout << "rte:        '[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06'"<<std::endl;
  std::cout << "             (with OR without Channel-Nr. in []. This is being autodetected.)"<<std::endl;
  std::cout << "CANMon:     'RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB'"<<std::endl;
  std::cout << "CANoe:      '  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF'"<<std::endl;
  std::cout << "A1ASCII:    'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff    '..."<<std::endl;
  std::cout << "            ...'   446270'"<<std::endl;
  std::cout << "PCANView:   '    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15'"<<std::endl;
  std::cout << "JohnDeere:  'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488  '..."<<std::endl;
  std::cout << "            ...'    17920  ....... '"<<std::endl;
  std::cout << "JRF:        '41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF'"<<std::endl;
#else
  std::cout << "can_server: '104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0'"<<std::endl;
  std::cout << "rte:        '[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06'"<<std::endl;
  std::cout << "             (with OR without Channel-Nr. in []. This is being autodetected.)"<<std::endl;
  std::cout << "CANMon:     'RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB'"<<std::endl;
  std::cout << "CANoe:      '  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF'"<<std::endl;
  std::cout << "A1ASCII:    'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270'"<<std::endl;
  std::cout << "PCANView:   '    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15'"<<std::endl;
  std::cout << "JohnDeere:  'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....... '"<<std::endl;
  std::cout << "JRF:        '41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF'"<<std::endl;
#endif

  exit(0);
}

std::pair< int, PtrDataFrame_t > parseLogLineCANMon(
    std::string const &acr_line) //RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB
{
  unsigned arru_d[8];
  int iA, iB;
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%*s %i %*i %x%*s%x %x %x %x %x %x %x %x",
      &iA, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = (parsed_count >= 2) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t > parseLogLineJohnDeere(
  std::string const &acr_line) // "r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....\\. "
{
  int iA, iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "r Xtd %*i %x %*i %x %x %x %x %x %x %x %x %*i   %*i %i      17920  %*s ",
      &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iA);
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t > parseLogLineCanServer(
  std::string const &acr_line) // "104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0"
{
  long long iA;
  int iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%lli %*i %*i %*i %*i %*i %x %x %x %x %x %x %x %x %x",
      &iA, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  size_t t_sizeData = size_t(parsed_count-2);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + t_sizeData, back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t > parseLogLineCANoe(
  std::string const &acr_line) // "  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF "
{
  float f1;
  int iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %f %*i %xx %*s %*s %u %x %x %x %x %x %x %x %x",      
      &f1, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  uint64_t const cui64_time = uint64_t(f1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t > parseLogLineA1ASCII(
  std::string const &acr_line) // "m e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270"
{
  char ch_baseOrExtended = '.';
  int iA, iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "m %c 0x%x %u 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x %i",
      &ch_baseOrExtended, &iA, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iB);
  int i_result = parsed_count == (4+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  bool const cb_extendedFrameFormat = 'e' == ch_baseOrExtended;
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iB, iA, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}



std::pair< int, PtrDataFrame_t > parseLogLineRte(
  std::string const &acr_line) // "[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06"
{
  int iB, iDb = 0; // "%i* %x
  unsigned arru_d[8];
  uint64_t iA;
  static int const sci_minimal_expected_parse_count = 4;
  int parsed_count = 0;
  static int remembered_mode = 0;

  for (int current_mode = remembered_mode; ; ++current_mode) {
    switch (current_mode) {
    case 0:
      break;
    case 1:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%*s %*s %Li.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
      if (parsed_count <= -1 || sci_minimal_expected_parse_count <= parsed_count)
        remembered_mode = current_mode;
      break;
    case 2:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%*s %Li.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
      if (remembered_mode)
        ;
      else if (-1 < parsed_count && parsed_count < sci_minimal_expected_parse_count)
        exit_with_error("wrong rte-log format. use -a for absolute timestamps!");
      else
        remembered_mode = current_mode;
      break;
    }
    if (remembered_mode)
      break; // repeat until remembered_mode is set
  }
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t > parseLogLineRte2(
  std::string const &acr_line)
{
    static uint64_t sui64_rteTime = 0;
    int big;
    int little;
    char *cursor;
    size_t byte;

    /* skip over any initial HW */
    for (cursor = const_cast<char *>(acr_line.c_str()); *cursor != 0 && !isspace (*cursor); cursor++)
        /* empty */;

    /*
     * must specify base 10 because of leading 0's
     */
    big = strtol (cursor, &cursor, 10);
    if (*cursor != '.') {
        fprintf (stderr, "unexpected terminator %c (%d): %s\n", *cursor, *cursor, acr_line.c_str());
        return std::make_pair( -4, PtrDataFrame_t(0) );
    }
    little = strtol (&cursor[1], &cursor, 10);
    while (isspace (*cursor))
        cursor++;
    if (*cursor == 'X')
        cursor++;
    else {
        fprintf (stderr, "unexpected field %c\n", *cursor);
        return std::make_pair( -5, PtrDataFrame_t(0) );
    }

    sui64_rteTime += big;
    uint64_t const cui64_time = sui64_rteTime;
    uint32_t const cui32_identifier = strtol (cursor, &cursor, 16);
    size_t t_sizeData = uint8_t (strtol (cursor, &cursor, 0));


    if (t_sizeData > 8) {
        fprintf (stderr, "protocol violation at time %lld: dlc of %d too long! (forcing to 8)\n", cui64_time, t_sizeData);
        t_sizeData = 8;
    }

    std::vector< uint8_t > vec_data(t_sizeData);

    for (byte = 0; byte < 8; byte++) {
        if (byte >= t_sizeData)
            break;

        vec_data[byte] = uint8_t (strtol (cursor, &cursor, 16));
    }

    return std::make_pair( 0, new DataFrame_c( cui64_time, cui32_identifier, vec_data ) );
}




std::pair< int, PtrDataFrame_t > parseLogLineTrc(
  std::string const &acr_line) // "    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15"
{
  unsigned arru_d[8];
  int iA, iB, iDb;
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %*s %u.%*u %*s %x %u %x %x %x %x %x %x %x %x",
      &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t > parseLogLineJrf(
  std::string const &acr_line) // "41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF"
{
  float fA;
  int iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%f,%x,%x,%x,%x,%x,%x,%x,%x,%x",
      &fA, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  uint64_t const cui64_time = uint64_t(fA*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t > defaultParseLogLine(std::string const &acr_line)
{
  (void)acr_line;
  std::pair< int, PtrDataFrame_t > result = std::make_pair(
      -1,
      PtrDataFrame_t(0) );
  exit_with_error("Unknown Log-Type!");
  return std::make_pair(1, result.second); // return just to satisfy compiler. exit_with_error will exit anyway ;)
}

void partiallyInterpretePgnsVtEcu(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    if (at_ptrFrame->dataOctet(0) < 0x10) std::cout << "Activation ";
    if ((at_ptrFrame->dataOctet(0) >=0x10) && (at_ptrFrame->dataOctet(0) < 0xF0)) std::cout << "Command ";

    int8_t i8_uidBytePos = -1;

    switch (at_ptrFrame->dataOctet(0))
    {
      case 0x00: std::cout << "Soft Key"; i8_uidBytePos = 3; break;
      case 0x01: std::cout << "Button"; i8_uidBytePos = 3; break;
      case 0x02: std::cout << "Pointing Event"; break;
      case 0x03: std::cout << "VT Select Input Object"; i8_uidBytePos = 2; break;
      case 0x04: std::cout << "VT ESC"; i8_uidBytePos = 2; break;
      case 0x05: std::cout << "VT Change Numeric Value"; i8_uidBytePos = 2; break;
      case 0x06: std::cout << "VT Change Active Mask"; i8_uidBytePos = 2; break;
      case 0x07: std::cout << "VT Change Soft Key Mask"; i8_uidBytePos = 4; break;
      case 0x08: std::cout << "Input String Value"; i8_uidBytePos = 2; break;

      case 0x11: std::cout << "Object Pool Transfer"; break;
      case 0x12: std::cout << "End of Object Pool"; break;

      case 0x92: std::cout << "ESC"; i8_uidBytePos = 2; break;

      case 0xC0: std::cout << "Get Memory"; break;
      case 0xC2: std::cout << "Get Number Of Soft Keys"; break;
      case 0xC3: std::cout << "Get Text Font Data"; break;
      case 0xC7: std::cout << "Get Hardware"; break;

      case 0xD0: std::cout << "Store Version"; break;
      case 0xD1: std::cout << "Load Version"; break;
      case 0xD2: std::cout << "Delete Version"; break;

      case 0xA0: std::cout << "Hide/Show Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) std::cout << " -show-"; else std::cout << " -hide-"; break;
      case 0xA1: std::cout << "Enable/Disable Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) std::cout << " -enable-"; else std::cout << " -disable-"; break;
      case 0xA2: std::cout << "Select Input Object"; i8_uidBytePos = 2; break;
      case 0xA3: std::cout << "Control Audio Device"; break;
      case 0xA4: std::cout << "Set Audio Volume"; break;
      case 0xA5: std::cout << "Change Child Location"; i8_uidBytePos = 4; break;
      case 0xA6: std::cout << "Change Size"; i8_uidBytePos = 2; break;
      case 0xA7: std::cout << "Change Background Colour"; i8_uidBytePos = 2; break;
      case 0xA8: std::cout << "Change Numeric Value"; i8_uidBytePos = 2; break;
      case 0xA9: std::cout << "Change End Point"; i8_uidBytePos = 2; break;
      case 0xAA: std::cout << "Change Font Attributes"; i8_uidBytePos = 2; break;
      case 0xAB: std::cout << "Change Line Attributes"; i8_uidBytePos = 2; break;
      case 0xAC: std::cout << "Change Fill Attributes"; i8_uidBytePos = 2; break;
      case 0xAD: std::cout << "Change Active Mask"; i8_uidBytePos = 4; break;
      case 0xAE: std::cout << "Change Soft Key Mask"; i8_uidBytePos = 5; break;
      case 0xAF: std::cout << "Change Attribute"; i8_uidBytePos = 2; std::cout << " AID="<<uint16_t(at_ptrFrame->dataOctet(4-1)); break;

      case 0xB0: std::cout << "Change Priority"; i8_uidBytePos = 2; break;
      case 0xB1: std::cout << "Change List Item"; i8_uidBytePos = 2; break;
      case 0xB2: std::cout << "Delete Object Pool"; break;
      case 0xB3: std::cout << "Change String Value"; i8_uidBytePos = 2; break;
      case 0xB4: std::cout << "Change Child Position"; i8_uidBytePos = 4; break;

      case 0xFE: std::cout << "VT Status Message: Active WS-Master's SA: "<<uint16_t(at_ptrFrame->dataOctet(1)); break;
      case 0xFF: std::cout << "Working Set Maintenance"; break;
    }
    if (i8_uidBytePos > 0) std::cout << " ObjectID="<<std::dec<<( (uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos+1-1)<<8)) | (uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos-1))) );
  }
}

void interpretePgnsVtFromEcu(PtrDataFrame_t at_ptrFrame)
{
  partiallyInterpretePgnsVtEcu(at_ptrFrame);
  bool const cb_response = at_ptrFrame->dataOctet(0) < 0x10;
  if (cb_response)
    std::cout << " Response";
}

void interpretePgnsVtToEcu(PtrDataFrame_t at_ptrFrame)
{
  partiallyInterpretePgnsVtEcu(at_ptrFrame);
  bool const cb_response =
    (at_ptrFrame->dataOctet(0) >=0x10) &&
    (at_ptrFrame->dataOctet(0) < 0xF0);
  if (cb_response)
    std::cout << " Response";  
}

void interpretePgnsCl2Fs(
    PtrDataFrame_t at_ptrFrame)
{
  uint16_t i_pathNameLength = 0, i_destPathNameLength = 0;

  std::cout << "SA: " << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << " DA: " << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " length Data: " << std::setw(4)<<std::setfill('0') << uint32_t(at_ptrFrame->dataSize());
  switch (uint32_t(at_ptrFrame->dataOctet(0)))
  {
    case 0x00: std::cout << "\tClient Connection Maintenance";
      break;

    case 0x01: std::cout << "\tGet File Server Properties Request ";
      break;

    case 0x10: std::cout << "\tGet Current Directory Request TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      break;

    case 0x11: std::cout << "\tChange Current Directory Request TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
               i_pathNameLength = uint16_t((at_ptrFrame->dataOctet(3)<<8) | (at_ptrFrame->dataOctet(2)));
               std::cout << " Path Name Length : " << i_pathNameLength;
               std::cout << " Path Name : ";
              for (uint16_t i = 0; i < i_pathNameLength; i++)
              {
                std::cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
              }
      break;

    case 0x20: std::cout << "\tOpen File Request TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));

              if (at_ptrFrame->dataOctet(2) == 0) std::cout << " Open file for reading only";
              if (at_ptrFrame->dataOctet(2) == 1) std::cout << " Open file for writing only";
              if (at_ptrFrame->dataOctet(2) == 2) std::cout << " Open file for reading and writing only";
              if (at_ptrFrame->dataOctet(2) == 3) std::cout << " Open directory";

              (at_ptrFrame->dataOctet(2) & 4)? std::cout << " Create a new File if not existing": std::cout << " Open an existing File(fails if non-existent file)";

              (at_ptrFrame->dataOctet(2) & 8)? std::cout << " Open file for appending data the end of the file": std::cout << " Open file for random access";

              (at_ptrFrame->dataOctet(2) & 16)? std::cout << " Open file with exclusive access(fails if already open)": std::cout << " Open file for shared access";

              i_pathNameLength = uint16_t((at_ptrFrame->dataOctet(4)<<8) | (at_ptrFrame->dataOctet(3)));

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: ";
              for (uint16_t i = 0; i <   i_pathNameLength; i++)
              {
                std::cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
              }
      break;

    case 0x21: std::cout << "\tSeek File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << " Handle: " << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               std::cout << " Position Mode: " << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(3));
               std::cout << " Offset: " << std::hex << ( (static_cast<uint32_t>(at_ptrFrame->dataOctet(7))<<24) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(6))<<16) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(5))<<8) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))) );
      break;

    case 0x22: std::cout << "\tRead File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << " Handle: "     <<std::hex<<static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               std::cout << " Count: "      <<std::hex<<( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) );
               if (at_ptrFrame->dataOctet(5) == 0x00) std::cout << " Do not report hidden files a directory listing";
               if (at_ptrFrame->dataOctet(5) == 0x01) std::cout << " Report hidden files in a directory listing";
               if (at_ptrFrame->dataOctet(5) == 0xFF) std::cout << " Parameter ist not available, FS shall not report hidden in a directory listing";
               if ( (at_ptrFrame->dataOctet(5) >= 0x02) && (at_ptrFrame->dataOctet(5) <= 0xFE) ) std::cout << " ERROR wrong Error Code";
      break;

    case 0x23: std::cout << "\tWrite File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << " Handle: " << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               std::cout << " Number of Byte written Data: " <<std::dec<<( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) );
               std::cout << " Data: ";
               for (int i = 0; i < ( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) ); i++)
               {
                 std::cout << "0x" << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(5 + i)) << " ";
               }
      break;

    case 0x24: std::cout << "\tClose File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << " Handle: " << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
      break;

    case 0x30: std::cout << "\tMove / Copy / Rename File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << ((at_ptrFrame->dataOctet(2) && 0x04)? " recursive" : " not recursive");
               std::cout << ((at_ptrFrame->dataOctet(2) && 0x02)? " force" : " not force");
               std::cout << ((at_ptrFrame->dataOctet(2) && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               std::cout << " Source Path Name Length: "  << std::hex << i_pathNameLength;
               std::cout << " Source Path Name: ";
               for (int i = 0; i < i_pathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(7 + i));
               }

               i_destPathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(6))<<8) | at_ptrFrame->dataOctet(5);

               std::cout << " Destination Path Name Length: " << std::hex << i_destPathNameLength;
               std::cout << " Destination Path Name: ";
               for (int i = 0; i < i_destPathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(7 + i_pathNameLength + i));
               }
      break;

    case 0x31: std::cout << "\tDelete File Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               std::cout << ((at_ptrFrame->dataOctet(2) && 0x04)? " recursive" : " not recursive");
               std::cout << ((at_ptrFrame->dataOctet(2) && 0x02)? " force" : " not force");
               std::cout << ((at_ptrFrame->dataOctet(2) && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: ";
              for (uint16_t i = 0; i <  i_pathNameLength; i++)
              {
                std::cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
              }

      break;
    case 0x32: std::cout << "\tGet Files Attributes Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))<<8) | at_ptrFrame->dataOctet(2);

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
               }
      break;
    case 0x33: std::cout << "\tSet File Attributes Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               if (at_ptrFrame->dataOctet(2) == 0x1C) std::cout <<" Clear read-only attribute";
               if (at_ptrFrame->dataOctet(2) == 0x1D) std::cout <<" Set read-only attribute";
               if (at_ptrFrame->dataOctet(2) == 0x13) std::cout <<" Clear hidden attribute";
               if (at_ptrFrame->dataOctet(2) == 0x17) std::cout <<" Set hidden attribute";

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
               }
      break;
    case 0x34: std::cout << "\tGet File Date & File Time Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))<<8) | at_ptrFrame->dataOctet(2);

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
               }
      break;
    case 0x40: std::cout << "\tInitialize Volume Request TAN: " << std::setw(2) << std::hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               std::cout << " Space : "        <<std::hex<< ((static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 24)
			                                    | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16)
												| (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) <<  8)
												| (static_cast<uint32_t>(at_ptrFrame->dataOctet(2))      ));
               if (at_ptrFrame->dataOctet(6) == 0x00) std::cout <<" create  a new volume using all available space";
               if (at_ptrFrame->dataOctet(6) == 0x02) std::cout <<" create  a new volume using space specified";
               if (at_ptrFrame->dataOctet(6) == 0x01) std::cout <<" Overwrite the existing volume";

               i_pathNameLength = static_cast<uint16_t>(at_ptrFrame->dataOctet(8)<<8) | at_ptrFrame->dataOctet(7);

               std::cout << " Path Name Length: " << std::hex << i_pathNameLength;
               std::cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 std::cout << uint8_t(at_ptrFrame->dataOctet(9 + i));
               }
      break;
    default: return;
  }
}





void interpretePgn(uint32_t);

void interpretePgnAcknowledge(PtrDataFrame_t at_ptrFrame)
{
  (void)at_ptrFrame;
}

void interpreteWheelBasedSpeedDist(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Wheel-based implement speed
      std::cout << std::dec << "WBISp: " << ((at_ptrFrame->dataOctet(1) << 8) | at_ptrFrame->dataOctet(0)) * 0.001 << " m/s ";

      // Wheel-based distance
      std::cout << "WBIDs: " << static_cast<uint32_t>( (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 24)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 8)
                                                     |  static_cast<uint32_t>(at_ptrFrame->dataOctet(2))
                                                     ) * 0.001 << " m ";

      // Maximum time of tractor power
      std::cout << "MaxTimeTP: " << static_cast<uint16_t>(at_ptrFrame->dataOctet(6)) << " min ";

      // Operator direction reversed
      switch (at_ptrFrame->dataOctet(7) >> 6) {
          case 0x0: std::cout << "Operator direction: NotReversed "; break;
          case 0x1: std::cout << "Operator direction: Reversed "; break;
          case 0x2: std::cout << "Operator direction: Error "; break;
          case 0x3: std::cout << "Operator direction: Not available "; break;
      }

      // Start/Stop state
      switch ((at_ptrFrame->dataOctet(7) >> 4) & 0x03) {
        case 0x0: std::cout << "St/Sp state: stop "; break;
        case 0x1: std::cout << "St/Sp state: start "; break;
        case 0x2: std::cout << "St/Sp state: error "; break;
        case 0x3: std::cout << "St/Sp state: n/a "; break;
      }

      // Key switch state
      switch ((at_ptrFrame->dataOctet(7) >> 2) & 0x03) {
        case 0x0: std::cout << "Key sws: off "; break;
        case 0x1: std::cout << "Key sws: not off "; break;
        case 0x2: std::cout << "Key sws: error "; break;
        case 0x3: std::cout << "Key sws: n/a "; break;
      }

      // Ground based direction
      switch (at_ptrFrame->dataOctet(7) & 0x03) {
        case 0x0: std::cout << "WB Dir: Reverse "; break;
        case 0x1: std::cout << "WB Dir: Forward "; break;
        case 0x2: std::cout << "WB Dir: Error "; break;
        case 0x3: std::cout << "WB Dir: N/A "; break;
      }
  }
}

void interpreteGroundBasedSpeedDist(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Wheel-based implement speed
      std::cout << std::dec << "GBISp: " << ((at_ptrFrame->dataOctet(1) << 8) | at_ptrFrame->dataOctet(0)) * 0.001 << " m/s ";

      // Wheel-based distance
      std::cout << "GBIDs: " << static_cast<uint32_t>( (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 24)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 8)
                                                     |  static_cast<uint32_t>(at_ptrFrame->dataOctet(2))
                                                     ) * 0.001 << " m ";

      // Byte 7 reserved.

      // Bit 8 to 3 reserved

      // Ground based direction
      switch (at_ptrFrame->dataOctet(7) & 0x03) {
        case 0x0: std::cout << "GB Dir: Reverse "; break;
        case 0x1: std::cout << "GB Dir: Forward "; break;
        case 0x2: std::cout << "GB Dir: Error "; break;
        case 0x3: std::cout << "GB Dir: N/A "; break;
      }
  }
}

void interpreteRearPTOstate(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear PTO output shaft speed
      std::cout << std::dec << "Rear PTO output shaft speed: " << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(1)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(0)) ) * 0.125 << " 1/min ";

      // Rear PTO output shaft speed setpoint
      std::cout << std::dec << "Rear PTO output shaft speed setpoint: " << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(2)) ) * 0.125 << " 1/min ";

      // Rear PTO engagement
      switch (at_ptrFrame->dataOctet(4) >> 6) {
          case 0x0: std::cout << "RPTO eng: disen "; break;
          case 0x1: std::cout << "RPTO eng: en "; break;
          case 0x2: std::cout << "RPTO eng: Error "; break;
          case 0x3: std::cout << "RPTO eng: n/a "; break;
      }

      // Rear PTO mode
      switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x03) {
        case 0x0: std::cout << "RPTO mod: 540 "; break;
        case 0x1: std::cout << "RPTO mod: 1000 "; break;
        case 0x2: std::cout << "RPTO mod: Error "; break;
        case 0x3: std::cout << "RPTO mod: n/a "; break;
      }

      // Rear PTO economy mode
      switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x03) {
        case 0x0: std::cout << "RPTO ec mod: disen "; break;
        case 0x1: std::cout << "RPTO ec mod: en "; break;
        case 0x2: std::cout << "RPTO ec mod: error "; break;
        case 0x3: std::cout << "RPTO ec mod: n/a "; break;
      }

      // Rear PTO mode request status
      switch (at_ptrFrame->dataOctet(4) & 0x03) {
        case 0x0: std::cout << "RPTO mod rq st: disen "; break;
        case 0x1: std::cout << "RPTO mod rq st: en "; break;
        case 0x2: std::cout << "RPTO mod rq st: Error "; break;
        case 0x3: std::cout << "RPTO mod rq st: N/A "; break;
      }

      // Rear PTO mode
      switch (at_ptrFrame->dataOctet(5) >> 6) {
          case 0x0: std::cout << "RPTO mode: external request "; break;
          case 0x1: std::cout << "RPTO mode: controll override "; break;
          case 0x2: std::cout << "RPTO mode: Error "; break;
          case 0x3: std::cout << "RPTO mode: n/a "; break;
      }

      // Rear PTO Economy mode request status
      switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x03) {
        case 0x0: std::cout << "RPTO economy mode request status: external request "; break;
        case 0x1: std::cout << "RPTO economy mode request status: controll override "; break;
        case 0x2: std::cout << "RPTO economy mode request status: Error "; break;
        case 0x3: std::cout << "RPTO economy mode request status: n/a "; break;
      }

      // Rear PTO shaft speed limit status
      switch ((at_ptrFrame->dataOctet(5) >> 1) & 0x07) {
        case 0x0: std::cout << "RPTO shaft speed limit status: no limit "; break;
        case 0x1: std::cout << "RPTO shaft speed limit status: op limit "; break;
        case 0x2: std::cout << "RPTO shaft speed limit status: lim high "; break;
        case 0x3: std::cout << "RPTO shaft speed limit status: lim low "; break;
        case 0x4: std::cout << "RPTO shaft speed limit status: reserved "; break;
        case 0x5: std::cout << "RPTO shaft speed limit status: reserved "; break;
        case 0x6: std::cout << "RPTO shaft speed limit status: nr fault "; break;
        case 0x7: std::cout << "RPTO shaft speed limit status: n/a "; break;
      }

      // Bits 1 is reserved
      // Bytes 7 and 8 are reserved
  }
}

void interpreteRearHitch(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear hitch position in %
      std::cout << std::dec << "RearHitchPos: " << at_ptrFrame->dataOctet(0) * 0.4 << "% ";

      // Rear hith in-work indication.
      switch (at_ptrFrame->dataOctet(1) >> 6) {
          case 0x0: std::cout << "RH in-wo ind: out-of-work "; break;
          case 0x1: std::cout << "RH in-wo ind: in-work "; break;
          case 0x2: std::cout << "RH in-wo ind: error "; break;
          case 0x3: std::cout << "RH in-wo ind: n/a "; break;
      }

      // Rear hitch position limit status
      switch ((at_ptrFrame->dataOctet(1) >> 3) & 0x07) {
        case 0x0: std::cout << "RH pos lim st: no limit "; break;
        case 0x1: std::cout << "RH pos lim st: op limit "; break;
        case 0x2: std::cout << "RH pos lim st: lim high "; break;
        case 0x3: std::cout << "RH pos lim st: lim low "; break;
        case 0x4: std::cout << "RH pos lim st: reserved "; break;
        case 0x5: std::cout << "RH pos lim st: reserved "; break;
        case 0x6: std::cout << "RH pos lim st: nr fault "; break;
        case 0x7: std::cout << "RH pos lim st: n/a "; break;
      }

      // Rear nominal lover link force
      std::cout << std::dec << "Rear nom low link force: " << int8_t(at_ptrFrame->dataOctet(2)) * 0.8 << "% ";

      // Rear draft
      std::cout << std::dec << "Rear draft: " << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) ) * 10 - 320000 << "N ";

      // Bits 6 to 8 are reserved
  }
}

void interpretePgnLanguage(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Language Code
    std::cout << "LC:" << at_ptrFrame->asciiDataOctet(0) << at_ptrFrame->asciiDataOctet(1) << " ";

    // Decimal Point
    switch (at_ptrFrame->dataOctet(2) >> 6) {
      case 0x0: std::cout << "dp:, "; break;
      case 0x1: std::cout << "dp:. "; break;
      case 0x2: std::cout << "dp:R "; break;
      case 0x3: std::cout << "dp:N "; break;
    }
    // Time Format
    switch ((at_ptrFrame->dataOctet(2) >> 4) & 0x03) {
      case 0x0: std::cout << "t:24 "; break;
      case 0x1: std::cout << "t:12 "; break;
      case 0x2: std::cout << "t:R  "; break;
      case 0x3: std::cout << "t:N  "; break;
    }
    // Date Format
    switch (at_ptrFrame->dataOctet(3)) {
      case 0x0: std::cout << "d:ddmmyyyy "; break;
      case 0x1: std::cout << "d:ddyyyymm "; break;
      case 0x2: std::cout << "d:mmyyyydd "; break;
      case 0x3: std::cout << "d:mmddyyyy "; break;
      case 0x4: std::cout << "d:yyyymmdd "; break;
      case 0x5: std::cout << "d:yyyyddmm "; break;
      default:  std::cout << "d:UNSPECIF "; break;
    }
    // Distance
    switch ((at_ptrFrame->dataOctet(4) >> 6) & 0x3) {
      case 0x0: std::cout << "d:M  "; break;
      case 0x1: std::cout << "d:IU "; break;
      case 0x2: std::cout << "d:R  "; break;
      case 0x3: std::cout << "d:N  "; break;
    }
    // Area
    switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x3) {
      case 0x0: std::cout << "a:M  "; break;
      case 0x1: std::cout << "a:IU "; break;
      case 0x2: std::cout << "a:R  "; break;
      case 0x3: std::cout << "a:N  "; break;
    }
    // Volume
    switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x3) {
      case 0x0: std::cout << "v:M "; break;
      case 0x1: std::cout << "v:I "; break;
      case 0x2: std::cout << "v:U "; break;
      case 0x3: std::cout << "v:N "; break;
    }
    // Mass
    switch ((at_ptrFrame->dataOctet(4) >> 0) & 0x3) {
      case 0x0: std::cout << "m:M "; break;
      case 0x1: std::cout << "m:I "; break;
      case 0x2: std::cout << "m:U "; break;
      case 0x3: std::cout << "m:N "; break;
    }
    // Temperature
    switch ((at_ptrFrame->dataOctet(5) >> 6) & 0x3) {
      case 0x0: std::cout << "t:M  "; break;
      case 0x1: std::cout << "t:IU "; break;
      case 0x2: std::cout << "t:R  "; break;
      case 0x3: std::cout << "t:N  "; break;
    }
    // Pressure
    switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x3) {
      case 0x0: std::cout << "p:M  "; break;
      case 0x1: std::cout << "p:IU "; break;
      case 0x2: std::cout << "p:R  "; break;
      case 0x3: std::cout << "p:N  "; break;
    }
    // Force
    switch ((at_ptrFrame->dataOctet(5) >> 2) & 0x3) {
      case 0x0: std::cout << "f:M  "; break;
      case 0x1: std::cout << "f:IU "; break;
      case 0x2: std::cout << "f:R  "; break;
      case 0x3: std::cout << "f:N  "; break;
    }
    // System
    switch ((at_ptrFrame->dataOctet(5) >> 0) & 0x3) {
      case 0x0: std::cout << "s:M "; break;
      case 0x1: std::cout << "s:I "; break;
      case 0x2: std::cout << "s:U "; break;
      case 0x3: std::cout << "s:N "; break;
    }
  }
}

void interpreteLightingCommand(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Tractor high-beam head lights
    switch ((at_ptrFrame->dataOctet(0) >> 6) & 0x3) {
      case 0x0: std::cout << "Tractor high-beam head lights: deactivate "; break;
      case 0x1: std::cout << "Tractor high-beam head lights: activate "; break;
      case 0x2: std::cout << "Tractor high-beam head lights: reserve "; break;
      case 0x3: std::cout << "Tractor high-beam head lights: don't care "; break;
    }
    // Tractor low-beam head lights
    switch ((at_ptrFrame->dataOctet(0) >> 4) & 0x3) {
      case 0x0: std::cout << "Tractor low-beam head lights: deactivate "; break;
      case 0x1: std::cout << "Tractor low-beam head lights: activate "; break;
      case 0x2: std::cout << "Tractor low-beam head lights: reserve "; break;
      case 0x3: std::cout << "Tractor low-beam head lights: don't care "; break;
    }
    // Tractor alternate head lights
    switch ((at_ptrFrame->dataOctet(0) >> 2) & 0x3) {
      case 0x0: std::cout << "Tractor alternate head lights: deactivate "; break;
      case 0x1: std::cout << "Tractor alternate head lights: activate "; break;
      case 0x2: std::cout << "Tractor alternate head lights: reserve "; break;
      case 0x3: std::cout << "Tractor alternate head lights: don't care "; break;
    }
    // Daytime running lights
    switch ((at_ptrFrame->dataOctet(0) >> 0) & 0x3) {
      case 0x0: std::cout << "Daytime running lights: deactivate "; break;
      case 0x1: std::cout << "Daytime running lights: activate "; break;
      case 0x2: std::cout << "Daytime running lights: reserve "; break;
      case 0x3: std::cout << "Daytime running lights: don't care "; break;
    }


    // Left-turn signal lights
    switch ((at_ptrFrame->dataOctet(1) >> 6) & 0x3) {
      case 0x0: std::cout << "Left-turn signal lights: deactivate "; break;
      case 0x1: std::cout << "Left-turn signal lights: activate "; break;
      case 0x2: std::cout << "Left-turn signal lights: reserve "; break;
      case 0x3: std::cout << "Left-turn signal lights: don't care "; break;
    }
    // Right-turn signal lights
    switch ((at_ptrFrame->dataOctet(1) >> 4) & 0x3) {
      case 0x0: std::cout << "Right-turn signal lights: deactivate "; break;
      case 0x1: std::cout << "Right-turn signal lights: activate "; break;
      case 0x2: std::cout << "Right-turn signal lights: reserve "; break;
      case 0x3: std::cout << "Right-turn signal lights: don't care "; break;
    }
    // Beacon lights
    switch ((at_ptrFrame->dataOctet(1) >> 2) & 0x3) {
      case 0x0: std::cout << "Beacon lights: deactivate "; break;
      case 0x1: std::cout << "Beacon lights: activate "; break;
      case 0x2: std::cout << "Beacon lights: reserve "; break;
      case 0x3: std::cout << "Beacon lights: don't care "; break;
    }
    // Front fog lights
    switch ((at_ptrFrame->dataOctet(1) >> 0) & 0x3) {
      case 0x0: std::cout << "Front fog: deactivate "; break;
      case 0x1: std::cout << "Front fog: activate "; break;
      case 0x2: std::cout << "Front fog: reserve "; break;
      case 0x3: std::cout << "Front fog: don't care "; break;
    }


    // Left stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 6) & 0x3) {
      case 0x0: std::cout << "Left stop lights: deactivate "; break;
      case 0x1: std::cout << "Left stop lights: activate "; break;
      case 0x2: std::cout << "Left stop lights: reserve "; break;
      case 0x3: std::cout << "Left stop lights: don't care "; break;
    }
    // Right stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 4) & 0x3) {
      case 0x0: std::cout << "Right stop lights: deactivate "; break;
      case 0x1: std::cout << "Right stop lights: activate "; break;
      case 0x2: std::cout << "Right stop lights: reserve "; break;
      case 0x3: std::cout << "Right stop lights: don't care "; break;
    }
    // Center stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 2) & 0x3) {
      case 0x0: std::cout << "Center stop lights: deactivate "; break;
      case 0x1: std::cout << "Center stop lights: activate "; break;
      case 0x2: std::cout << "Center stop lights: reserve "; break;
      case 0x3: std::cout << "Center stop lights: don't care "; break;
    }
    // Back up light and alarm horn
    switch ((at_ptrFrame->dataOctet(2) >> 0) & 0x3) {
      case 0x0: std::cout << "Back up light and alarm horn: deactivate "; break;
      case 0x1: std::cout << "Back up light and alarm horn: activate "; break;
      case 0x2: std::cout << "Back up light and alarm horn: reserve "; break;
      case 0x3: std::cout << "Back up light and alarm horn: don't care "; break;
    }


    // Tractor marker lights
    switch ((at_ptrFrame->dataOctet(3) >> 6) & 0x3) {
      case 0x0: std::cout << "Tractor marker lights: deactivate "; break;
      case 0x1: std::cout << "Tractor marker lights: activate "; break;
      case 0x2: std::cout << "Tractor marker lights: reserve "; break;
      case 0x3: std::cout << "Tractor marker lights: don't care "; break;
    }
    // Implement marker lights
    switch ((at_ptrFrame->dataOctet(3) >> 4) & 0x3) {
      case 0x0: std::cout << "Implement marker lights: deactivate "; break;
      case 0x1: std::cout << "Implement marker lights: activate "; break;
      case 0x2: std::cout << "Implement marker lights: reserve "; break;
      case 0x3: std::cout << "Implement marker lights: don't care "; break;
    }
    // Tractor clearance lights
    switch ((at_ptrFrame->dataOctet(3) >> 2) & 0x3) {
      case 0x0: std::cout << "Tractor clearance lights: deactivate "; break;
      case 0x1: std::cout << "Tractor clearance lights: activate "; break;
      case 0x2: std::cout << "Tractor clearance lights: reserve "; break;
      case 0x3: std::cout << "Tractor clearance lights: don't care "; break;
    }
    // Implement clearance lights
    switch ((at_ptrFrame->dataOctet(3) >> 0) & 0x3) {
      case 0x0: std::cout << "Implement clearance lights: deactivate "; break;
      case 0x1: std::cout << "Implement clearance lights: activate "; break;
      case 0x2: std::cout << "Implement clearance lights: reserve "; break;
      case 0x3: std::cout << "Implement clearance lights: don't care "; break;
    }


    // Tractor rear hight mounted work lights
    switch ((at_ptrFrame->dataOctet(4) >> 6) & 0x3) {
      case 0x0: std::cout << "Tractor rear hight mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor rear hight mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor rear hight mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor rear hight mounted work lights: don't care "; break;
    }
    // Tractor rear low mounted work lights
    switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x3) {
      case 0x0: std::cout << "Tractor rear low mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor rear low mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor rear low mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor rear low mounted work lights: don't care "; break;
    }
    // Tractor underside work lights
    switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x3) {
      case 0x0: std::cout << "Tractor underside work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor underside work lights: activate "; break;
      case 0x2: std::cout << "Tractor underside work lights: reserve "; break;
      case 0x3: std::cout << "Tractor underside work lights: don't care "; break;
    }
    // Rear fog light
    switch ((at_ptrFrame->dataOctet(4) >> 0) & 0x3) {
      case 0x0: std::cout << "Rear fog light: deactivate "; break;
      case 0x1: std::cout << "Rear fog light: activate "; break;
      case 0x2: std::cout << "Rear fog light: reserve "; break;
      case 0x3: std::cout << "Rear fog light: don't care "; break;
    }


    // Tractor front high-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 6) & 0x3) {
      case 0x0: std::cout << "Tractor front high-mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor front high-mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor front high-mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor front high-mounted work lights: don't care "; break;
    }
    // Tractor front low-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x3) {
      case 0x0: std::cout << "Tractor front low-mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor front low-mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor front low-mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor front low-mounted work lights: don't care "; break;
    }
    // Tractor side high-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 2) & 0x3) {
      case 0x0: std::cout << "Tractor side high-mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor side high-mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor side high-mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor side high-mounted work lights: don't care "; break;
    }
    // Tractor side low-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 0) & 0x3) {
      case 0x0: std::cout << "Tractor side low-mounted work lights: deactivate "; break;
      case 0x1: std::cout << "Tractor side low-mounted work lights: activate "; break;
      case 0x2: std::cout << "Tractor side low-mounted work lights: reserve "; break;
      case 0x3: std::cout << "Tractor side low-mounted work lights: don't care "; break;
    }


    // Implement left forward work lights
    switch ((at_ptrFrame->dataOctet(6) >> 6) & 0x3) {
      case 0x0: std::cout << "Implement left forward work lights: deactivate "; break;
      case 0x1: std::cout << "Implement left forward work lights: activate "; break;
      case 0x2: std::cout << "Implement left forward work lights: reserve "; break;
      case 0x3: std::cout << "Implement left forward work lights: don't care "; break;
    }
    // Implement right forward work lights
    switch ((at_ptrFrame->dataOctet(6) >> 4) & 0x3) {
      case 0x0: std::cout << "Implement right forward work lights: deactivate "; break;
      case 0x1: std::cout << "Implement right forward work lights: activate "; break;
      case 0x2: std::cout << "Implement right forward work lights: reserve "; break;
      case 0x3: std::cout << "Implement right forward work lights: don't care "; break;
    }
    // Implement OEM option 1 light
    switch ((at_ptrFrame->dataOctet(6) >> 2) & 0x3) {
      case 0x0: std::cout << "Implement OEM option 1 light: deactivate "; break;
      case 0x1: std::cout << "Implement OEM option 1 light: activate "; break;
      case 0x2: std::cout << "Implement OEM option 1 light: reserve "; break;
      case 0x3: std::cout << "Implement OEM option 1 light: don't care "; break;
    }
    // Implement OEM option 2 light
    switch ((at_ptrFrame->dataOctet(6) >> 0) & 0x3) {
      case 0x0: std::cout << "Implement OEM option 2 light: deactivate "; break;
      case 0x1: std::cout << "Implement OEM option 2 light: activate "; break;
      case 0x2: std::cout << "Implement OEM option 2 light: reserve "; break;
      case 0x3: std::cout << "Implement OEM option 2 light: don't care "; break;
    }


    // Implement rear work lights
    switch ((at_ptrFrame->dataOctet(7) >> 6) & 0x3) {
      case 0x0: std::cout << "Implement rear work lights: deactivate "; break;
      case 0x1: std::cout << "Implement rear work lights: activate "; break;
      case 0x2: std::cout << "Implement rear work lights: reserve "; break;
      case 0x3: std::cout << "Implement rear work lights: don't care "; break;
    }
    // Implement left-facing work lights
    switch ((at_ptrFrame->dataOctet(7) >> 4) & 0x3) {
      case 0x0: std::cout << "Implement left-facing work lights: deactivate "; break;
      case 0x1: std::cout << "Implement left-facing work lights: activate "; break;
      case 0x2: std::cout << "Implement left-facing work lights: reserve "; break;
      case 0x3: std::cout << "Implement left-facing work lights: don't care "; break;
    }
    // Implement right-facing work lights
    switch ((at_ptrFrame->dataOctet(7) >> 2) & 0x3) {
      case 0x0: std::cout << "Implement right-facing work lights: deactivate "; break;
      case 0x1: std::cout << "Implement right-facing work lights: activate "; break;
      case 0x2: std::cout << "Implement right-facing work lights: reserve "; break;
      case 0x3: std::cout << "Implement right-facing work lights: don't care "; break;
    }
    // Lighting data message request
    switch ((at_ptrFrame->dataOctet(7) >> 0) & 0x3) {
      case 0x0: std::cout << "Lighting data message request: deactivate "; break;
      case 0x1: std::cout << "Lighting data message request: activate "; break;
      case 0x2: std::cout << "Lighting data message request: reserve "; break;
      case 0x3: std::cout << "Lighting data message request: don't care "; break;
    }
  }
}

void interpreteEngineSpeedMsg(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Engine speed
    std::cout << "Engine speed:" << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) ) * 0.125 << " ";
  }
}

void interpreteVehicleSpeed(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Vehicle speed
    // J1939-71
    // 2 bytes Compass bearing 1/128 deg/bit, 0 offset, range 0 to 501.99deg
    // 2 bytes Naviagion based vehicle speed 1/256 km/h per bit, 0 offset, datarange 0 to 250.996 km/h
    // 2 bytes pitch 1/128 deg/bit, -200 deg offset, -200 to 301.99 deg
    // 2 bytes altitude 0.125 m/bit, -2500 m offset, -2500 to 5531.875 m
    std::cout << "Compass bearing:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(1)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(0)) ) / 128) << "° "
         << "Naviagion based vehicle speed:" << static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(2)) ) / 256 << " km/h "
         << "pitch:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(5)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) ) / 128) - 200 << "° "
         << "altitude:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(7)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(6)) ) * 0.125 ) - 2500 << " m ";
  }
}

void interpreteVehiclePosition(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Vehicle position
    // J1939-71
    // 4 bytes Latitude  10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    // 4 bytes Longitude 10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    std::cout << "Latitude: " << (static_cast<uint32_t>( (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 24)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(2)) << 16)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(1)) << 8)
                                                     |  static_cast<uint32_t>(at_ptrFrame->dataOctet(0))
                                                     ) * 0.0000001) - 210 << "° "
         << "Longitude: " << (static_cast<uint32_t>( (static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 24)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 16)
                                                     | (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 8)
                                                     |  static_cast<uint32_t>(at_ptrFrame->dataOctet(4))
                                                     ) * 0.0000001) - 210 << "° ";
  }
}

/** For extend port measured flow, retract port measured flow, extend
  * port estimated flow, retract port estimated flow.
  */
inline int portMeasuredOrEstimatedFlowPercent(uint8_t ui8_octet)
{
  return int(ui8_octet) - 125;
}

std::string auxiliaryValveFailSaveModeMeasuredRepr(unsigned ui_2lsb)
{
  switch (ui_2lsb & 0x3u) {
  case 0u:
    return "block";
  case 1u:
    return "float";
  case 2u:
    return "error indication";
  case 3u:
    return "not available";
  default:
    return "(protocol error)";
  }
}

std::string valveStateRepr(unsigned ui_4lsb)
{
  switch (ui_4lsb & 0xFu) {
  case 0u:
    return "blocked";
  case 1u:
    return "extend";
  case 2u:
    return "retract";
  case 3u:
    return "floating";
  default:
    return "reserved";
  case 14u:
    return "error indication";
  case 15u:
    return "not available";
  }
}

std::string persistentLimitStatusRepr(unsigned ui_3lsb)
{
  switch (ui_3lsb & 0x7u) {
  case 0u:
    return "not limited";
  case 1u:
    return "operator limited/controlled";
  case 2u:
    return "limited high";
  case 3u:
    return "limited low";
  default:
    return "reserved";
  case 6u:
    return "non-recoverable fault";
  case 7u:
    return "not available";
  }
}

void interpreteValveEstimatedFlow(PtrDataFrame_t at_ptrFrame)
{
  // for different valves resp. different values of at_ptrFrame->pgn()
  std::cout <<
    "Extend port estimated flow: " << portMeasuredOrEstimatedFlowPercent(at_ptrFrame->dataOctet(0)) << "% " <<
    "Retract port estimated flow: " << portMeasuredOrEstimatedFlowPercent(at_ptrFrame->dataOctet(1)) << "% " <<
    "Fail save mode - measured: " << auxiliaryValveFailSaveModeMeasuredRepr(unsigned(at_ptrFrame->dataOctet(2)) >> 6) << " " <<
    "Valve state: " << valveStateRepr(at_ptrFrame->dataOctet(2)) << " " <<
    "Limit status: " << persistentLimitStatusRepr(at_ptrFrame->dataOctet(3) >> 5);
}

inline double extendOrRetractPortPressureRepr(
    std::vector< uint8_t > const &ar_data,
    size_t t_offset)
{
  return 5e3 * (0x100u * ar_data[t_offset+1] + ar_data[t_offset]); // unit: Pa
}

inline double returnPortPressureRepr(unsigned at_octet)
{
  return 16e3 * at_octet; // unit: Pa
}

void interpreteValveMeasuredFlow(PtrDataFrame_t at_ptrFrame)
{
  // for different valves resp. different values of at_ptrFrame->pgn()
  std::cout <<
    "Extend port measured flow: " << portMeasuredOrEstimatedFlowPercent(at_ptrFrame->dataOctet(0)) << "% " <<
    "Retract port measured flow: " << portMeasuredOrEstimatedFlowPercent(at_ptrFrame->dataOctet(1)) << "% " <<
    "Extend port pressure: " << std::scientific << extendOrRetractPortPressureRepr(at_ptrFrame->data(), 2) << " Pa " <<
    "Retract port pressure: " << std::scientific << extendOrRetractPortPressureRepr(at_ptrFrame->data(), 4) << " Pa " <<
    "Return port pressure: " << std::scientific << returnPortPressureRepr(at_ptrFrame->dataOctet(6)) << " Pa " <<
    "Limit Status: " << persistentLimitStatusRepr(at_ptrFrame->dataOctet(3) >> 5);
}

inline double commandPortFlowPercent(uint8_t ui8_octet)
{
  return 0.4 * ui8_octet;
}

std::string commandAuxiliaryFailSaveMode(unsigned ui_2lsb)
{
  switch (ui_2lsb & 0x3u) {
  case 0u:
    return "block";
  case 1u:
    return "float";
  case 2u:
    return "reserved";
  case 3u:
    return "don't care";
  default:
    return "(protocol error)";
  }
}

std::string commandAuxiliaryStateRepr(unsigned ui_4lsb)
{
  switch (ui_4lsb & 0xFu) {
  case 0u:
    return "block";
  case 1u:
    return "extend";
  case 2u:
    return "retract";
  case 3u:
    return "float";
  default:
    return "reserved";
  case 15u:
    return "don't care";
  }
}


void interpreteValveCommand(PtrDataFrame_t at_ptrFrame)
{
  // for different valves resp. different values of at_ptrFrame->pgn()
  std::cout <<
    "Auxiliary port flow - command: " << commandPortFlowPercent(at_ptrFrame->dataOctet(0)) << "% " <<
    "Auxiliary fail safe mode - command: " << commandAuxiliaryFailSaveMode(at_ptrFrame->dataOctet(2) >> 6) << " " <<
    "Auxiliary state - command: " << commandAuxiliaryStateRepr(at_ptrFrame->dataOctet(2));
}

TransferCollection_c::PtrConnection_t getTransferConnection(
    TransferCollection_c::Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  TransferCollection_c::PtrConnection_t t_ptrConnection =
    gs_main.mc_trans.getConnection(
        ae_variant,
        aui8_transferSourceAddress,
        aui8_transferDestinationAddress);
  if (!t_ptrConnection) {
    std::cout << " (ERROR, no " <<
      (TransferCollection_c::variant_tp == ae_variant ? "TP" : "ETP") <<
      " " << std::hex << unsigned(aui8_transferSourceAddress) <<
      "->" << std::hex << unsigned(aui8_transferDestinationAddress) <<") ";
  }
  return t_ptrConnection;
}

void endOfTransfer(
    PtrDataFrame_t at_ptrFrame,
    TransferCollection_c::Variant_e ae_variant);

void interpretePgnsTPETP(PtrDataFrame_t at_ptrFrame)
{
  bool b_streamEnd = false;
  TransferCollection_c::Variant_e e_variant = TransferCollection_c::variant_etp;

  if (at_ptrFrame->dataSize() != 8)
  {
    std::cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
    return;
  }

  switch (at_ptrFrame->pgn()) {

  case TP_CONN_MANAGE_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_CONN_MANAGE_PGN:
    switch (at_ptrFrame->dataOctet(0)) {
    case 0x10:
      std::cout << "RTS - Request to Send (TP)            ";
      {
        size_t const ct_sizeTransferData = size_t(at_ptrFrame->dataOctet(2)) << 8 | at_ptrFrame->dataOctet(1);
        (void)gs_main.mc_trans.newConnection(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress(),
            ct_sizeTransferData);
      }
      break;
    case 0x14:
      std::cout << "RTS - Request to Send (ETP)           ";
      {
        size_t const ct_sizeTransferData = (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 24) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(2)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(1)));
        (void)gs_main.mc_trans.newConnection(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress(),
            ct_sizeTransferData);
      }
      break;
    case 0x11:
      std::cout << "CTS - Clear to Send (TP)              ";

      break;
    case 0x15:
      std::cout << "CTS - Clear to Send (ETP)             ";
      break;

    case 0x16:
      std::cout << "DPO - Data Packet Offset (ETP)        ";
      {
        TransferCollection_c::PtrConnection_t t_ptrConnection =
          getTransferConnection(
              e_variant,
              at_ptrFrame->sourceAddress(),
              at_ptrFrame->destinationAddress());
        if (t_ptrConnection)
          t_ptrConnection->mui32_packetOffSet =
            ((static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 8 ) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(2))));
      }
      break;
    case 0x13:
      std::cout << "EoMACK - End of Message Ack (TP)      ";
      b_streamEnd = true;
      break;
    case 0x17:
      std::cout << "EoMACK - End of Message Ack (ETP)     ";
      b_streamEnd = true;
      break;
    case 0x20:
      std::cout << "BAM - Broadcast Announce Msg (TP)     ";
      break;
    case 0xFF:
      std::cout << "CONNABORT - Connection Abort (TP/ETP) ";
      gs_main.mc_trans.deleteConnection(e_variant, at_ptrFrame->sourceAddress(), at_ptrFrame->destinationAddress());
      gs_main.mc_trans.deleteConnection(e_variant, at_ptrFrame->destinationAddress(), at_ptrFrame->sourceAddress());
      break;
    default: return;
    }
    {
      TransferCollection_c::PtrConnection_t t_ptrConnection =
        getTransferConnection(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress());
      if (t_ptrConnection) {
        t_ptrConnection->mui32_embeddedPgn =
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)));
        std::cout << " on " << std::setw(6) << std::setfill('0') << t_ptrConnection->mui32_embeddedPgn << " (";
        interpretePgn(t_ptrConnection->mui32_embeddedPgn);
        std::cout << ")";
      }
    }
    break;

  case TP_DATA_TRANSFER_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_DATA_TRANSFER_PGN:
    std::cout << "DATA - Data Packet #"<<std::setw(2)<<std::setfill(' ')<<std::dec<<uint16_t(at_ptrFrame->dataOctet(0));
    {
      TransferCollection_c::PtrConnection_t t_ptrConnection =
        getTransferConnection(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress());
      if (!t_ptrConnection)
        ;
      else if ((at_ptrFrame->pgn() == TP_DATA_TRANSFER_PGN)) {
        for (int i = 0; i < 7; i++) {
          if ((7 *  (at_ptrFrame->dataOctet(0) - 1)) + i >= int(t_ptrConnection->mvec_data.size()))
            break;
          size_t t_index = (7 *  (at_ptrFrame->dataOctet(0) - 1)) + i;
          (t_ptrConnection->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
        }
      } else if (at_ptrFrame->pgn() == ETP_DATA_TRANSFER_PGN) {
        for (int i = 0; i < 7; i++) {
          size_t t_index = (t_ptrConnection->mui32_packetOffSet * 7 + (7 *  (at_ptrFrame->dataOctet(0) - 1))) + i;
          if (t_index >= t_ptrConnection->mvec_data.size())
            break;
          (t_ptrConnection->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
        }
      }
    }
    break;
  default:
    break;
  }

  if (b_streamEnd) { //instead of obsolete endOfTransfer(e_variant);
    TransferCollection_c::PtrConnection_t t_ptrConnection =
      getTransferConnection(
          e_variant,
          at_ptrFrame->destinationAddress(),
          at_ptrFrame->sourceAddress());
    if (t_ptrConnection) {
      endOfTransfer(at_ptrFrame, e_variant);
    }
  }
}

void endOfTransfer(
    PtrDataFrame_t at_ptrFrame,
    TransferCollection_c::Variant_e ae_variant)
{
  TransferCollection_c::PtrConnection_t t_ptrConnection =
    getTransferConnection(
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  if (t_ptrConnection) {
    PtrDataFrame_t t_ptrArtificialFrame = new DataFrame_c(
        t_ptrConnection->mvec_data,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());

    // output in both ways, dump (raw) and interprete:
    dump(t_ptrArtificialFrame);
    interpretePgnData(t_ptrArtificialFrame);

    gs_main.mc_trans.deleteConnection(
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  }
}

Interprete_t *getPgnDataInterpreter(PtrDataFrame_t at_ptrFrame)
{
  switch (at_ptrFrame->pgn())
  {
  case VT_TO_ECU_PGN:                           return interpretePgnsVtToEcu;
  case ECU_TO_VT_PGN:                           return interpretePgnsVtFromEcu;
  case ACKNOWLEDGEMENT_PGN:                     return interpretePgnAcknowledge;
  case CLIENT_TO_FS_PGN:                        return interpretePgnsCl2Fs;
  case FS_TO_CLIENT_PGN:                        return interpretePgnsFs2Cl;
  case ETP_DATA_TRANSFER_PGN:                   return interpretePgnsTPETP;
  case ETP_CONN_MANAGE_PGN:                     return interpretePgnsTPETP;
  case TP_DATA_TRANSFER_PGN:                    return interpretePgnsTPETP;
  case TP_CONN_MANAGE_PGN:                      return interpretePgnsTPETP;
  case LANGUAGE_PGN:                            return interpretePgnLanguage;
  case LIGHTING_COMMAND_PGN:                    return interpreteLightingCommand;
  case REAR_PTO_STATE_PGN:                      return interpreteRearPTOstate;
  case REAR_HITCH_STATE_PGN:                    return interpreteRearHitch;
  case WHEEL_BASED_SPEED_DIST_PGN:              return interpreteWheelBasedSpeedDist;
  case GROUND_BASED_SPEED_DIST_PGN:             return interpreteGroundBasedSpeedDist;
  case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE:  return interpreteEngineSpeedMsg;
  case VEHICLE_DIRECTION_SPEED:                 return interpreteVehicleSpeed;
  case VEHICLE_POSITION:                        return interpreteVehiclePosition;
  case AUX_VALVE_0_ESTIMATED_FLOW:
  case AUX_VALVE_1_ESTIMATED_FLOW:
  case AUX_VALVE_2_ESTIMATED_FLOW:
  case AUX_VALVE_3_ESTIMATED_FLOW:
  case AUX_VALVE_4_ESTIMATED_FLOW:
  case AUX_VALVE_5_ESTIMATED_FLOW:
  case AUX_VALVE_6_ESTIMATED_FLOW:
  case AUX_VALVE_7_ESTIMATED_FLOW:
  case AUX_VALVE_8_ESTIMATED_FLOW:
  case AUX_VALVE_9_ESTIMATED_FLOW:
  case AUX_VALVE_10_ESTIMATED_FLOW:
  case AUX_VALVE_11_ESTIMATED_FLOW:
  case AUX_VALVE_12_ESTIMATED_FLOW:
  case AUX_VALVE_13_ESTIMATED_FLOW:
  case AUX_VALVE_14_ESTIMATED_FLOW:
  case AUX_VALVE_15_ESTIMATED_FLOW:
    return interpreteValveEstimatedFlow;
  case AUX_VALVE_0_MEASURED_FLOW:
  case AUX_VALVE_1_MEASURED_FLOW:
  case AUX_VALVE_2_MEASURED_FLOW:
  case AUX_VALVE_3_MEASURED_FLOW:
  case AUX_VALVE_4_MEASURED_FLOW:
  case AUX_VALVE_5_MEASURED_FLOW:
  case AUX_VALVE_6_MEASURED_FLOW:
  case AUX_VALVE_7_MEASURED_FLOW:
  case AUX_VALVE_8_MEASURED_FLOW:
  case AUX_VALVE_9_MEASURED_FLOW:
  case AUX_VALVE_10_MEASURED_FLOW:
  case AUX_VALVE_11_MEASURED_FLOW:
  case AUX_VALVE_12_MEASURED_FLOW:
  case AUX_VALVE_13_MEASURED_FLOW:
  case AUX_VALVE_14_MEASURED_FLOW:
  case AUX_VALVE_15_MEASURED_FLOW:
    return interpreteValveMeasuredFlow;
  case AUX_VALVE_0_COMMAND:
  case AUX_VALVE_1_COMMAND:
  case AUX_VALVE_2_COMMAND:
  case AUX_VALVE_3_COMMAND:
  case AUX_VALVE_4_COMMAND:
  case AUX_VALVE_5_COMMAND:
  case AUX_VALVE_6_COMMAND:
  case AUX_VALVE_7_COMMAND:
  case AUX_VALVE_8_COMMAND:
  case AUX_VALVE_9_COMMAND:
  case AUX_VALVE_10_COMMAND:
  case AUX_VALVE_11_COMMAND:
  case AUX_VALVE_12_COMMAND:
  case AUX_VALVE_13_COMMAND:
  case AUX_VALVE_14_COMMAND:
  case AUX_VALVE_15_COMMAND:
    return interpreteValveCommand;
  case PROCESS_DATA_PGN:
  case GUIDANCE_MACHINE_STATUS:
  case GUIDANCE_SYSTEM_CMD:
  case ISOBUS_CERTIFICATION_PGN:
  case REQUEST_PGN_MSG_PGN:
  case ADDRESS_CLAIM_PGN:
  case PROPRIETARY_A_PGN:
  case PROPRIETARY_A2_PGN:
  case WORKING_SET_MEMBER_PGN:
  case WORKING_SET_MASTER_PGN:
  case LIGHTING_DATA_PGN:
  case HITCH_PTO_COMMANDS:
  case FRONT_PTO_STATE_PGN:
  case FRONT_HITCH_STATE_PGN:
  case MAINTAIN_POWER_REQUEST_PGN:
  case SELECTED_SPEED_CMD:
  case SELECTED_SPEED_MESSAGE:
  case SOFTWARE_IDENTIFICATION_PGN:
  case TIME_DATE_PGN:
  case PROPRIETARY_B_PGN:
  case NMEA_GPS_POSITION_RAPID_UPDATE_PGN:
  case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:
  case NMEA_GPS_POSITION_DATA_PGN:
  case NMEA_GPS_DIRECTION_DATA_PGN:
  case NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS:
  default:
    break;
  }
  return 0;
}

void interpretePgnData(PtrDataFrame_t at_ptrFrame)
{
  Interprete_t *p_f = getPgnDataInterpreter(at_ptrFrame);
  if (p_f)
    p_f(at_ptrFrame);
}

void interpretePgn(uint32_t rui32_pgn)
{
  switch (rui32_pgn)
  {
  case VT_TO_ECU_PGN:                           std::cout << "VT_TO_ECU         "; break;
  case ECU_TO_VT_PGN:                           std::cout << "ECU_TO_VT         "; break;
  case ACKNOWLEDGEMENT_PGN:                     std::cout << "ACKNOWLEDGEMENT   "; break;
  case PROCESS_DATA_PGN:                        std::cout << "PROCESS_DATA      "; break;
  case CLIENT_TO_FS_PGN:                        std::cout << "CLIENT_TO_FS      "; break;
  case FS_TO_CLIENT_PGN:                        std::cout << "FS_TO_CLIENT      "; break;
  case GUIDANCE_MACHINE_STATUS:                 std::cout << "GUIDANCE_MACH_ST  "; break;
  case GUIDANCE_SYSTEM_CMD:                     std::cout << "GUIDANCE_SYS_CMD  "; break;
  case ISOBUS_CERTIFICATION_PGN:                std::cout << "ISOBUS_CERTIFICAT."; break;
  case ETP_DATA_TRANSFER_PGN:                   std::cout << "ETP_DATA_TRANSFER "; break;
  case ETP_CONN_MANAGE_PGN:                     std::cout << "ETP_CONN_MANAGE   "; break;
  case REQUEST_PGN_MSG_PGN:                     std::cout << "REQUEST_MSG       "; break;
  case TP_DATA_TRANSFER_PGN:                    std::cout << "TP_DATA_TRANSFER  "; break;
  case TP_CONN_MANAGE_PGN:                      std::cout << "TP_CONN_MANAGE    "; break;
  case ADDRESS_CLAIM_PGN:                       std::cout << "ADDRESS_CLAIM     "; break;
  case PROPRIETARY_A_PGN:                       std::cout << "PROPRIETARY_A     "; break;
  case PROPRIETARY_A2_PGN:                      std::cout << "PROPRIETARY_A2    "; break;
  case WORKING_SET_MEMBER_PGN:                  std::cout << "WORKING_SET_MEMBER "; break;
  case WORKING_SET_MASTER_PGN:                  std::cout << "WORKING_SET_MASTER "; break;
  case LANGUAGE_PGN:                            std::cout << "LANGUAGE          "; break;
  case LIGHTING_DATA_PGN:                       std::cout << "LIGHTING_DATA     "; break;
  case LIGHTING_COMMAND_PGN:                    std::cout << "LIGHTING_COMMAND  "; break;
  case HITCH_PTO_COMMANDS:                      std::cout << "HITCH_PTO_COMMANDS "; break;
  case REAR_PTO_STATE_PGN:                      std::cout << "REAR_PTO_STATE    "; break;
  case FRONT_PTO_STATE_PGN:                     std::cout << "FRONT_PTO_STATE   "; break;
  case REAR_HITCH_STATE_PGN:                    std::cout << "REAR_HITCH_STATE  "; break;
  case FRONT_HITCH_STATE_PGN:                   std::cout << "FRONT_HITCH_STATE "; break;
  case MAINTAIN_POWER_REQUEST_PGN:              std::cout << "MAINTAIN_POWER_REQ"; break;
  case WHEEL_BASED_SPEED_DIST_PGN:              std::cout << "WHEEL_BASED_SPEED_DIST "; break;
  case GROUND_BASED_SPEED_DIST_PGN:             std::cout << "GROUND_BASED_SPEED_DIST "; break;
  case SELECTED_SPEED_CMD:                      std::cout << "SELECTED_SPEED_CMD "; break;
  case SELECTED_SPEED_MESSAGE:                  std::cout << "SELECTED_SPEED_MESSAGE "; break;
  case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE:  std::cout << "ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE "; break;
  case SOFTWARE_IDENTIFICATION_PGN:             std::cout << "SOFTWARE_IDENTIFICATION "; break;
  case TIME_DATE_PGN:                           std::cout << "TIME_DATE         "; break;
  case VEHICLE_DIRECTION_SPEED:                 std::cout << "VEHICLE_DIRECTION_SPEED "; break;
  case VEHICLE_POSITION:                        std::cout << "VEHICLE_POSITION "; break;
  case PROPRIETARY_B_PGN:                       std::cout << "PROPRIETARY_B(1of) "; break;
  case NMEA_GPS_POSITION_RAPID_UPDATE_PGN:      std::cout << "NMEA_GPS_POSITION_RAPID_UPDATE "; break;
  case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:       std::cout << "NMEA_GPS_COG_SOG_RAPID_UPDATE "; break;
  case NMEA_GPS_POSITION_DATA_PGN:              std::cout << "NMEA_GPS_POSITION_DATA "; break;
  case NMEA_GPS_DIRECTION_DATA_PGN:             std::cout << "NMEA_GPS_DIRECTION_DATA "; break;
  case NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS:  std::cout << "GNSS Pseudorange Noise Statistics "; break;

  case AUX_VALVE_0_ESTIMATED_FLOW:
  case AUX_VALVE_1_ESTIMATED_FLOW:
  case AUX_VALVE_2_ESTIMATED_FLOW:
  case AUX_VALVE_3_ESTIMATED_FLOW:
  case AUX_VALVE_4_ESTIMATED_FLOW:
  case AUX_VALVE_5_ESTIMATED_FLOW:
  case AUX_VALVE_6_ESTIMATED_FLOW:
  case AUX_VALVE_7_ESTIMATED_FLOW:
  case AUX_VALVE_8_ESTIMATED_FLOW:
  case AUX_VALVE_9_ESTIMATED_FLOW:
  case AUX_VALVE_10_ESTIMATED_FLOW:
  case AUX_VALVE_11_ESTIMATED_FLOW:
  case AUX_VALVE_12_ESTIMATED_FLOW:
  case AUX_VALVE_13_ESTIMATED_FLOW:
  case AUX_VALVE_14_ESTIMATED_FLOW:
  case AUX_VALVE_15_ESTIMATED_FLOW:
    std::cout << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_ESTIMATED_FLOW << "_ESTIMATED_FLOW ";
    break;
  case AUX_VALVE_0_MEASURED_FLOW:
  case AUX_VALVE_1_MEASURED_FLOW:
  case AUX_VALVE_2_MEASURED_FLOW:
  case AUX_VALVE_3_MEASURED_FLOW:
  case AUX_VALVE_4_MEASURED_FLOW:
  case AUX_VALVE_5_MEASURED_FLOW:
  case AUX_VALVE_6_MEASURED_FLOW:
  case AUX_VALVE_7_MEASURED_FLOW:
  case AUX_VALVE_8_MEASURED_FLOW:
  case AUX_VALVE_9_MEASURED_FLOW:
  case AUX_VALVE_10_MEASURED_FLOW:
  case AUX_VALVE_11_MEASURED_FLOW:
  case AUX_VALVE_12_MEASURED_FLOW:
  case AUX_VALVE_13_MEASURED_FLOW:
  case AUX_VALVE_14_MEASURED_FLOW:
  case AUX_VALVE_15_MEASURED_FLOW:
    std::cout << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_MEASURED_FLOW << "_MEASURED_FLOW ";
    break;
  case AUX_VALVE_0_COMMAND:
  case AUX_VALVE_1_COMMAND:
  case AUX_VALVE_2_COMMAND:
  case AUX_VALVE_3_COMMAND:
  case AUX_VALVE_4_COMMAND:
  case AUX_VALVE_5_COMMAND:
  case AUX_VALVE_6_COMMAND:
  case AUX_VALVE_7_COMMAND:
  case AUX_VALVE_8_COMMAND:
  case AUX_VALVE_9_COMMAND:
  case AUX_VALVE_10_COMMAND:
  case AUX_VALVE_11_COMMAND:
  case AUX_VALVE_12_COMMAND:
  case AUX_VALVE_13_COMMAND:
  case AUX_VALVE_14_COMMAND:
  case AUX_VALVE_15_COMMAND:
    std::cout << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_COMMAND << "_COMMAND ";
    break;
  default:
    std::cout << std::hex << "0x" << rui32_pgn << std::dec;
    break;
  }
}

ParseLogLine_t *getLogLineParser(size_t at_choice)
{
  static ParseLogLine_t * const carrpt_parseLogLine[] = {
    parseLogLineCanServer,
    parseLogLineRte,
    parseLogLineCANMon,
    parseLogLineCANoe,
    parseLogLineA1ASCII,
    parseLogLineTrc,
    parseLogLineJohnDeere,
    parseLogLineRte2,
    parseLogLineJrf,
    defaultParseLogLine
  };

  size_t t_sizeLogLine = sizeof carrpt_parseLogLine / sizeof carrpt_parseLogLine[0];
  return carrpt_parseLogLine[
      at_choice < t_sizeLogLine ? at_choice : t_sizeLogLine - 1 ];
}

std::pair< int, PtrDataFrame_t > parseLogLine(
  std::string const &acr_line)
{
#if DEBUG
  std::cout << "Reading " << acr_line << std::endl;
#endif
  std::pair< int, PtrDataFrame_t > result = gs_main.pt_parseLogLine(acr_line);
  if (result.first == 0) // no error
  { /// Printout interpreted line
    PtrDataFrame_t t_ptrFrame = result.second;

    // Timestamp
    std::cout << std::setfill (' ') << std::dec << std::setw (10) << (t_ptrFrame->time()/1000) << "." << std::setfill('0')<<std::setw(3)<<(t_ptrFrame->time()%1000)<< "   ";

    // CAN-ID / number of bytes
    std::cout << std::setfill ('0') << std::hex << std::setw (8) << t_ptrFrame->identifier() << "  " << uint16_t(t_ptrFrame->dataSize()) << " ";

    // Databytes (HEX)
    size_t i;
    for (i=0; i< t_ptrFrame->dataSize(); i++) std::cout << " " << std::setw (2) <<uint16_t(t_ptrFrame->dataOctet(i));
    for (;    i<8;              i++) std::cout << "   ";
    std::cout << "  ";

    // Databytes (HEX)
    for (i=0; i< t_ptrFrame->dataSize(); i++) std::cout << std::setw (1) << char(t_ptrFrame->asciiDataOctet(i));
    for (;    i<8;              i++) std::cout << " ";

    if (t_ptrFrame->isExtendedFrameFormat()) {
      // SA
      std::cout << "  "<<std::setw(2) << uint16_t(t_ptrFrame->sourceAddress()) << "->";
      // DA
      if (t_ptrFrame->isPdu1()) std::cout << std::setw(2) << uint16_t(t_ptrFrame->ps());
      else std::cout << "FF";

      // Priority
      std::cout << " (" << uint16_t(t_ptrFrame->prio()) << ")";

      // PGN
      std::cout << " " << std::setw(6) << t_ptrFrame->pgn() << " => ";
      // Interpreted PGN
      interpretePgn(t_ptrFrame->pgn());
      // Interpreted PGN-Data
      interpretePgnData (t_ptrFrame);
      std::cout << std::endl;
    }
  }
  else
  {
    // report original line:
    std::cout << "(" << acr_line << ")" <<std::endl;
  }
  return result;
}

void checkAlives(PtrDataFrame_t at_ptrFrame)
{
  if ( ((at_ptrFrame->pgn() == VT_TO_ECU_PGN) && (at_ptrFrame->dataOctet (0) == 0xFE))
       ||
       ((at_ptrFrame->pgn() == ECU_TO_VT_PGN) && (at_ptrFrame->dataOctet (0) == 0xFF))
     )
  {
    gs_main.m_alive.alives(AliveCollection_c::categoryVtServerClientPeriodic, at_ptrFrame->sourceAddress()).push_back (at_ptrFrame->time());
  }


  if ( ((at_ptrFrame->pgn() == PROCESS_DATA_PGN) && (at_ptrFrame->dataOctet (0) == 0x0E))
       ||
       ((at_ptrFrame->pgn() == PROCESS_DATA_PGN) && (at_ptrFrame->dataOctet (0) == 0x0F))
     )
  {
    gs_main.m_alive.alives(AliveCollection_c::categoryTcServerClientPeriodic, at_ptrFrame->sourceAddress()).push_back (at_ptrFrame->time());
  }

}



void checkSingles(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->pgn() == ACKNOWLEDGEMENT_PGN)
  {
    gs_main.m_alive.alives(AliveCollection_c::categoryNotAcknowledge, at_ptrFrame->sourceAddress()).push_back (at_ptrFrame->time());
  }
}



void checkHandshakingsVtCommands(PtrDataFrame_t at_ptrFrame)
{
  AliveCollection_c::Category_e e_category; // no need to initialize.

  if ((at_ptrFrame->dataOctet(0) >= 0x60) && (at_ptrFrame->dataOctet(0) < 0xF0))
  { // not a maintenance command!
    msgType_e msgType_direction;
    uint8_t ui8_saClient;
    if (at_ptrFrame->pgn() == VT_TO_ECU_PGN)
    {
      e_category = AliveCollection_c::categoryVtCommandResponse;
      msgType_direction = msgTypeResponse;
      ui8_saClient = at_ptrFrame->ps();
    }
    else if (at_ptrFrame->pgn() == ECU_TO_VT_PGN)
    {
      e_category = AliveCollection_c::categoryVtCommandResponse;
      msgType_direction = msgTypeCommand;
      ui8_saClient = at_ptrFrame->sourceAddress();
    }
    else return;
    gs_main.m_alive.response(e_category, ui8_saClient).push_back (msgType_direction);
    gs_main.m_alive.alives(e_category, ui8_saClient).push_back (at_ptrFrame->time());
    return;
  }
}


void checkHandshakingTP(PtrDataFrame_t at_ptrFrame)
{
  msgType_e mtype;
  uint8_t ui8_saClient=0xFE; // default to avoid compiler warning. The code below should make sure that it is always initialized properly!
  switch (at_ptrFrame->pgn()) {

  case ETP_DATA_TRANSFER_PGN:
  case TP_DATA_TRANSFER_PGN:
    ui8_saClient = at_ptrFrame->sourceAddress();
    mtype = msgTypeDATA;
    break;

  case ETP_CONN_MANAGE_PGN:
  case TP_CONN_MANAGE_PGN:
    switch (at_ptrFrame->dataOctet (0)) {
    case 0x10:
    case 0x14:
      mtype = msgTypeRTS;
      ui8_saClient = at_ptrFrame->sourceAddress();
      break;
    case 0x11:
    case 0x15:
      mtype = msgTypeCTS;
      ui8_saClient = at_ptrFrame->destinationAddress();
      break;
    case 0x16:
      mtype = msgTypeDPO;
      ui8_saClient = at_ptrFrame->sourceAddress();
      break;
    case 0x13:
    case 0x17:
      mtype = msgTypeEOMACK;
      ui8_saClient = at_ptrFrame->destinationAddress();
      break;
    case 0xFF:
      mtype = msgTypeCONNABORT;
      ui8_saClient = at_ptrFrame->sourceAddress();
      // this message is special case! This aborts SA->DA and also DA->SA!
      gs_main.m_alive.response(AliveCollection_c::categoryEtpTpCommunicationTimes, at_ptrFrame->destinationAddress()).push_back (mtype);
      gs_main.m_alive.alives(AliveCollection_c::categoryEtpTpCommunicationTimes, at_ptrFrame->destinationAddress()).push_back (at_ptrFrame->time());
      break; // CONN ABORT BY SENDER!
    }
    break;

  default:
    return;
  }

  gs_main.m_alive.response(AliveCollection_c::categoryEtpTpCommunicationTimes, ui8_saClient).push_back (mtype);
  gs_main.m_alive.alives(AliveCollection_c::categoryEtpTpCommunicationTimes, ui8_saClient).push_back (at_ptrFrame->time());
}

int main (int argc, char** argv)
{
  std::cerr << "ISOBUS-Logalizer (c) 2007-2010 OSB AG." << std::endl << std::endl;

  if (argc < 2)
    exit_with_usage(argv[0]);

  std::string str_filename = argv[1];
  PtrInputStream_t t_ptrIn = PtrInputStream_t( new InputStream_c(str_filename) );

  if (!t_ptrIn->isOpen()) exit_with_error("Couldn't open file");

  gs_main.pt_parseLogLine = argc >= 3 ?
    getLogLineParser(atoi(argv[2])) : // follow user's suggestion
    parseLogLineCanServer; // default to can_server

  gs_main.mt_sizeMultipacketWrap = argc >= 4 ?
    atoi(argv[3]) : // follow user's suggestion
    32; // default to can_server

  for (;;) {
    std::string str_line;
    getline(t_ptrIn->raw(), str_line);
    bool const cb_failReading = t_ptrIn->raw().fail();
    if (cb_failReading)
      break;
    std::pair< int, PtrDataFrame_t > parse_result = parseLogLine(str_line);
    if (0 == parse_result.first)
    {
      checkAlives(parse_result.second);
      checkSingles(parse_result.second);
      checkHandshakingsVtCommands(parse_result.second);
      checkHandshakingTP(parse_result.second);
    }    
  }
  t_ptrIn = PtrInputStream_t(0);

  std::cerr << "Alriiiiite... printing out the results..." << std::endl;
  gs_main.m_alive.report();
}

void interpretePgnsFs2Cl(
    PtrDataFrame_t at_ptrFrame)
{
  uint32_t i_pathNameLength = 0;
  uint16_t msg_count = 0;
  uint16_t msg_offset = 0;

  std::cout << "SA: " << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << " DA: " << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " length Data: " << std::setw(4)<<std::setfill('0') << uint32_t(at_ptrFrame->dataSize());

  switch (uint32_t(at_ptrFrame->dataOctet(0))) {
    case 0x00:
      std::cout << "\tFile Server Status: ";
        (at_ptrFrame->dataOctet(1) & 0x2) ? std::cout << "Busy writing" : std::cout << "Not Busy writing";
        (at_ptrFrame->dataOctet(1) & 0x1) ? std::cout << " Busy reading" : std::cout << " Not Busy reading";
      std::cout << " Number of Open Files: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(2));
      break; //File Server Status

    case 0x01:

      std::cout << "\tGet File Server Properties Response Version Number:";
      switch (at_ptrFrame->dataOctet(1)) {
        case 0x00: std::cout << " Draft version of the Standard"; break;
        case 0x01: std::cout << " Final Draft version of the Standard"; break;
        case 0x02: std::cout << " First published version of the Standard"; break;
      }
      std::cout << " Maximum Number of Simuntaneously Open Files: " << std::hex << uint32_t(at_ptrFrame->dataOctet(2));
      std::cout << " File Server Capabilities : ";
      (at_ptrFrame->dataOctet(3)) ? std::cout << " File server supports multiple volumes"
                : std::cout << " File server does not supports multiple volumes";
      break; //Get File Server Properties Response

    case 0x10:

      std::cout << "\tGet Current Directory Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      std::cout << " Total Space: " << std::setw(8) <<  ((static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 24) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 16) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 8 ) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)))) << "(* 512 bytes)";
      std::cout << " Free Space: " << std::setw(8) << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(10)) << 24) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 9)) << 16) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 8)) << 8 ) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 7)))) << "(* 512 bytes)";
      i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(12))<<8) | at_ptrFrame->dataOctet(11);
      std::cout << " Path Name Length: " << std::setw(4) << i_pathNameLength;

      std::cout << " Path Name: ";
      for (uint16_t i_count = 0; i_count < i_pathNameLength; i_count++)
      {
        std::cout << uint8_t(at_ptrFrame->dataOctet(13 + i_count));
      }

      break; //Get Current Directory Response
    case 0x11:

      std::cout << "\tChange Current Directory Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));

      break; //Chanege Current Directory Response
    case 0x20:

      std::cout << "\tOpen File Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      std::cout << " Handle: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(3));
      decodeAttributes(at_ptrFrame->dataOctet(4));

      break; //Open File Response
    case 0x21:

      std::cout << "\tSeek File Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      std::cout << " Position : " << std::setw(8) << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 24) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 16) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 8 ) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))));
      break; //Seek File Response
    case 0x22:

      std::cout << "\tRead File/Directory Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));

      msg_count = uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3));
      msg_offset = 5;

      std::cout << " Count: " << std::setw(4) << msg_count << " ";

      //case file-data
      if (at_ptrFrame->dataSize() == msg_count + 5u) {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          std::cout << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(5 + loop)) << " ";
        }
      }
      //else directory entries
      else {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          uint8_t filenamelength = uint8_t(at_ptrFrame->dataOctet(msg_offset));
          if (filenamelength == 0xff)
            break;
          std::cout << " Filename length: " << uint16_t(filenamelength) << " ";
          std::cout << " Filename: ";
          uint8_t loopfilename = 0;
          for (; loopfilename < filenamelength; loopfilename++) {
            std::cout << uint8_t(at_ptrFrame->dataOctet(msg_offset + 1 + loopfilename));
          }
          decodeAttributes(at_ptrFrame->dataOctet(msg_offset + 1 + loopfilename));
          decodeDate(uint16_t(at_ptrFrame->dataOctet(msg_offset + 3 + loopfilename) << 8 | uint16_t(at_ptrFrame->dataOctet(msg_offset + 2 + loopfilename))));
          decodeTime(uint16_t(at_ptrFrame->dataOctet(msg_offset + 5 + loopfilename) << 8 | uint16_t(at_ptrFrame->dataOctet(msg_offset + 4 + loopfilename))));
          std::cout << " FileSize: " << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 9 + loopfilename)) << 24) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 8 + loopfilename)) << 16) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 7 + loopfilename)) << 8 ) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 6 + loopfilename))));
          msg_offset = uint16_t(msg_offset + filenamelength + 10);
        }
      }

      break; //Read File/Directory Response
    case 0x23:

      std::cout << "\tWrite File Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      std::cout << " Count: " << std::setw(4) << uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3));
      break; // Write File Response
    case 0x24:

      std::cout << "\tClose File Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Close File Response
    case 0x30:

      std::cout << "\tMove File Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Move File Response
    case 0x31:

      std::cout << "\tDelete File Repsonse TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Delete File Repsonse
    case 0x32:

      std::cout << "\tGet File Attribute Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      decodeAttributes(at_ptrFrame->dataOctet(3));
      std::cout << " Size: " << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 24) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 16) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 8 ) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))));
      break; // Get File Attribute Response
    case 0x33:

      std::cout << "\tSet File Attribute Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Set File Attribute Response
    case 0x34:

      std::cout << "\tGet File Date & Time Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      decodeDate(uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3)));
      decodeTime(uint16_t((at_ptrFrame->dataOctet(6)<<8) | at_ptrFrame->dataOctet(5)));
      break; // Get File Date & Time Response
    case 0x40:

      std::cout << "\tInitialize Volume Response TAN: " << std::setw(2) << std::hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      decodeAttributes(at_ptrFrame->dataOctet(3));
      break; // Initialize Volume Response
    default: return;
  }
}

void dump(PtrDataFrame_t at_ptrFrame)
{
  size_t local_multipacketWrap = gs_main.mt_sizeMultipacketWrap;
  if (local_multipacketWrap > at_ptrFrame->dataSize())
    local_multipacketWrap = at_ptrFrame->dataSize();

  std::cout << ")" << std::endl;

  std::cout << "              " << std::hex << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << "->" << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " " << std::dec << std::setw(7)<<std::setfill(' ') << uint32_t(at_ptrFrame->dataSize()) << "  " << std::hex;

  for (size_t blockoffset=0; blockoffset < at_ptrFrame->dataSize(); blockoffset += local_multipacketWrap)
  {
    if (blockoffset > 0)
      std::cout << "                              ";

    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        std::cout << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->dataOctet(blockoffset+inblock)) << " ";
      else
        std::cout << "   ";
    }
    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        std::cout << getAscii (at_ptrFrame->dataOctet(blockoffset+inblock));
      else
        std::cout << " ";
    }
    std::cout << std::endl;
  }
}

void decodeErrorCode(uint8_t i_errorCode)
{
  switch (uint32_t(i_errorCode)) {
    case 0x00:
      std::cout << " Success" ;
      break;
    case 0x01:
      std::cout << " Access Denied" ;
      break;
    case 0x02:
      std::cout << " Invalid Access" ;
      break;
    case 0x03:
      std::cout << " Too many files open" ;
      break;
    case 0x04:
      std::cout << " File or path not found" ;
      break;
    case 0x05:
      std::cout << " Invalid Handle" ;
      break;
    case 0x06:
      std::cout << " Invalid given source name" ;
      break;
    case 0x07:
      std::cout << " Invalid given destination name" ;
      break;
    case 0x08:
      std::cout << " Out of free space" ;
      break;
    case 0x09:
      std::cout << " Failure during write operation" ;
      break;
    case 0x0a:
      std::cout << " Volume is possibly not initialized" ;
      break;
    case 0x0b:
      std::cout << " Failure during a read operation" ;
      break;
    case 0x0c:
      std::cout << " Function not supported" ;
      break;
    case 0x2A:
      std::cout << " Invalid request length" ;
      break;
    case 0x2B:
      std::cout << " Out of memory" ;
      break;
    case 0x2C:
      std::cout << " Any other error" ;
      break;
    case 0x2D:
      std::cout << " End of file reached" ;
      break;
    default:
      std::cout << " Reserved" ;
      break;
  }
}

void decodeAttributes(uint8_t i_attributes)
{
  (i_attributes & 128)? std::cout << " Volume is case-sensitive"  : std::cout << " Volume is case-insensitive" ;
  (i_attributes & 64)? std::cout << " Volume is not removable"  : std::cout << " Volume is removable" ;
  (i_attributes & 32)? std::cout << " Volume does support long filenames"  : std::cout << " Volume does not support long filenames" ;
  (i_attributes & 16)? std::cout << " Handle specifies a directory"  : std::cout << " Handle does not specify a directory" ;
  (i_attributes & 8)? std::cout << " Handle specifies a volume"  : std::cout << " Handle does not specify a volume" ;
  (i_attributes & 2)? std::cout << " Hidden attribute is set"  : std::cout << " Hidden attribute is not set" ;
  (i_attributes & 1)? std::cout << " Read-only attribute is set"  : std::cout << " Read-only attribute is not set" ;
}

void decodeDate(uint16_t i_date)
{

  std::cout << " " << std::setw(4) << std::dec << uint32_t(1980 + ((i_date >> 9) & 0x7F)) << "-" << std::setw(2) << std::dec << uint32_t((i_date >> 5) & 0xF) << "-" << std::setw(2) << std::dec << uint32_t((i_date) & 0x1F);

}

void decodeTime(uint16_t i_time)
{
  std::cout << " " << std::setw(2) << std::dec << uint32_t(((i_time >> 11) & 0x1F)) << "-" << std::setw(2) << std::dec << uint32_t(((i_time >> 5) & 0x3F)) << "-" << std::setw(2) << std::dec << uint32_t(2 * ((i_time) & 0x1F));
}
