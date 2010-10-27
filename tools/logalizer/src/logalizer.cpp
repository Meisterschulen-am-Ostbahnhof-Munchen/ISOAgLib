/*
  logalizer.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#define HAL_PATH_ISOAGLIB IsoAgLib/hal/pc
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/isoaglib_config.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <stdlib.h>
#include <yasper.h>
#include <algorithm>

using namespace std;

char getAscii(uint8_t val)
{
  return ((val >= 0x20) && (val < 0x7F)) ? (char(val)) : '.';
}

class DataFrame_c {
public:
  DataFrame_c(
      uint64_t aui64_time,
      uint32_t aui64_identifier,
      std::vector< uint8_t > const &acrvec_data
    ) :
    mui64_time(aui64_time),
    mui64_identifier(aui64_identifier),
    mvec_data(acrvec_data),
    mb_canExt(false)
  {}
  DataFrame_c( // artificial frame for frame data and addresses only!
      std::vector< uint8_t > const &acrvec_data,
      uint8_t aui8_sourceAddress,
      uint8_t aui8_destinationAddress
    ) :
    mui64_time(0),
    mui64_identifier(uint32_t(aui8_destinationAddress) << 8 | aui8_sourceAddress),
    mvec_data(acrvec_data)
  {}
  uint8_t prio() const {
    return uint8_t(mui64_identifier >> 26);
  }
  uint8_t dp() const {
    return (mui64_identifier >> 24) & 0x03;
  }
  uint8_t sourceAddress() const {
     return mui64_identifier & 0xFF;
  }
  uint8_t ps() const {
    return (mui64_identifier >>  8) & 0xFF;
  }
  uint8_t destinationAddress() const {
    return (mui64_identifier >> 8) & 0xFF;
  }
  uint8_t pf() const {
    return (mui64_identifier >> 16) & 0xFF;
  }
  uint32_t pgn() const {
    return
      (dp() << 16) |
      (pf() << 8) |
      ((pf() >= 0xF0) ? ps() : 0);
  }
  uint64_t time() const {
    return mui64_time;
  }
  uint8_t dataOctet(size_t at_index) const {
    return mvec_data[at_index];
  }
  char asciiDataOctet(size_t at_index) const {
    return getAscii(mvec_data[at_index]);
  }
  std::vector< uint8_t > const &data() const {
    return mvec_data;
  }
  size_t dataSize() const {
    return mvec_data.size();
  }
  bool isPdu1() const {
    return pf() < 0xF0;
  }
  uint64_t identifier() const {
    return mui64_identifier;
  };
private:
  uint64_t mui64_time;
  uint32_t mui64_identifier;
  std::vector< uint8_t > mvec_data;
  bool mb_canExt; /// @todo SOON-260: to be used! =)
};

typedef yasper::ptr< DataFrame_c > PtrDataFrame_t;

class TransferCollection_c {
public:
  enum Variant_e {
    variant_tp,
    variant_etp,
  };

  struct Session_s {
    Session_s(size_t at_sizeTransferData) :
      mvec_data(at_sizeTransferData, 0),
      mui32_embeddedPgn(0),
      mui32_packetOffSet(0)
    {}
    std::vector< uint8_t > mvec_data;
    uint32_t mui32_embeddedPgn;
    uint32_t mui32_packetOffSet;
  };

  typedef yasper::ptr< Session_s > PtrSession_t;

  PtrSession_t newSession(
      Variant_e ae_variant,
      uint8_t aui8_transferSourceAddress,
      uint8_t aui8_transferDestinationAddress,
      size_t at_sizeTransferData);

  PtrSession_t getSession(
      Variant_e ae_variant,
      uint8_t aui8_transferSourceAddress,
      uint8_t aui8_transferDestinationAddress);

  void deleteSession(
      Variant_e ae_variant,
      uint8_t aui8_transferSourceAddress,
      uint8_t aui8_transferDestinationAddress);

private:
  struct Key_s {
    Key_s(
        Variant_e ae_variant,
        uint8_t aui8_transferSourceAddress,
        uint8_t aui8_transferDestinationAddress) :
      me_variant(ae_variant),
      mui8_transferSourceAddress(aui8_transferSourceAddress),
      mui8_transferDestinationAddress(aui8_transferDestinationAddress)
    {}

    Variant_e me_variant;
    uint8_t mui8_transferSourceAddress;
    uint8_t mui8_transferDestinationAddress;
  };

  friend bool operator<(
      Key_s const &arcs_left,
      Key_s const &arcs_right);

  typedef std::map< Key_s, PtrSession_t > Transfers_t;
  Transfers_t t_transfers;
};

TransferCollection_c::PtrSession_t TransferCollection_c::newSession(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress,
    size_t at_sizeTransferData)
{
  PtrSession_t t_ptrSession = new Session_s(at_sizeTransferData);
  t_transfers[
      Key_s(
          ae_variant,
          aui8_transferSourceAddress,
          aui8_transferDestinationAddress)
    ] = t_ptrSession;
  return t_ptrSession;
}

TransferCollection_c::PtrSession_t TransferCollection_c::getSession(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  Transfers_t::iterator it_transfer =
    t_transfers.find(
        Key_s(
            ae_variant,
            aui8_transferSourceAddress,
            aui8_transferDestinationAddress));
  return
    it_transfer == t_transfers.end() ?
    TransferCollection_c::PtrSession_t(0) :
    it_transfer->second;
}

void TransferCollection_c::deleteSession(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  t_transfers.erase(
      Key_s(
          ae_variant,
          aui8_transferSourceAddress,
          aui8_transferDestinationAddress) );
}

bool operator<(
    TransferCollection_c::Key_s const &arcs_left,
    TransferCollection_c::Key_s const &arcs_right)
{
  return
    arcs_left.me_variant < arcs_right.me_variant &&
    arcs_left.mui8_transferSourceAddress < arcs_right.mui8_transferSourceAddress &&
    arcs_left.mui8_transferDestinationAddress < arcs_right.mui8_transferDestinationAddress;
}

void interpretePgnsCl2Fs();
void interpretePgnsFs2Cl();

typedef enum {
  logTypeCanServer=0, // default!
  logTypeRte,
  logTypeCANMon,
  logTypeCANoe,
  logTypeA1ASCII,
  logTypeTrc,
  logTypeJohnDeere,
  logTypeRte2,
  logTypeJrf
} logType_t;

// RAII Programming Idiom for file being the resource:
class InputStream_c {
public:
  InputStream_c(string str_filename) :
    mifs_in(),
    mpis_in( (str_filename == "-")? &cin : &mifs_in )
  {
    if (isFile()) {
      mifs_in.open(str_filename.c_str());
    }
  }
  ~InputStream_c() {
    if (isFile())
      mifs_in.close();
  }
  istream &raw() {
    return *mpis_in;
  }

  bool isOpen() {
    return !isFile() || mifs_in.is_open();
  }
private:
  // don't let the public copy it:
  InputStream_c(InputStream_c const &);
  bool isFile() {
    return mpis_in != &cin;
  }
  ifstream mifs_in;
  istream *mpis_in;
};

typedef void Interprete_t(PtrDataFrame_t);
Interprete_t interpretePgnData;
Interprete_t interpretePgnsCl2Fs;
Interprete_t interpretePgnsFs2Cl;
Interprete_t dump;
void decodeErrorCode(uint8_t);
void decodeAttributes(uint8_t);
void decodeDate(uint16_t);
void decodeTime(uint16_t);
Interprete_t interpretePgnsVtFromEcu;
Interprete_t interpretePgnsVtToEcu;

typedef yasper::ptr< InputStream_c > PtrInputStream_t;

struct Main_s {
  size_t mt_sizeMultipacketWrap; // default will be set when parsing parameters
  TransferCollection_c mc_trans;
  logType_t mt_logType;
  PtrInputStream_t t_ptrIn;

  Main_s() :
    mt_sizeMultipacketWrap(0),
    mc_trans(),
    mt_logType(),
    t_ptrIn(0)
  {}
} gs_main;

void exit_with_usage(const char* progname)
{
  cout << "Usage: (1) " << progname << " <logFile> [logType] [wrapMultipacket]" << endl << endl;
  cout << "logFile:  filepath or - (dash, means standard input rather than a real file)"<<endl;
  cout << "logTypes: 0 -> can_server [DEFAULT]"<<endl;
  cout << "          1 -> rte"<<endl;
  cout << "          2 -> CANMon"<<endl;
  cout << "          3 -> CANoe ASCII (.asc)"<<endl;
  cout << "          4 -> A1ASCII"<<endl;
  cout << "          5 -> PCANView"<<endl;
  cout << "          6 -> JohnDeere"<<endl;
  cout << "          7 -> rte2"<<endl;
  cout << "          8 -> JRF (.jrf)"<<endl;
  cout << endl;
  cout << "wrapMultipacket: Number of data-bytes to display per line. Defaults to 32." << endl;
  cout << endl;
#ifdef WIN32
  cout << "can_server: '104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0'"<<endl;
  cout << "rte:        '[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06'"<<endl;
  cout << "             (with OR without Channel-Nr. in []. This is being autodetected.)"<<endl;
  cout << "CANMon:     'RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB'"<<endl;
  cout << "CANoe:      '  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF'"<<endl;
  cout << "A1ASCII:    'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff    '..."<<endl;
  cout << "            ...'   446270'"<<endl;
  cout << "PCANView:   '    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15'"<<endl;
  cout << "JohnDeere:  'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488  '..."<<endl;
  cout << "            ...'    17920  ....... '"<<endl;
  cout << "JRF:        '41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF'"<<endl;
#else
  cout << "can_server: '104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0'"<<endl;
  cout << "rte:        '[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06'"<<endl;
  cout << "             (with OR without Channel-Nr. in []. This is being autodetected.)"<<endl;
  cout << "CANMon:     'RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB'"<<endl;
  cout << "CANoe:      '  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF'"<<endl;
  cout << "A1ASCII:    'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270'"<<endl;
  cout << "PCANView:   '    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15'"<<endl;
  cout << "JohnDeere:  'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....... '"<<endl;
  cout << "JRF:        '41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF'"<<endl;
#endif

  exit(0);
}

void exit_with_error(const char* error_message)
{
  cerr << error_message << endl;
  exit(-1);
}

pair< int, PtrDataFrame_t > parseLogLineCANMon() //RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif

  unsigned arru_d[8];
  int iA, iB;
  int parsed_count = sscanf(
      line.c_str(),
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

pair< int, PtrDataFrame_t > parseLogLineJohnDeere() // "r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....\\. "
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
  int iA, iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      line.c_str(),
      "r Xtd %*i %x %*i %x %x %x %x %x %x %x %x %*i   %*i %i      17920  %s ",
      &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iA);
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}

pair< int, PtrDataFrame_t > parseLogLineCanServer() // "104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0"
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
  long long iA;
  int iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      line.c_str(),
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


pair< int, PtrDataFrame_t > parseLogLineCANoe() // "  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF "
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
  float f1;
  int iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      line.c_str(),
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


pair< int, PtrDataFrame_t > parseLogLineA1ASCII() // "m e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270"
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
/// @todo SOON-260: handle std/ext, too...
  int iA, iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      line.c_str(),
      "m e 0x%x %u 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x %i",
      &iA, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iB);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iB, iA, vec_data);
  return std::make_pair( i_result, t_ptrFrame );
}



pair< int, PtrDataFrame_t > parseLogLineRte() // "[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06"
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif

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
          line.c_str(),
          "%*s %*s %Li.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
      if (parsed_count <= -1 || sci_minimal_expected_parse_count <= parsed_count)
        remembered_mode = current_mode;
      break;
    case 2:
      parsed_count = sscanf(
          line.c_str(),
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


pair< int, PtrDataFrame_t > parseLogLineRte2 ()
{
    static uint64_t sui64_rteTime = 0;
    int big;
    int little;
    char *cursor;
    size_t byte;
    string line;

    getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
    cout << line << endl;
#endif

    /* skip over any initial HW */
    for (cursor = const_cast<char *>(line.c_str()); *cursor != 0 && !isspace (*cursor); cursor++)
        /* empty */;

    /*
     * must specify base 10 because of leading 0's
     */
    big = strtol (cursor, &cursor, 10);
    if (*cursor != '.') {
        fprintf (stderr, "unexpected terminator %c (%d): %s\n", *cursor, *cursor, line.c_str());
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
        if (byte > t_sizeData)     /* TODO: shouldn't this be >= ? */
            break;

        vec_data[byte] = uint8_t (strtol (cursor, &cursor, 16));
    }

    return std::make_pair( 0, new DataFrame_c( cui64_time, cui32_identifier, vec_data ) );
}




pair< int, PtrDataFrame_t > parseLogLineTrc() // "    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15"
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
  unsigned arru_d[8];
  int iA, iB, iDb;
  int parsed_count = sscanf(
      line.c_str(),
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


pair< int, PtrDataFrame_t > parseLogLineJrf() // "41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF"
{
  string line;
  getline (gs_main.t_ptrIn->raw(), line);
#if DEBUG
  cout << line << endl;
#endif
  float fA;
  int iB;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      line.c_str(),
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


void partiallyInterpretePgnsVtEcu(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    if (at_ptrFrame->dataOctet(0) < 0x10) cout << "Activation ";
    if ((at_ptrFrame->dataOctet(0) >=0x10) && (at_ptrFrame->dataOctet(0) < 0xF0)) cout << "Command ";

    int8_t i8_uidBytePos = -1;

    switch (at_ptrFrame->dataOctet(0))
    {
      case 0x00: cout << "Soft Key"; i8_uidBytePos = 3; break;
      case 0x01: cout << "Button"; i8_uidBytePos = 3; break;
      case 0x02: cout << "Pointing Event"; break;
      case 0x03: cout << "VT Select Input Object"; i8_uidBytePos = 2; break;
      case 0x04: cout << "VT ESC"; i8_uidBytePos = 2; break;
      case 0x05: cout << "VT Change Numeric Value"; i8_uidBytePos = 2; break;
      case 0x06: cout << "VT Change Active Mask"; i8_uidBytePos = 2; break;
      case 0x07: cout << "VT Change Soft Key Mask"; i8_uidBytePos = 4; break;
      case 0x08: cout << "Input String Value"; i8_uidBytePos = 2; break;

      case 0x11: cout << "Object Pool Transfer"; break;
      case 0x12: cout << "End of Object Pool"; break;

      case 0x92: cout << "ESC"; i8_uidBytePos = 2; break;

      case 0xC0: cout << "Get Memory"; break;
      case 0xC2: cout << "Get Number Of Soft Keys"; break;
      case 0xC3: cout << "Get Text Font Data"; break;
      case 0xC7: cout << "Get Hardware"; break;

      case 0xD0: cout << "Store Version"; break;
      case 0xD1: cout << "Load Version"; break;
      case 0xD2: cout << "Delete Version"; break;

      case 0xA0: cout << "Hide/Show Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) cout << " -show-"; else cout << " -hide-"; break;
      case 0xA1: cout << "Enable/Disable Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) cout << " -enable-"; else cout << " -disable-"; break;
      case 0xA2: cout << "Select Input Object"; i8_uidBytePos = 2; break;
      case 0xA3: cout << "Control Audio Device"; break;
      case 0xA4: cout << "Set Audio Volume"; break;
      case 0xA5: cout << "Change Child Location"; i8_uidBytePos = 4; break;
      case 0xA6: cout << "Change Size"; i8_uidBytePos = 2; break;
      case 0xA7: cout << "Change Background Colour"; i8_uidBytePos = 2; break;
      case 0xA8: cout << "Change Numeric Value"; i8_uidBytePos = 2; break;
      case 0xA9: cout << "Change End Point"; i8_uidBytePos = 2; break;
      case 0xAA: cout << "Change Font Attributes"; i8_uidBytePos = 2; break;
      case 0xAB: cout << "Change Line Attributes"; i8_uidBytePos = 2; break;
      case 0xAC: cout << "Change Fill Attributes"; i8_uidBytePos = 2; break;
      case 0xAD: cout << "Change Active Mask"; i8_uidBytePos = 4; break;
      case 0xAE: cout << "Change Soft Key Mask"; i8_uidBytePos = 5; break;
      case 0xAF: cout << "Change Attribute"; i8_uidBytePos = 2; cout << " AID="<<uint16_t(at_ptrFrame->dataOctet(4-1)); break;

      case 0xB0: cout << "Change Priority"; i8_uidBytePos = 2; break;
      case 0xB1: cout << "Change List Item"; i8_uidBytePos = 2; break;
      case 0xB2: cout << "Delete Object Pool"; break;
      case 0xB3: cout << "Change String Value"; i8_uidBytePos = 2; break;
      case 0xB4: cout << "Change Child Position"; i8_uidBytePos = 4; break;

      case 0xFE: cout << "VT Status Message: Active WS-Master's SA: "<<uint16_t(at_ptrFrame->dataOctet(1)); break;
      case 0xFF: cout << "Working Set Maintenance"; break;
    }
    if (i8_uidBytePos > 0) cout << " ObjectID="<<dec<<( (uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos+1-1)<<8)) | (uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos-1))) );
  }
}

void interpretePgnsVtFromEcu(PtrDataFrame_t at_ptrFrame)
{
  partiallyInterpretePgnsVtEcu(at_ptrFrame);
  bool const cb_response = at_ptrFrame->dataOctet(0) < 0x10;
  if (cb_response)
    cout << " Response";
}

void interpretePgnsVtToEcu(PtrDataFrame_t at_ptrFrame)
{
  partiallyInterpretePgnsVtEcu(at_ptrFrame);
  bool const cb_response =
    (at_ptrFrame->dataOctet(0) >=0x10) &&
    (at_ptrFrame->dataOctet(0) < 0xF0);
  if (cb_response)
    cout << " Response";  
}

void interpretePgnsCl2Fs(
    PtrDataFrame_t at_ptrFrame)
{
  uint16_t i_pathNameLength = 0, i_destPathNameLength = 0;

  cout << "SA: " << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << " DA: " << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " length Data: " << setw(4)<<setfill('0') << uint32_t(at_ptrFrame->dataSize());
  switch (uint32_t(at_ptrFrame->dataOctet(0)))
  {
    case 0x00: cout << "\tClient Connection Maintenance";
      break;

    case 0x01: cout << "\tGet File Server Properties Request ";
      break;

    case 0x10: cout << "\tGet Current Directory Request TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      break;

    case 0x11: cout << "\tChange Current Directory Request TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
               i_pathNameLength = uint16_t((at_ptrFrame->dataOctet(3)<<8) | (at_ptrFrame->dataOctet(2)));
               cout << " Path Name Length : " << i_pathNameLength;
               cout << " Path Name : ";
              for (uint16_t i = 0; i < i_pathNameLength; i++)
              {
                cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
              }
      break;

    case 0x20: cout << "\tOpen File Request TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));

              if (at_ptrFrame->dataOctet(2) == 0) cout << " Open file for reading only";
              if (at_ptrFrame->dataOctet(2) == 1) cout << " Open file for writing only";
              if (at_ptrFrame->dataOctet(2) == 2) cout << " Open file for reading and writing only";
              if (at_ptrFrame->dataOctet(2) == 3) cout << " Open directory";

              (at_ptrFrame->dataOctet(2) & 4)? cout << " Create a new File if not existing": cout << " Open an existing File(fails if non-existent file)";

              (at_ptrFrame->dataOctet(2) & 8)? cout << " Open file for appending data the end of the file": cout << " Open file for random access";

              (at_ptrFrame->dataOctet(2) & 16)? cout << " Open file with exclusive access(fails if already open)": cout << " Open file for shared access";

              i_pathNameLength = uint16_t((at_ptrFrame->dataOctet(4)<<8) | (at_ptrFrame->dataOctet(3)));

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: ";
              for (uint16_t i = 0; i <   i_pathNameLength; i++)
              {
                cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
              }
      break;

    case 0x21: cout << "\tSeek File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << " Handle: " << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               cout << " Position Mode: " << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(3));
               cout << " Offset: " << hex << ( (static_cast<uint32_t>(at_ptrFrame->dataOctet(7))<<24) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(6))<<16) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(5))<<8) | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))) );
      break;

    case 0x22: cout << "\tRead File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << " Handle: "     <<hex<<static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               cout << " Count: "      <<hex<<( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) );
               if (at_ptrFrame->dataOctet(5) == 0x00) cout << " Do not report hidden files a directory listing";
               if (at_ptrFrame->dataOctet(5) == 0x01) cout << " Report hidden files in a directory listing";
               if (at_ptrFrame->dataOctet(5) == 0xFF) cout << " Parameter ist not available, FS shall not report hidden in a directory listing";
               if ( (at_ptrFrame->dataOctet(5) >= 0x02) && (at_ptrFrame->dataOctet(5) <= 0xFE) ) cout << " ERROR wrong Error Code";
      break;

    case 0x23: cout << "\tWrite File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << " Handle: " << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
               cout << " Number of Byte written Data: " <<dec<<( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) );
               cout << " Data: ";
               for (int i = 0; i < ( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))) ); i++)
               {
                 cout << "0x" << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(5 + i)) << " ";
               }
      break;

    case 0x24: cout << "\tClose File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << " Handle: " << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(2));
      break;

    case 0x30: cout << "\tMove / Copy / Rename File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << ((at_ptrFrame->dataOctet(2) && 0x04)? " recursive" : " not recursive");
               cout << ((at_ptrFrame->dataOctet(2) && 0x02)? " force" : " not force");
               cout << ((at_ptrFrame->dataOctet(2) && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               cout << " Source Path Name Length: "  << hex << i_pathNameLength;
               cout << " Source Path Name: ";
               for (int i = 0; i < i_pathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(7 + i));
               }

               i_destPathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(6))<<8) | at_ptrFrame->dataOctet(5);

               cout << " Destination Path Name Length: " << hex << i_destPathNameLength;
               cout << " Destination Path Name: ";
               for (int i = 0; i < i_destPathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(7 + i_pathNameLength + i));
               }
      break;

    case 0x31: cout << "\tDelete File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               cout << ((at_ptrFrame->dataOctet(2) && 0x04)? " recursive" : " not recursive");
               cout << ((at_ptrFrame->dataOctet(2) && 0x02)? " force" : " not force");
               cout << ((at_ptrFrame->dataOctet(2) && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: ";
              for (uint16_t i = 0; i <  i_pathNameLength; i++)
              {
                cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
              }

      break;
    case 0x32: cout << "\tGet Files Attributes Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))<<8) | at_ptrFrame->dataOctet(2);

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
               }
      break;
    case 0x33: cout << "\tSet File Attributes Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               if (at_ptrFrame->dataOctet(2) == 0x1C) cout <<" Clear read-only attribute";
               if (at_ptrFrame->dataOctet(2) == 0x1D) cout <<" Set read-only attribute";
               if (at_ptrFrame->dataOctet(2) == 0x13) cout <<" Clear hidden attribute";
               if (at_ptrFrame->dataOctet(2) == 0x17) cout <<" Set hidden attribute";

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(4))<<8) | at_ptrFrame->dataOctet(3);

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(5 + i));
               }
      break;
    case 0x34: cout << "\tGet File Date & File Time Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));

               i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(3))<<8) | at_ptrFrame->dataOctet(2);

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(4 + i));
               }
      break;
    case 0x40: cout << "\tInitialize Volume Request TAN: " << setw(2) << hex << static_cast<uint32_t>(at_ptrFrame->dataOctet(1));
               cout << " Space : "        <<hex<< ((static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 24)
			                                    | (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16)
												| (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) <<  8)
												| (static_cast<uint32_t>(at_ptrFrame->dataOctet(2))      ));
               if (at_ptrFrame->dataOctet(6) == 0x00) cout <<" create  a new volume using all available space";
               if (at_ptrFrame->dataOctet(6) == 0x02) cout <<" create  a new volume using space specified";
               if (at_ptrFrame->dataOctet(6) == 0x01) cout <<" Overwrite the existing volume";

               i_pathNameLength = static_cast<uint16_t>(at_ptrFrame->dataOctet(8)<<8) | at_ptrFrame->dataOctet(7);

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                 cout << uint8_t(at_ptrFrame->dataOctet(9 + i));
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
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
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
          case 0x0: cout << "Operator direction: NotReversed "; break;
          case 0x1: cout << "Operator direction: Reversed "; break;
          case 0x2: cout << "Operator direction: Error "; break;
          case 0x3: cout << "Operator direction: Not available "; break;
      }

      // Start/Stop state
      switch ((at_ptrFrame->dataOctet(7) >> 4) & 0x03) {
        case 0x0: cout << "St/Sp state: stop "; break;
        case 0x1: cout << "St/Sp state: start "; break;
        case 0x2: cout << "St/Sp state: error "; break;
        case 0x3: cout << "St/Sp state: n/a "; break;
      }

      // Key switch state
      switch ((at_ptrFrame->dataOctet(7) >> 2) & 0x03) {
        case 0x0: cout << "Key sws: off "; break;
        case 0x1: cout << "Key sws: not off "; break;
        case 0x2: cout << "Key sws: error "; break;
        case 0x3: cout << "Key sws: n/a "; break;
      }

      // Ground based direction
      switch (at_ptrFrame->dataOctet(7) & 0x03) {
        case 0x0: cout << "WB Dir: Reverse "; break;
        case 0x1: cout << "WB Dir: Forward "; break;
        case 0x2: cout << "WB Dir: Error "; break;
        case 0x3: cout << "WB Dir: N/A "; break;
      }
  }
}

void interpreteGroundBasedSpeedDist(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
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
        case 0x0: cout << "GB Dir: Reverse "; break;
        case 0x1: cout << "GB Dir: Forward "; break;
        case 0x2: cout << "GB Dir: Error "; break;
        case 0x3: cout << "GB Dir: N/A "; break;
      }
  }
}

void interpreteRearPTOstate(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear PTO output shaft speed
      std::cout << std::dec << "Rear PTO output shaft speed: " << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(1)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(0)) ) * 0.125 << " 1/min ";

      // Rear PTO output shaft speed setpoint
      std::cout << std::dec << "Rear PTO output shaft speed setpoint: " << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(2)) ) * 0.125 << " 1/min ";

      // Rear PTO engagement
      switch (at_ptrFrame->dataOctet(4) >> 6) {
          case 0x0: cout << "RPTO eng: disen "; break;
          case 0x1: cout << "RPTO eng: en "; break;
          case 0x2: cout << "RPTO eng: Error "; break;
          case 0x3: cout << "RPTO eng: n/a "; break;
      }

      // Rear PTO mode
      switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x03) {
        case 0x0: cout << "RPTO mod: 540 "; break;
        case 0x1: cout << "RPTO mod: 1000 "; break;
        case 0x2: cout << "RPTO mod: Error "; break;
        case 0x3: cout << "RPTO mod: n/a "; break;
      }

      // Rear PTO economy mode
      switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x03) {
        case 0x0: cout << "RPTO ec mod: disen "; break;
        case 0x1: cout << "RPTO ec mod: en "; break;
        case 0x2: cout << "RPTO ec mod: error "; break;
        case 0x3: cout << "RPTO ec mod: n/a "; break;
      }

      // Rear PTO mode request status
      switch (at_ptrFrame->dataOctet(4) & 0x03) {
        case 0x0: cout << "RPTO mod rq st: disen "; break;
        case 0x1: cout << "RPTO mod rq st: en "; break;
        case 0x2: cout << "RPTO mod rq st: Error "; break;
        case 0x3: cout << "RPTO mod rq st: N/A "; break;
      }

      // Rear PTO mode
      switch (at_ptrFrame->dataOctet(5) >> 6) {
          case 0x0: cout << "RPTO mode: external request "; break;
          case 0x1: cout << "RPTO mode: controll override "; break;
          case 0x2: cout << "RPTO mode: Error "; break;
          case 0x3: cout << "RPTO mode: n/a "; break;
      }

      // Rear PTO Economy mode request status
      switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x03) {
        case 0x0: cout << "RPTO economy mode request status: external request "; break;
        case 0x1: cout << "RPTO economy mode request status: controll override "; break;
        case 0x2: cout << "RPTO economy mode request status: Error "; break;
        case 0x3: cout << "RPTO economy mode request status: n/a "; break;
      }

      // Rear PTO shaft speed limit status
      switch ((at_ptrFrame->dataOctet(5) >> 1) & 0x07) {
        case 0x0: cout << "RPTO shaft speed limit status: no limit "; break;
        case 0x1: cout << "RPTO shaft speed limit status: op limit "; break;
        case 0x2: cout << "RPTO shaft speed limit status: lim high "; break;
        case 0x3: cout << "RPTO shaft speed limit status: lim low "; break;
        case 0x4: cout << "RPTO shaft speed limit status: reserved "; break;
        case 0x5: cout << "RPTO shaft speed limit status: reserved "; break;
        case 0x6: cout << "RPTO shaft speed limit status: nr fault "; break;
        case 0x7: cout << "RPTO shaft speed limit status: n/a "; break;
      }

      // Bits 1 is reserved
      // Bytes 7 and 8 are reserved
  }
}

void interpreteRearHitch(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear hitch position in %
      std::cout << std::dec << "RearHitchPos: " << at_ptrFrame->dataOctet(0) * 0.4 << "% ";

      // Rear hith in-work indication.
      switch (at_ptrFrame->dataOctet(1) >> 6) {
          case 0x0: cout << "RH in-wo ind: out-of-work "; break;
          case 0x1: cout << "RH in-wo ind: in-work "; break;
          case 0x2: cout << "RH in-wo ind: error "; break;
          case 0x3: cout << "RH in-wo ind: n/a "; break;
      }

      // Rear hitch position limit status
      switch ((at_ptrFrame->dataOctet(1) >> 3) & 0x07) {
        case 0x0: cout << "RH pos lim st: no limit "; break;
        case 0x1: cout << "RH pos lim st: op limit "; break;
        case 0x2: cout << "RH pos lim st: lim high "; break;
        case 0x3: cout << "RH pos lim st: lim low "; break;
        case 0x4: cout << "RH pos lim st: reserved "; break;
        case 0x5: cout << "RH pos lim st: reserved "; break;
        case 0x6: cout << "RH pos lim st: nr fault "; break;
        case 0x7: cout << "RH pos lim st: n/a "; break;
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
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Language Code
    cout << "LC:" << at_ptrFrame->asciiDataOctet(0) << at_ptrFrame->asciiDataOctet(1) << " ";

    // Decimal Point
    switch (at_ptrFrame->dataOctet(2) >> 6) {
      case 0x0: cout << "dp:, "; break;
      case 0x1: cout << "dp:. "; break;
      case 0x2: cout << "dp:R "; break;
      case 0x3: cout << "dp:N "; break;
    }
    // Time Format
    switch ((at_ptrFrame->dataOctet(2) >> 4) & 0x03) {
      case 0x0: cout << "t:24 "; break;
      case 0x1: cout << "t:12 "; break;
      case 0x2: cout << "t:R  "; break;
      case 0x3: cout << "t:N  "; break;
    }
    // Date Format
    switch (at_ptrFrame->dataOctet(3)) {
      case 0x0: cout << "d:ddmmyyyy "; break;
      case 0x1: cout << "d:ddyyyymm "; break;
      case 0x2: cout << "d:mmyyyydd "; break;
      case 0x3: cout << "d:mmddyyyy "; break;
      case 0x4: cout << "d:yyyymmdd "; break;
      case 0x5: cout << "d:yyyyddmm "; break;
      default:  cout << "d:UNSPECIF "; break;
    }
    // Distance
    switch ((at_ptrFrame->dataOctet(4) >> 6) & 0x3) {
      case 0x0: cout << "d:M  "; break;
      case 0x1: cout << "d:IU "; break;
      case 0x2: cout << "d:R  "; break;
      case 0x3: cout << "d:N  "; break;
    }
    // Area
    switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x3) {
      case 0x0: cout << "a:M  "; break;
      case 0x1: cout << "a:IU "; break;
      case 0x2: cout << "a:R  "; break;
      case 0x3: cout << "a:N  "; break;
    }
    // Volume
    switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x3) {
      case 0x0: cout << "v:M "; break;
      case 0x1: cout << "v:I "; break;
      case 0x2: cout << "v:U "; break;
      case 0x3: cout << "v:N "; break;
    }
    // Mass
    switch ((at_ptrFrame->dataOctet(4) >> 0) & 0x3) {
      case 0x0: cout << "m:M "; break;
      case 0x1: cout << "m:I "; break;
      case 0x2: cout << "m:U "; break;
      case 0x3: cout << "m:N "; break;
    }
    // Temperature
    switch ((at_ptrFrame->dataOctet(5) >> 6) & 0x3) {
      case 0x0: cout << "t:M  "; break;
      case 0x1: cout << "t:IU "; break;
      case 0x2: cout << "t:R  "; break;
      case 0x3: cout << "t:N  "; break;
    }
    // Pressure
    switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x3) {
      case 0x0: cout << "p:M  "; break;
      case 0x1: cout << "p:IU "; break;
      case 0x2: cout << "p:R  "; break;
      case 0x3: cout << "p:N  "; break;
    }
    // Force
    switch ((at_ptrFrame->dataOctet(5) >> 2) & 0x3) {
      case 0x0: cout << "f:M  "; break;
      case 0x1: cout << "f:IU "; break;
      case 0x2: cout << "f:R  "; break;
      case 0x3: cout << "f:N  "; break;
    }
    // System
    switch ((at_ptrFrame->dataOctet(5) >> 0) & 0x3) {
      case 0x0: cout << "s:M "; break;
      case 0x1: cout << "s:I "; break;
      case 0x2: cout << "s:U "; break;
      case 0x3: cout << "s:N "; break;
    }
  }
}

void interpreteLightingCommand(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Tractor high-beam head lights
    switch ((at_ptrFrame->dataOctet(0) >> 6) & 0x3) {
      case 0x0: cout << "Tractor high-beam head lights: deactivate "; break;
      case 0x1: cout << "Tractor high-beam head lights: activate "; break;
      case 0x2: cout << "Tractor high-beam head lights: reserve "; break;
      case 0x3: cout << "Tractor high-beam head lights: don't care "; break;
    }
    // Tractor low-beam head lights
    switch ((at_ptrFrame->dataOctet(0) >> 4) & 0x3) {
      case 0x0: cout << "Tractor low-beam head lights: deactivate "; break;
      case 0x1: cout << "Tractor low-beam head lights: activate "; break;
      case 0x2: cout << "Tractor low-beam head lights: reserve "; break;
      case 0x3: cout << "Tractor low-beam head lights: don't care "; break;
    }
    // Tractor alternate head lights
    switch ((at_ptrFrame->dataOctet(0) >> 2) & 0x3) {
      case 0x0: cout << "Tractor alternate head lights: deactivate "; break;
      case 0x1: cout << "Tractor alternate head lights: activate "; break;
      case 0x2: cout << "Tractor alternate head lights: reserve "; break;
      case 0x3: cout << "Tractor alternate head lights: don't care "; break;
    }
    // Daytime running lights
    switch ((at_ptrFrame->dataOctet(0) >> 0) & 0x3) {
      case 0x0: cout << "Daytime running lights: deactivate "; break;
      case 0x1: cout << "Daytime running lights: activate "; break;
      case 0x2: cout << "Daytime running lights: reserve "; break;
      case 0x3: cout << "Daytime running lights: don't care "; break;
    }


    // Left-turn signal lights
    switch ((at_ptrFrame->dataOctet(1) >> 6) & 0x3) {
      case 0x0: cout << "Left-turn signal lights: deactivate "; break;
      case 0x1: cout << "Left-turn signal lights: activate "; break;
      case 0x2: cout << "Left-turn signal lights: reserve "; break;
      case 0x3: cout << "Left-turn signal lights: don't care "; break;
    }
    // Right-turn signal lights
    switch ((at_ptrFrame->dataOctet(1) >> 4) & 0x3) {
      case 0x0: cout << "Right-turn signal lights: deactivate "; break;
      case 0x1: cout << "Right-turn signal lights: activate "; break;
      case 0x2: cout << "Right-turn signal lights: reserve "; break;
      case 0x3: cout << "Right-turn signal lights: don't care "; break;
    }
    // Beacon lights
    switch ((at_ptrFrame->dataOctet(1) >> 2) & 0x3) {
      case 0x0: cout << "Beacon lights: deactivate "; break;
      case 0x1: cout << "Beacon lights: activate "; break;
      case 0x2: cout << "Beacon lights: reserve "; break;
      case 0x3: cout << "Beacon lights: don't care "; break;
    }
    // Front fog lights
    switch ((at_ptrFrame->dataOctet(1) >> 0) & 0x3) {
      case 0x0: cout << "Front fog: deactivate "; break;
      case 0x1: cout << "Front fog: activate "; break;
      case 0x2: cout << "Front fog: reserve "; break;
      case 0x3: cout << "Front fog: don't care "; break;
    }


    // Left stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 6) & 0x3) {
      case 0x0: cout << "Left stop lights: deactivate "; break;
      case 0x1: cout << "Left stop lights: activate "; break;
      case 0x2: cout << "Left stop lights: reserve "; break;
      case 0x3: cout << "Left stop lights: don't care "; break;
    }
    // Right stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 4) & 0x3) {
      case 0x0: cout << "Right stop lights: deactivate "; break;
      case 0x1: cout << "Right stop lights: activate "; break;
      case 0x2: cout << "Right stop lights: reserve "; break;
      case 0x3: cout << "Right stop lights: don't care "; break;
    }
    // Center stop lights
    switch ((at_ptrFrame->dataOctet(2) >> 2) & 0x3) {
      case 0x0: cout << "Center stop lights: deactivate "; break;
      case 0x1: cout << "Center stop lights: activate "; break;
      case 0x2: cout << "Center stop lights: reserve "; break;
      case 0x3: cout << "Center stop lights: don't care "; break;
    }
    // Back up light and alarm horn
    switch ((at_ptrFrame->dataOctet(2) >> 0) & 0x3) {
      case 0x0: cout << "Back up light and alarm horn: deactivate "; break;
      case 0x1: cout << "Back up light and alarm horn: activate "; break;
      case 0x2: cout << "Back up light and alarm horn: reserve "; break;
      case 0x3: cout << "Back up light and alarm horn: don't care "; break;
    }


    // Tractor marker lights
    switch ((at_ptrFrame->dataOctet(3) >> 6) & 0x3) {
      case 0x0: cout << "Tractor marker lights: deactivate "; break;
      case 0x1: cout << "Tractor marker lights: activate "; break;
      case 0x2: cout << "Tractor marker lights: reserve "; break;
      case 0x3: cout << "Tractor marker lights: don't care "; break;
    }
    // Implement marker lights
    switch ((at_ptrFrame->dataOctet(3) >> 4) & 0x3) {
      case 0x0: cout << "Implement marker lights: deactivate "; break;
      case 0x1: cout << "Implement marker lights: activate "; break;
      case 0x2: cout << "Implement marker lights: reserve "; break;
      case 0x3: cout << "Implement marker lights: don't care "; break;
    }
    // Tractor clearance lights
    switch ((at_ptrFrame->dataOctet(3) >> 2) & 0x3) {
      case 0x0: cout << "Tractor clearance lights: deactivate "; break;
      case 0x1: cout << "Tractor clearance lights: activate "; break;
      case 0x2: cout << "Tractor clearance lights: reserve "; break;
      case 0x3: cout << "Tractor clearance lights: don't care "; break;
    }
    // Implement clearance lights
    switch ((at_ptrFrame->dataOctet(3) >> 0) & 0x3) {
      case 0x0: cout << "Implement clearance lights: deactivate "; break;
      case 0x1: cout << "Implement clearance lights: activate "; break;
      case 0x2: cout << "Implement clearance lights: reserve "; break;
      case 0x3: cout << "Implement clearance lights: don't care "; break;
    }


    // Tractor rear hight mounted work lights
    switch ((at_ptrFrame->dataOctet(4) >> 6) & 0x3) {
      case 0x0: cout << "Tractor rear hight mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor rear hight mounted work lights: activate "; break;
      case 0x2: cout << "Tractor rear hight mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor rear hight mounted work lights: don't care "; break;
    }
    // Tractor rear low mounted work lights
    switch ((at_ptrFrame->dataOctet(4) >> 4) & 0x3) {
      case 0x0: cout << "Tractor rear low mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor rear low mounted work lights: activate "; break;
      case 0x2: cout << "Tractor rear low mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor rear low mounted work lights: don't care "; break;
    }
    // Tractor underside work lights
    switch ((at_ptrFrame->dataOctet(4) >> 2) & 0x3) {
      case 0x0: cout << "Tractor underside work lights: deactivate "; break;
      case 0x1: cout << "Tractor underside work lights: activate "; break;
      case 0x2: cout << "Tractor underside work lights: reserve "; break;
      case 0x3: cout << "Tractor underside work lights: don't care "; break;
    }
    // Rear fog light
    switch ((at_ptrFrame->dataOctet(4) >> 0) & 0x3) {
      case 0x0: cout << "Rear fog light: deactivate "; break;
      case 0x1: cout << "Rear fog light: activate "; break;
      case 0x2: cout << "Rear fog light: reserve "; break;
      case 0x3: cout << "Rear fog light: don't care "; break;
    }


    // Tractor front high-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 6) & 0x3) {
      case 0x0: cout << "Tractor front high-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor front high-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor front high-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor front high-mounted work lights: don't care "; break;
    }
    // Tractor front low-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 4) & 0x3) {
      case 0x0: cout << "Tractor front low-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor front low-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor front low-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor front low-mounted work lights: don't care "; break;
    }
    // Tractor side high-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 2) & 0x3) {
      case 0x0: cout << "Tractor side high-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor side high-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor side high-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor side high-mounted work lights: don't care "; break;
    }
    // Tractor side low-mounted work lights
    switch ((at_ptrFrame->dataOctet(5) >> 0) & 0x3) {
      case 0x0: cout << "Tractor side low-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor side low-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor side low-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor side low-mounted work lights: don't care "; break;
    }


    // Implement left forward work lights
    switch ((at_ptrFrame->dataOctet(6) >> 6) & 0x3) {
      case 0x0: cout << "Implement left forward work lights: deactivate "; break;
      case 0x1: cout << "Implement left forward work lights: activate "; break;
      case 0x2: cout << "Implement left forward work lights: reserve "; break;
      case 0x3: cout << "Implement left forward work lights: don't care "; break;
    }
    // Implement right forward work lights
    switch ((at_ptrFrame->dataOctet(6) >> 4) & 0x3) {
      case 0x0: cout << "Implement right forward work lights: deactivate "; break;
      case 0x1: cout << "Implement right forward work lights: activate "; break;
      case 0x2: cout << "Implement right forward work lights: reserve "; break;
      case 0x3: cout << "Implement right forward work lights: don't care "; break;
    }
    // Implement OEM option 1 light
    switch ((at_ptrFrame->dataOctet(6) >> 2) & 0x3) {
      case 0x0: cout << "Implement OEM option 1 light: deactivate "; break;
      case 0x1: cout << "Implement OEM option 1 light: activate "; break;
      case 0x2: cout << "Implement OEM option 1 light: reserve "; break;
      case 0x3: cout << "Implement OEM option 1 light: don't care "; break;
    }
    // Implement OEM option 2 light
    switch ((at_ptrFrame->dataOctet(6) >> 0) & 0x3) {
      case 0x0: cout << "Implement OEM option 2 light: deactivate "; break;
      case 0x1: cout << "Implement OEM option 2 light: activate "; break;
      case 0x2: cout << "Implement OEM option 2 light: reserve "; break;
      case 0x3: cout << "Implement OEM option 2 light: don't care "; break;
    }


    // Implement rear work lights
    switch ((at_ptrFrame->dataOctet(7) >> 6) & 0x3) {
      case 0x0: cout << "Implement rear work lights: deactivate "; break;
      case 0x1: cout << "Implement rear work lights: activate "; break;
      case 0x2: cout << "Implement rear work lights: reserve "; break;
      case 0x3: cout << "Implement rear work lights: don't care "; break;
    }
    // Implement left-facing work lights
    switch ((at_ptrFrame->dataOctet(7) >> 4) & 0x3) {
      case 0x0: cout << "Implement left-facing work lights: deactivate "; break;
      case 0x1: cout << "Implement left-facing work lights: activate "; break;
      case 0x2: cout << "Implement left-facing work lights: reserve "; break;
      case 0x3: cout << "Implement left-facing work lights: don't care "; break;
    }
    // Implement right-facing work lights
    switch ((at_ptrFrame->dataOctet(7) >> 2) & 0x3) {
      case 0x0: cout << "Implement right-facing work lights: deactivate "; break;
      case 0x1: cout << "Implement right-facing work lights: activate "; break;
      case 0x2: cout << "Implement right-facing work lights: reserve "; break;
      case 0x3: cout << "Implement right-facing work lights: don't care "; break;
    }
    // Lighting data message request
    switch ((at_ptrFrame->dataOctet(7) >> 0) & 0x3) {
      case 0x0: cout << "Lighting data message request: deactivate "; break;
      case 0x1: cout << "Lighting data message request: activate "; break;
      case 0x2: cout << "Lighting data message request: reserve "; break;
      case 0x3: cout << "Lighting data message request: don't care "; break;
    }
  }
}

void interpreteEngineSpeedMsg(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Engine speed
    cout << "Engine speed:" << static_cast<uint16_t>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) ) * 0.125 << " ";
  }
}

void interpreteVehicleSpeed(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Vehicle speed
    // J1939-71
    // 2 bytes Compass bearing 1/128 deg/bit, 0 offset, range 0 to 501.99deg
    // 2 bytes Naviagion based vehicle speed 1/256 km/h per bit, 0 offset, datarange 0 to 250.996 km/h
    // 2 bytes pitch 1/128 deg/bit, -200 deg offset, -200 to 301.99 deg
    // 2 bytes altitude 0.125 m/bit, -2500 m offset, -2500 to 5531.875 m
    cout << "Compass bearing:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(1)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(0)) ) / 128) << "° "
         << "Naviagion based vehicle speed:" << static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(2)) ) / 256 << " km/h "
         << "pitch:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(5)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) ) / 128) - 200 << "° "
         << "altitude:" << (static_cast<double>( (static_cast<uint16_t>(at_ptrFrame->dataOctet(7)) << 8) | static_cast<uint16_t>(at_ptrFrame->dataOctet(6)) ) * 0.125 ) - 2500 << " m ";
  }
}

void interpreteVehiclePosition(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Vehicle position
    // J1939-71
    // 4 bytes Latitude  10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    // 4 bytes Longitude 10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    cout << "Latitude: " << (static_cast<uint32_t>( (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 24)
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

TransferCollection_c::PtrSession_t getTransferSession(
    TransferCollection_c::Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  TransferCollection_c::PtrSession_t t_ptrSession =
    gs_main.mc_trans.getSession(
        ae_variant,
        aui8_transferSourceAddress,
        aui8_transferDestinationAddress);
  if (!t_ptrSession) {
    cout << " (ERROR, no " <<
      (TransferCollection_c::variant_tp == ae_variant ? "TP" : "ETP") <<
      " " << hex << unsigned(aui8_transferSourceAddress) <<
      "->" << hex << unsigned(aui8_transferDestinationAddress) <<") ";
  }
  return t_ptrSession;
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
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
    return;
  }

  switch (at_ptrFrame->pgn()) {

  case TP_CONN_MANAGE_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_CONN_MANAGE_PGN:
    switch (at_ptrFrame->dataOctet(0)) {
    case 0x10:
      cout << "RTS - Request to Send (TP)            ";
      {
        size_t const ct_sizeTransferData = size_t(at_ptrFrame->dataOctet(2)) << 8 | at_ptrFrame->dataOctet(1);
        (void)gs_main.mc_trans.newSession(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress(),
            ct_sizeTransferData);
      }
      break;
    case 0x14:
      cout << "RTS - Request to Send (ETP)           ";
      {
        size_t const ct_sizeTransferData = (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 24) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(2)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(1)));
        (void)gs_main.mc_trans.newSession(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress(),
            ct_sizeTransferData);
      }
      break;
    case 0x11:
      cout << "CTS - Clear to Send (TP)              ";

      break;
    case 0x15:
      cout << "CTS - Clear to Send (ETP)             ";
      break;

    case 0x16:
      cout << "DPO - Data Packet Offset (ETP)        ";
      {
        TransferCollection_c::PtrSession_t t_ptrSession =
          getTransferSession(
              e_variant,
              at_ptrFrame->sourceAddress(),
              at_ptrFrame->destinationAddress());
        if (t_ptrSession)
          t_ptrSession->mui32_packetOffSet =
            ((static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 8 ) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(2))));
      }
      break;
    case 0x13:
      cout << "EoMACK - End of Message Ack (TP)      ";
      b_streamEnd = true;
      break;
    case 0x17:
      cout << "EoMACK - End of Message Ack (ETP)     ";
      b_streamEnd = true;
      break;
    case 0x20:
      cout << "BAM - Broadcast Announce Msg (TP)     ";
      break;
    case 0xFF:
      cout << "CONNABORT - Connection Abort (TP/ETP) ";
      gs_main.mc_trans.deleteSession(e_variant, at_ptrFrame->sourceAddress(), at_ptrFrame->destinationAddress());
      break;
    default: return;
    }
    {
      TransferCollection_c::PtrSession_t t_ptrSession =
        getTransferSession(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress());
      if (t_ptrSession) {
        t_ptrSession->mui32_embeddedPgn =
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)));
        cout << " on " << setw(6) << setfill('0') << t_ptrSession->mui32_embeddedPgn << " (";
        interpretePgn(t_ptrSession->mui32_embeddedPgn);
        cout << ")";
      }
    }
    break;

  case TP_DATA_TRANSFER_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_DATA_TRANSFER_PGN:
    cout << "DATA - Data Packet #"<<setw(2)<<setfill(' ')<<dec<<uint16_t(at_ptrFrame->dataOctet(0));
    {
      TransferCollection_c::PtrSession_t t_ptrSession =
        getTransferSession(
            e_variant,
            at_ptrFrame->sourceAddress(),
            at_ptrFrame->destinationAddress());
      if (!t_ptrSession)
        ;
      else if ((at_ptrFrame->pgn() == TP_DATA_TRANSFER_PGN)) {
        for (int i = 0; i < 7; i++) {
          if ((7 *  (at_ptrFrame->dataOctet(0) - 1)) + i >= int(t_ptrSession->mvec_data.size()))
            break;
          size_t t_index = (7 *  (at_ptrFrame->dataOctet(0) - 1)) + i;
          (t_ptrSession->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
        }
      } else if (at_ptrFrame->pgn() == ETP_DATA_TRANSFER_PGN) {
        for (int i = 0; i < 7; i++) {
          size_t t_index = (t_ptrSession->mui32_packetOffSet * 7 + (7 *  (at_ptrFrame->dataOctet(0) - 1))) + i;
          if (t_index >= t_ptrSession->mvec_data.size())
            break;
          (t_ptrSession->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
        }
      }
    }
    break;
  default:
    break;
  }

  if (b_streamEnd) { //instead of obsolete endOfTransfer(e_variant);
    TransferCollection_c::PtrSession_t t_ptrSession =
      getTransferSession(
          e_variant,
          at_ptrFrame->destinationAddress(),
          at_ptrFrame->sourceAddress());
    if (t_ptrSession) {
      endOfTransfer(at_ptrFrame, e_variant);
    }
  }
}

void endOfTransfer(
    PtrDataFrame_t at_ptrFrame,
    TransferCollection_c::Variant_e ae_variant)
{
  TransferCollection_c::PtrSession_t t_ptrSession =
    getTransferSession(
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  if (t_ptrSession) {
    PtrDataFrame_t t_ptrArtificialFrame = new DataFrame_c(
        t_ptrSession->mvec_data,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());

    // output in both ways, dump (raw) and interprete:
    dump(t_ptrArtificialFrame);
    interpretePgnData(t_ptrArtificialFrame);

    gs_main.mc_trans.deleteSession(
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  }
}

void interpretePgnData(PtrDataFrame_t at_ptrFrame)
{
  Interprete_t *p_f = 0;

  switch (at_ptrFrame->pgn())
  {
  case VT_TO_ECU_PGN:                           p_f = interpretePgnsVtToEcu; break;
  case ECU_TO_VT_PGN:                           p_f = interpretePgnsVtFromEcu; break;
  case ACKNOWLEDGEMENT_PGN:                     p_f = interpretePgnAcknowledge; break;
  case CLIENT_TO_FS_PGN:                        p_f = interpretePgnsCl2Fs; break;
  case FS_TO_CLIENT_PGN:                        p_f = interpretePgnsFs2Cl; break;
  case ETP_DATA_TRANSFER_PGN:                   p_f = interpretePgnsTPETP; break;
  case ETP_CONN_MANAGE_PGN:                     p_f = interpretePgnsTPETP; break;
  case TP_DATA_TRANSFER_PGN:                    p_f = interpretePgnsTPETP; break;
  case TP_CONN_MANAGE_PGN:                      p_f = interpretePgnsTPETP; break;
  case LANGUAGE_PGN:                            p_f = interpretePgnLanguage; break;
  case LIGHTING_COMMAND_PGN:                    p_f = interpreteLightingCommand; break;
  case REAR_PTO_STATE_PGN:                      p_f = interpreteRearPTOstate; break;
  case REAR_HITCH_STATE_PGN:                    p_f = interpreteRearHitch; break;
  case WHEEL_BASED_SPEED_DIST_PGN:              p_f = interpreteWheelBasedSpeedDist; break;
  case GROUND_BASED_SPEED_DIST_PGN:             p_f = interpreteGroundBasedSpeedDist; break;
  case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE:  p_f = interpreteEngineSpeedMsg; break;
  case VEHICLE_DIRECTION_SPEED:                 p_f = interpreteVehicleSpeed; break;
  case VEHICLE_POSITION:                        p_f = interpreteVehiclePosition; break;

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
    /// @todo SOON-260: to be done...
#define AUX_VALVE_0_ESTIMATED_FLOW  0x00FE10LU
#define AUX_VALVE_1_ESTIMATED_FLOW  0x00FE11LU
    //...
#define AUX_VALVE_0_MEASURED_FLOW   0x00FE20LU
#define AUX_VALVE_1_MEASURED_FLOW   0x00FE21LU
    //...
#define AUX_VALVE_0_COMMAND         0x00FE30LU
#define AUX_VALVE_1_COMMAND         0x00FE31LU
    //...
  }

  if (p_f)
    p_f(at_ptrFrame);
}

void interpretePgn(uint32_t rui32_pgn)
{
  switch (rui32_pgn)
  {
    case VT_TO_ECU_PGN:                  cout << "VT_TO_ECU         "; break;
    case ECU_TO_VT_PGN:                  cout << "ECU_TO_VT         "; break;
    case ACKNOWLEDGEMENT_PGN:            cout << "ACKNOWLEDGEMENT   "; break;
    case PROCESS_DATA_PGN:               cout << "PROCESS_DATA      "; break;
    case CLIENT_TO_FS_PGN:               cout << "CLIENT_TO_FS      "; break;
    case FS_TO_CLIENT_PGN:               cout << "FS_TO_CLIENT      "; break;
    case GUIDANCE_MACHINE_STATUS:        cout << "GUIDANCE_MACH_ST  "; break;
    case GUIDANCE_SYSTEM_CMD:            cout << "GUIDANCE_SYS_CMD  "; break;
    case ISOBUS_CERTIFICATION_PGN:       cout << "ISOBUS_CERTIFICAT."; break;
    case ETP_DATA_TRANSFER_PGN:          cout << "ETP_DATA_TRANSFER "; break;
    case ETP_CONN_MANAGE_PGN:            cout << "ETP_CONN_MANAGE   "; break;
    case REQUEST_PGN_MSG_PGN:            cout << "REQUEST_MSG       "; break;
    case TP_DATA_TRANSFER_PGN:           cout << "TP_DATA_TRANSFER  "; break;
    case TP_CONN_MANAGE_PGN:             cout << "TP_CONN_MANAGE    "; break;
    case ADDRESS_CLAIM_PGN:              cout << "ADDRESS_CLAIM     "; break;
    case PROPRIETARY_A_PGN:              cout << "PROPRIETARY_A     "; break;
    case PROPRIETARY_A2_PGN:             cout << "PROPRIETARY_A2    "; break;
    case WORKING_SET_MEMBER_PGN:         cout << "WORKING_SET_MEMBER "; break;
    case WORKING_SET_MASTER_PGN:         cout << "WORKING_SET_MASTER "; break;
    case LANGUAGE_PGN:                   cout << "LANGUAGE          "; break;
    case LIGHTING_DATA_PGN:              cout << "LIGHTING_DATA     "; break;
    case LIGHTING_COMMAND_PGN:           cout << "LIGHTING_COMMAND  "; break;
    case HITCH_PTO_COMMANDS:             cout << "HITCH_PTO_COMMANDS "; break;
    case REAR_PTO_STATE_PGN:             cout << "REAR_PTO_STATE    "; break;
    case FRONT_PTO_STATE_PGN:            cout << "FRONT_PTO_STATE   "; break;
    case REAR_HITCH_STATE_PGN:           cout << "REAR_HITCH_STATE  "; break;
    case FRONT_HITCH_STATE_PGN:          cout << "FRONT_HITCH_STATE "; break;
    case MAINTAIN_POWER_REQUEST_PGN:     cout << "MAINTAIN_POWER_REQ"; break;
    case WHEEL_BASED_SPEED_DIST_PGN:     cout << "WHEEL_BASED_SPEED_DIST "; break;
    case GROUND_BASED_SPEED_DIST_PGN:    cout << "GROUND_BASED_SPEED_DIST "; break;
    case SELECTED_SPEED_CMD:             cout << "SELECTED_SPEED_CMD "; break;
    case SELECTED_SPEED_MESSAGE:         cout << "SELECTED_SPEED_MESSAGE "; break;
    case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE: cout << "ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE "; break;
    case SOFTWARE_IDENTIFICATION_PGN:    cout << "SOFTWARE_IDENTIFICATION "; break;
    case TIME_DATE_PGN:                  cout << "TIME_DATE         "; break;
    case VEHICLE_DIRECTION_SPEED:        cout << "VEHICLE_DIRECTION_SPEED "; break;
    case VEHICLE_POSITION:               cout << "VEHICLE_POSITION "; break;
    case PROPRIETARY_B_PGN:              cout << "PROPRIETARY_B(1of) "; break;
    case NMEA_GPS_POSITION_RAPID_UPDATE_PGN: cout << "NMEA_GPS_POSITION_RAPID_UPDATE "; break;
    case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:  cout << "NMEA_GPS_COG_SOG_RAPID_UPDATE "; break;
    case NMEA_GPS_POSITION_DATA_PGN:     cout << "NMEA_GPS_POSITION_DATA "; break;
    case NMEA_GPS_DIRECTION_DATA_PGN:    cout << "NMEA_GPS_DIRECTION_DATA "; break;
    case NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS:  cout << "GNSS Pseudorange Noise Statistics "; break;
    default:                             cout << std::hex << "0x" << rui32_pgn << std::dec; break;
        /// @todo SOON-260: to be done...
#define AUX_VALVE_0_ESTIMATED_FLOW  0x00FE10LU
#define AUX_VALVE_1_ESTIMATED_FLOW  0x00FE11LU
        //...
#define AUX_VALVE_0_MEASURED_FLOW   0x00FE20LU
#define AUX_VALVE_1_MEASURED_FLOW   0x00FE21LU
        //...
#define AUX_VALVE_0_COMMAND         0x00FE30LU
#define AUX_VALVE_1_COMMAND         0x00FE31LU
        //...
  }
}




pair< int, PtrDataFrame_t > parseLogLine()
{
  int const ci_errorCodeIfNotOverridden = -1;
  pair< int, PtrDataFrame_t > result = std::make_pair(
      ci_errorCodeIfNotOverridden, // default to error, will always be set
      PtrDataFrame_t(0) );
  switch (gs_main.mt_logType)
  {
    case logTypeCanServer: result = parseLogLineCanServer();   break;
    case logTypeRte:       result = parseLogLineRte();         break;
    case logTypeCANMon:    result = parseLogLineCANMon();      break;
    case logTypeCANoe:     result = parseLogLineCANoe();       break;
    case logTypeA1ASCII:   result = parseLogLineA1ASCII();     break;
    case logTypeTrc:       result = parseLogLineTrc();         break;
    case logTypeJohnDeere: result = parseLogLineJohnDeere();   break;
    case logTypeRte2:      result = parseLogLineRte2();        break;
    case logTypeJrf:       result = parseLogLineJrf();         break;
    default:               exit_with_error("Unknown Log-Type!"); return std::make_pair(1, result.second); // return just to satisfy compiler. exit_with_error will exit anyway ;)
  }
  if (result.first == 0) // no error
  { /// Printout interpreted line
    PtrDataFrame_t t_ptrFrame = result.second;

    // Timestamp
    cout << setfill (' ') << dec << setw (10) << (t_ptrFrame->time()/1000) << "." << setfill('0')<<setw(3)<<(t_ptrFrame->time()%1000)<< "   ";

    // CAN-ID / number of bytes
    cout << setfill ('0') << hex << setw (8) << t_ptrFrame->identifier() << "  " << uint16_t(t_ptrFrame->dataSize()) << " ";

    // Databytes (HEX)
    size_t i;
    for (i=0; i< t_ptrFrame->dataSize(); i++) cout << " " << setw (2) <<uint16_t(t_ptrFrame->dataOctet(i));
    for (;    i<8;              i++) cout << "   ";
    cout << "  ";

    // Databytes (HEX)
    for (i=0; i< t_ptrFrame->dataSize(); i++) cout << setw (1) << char(t_ptrFrame->asciiDataOctet(i));
    for (;    i<8;              i++) cout << " ";

    // SA
    cout << "  "<<setw(2) << uint16_t(t_ptrFrame->sourceAddress()) << "->";
    // DA
    if (t_ptrFrame->isPdu1()) cout << setw(2) << uint16_t(t_ptrFrame->ps());
    else cout << "FF";

    // Priority
    cout << " (" << uint16_t(t_ptrFrame->prio()) << ")";

    // PGN
    cout << " " << setw(6) << t_ptrFrame->pgn() << " => ";
    // Interpreted PGN
    interpretePgn(t_ptrFrame->pgn());
    // Interpreted PGN-Data
    interpretePgnData (t_ptrFrame);
    cout << endl;
  }
  else
  {
    cout << "---- line missing - error in parsing!----"<<endl; /// @todo SOON-260: replace by the original line!
  }
  return result;
}


typedef enum
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
} msgType_t;

typedef struct
{
  vector<uint64_t> svec_alives [256];
  vector<msgType_t> svec_response [256];
  const char* name;
  int32_t alivePeriod; // ==0 => single Event - >0 ==> periodic Event - <0 ==> handshaking Event
} msgType_s;

msgType_s struct_messages [5];



void checkAlives(PtrDataFrame_t at_ptrFrame)
{
  if ( ((at_ptrFrame->pgn() == VT_TO_ECU_PGN) && (at_ptrFrame->dataOctet (0) == 0xFE))
       ||
       ((at_ptrFrame->pgn() == ECU_TO_VT_PGN) && (at_ptrFrame->dataOctet (0) == 0xFF))
     )
  {
    struct_messages[0].svec_alives [at_ptrFrame->sourceAddress()].push_back (at_ptrFrame->time());
  }


  if ( ((at_ptrFrame->pgn() == PROCESS_DATA_PGN) && (at_ptrFrame->dataOctet (0) == 0x0E))
       ||
       ((at_ptrFrame->pgn() == PROCESS_DATA_PGN) && (at_ptrFrame->dataOctet (0) == 0x0F))
     )
  {
    struct_messages[1].svec_alives [at_ptrFrame->sourceAddress()].push_back (at_ptrFrame->time());
  }

}



void checkSingles(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->pgn() == ACKNOWLEDGEMENT_PGN)
  {
    struct_messages[2].svec_alives [at_ptrFrame->sourceAddress()].push_back (at_ptrFrame->time());
  }
}



void checkHandshakingsVtCommands(PtrDataFrame_t at_ptrFrame)
{
  int mode; // no need to initialize.

  if ((at_ptrFrame->dataOctet(0) >= 0x60) && (at_ptrFrame->dataOctet(0) < 0xF0))
  { // not a maintenance command!
    msgType_t msgType_direction;
    uint8_t ui8_saClient;
    if (at_ptrFrame->pgn() == VT_TO_ECU_PGN)
    {
      mode = 3;
      msgType_direction = msgTypeResponse;
      ui8_saClient = at_ptrFrame->ps();
    }
    else if (at_ptrFrame->pgn() == ECU_TO_VT_PGN)
    {
      mode = 3;
      msgType_direction = msgTypeCommand;
      ui8_saClient = at_ptrFrame->sourceAddress();
    }
    else return;
    struct_messages[mode].svec_response [ui8_saClient].push_back (msgType_direction);
    struct_messages[mode].svec_alives   [ui8_saClient].push_back (at_ptrFrame->time());
    return;
  }
}


void checkHandshakingTP(PtrDataFrame_t at_ptrFrame)
{
  msgType_t mtype;
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
    case 0x14: mtype = msgTypeRTS; ui8_saClient = at_ptrFrame->sourceAddress(); break;
    case 0x11:
    case 0x15: mtype = msgTypeCTS; ui8_saClient = at_ptrFrame->destinationAddress(); break;
    case 0x16: mtype = msgTypeDPO; ui8_saClient = at_ptrFrame->sourceAddress(); break;
    case 0x13:
    case 0x17: mtype = msgTypeEOMACK; ui8_saClient = at_ptrFrame->destinationAddress();break;
    case 0xFF: mtype = msgTypeCONNABORT; ui8_saClient = at_ptrFrame->sourceAddress();
      // this message is special case! This aborts SA->DA and also DA->SA!
      struct_messages[4].svec_response [at_ptrFrame->destinationAddress()].push_back (mtype);
      struct_messages[4].svec_alives   [at_ptrFrame->destinationAddress()].push_back (at_ptrFrame->time());
      break; // CONN ABORT BY SENDER!
      /// @todo SOON-260: Damn, can't we detect ConnAbort by Receiver here, too?
    }
    break;

  default:
    return;
  }

  struct_messages[4].svec_response [ui8_saClient].push_back (mtype);
  struct_messages[4].svec_alives   [ui8_saClient].push_back (at_ptrFrame->time());
}


void setup()
{
  struct_messages [0].name = "VT Server<->Client Periodic Alive";
  struct_messages [0].alivePeriod = 1000;

  struct_messages [1].name = "TC Server<->Client Periodic Alive";
  struct_messages [1].alivePeriod = 2000;

  struct_messages [2].name = "NOT Acknowledge (NACK)";
  struct_messages [2].alivePeriod = 0; // means NO alivePeriod!

  struct_messages [3].name = "VT Command-Response times";
  struct_messages [3].alivePeriod = -1500; // means Handshaking!

  struct_messages [4].name = "(E)TP-communication times";
  struct_messages [4].alivePeriod = -500; // means Handshaking! (damn, we have mixed periods here...)
}

int main (int argc, char** argv)
{
  cerr << "ISOBUS-Logalizer (c) 2007-2010 OSB AG." << endl << endl;

  if (argc < 2)
    exit_with_usage(argv[0]);

  setup();

  string str_filename = argv[1];
  gs_main.t_ptrIn = PtrInputStream_t( new InputStream_c(str_filename) );

  if (!gs_main.t_ptrIn->isOpen()) exit_with_error("Couldn't open file");

  if (argc >= 3)
    gs_main.mt_logType = (logType_t) atoi (argv[2]);
  else // default to can_server
    gs_main.mt_logType = logTypeCanServer;

  if (argc >= 4)
    gs_main.mt_sizeMultipacketWrap = atoi (argv[3]);
  else // default to can_server
    gs_main.mt_sizeMultipacketWrap = 32;

  while (!gs_main.t_ptrIn->raw().eof())
  {
    pair< int, PtrDataFrame_t > parse_result = parseLogLine();
    if (0 == parse_result.first)
    {
      checkAlives(parse_result.second);
      checkSingles(parse_result.second);
      checkHandshakingsVtCommands(parse_result.second);
      checkHandshakingTP(parse_result.second);
    }
  }

  gs_main.t_ptrIn = PtrInputStream_t(0);

  std::cerr << "Alriiiiite... printing out the results..." << endl;

  for (int i=0; i<256; i++)
  {
    for (unsigned int loop_msg=0; loop_msg<(sizeof(struct_messages)/sizeof(msgType_s)); loop_msg++)
    {
      const int32_t ci32_alivePeriod = struct_messages[loop_msg].alivePeriod;
      if (struct_messages[loop_msg].svec_alives[i].size()>0)
      {
        if (ci32_alivePeriod > 0)
        { // we have a periodic event!
          std::cout << endl << "ISOBUS node with SA="<<hex<<i<<dec<<" had the following alive-times for ["<<struct_messages[loop_msg].name<<"] with alive-periods of "<<struct_messages[loop_msg].alivePeriod<<" ms:"<<endl;
        }
        else if (ci32_alivePeriod < 0)
        { // we have a handshaking event!
          std::cout << endl << "ISOBUS node with SA="<<hex<<i<<dec<<" had the following alive-times for ["<<struct_messages[loop_msg].name<<"] with alive-periods of "<<(-struct_messages[loop_msg].alivePeriod)<<" ms:"<<endl;
        }
        else
        { // sinlge events!! "== 0"
          std::cout << endl << "ISOBUS node with SA="<<hex<<i<<dec<<" sent out ["<<struct_messages[loop_msg].name<<"] at the following times:"<<endl;
        }
        vector<msgType_t>::iterator type_iter=struct_messages[loop_msg].svec_response[i].begin();
        for (vector<uint64_t>::iterator iter=struct_messages[loop_msg].svec_alives[i].begin();
             iter != struct_messages[loop_msg].svec_alives[i].end();
             iter++)
        {
          cout << setfill (' ');
          cout << "absolute time: "<<setw(10)<<(*iter/1000)<<"."<<setw(3)<<setfill('0')<<(*iter%1000)<<setfill(' ');
          if (iter != struct_messages[loop_msg].svec_alives[i].begin())
          {
            const uint64_t cui32_delta = ( *(iter) - *(iter-1) );
            cout<< "  relative time: "<<setw(10)<<cui32_delta;

            if (ci32_alivePeriod > 0)
            { // print out the alivePeriod-deviation!
              cout<<" deviation: ";
              int deviation = int ((double (int32_t (cui32_delta)-ci32_alivePeriod) / ci32_alivePeriod) * 100);
              uint8_t deviation_bias = (deviation > 0) ? '+' : '-';
              deviation = (deviation < 0) ? -deviation : deviation;
              if (deviation > 100)
              {
                std::cout << "EXTREME DEVIATION(!!) OF " << setw(10) << deviation << "0";
              }
              else
              {
                while (deviation > 10)
                {
                  cout << deviation_bias;
                  deviation -= 10;
                }
              }
            }
            else if (ci32_alivePeriod < 0)
            { // Handshaking
              if (type_iter == struct_messages[loop_msg].svec_response[i].end()) exit_with_error("No direction/msgType set but is expected. System failure.");
              int32_t i32_alivePeriodSpecial;
              switch (*type_iter)
              {
                case msgTypeResponse: cout << " Response  "; i32_alivePeriodSpecial = -ci32_alivePeriod; break;
                case msgTypeCommand:  cout << " Command   "; i32_alivePeriodSpecial = 0; break; // no timing-requirement here!
                case msgTypeRTS:      cout << " (E)TP-CONN: Request to Send (RTS)         "; i32_alivePeriodSpecial = 0; break; // no timing-requirement here!
                case msgTypeCTS:      cout << " (E)TP-CONN: Clear to Send (CTS)           "; i32_alivePeriodSpecial = 1250; break;
                case msgTypeDPO:      cout << " (E)TP-CONN: Data Packet Offset (DPO)      "; i32_alivePeriodSpecial = 1250; break; /// @todo SOON-260: set the correct values here!
                case msgTypeEOMACK:   cout << " (E)TP-CONN: End of Message ACK (EoMACK)   "; i32_alivePeriodSpecial = 1250; break;
                case msgTypeDATA:     cout << " (E)TP-DATA                                "; i32_alivePeriodSpecial = 250; break;
                case msgTypeCONNABORT:cout << " (E)TP-CONN: Connection Abort (CONNABORT)  "; i32_alivePeriodSpecial = -1; break; // doesn't come normally!
                default:              cout << " ???                                       "; i32_alivePeriodSpecial = 0; break;
              }
              if ( (*type_iter == msgTypeResponse) && (*(type_iter-1) == msgTypeResponse) ) cout << " - RESPONSE FOLLOWING A RESPONSE!";
              if ( (*type_iter == msgTypeCommand)  && (*(type_iter-1) == msgTypeCommand)  ) cout << " - COMMAND FOLLOWING A COMMAND!";
              if (i32_alivePeriodSpecial > 0)
              { // print out the time it took!
                if (cui32_delta > (unsigned int) (i32_alivePeriodSpecial)) cout << " *** !!! TIMEOUT - Check relative time!!!! ***";
                else
                {
                  int32_t time = int32_t ((cui32_delta*100) / i32_alivePeriodSpecial);
                  cout <<setw(2)<<time<< " percent of timeout ("<<setw(4)<<i32_alivePeriodSpecial<<"): (one '%' shows 10%) ";
                  while (time > 10)
                  {
                    cout << "%";
                    time -= 10;
                  }
                }
              }
              else if (i32_alivePeriodSpecial < 0)
              { // unsolicited messages (like CONNABORT)
                cout << "*** UNEXPECTED/UNSOLICITED MESSAGE ***";
              }
            }
          }
          cout << endl;
          if (type_iter != struct_messages[loop_msg].svec_response[i].end()) type_iter++;
        }
      }
    }
  }
}

void interpretePgnsFs2Cl(
    PtrDataFrame_t at_ptrFrame)
{
  uint32_t i_pathNameLength = 0;
  uint16_t msg_count = 0;
  uint16_t msg_offset = 0;

  cout << "SA: " << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << " DA: " << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " length Data: " << setw(4)<<setfill('0') << uint32_t(at_ptrFrame->dataSize());

  switch (uint32_t(at_ptrFrame->dataOctet(0))) {
    case 0x00:
      cout << "\tFile Server Status: ";
        (at_ptrFrame->dataOctet(1) & 0x2) ? cout << "Busy writing" : cout << "Not Busy writing";
        (at_ptrFrame->dataOctet(1) & 0x1) ? cout << " Busy reading" : cout << " Not Busy reading";
      cout << " Number of Open Files: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(2));
      break; //File Server Status

    case 0x01:

      cout << "\tGet File Server Properties Response Version Number:";
      switch (at_ptrFrame->dataOctet(1)) {
        case 0x00: cout << " Draft version of the Standard"; break;
        case 0x01: cout << " Final Draft version of the Standard"; break;
        case 0x02: cout << " First published version of the Standard"; break;
      }
      cout << " Maximum Number of Simuntaneously Open Files: " << hex << uint32_t(at_ptrFrame->dataOctet(2));
      cout << " File Server Capabilities : ";
      (at_ptrFrame->dataOctet(3)) ? cout << " File server supports multiple volumes"
                : cout << " File server does not supports multiple volumes";
      break; //Get File Server Properties Response

    case 0x10:

      cout << "\tGet Current Directory Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      cout << " Total Space: " << setw(8) <<  ((static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 24) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 16) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 8 ) |
		                                      (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)))) << "(* 512 bytes)";
      cout << " Free Space: " << setw(8) << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(10)) << 24) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 9)) << 16) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 8)) << 8 ) |
		                                    (static_cast<uint32_t>(at_ptrFrame->dataOctet( 7)))) << "(* 512 bytes)";
      i_pathNameLength = (static_cast<uint16_t>(at_ptrFrame->dataOctet(12))<<8) | at_ptrFrame->dataOctet(11);
      cout << " Path Name Length: " << setw(4) << i_pathNameLength;

      cout << " Path Name: ";
      for (uint16_t i_count = 0; i_count < i_pathNameLength; i_count++)
      {
        cout << uint8_t(at_ptrFrame->dataOctet(13 + i_count));
      }

      break; //Get Current Directory Response
    case 0x11:

      cout << "\tChange Current Directory Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));

      break; //Chanege Current Directory Response
    case 0x20:

      cout << "\tOpen File Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      cout << " Handle: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(3));
      decodeAttributes(at_ptrFrame->dataOctet(4));

      break; //Open File Response
    case 0x21:

      cout << "\tSeek File Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      cout << " Position : " << setw(8) << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 24) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 16) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 8 ) |
		                                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))));
      break; //Seek File Response
    case 0x22:

      cout << "\tRead File/Directory Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));

      msg_count = uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3));
      msg_offset = 5;

      cout << " Count: " << setw(4) << msg_count << " ";

      //case file-data
      if (at_ptrFrame->dataSize() == msg_count + 5u) {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          cout << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(5 + loop)) << " ";
        }
      }
      //else directory entries
      else {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          uint8_t filenamelength = uint8_t(at_ptrFrame->dataOctet(msg_offset));
          if (filenamelength == 0xff)
            break;
          cout << " Filename length: " << uint16_t(filenamelength) << " ";
          cout << " Filename: ";
          uint8_t loopfilename = 0;
          for (; loopfilename < filenamelength; loopfilename++) {
            cout << uint8_t(at_ptrFrame->dataOctet(msg_offset + 1 + loopfilename));
          }
          decodeAttributes(at_ptrFrame->dataOctet(msg_offset + 1 + loopfilename));
          decodeDate(uint16_t(at_ptrFrame->dataOctet(msg_offset + 3 + loopfilename) << 8 | uint16_t(at_ptrFrame->dataOctet(msg_offset + 2 + loopfilename))));
          decodeTime(uint16_t(at_ptrFrame->dataOctet(msg_offset + 5 + loopfilename) << 8 | uint16_t(at_ptrFrame->dataOctet(msg_offset + 4 + loopfilename))));
          cout << " FileSize: " << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 9 + loopfilename)) << 24) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 8 + loopfilename)) << 16) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 7 + loopfilename)) << 8 ) |
		                           (static_cast<uint32_t>(at_ptrFrame->dataOctet(msg_offset + 6 + loopfilename))));
          msg_offset = uint16_t(msg_offset + filenamelength + 10);
        }
      }

      break; //Read File/Directory Response
    case 0x23:

      cout << "\tWrite File Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      cout << " Count: " << setw(4) << uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3));
      break; // Write File Response
    case 0x24:

      cout << "\tClose File Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Close File Response
    case 0x30:

      cout << "\tMove File Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Move File Response
    case 0x31:

      cout << "\tDelete File Repsonse TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Delete File Repsonse
    case 0x32:

      cout << "\tGet File Attribute Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      decodeAttributes(at_ptrFrame->dataOctet(3));
      cout << " Size: " << ((static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 24) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 16) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) << 8 ) |
		                   (static_cast<uint32_t>(at_ptrFrame->dataOctet(4))));
      break; // Get File Attribute Response
    case 0x33:

      cout << "\tSet File Attribute Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      break; // Set File Attribute Response
    case 0x34:

      cout << "\tGet File Date & Time Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
      decodeErrorCode(at_ptrFrame->dataOctet(2));
      decodeDate(uint16_t((at_ptrFrame->dataOctet(4)<<8) | at_ptrFrame->dataOctet(3)));
      decodeTime(uint16_t((at_ptrFrame->dataOctet(6)<<8) | at_ptrFrame->dataOctet(5)));
      break; // Get File Date & Time Response
    case 0x40:

      cout << "\tInitialize Volume Response TAN: " << setw(2) << hex << uint32_t(at_ptrFrame->dataOctet(1));
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

  cout << ")" << endl;

  cout << "              " << hex << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << "->" << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " " << dec << setw(7)<<setfill(' ') << uint32_t(at_ptrFrame->dataSize()) << "  " << hex;

  for (size_t blockoffset=0; blockoffset < at_ptrFrame->dataSize(); blockoffset += local_multipacketWrap)
  {
    if (blockoffset > 0)
      cout << "                              ";

    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        cout << setw(2)<<setfill('0') << uint32_t(at_ptrFrame->dataOctet(blockoffset+inblock)) << " ";
      else
        cout << "   ";
    }
    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        cout << getAscii (at_ptrFrame->dataOctet(blockoffset+inblock));
      else
        cout << " ";
    }
    cout << endl;
  }
}

void decodeErrorCode(uint8_t i_errorCode)
{
  switch (uint32_t(i_errorCode)) {
    case 0x00:
      cout << " Success" ;
      break;
    case 0x01:
      cout << " Access Denied" ;
      break;
    case 0x02:
      cout << " Invalid Access" ;
      break;
    case 0x03:
      cout << " Too many files open" ;
      break;
    case 0x04:
      cout << " File or path not found" ;
      break;
    case 0x05:
      cout << " Invalid Handle" ;
      break;
    case 0x06:
      cout << " Invalid given source name" ;
      break;
    case 0x07:
      cout << " Invalid given destination name" ;
      break;
    case 0x08:
      cout << " Out of free space" ;
      break;
    case 0x09:
      cout << " Failure during write operation" ;
      break;
    case 0x0a:
      cout << " Volume is possibly not initialized" ;
      break;
    case 0x0b:
      cout << " Failure during a read operation" ;
      break;
    case 0x0c:
      cout << " Function not supported" ;
      break;
    case 0x2A:
      cout << " Invalid request length" ;
      break;
    case 0x2B:
      cout << " Out of memory" ;
      break;
    case 0x2C:
      cout << " Any other error" ;
      break;
    case 0x2D:
      cout << " End of file reached" ;
      break;
    default:
      cout << " Reserved" ;
      break;
  }
}

void decodeAttributes(uint8_t i_attributes)
{
  (i_attributes & 128)? cout << " Volume is case-sensitive"  : cout << " Volume is case-insensitive" ;
  (i_attributes & 64)? cout << " Volume is not removable"  : cout << " Volume is removable" ;
  (i_attributes & 32)? cout << " Volume does support long filenames"  : cout << " Volume does not support long filenames" ;
  (i_attributes & 16)? cout << " Handle specifies a directory"  : cout << " Handle does not specify a directory" ;
  (i_attributes & 8)? cout << " Handle specifies a volume"  : cout << " Handle does not specify a volume" ;
  (i_attributes & 2)? cout << " Hidden attribute is set"  : cout << " Hidden attribute is not set" ;
  (i_attributes & 1)? cout << " Read-only attribute is set"  : cout << " Read-only attribute is not set" ;
}

void decodeDate(uint16_t i_date)
{

  cout << " " << setw(4) << dec << uint32_t(1980 + ((i_date >> 9) & 0x7F)) << "-" << setw(2) << dec << uint32_t((i_date >> 5) & 0xF) << "-" << setw(2) << dec << uint32_t((i_date) & 0x1F);

}

void decodeTime(uint16_t i_time)
{
  cout << " " << setw(2) << dec << uint32_t(((i_time >> 11) & 0x1F)) << "-" << setw(2) << dec << uint32_t(((i_time >> 5) & 0x3F)) << "-" << setw(2) << dec << uint32_t(2 * ((i_time) & 0x1F));
}
