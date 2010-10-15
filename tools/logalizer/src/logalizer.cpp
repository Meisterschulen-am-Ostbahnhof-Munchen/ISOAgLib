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
#include <../../libs/misc/yasper.h>

using namespace std;

ifstream ifs_in;

uint64_t can_time;
uint32_t can_id;
uint8_t  can_data[8];
uint8_t can_bytes;
bool can_ext; /// @todo SOON-260: to be used! =)

int i_multipacketWrap=0; // default will be set when parsing parameters

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

  std::map< Key_s, PtrSession_t > mmap_transfers;
} gc_transferCollection;

TransferCollection_c::PtrSession_t TransferCollection_c::newSession(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress,
    size_t at_sizeTransferData)
{
  PtrSession_t t_ptrSession = new Session_s(at_sizeTransferData);
  mmap_transfers[
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
  return mmap_transfers[
      Key_s(
          ae_variant,
          aui8_transferSourceAddress,
          aui8_transferDestinationAddress) ];
}

void TransferCollection_c::deleteSession(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  mmap_transfers.erase(
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

char getAscii(uint8_t val) { return ((val >= 0x20) && (val < 0x7F)) ? (char(val)) : '.'; }

uint8_t  canGetPrio() { return uint8_t(can_id >> 26); }
uint8_t  canGetDp()   { return (can_id >> 24) & 0x03; }
uint8_t  canGetSa()   { return (can_id      ) & 0xFF; }
uint8_t  canGetPs()   { return (can_id >>  8) & 0xFF; }
uint8_t  canGetDa()   { return (can_id >>  8) & 0xFF; } // only if we have PDU1-format!
uint8_t  canGetPf()   { return (can_id >> 16) & 0xFF; }
uint32_t canGetPgn()  { return ((canGetDp() << 16) | (canGetPf() << 8) | ((canGetPf() >= 0xF0) ? canGetPs():0)); }
uint64_t canGetTime() { return (can_time); }
uint8_t  canGetData(int i) { return (can_data[i]); }
char     canGetAsciiData(int i) { return getAscii (can_data[i]); }
uint8_t  canGetBytes() {return (can_bytes); }
bool     canIsPdu1()  { return (canGetPf() < 0xF0); }// with DA

uint64_t rte_time = 0;

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

logType_t logType;

void analyzeCl2FS(uint8_t, uint8_t, uint16_t, uint8_t[]);
void analyzeFS2Cl(uint8_t, uint8_t, uint16_t, uint8_t[]);
void analyzeRest(uint8_t, uint8_t, uint16_t, uint8_t[], uint32_t);
void decodeErrorCode(uint8_t);
void decodeAttributes(uint8_t);
void decodeDate(uint16_t);
void decodeTime(uint16_t);
void interpretePgnsCl2FS();
void interpretePgnsFS2Cl();
void interpretePgnsVtEcu(bool);


void exit_with_usage(const char* progname)
{
  cout << "Usage: " << progname << " <logFile> [logType] [wrapMultipacket]" << endl << endl;
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



int parseLogLineCANMon() //RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif

  int i1, i2, i3, i4, i5, i6, i7, i8, iA, iB; // "%i* %x
  int parsed_count = sscanf (line.c_str(), "%*s %i %*i %x%*s%x %x %x %x %x %x %x %x", &iA, &iB, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);

  can_time = iA;
  can_id = iB;
  can_bytes = uint8_t(parsed_count-2);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count >= 2) ? 0 : -1;
}

int parseLogLineJohnDeere() // "r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....\\. "
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
  int i1, i2, i3, i4, i5, i6, i7, i8, iA, iB; // "%i* %x
  int parsed_count = sscanf (line.c_str(), "r Xtd %*i %x %*i %x %x %x %x %x %x %x %x %*i   %*i %i      17920  %s ", &iB, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &iA);
  can_time = iA;
  can_id = iB;
  can_bytes = uint8_t(parsed_count-2);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count >= 2) ? 0 : -1;
}

int parseLogLineCanServer() // "104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0"
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
  int i1, i2, i3, i4, i5, i6, i7, i8, iB; // "%i* %x
  long long iA;
  int parsed_count = sscanf (line.c_str(), "%lli %*i %*i %*i %*i %*i %x %x %x %x %x %x %x %x %x", &iA, &iB, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);

  can_time = iA;
  can_id = iB;
  can_bytes = uint8_t(parsed_count-2);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count >= 2) ? 0 : -1;
}


int parseLogLineCANoe() // "  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF "
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
  int i1, i2, i3, i4, i5, i6, i7, i8, iB, iDb; // "%i* %x
  float f1;
  int parsed_count = sscanf (line.c_str(), " %f %*i %xx %*s %*s %u %x %x %x %x %x %x %x %x", &f1, &iB, &iDb, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);

  can_time = uint64_t (f1*1000);
  can_id = iB;
  can_bytes = uint8_t(iDb);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count == (3+iDb)) ? 0 : -1;
}


int parseLogLineA1ASCII() // "m e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270"
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
/// @todo SOON-260: handle std/ext, too...
  int i1, i2, i3, i4, i5, i6, i7, i8, iA, iB, iDb; // "%i* %x
  int parsed_count = sscanf (line.c_str(), "m e 0x%x %u 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x %i", &iA, &iDb, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &iB);

  can_time = iB;
  can_id = iA;
  can_bytes = uint8_t(iDb);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count == (3+iDb)) ? 0 : -1;
}



int parseLogLineRte() // "[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06"
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif

  int i1, i2, i3, i4, i5, i6, i7, i8, iB, iDb = 0; // "%i* %x
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
          &iA, &iB, &iDb, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);
      if (parsed_count <= -1 || sci_minimal_expected_parse_count <= parsed_count)
        remembered_mode = current_mode;
      break;
    case 2:
      parsed_count = sscanf(
          line.c_str(),
          "%*s %Li.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);
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

  can_time = iA;
  can_id = iB;
  can_bytes = uint8_t(iDb);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count == (3+iDb)) ? 0 : -1;
}


int parseLogLineRte2 ()
{
    int big;
    int little;
    char *cursor;
    int byte;
    string line;

    getline (ifs_in, line);
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
        return -4;
    }
    little = strtol (&cursor[1], &cursor, 10);
    while (isspace (*cursor))
        cursor++;
    if (*cursor == 'X')
        cursor++;
    else {
        fprintf (stderr, "unexpected field %c\n", *cursor);
        return -5;
    }

    rte_time += big;
    can_time = rte_time;
    can_id = strtol (cursor, &cursor, 16);
    can_bytes = uint8_t (strtol (cursor, &cursor, 0));


    if (can_bytes > 8) {
        fprintf (stderr, "protocol violation at time %lld: dlc of %d too long! (forcing to 8)\n", can_time, can_bytes);
        can_bytes = 8;
    }

    for (byte = 0; byte < 8; byte++) {
        if (byte > can_bytes)     /* TODO: shouldn't this be >= ? */
            break;

        can_data[byte] = uint8_t (strtol (cursor, &cursor, 16));
    }

    return 0;
}




int parseLogLineTrc() // "    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15"
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
  int i1, i2, i3, i4, i5, i6, i7, i8, iA, iB, iDb; // "%i* %x
  int parsed_count = sscanf (line.c_str(), " %*s %u.%*u %*s %x %u %x %x %x %x %x %x %x %x", &iA, &iB, &iDb, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);

  can_time = iA;
  can_id = iB;
  can_bytes = uint8_t(iDb);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count == (3+iDb)) ? 0 : -1;
}


int parseLogLineJrf() // "41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF"
{
  string line;
  getline (ifs_in, line);
#if DEBUG
  cout << line << endl;
#endif
  int i1, i2, i3, i4, i5, i6, i7, i8, iB;
  float fA;
  int parsed_count = sscanf (line.c_str(), "%f,%x,%x,%x,%x,%x,%x,%x,%x,%x", &fA, &iB, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);
  can_time = uint64_t (fA*1000);
  can_id = iB;
  can_bytes = uint8_t(parsed_count-2);
  can_data[0] = uint8_t(i1);
  can_data[1] = uint8_t(i2);
  can_data[2] = uint8_t(i3);
  can_data[3] = uint8_t(i4);
  can_data[4] = uint8_t(i5);
  can_data[5] = uint8_t(i6);
  can_data[6] = uint8_t(i7);
  can_data[7] = uint8_t(i8);

  return (parsed_count >= 2) ? 0 : -1;
}


void interpretePgnsVtEcu(bool rb_toEcu)
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    if (canGetData(0) < 0x10) cout << "Activation ";
    if ((canGetData(0) >=0x10) && (canGetData(0) < 0xF0)) cout << "Command ";

    int8_t i8_uidBytePos = -1;

    switch (canGetData(0))
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

      case 0xA0: cout << "Hide/Show Object"; i8_uidBytePos = 2; if (canGetData(4-1)) cout << " -show-"; else cout << " -hide-"; break;
      case 0xA1: cout << "Enable/Disable Object"; i8_uidBytePos = 2; if (canGetData(4-1)) cout << " -enable-"; else cout << " -disable-"; break;
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
      case 0xAF: cout << "Change Attribute"; i8_uidBytePos = 2; cout << " AID="<<uint16_t(canGetData(4-1)); break;

      case 0xB0: cout << "Change Priority"; i8_uidBytePos = 2; break;
      case 0xB1: cout << "Change List Item"; i8_uidBytePos = 2; break;
      case 0xB2: cout << "Delete Object Pool"; break;
      case 0xB3: cout << "Change String Value"; i8_uidBytePos = 2; break;
      case 0xB4: cout << "Change Child Position"; i8_uidBytePos = 4; break;

      case 0xFE: cout << "VT Status Message: Active WS-Master's SA: "<<uint16_t(canGetData(1)); break;
      case 0xFF: cout << "Working Set Maintenance"; break;
    }
    if (i8_uidBytePos > 0) cout << " ObjectID="<<dec<<( (uint16_t(canGetData(i8_uidBytePos+1-1)<<8)) | (uint16_t(canGetData(i8_uidBytePos-1))) );
    if ((!rb_toEcu) && (canGetData(0) < 0x10)) cout << " Response";
    if ((rb_toEcu)  && (canGetData(0) >=0x10) && (canGetData(0) < 0xF0)) cout << " Response";
  }
}



void interpretePgnsCl2Fs()
{
  analyzeCl2FS(canGetSa(), canGetDa(),  canGetBytes(), can_data);
}

void interpretePgnsFs2Cl()
{
  analyzeFS2Cl(canGetSa(), canGetDa(), canGetBytes(), can_data);
}


void analyzeCl2FS(
    uint8_t SA,
    uint8_t DA,
    uint16_t lengthData,
    uint8_t data[])
{
  uint16_t i_pathNameLength = 0, i_destPathNameLength = 0;

  cout << "SA: " << setw(2)<<setfill('0') << uint32_t(SA) << " DA: " << setw(2)<<setfill('0') << uint32_t(DA) << " length Data: " << setw(4)<<setfill('0') << uint32_t(lengthData);
  switch (uint32_t(data[0]))
  {
    case 0x00: cout << "\tClient Connection Maintenance";
      break;

    case 0x01: cout << "\tGet File Server Properties Request ";
      break;

    case 0x10: cout << "\tGet Current Directory Request TAN: " << setw(2) << hex << uint32_t(data[1]);
      break;

    case 0x11: cout << "\tChange Current Directory Request TAN: " << setw(2) << hex << uint32_t(data[1]);
               i_pathNameLength = uint16_t((data[3]<<8) | (data[2]));
               cout << " Path Name Length : " << i_pathNameLength;
               cout << " Path Name : ";
              for (uint16_t i = 0; i < i_pathNameLength; i++)
              {
                 cout << uint8_t(data[4 + i]);
              }
      break;

    case 0x20: cout << "\tOpen File Request TAN: " << setw(2) << hex << uint32_t(data[1]);

              if (data[2] == 0) cout << " Open file for reading only";
              if (data[2] == 1) cout << " Open file for writing only";
              if (data[2] == 2) cout << " Open file for reading and writing only";
              if (data[2] == 3) cout << " Open directory";

              (data[2] & 4)? cout << " Create a new File if not existing": cout << " Open an existing File(fails if non-existent file)";

              (data[2] & 8)? cout << " Open file for appending data the end of the file": cout << " Open file for random access";

              (data[2] & 16)? cout << " Open file with exclusive access(fails if already open)": cout << " Open file for shared access";

              i_pathNameLength = uint16_t((data[4]<<8) | (data[3]));

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: ";
              for (uint16_t i = 0; i <   i_pathNameLength; i++)
              {
                 cout << uint8_t(data[5 + i]);
              }
      break;

    case 0x21: cout << "\tSeek File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << " Handle: " << hex << static_cast<uint32_t>(data[2]);
               cout << " Position Mode: " << hex << static_cast<uint32_t>(data[3]);
               cout << " Offset: " << hex << ( (static_cast<uint32_t>(data[7])<<24) | (static_cast<uint32_t>(data[6])<<16) | (static_cast<uint32_t>(data[5])<<8) | (static_cast<uint32_t>(data[4])) );
      break;

    case 0x22: cout << "\tRead File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << " Handle: "     <<hex<<static_cast<uint32_t>(data[2]);
               cout << " Count: "      <<hex<<( (static_cast<uint16_t>(data[4])<<8) | (static_cast<uint16_t>(data[3])) );
               if (data[5] == 0x00) cout << " Do not report hidden files a directory listing";
               if (data[5] == 0x01) cout << " Report hidden files in a directory listing";
               if (data[5] == 0xFF) cout << " Parameter ist not available, FS shall not report hidden in a directory listing";
               if ( (data[5] >= 0x02) && (data[5] <= 0xFE) ) cout << " ERROR wrong Error Code";
      break;

    case 0x23: cout << "\tWrite File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << " Handle: " << hex << static_cast<uint32_t>(data[2]);
               cout << " Number of Byte written Data: " <<dec<<( (static_cast<uint16_t>(data[4])<<8) | (static_cast<uint16_t>(data[3])) );
               cout << " Data: ";
               for (int i = 0; i < ( (static_cast<uint16_t>(data[4])<<8) | (static_cast<uint16_t>(data[3])) ); i++)
               {
                 cout << "0x" << hex << static_cast<uint32_t>(data[5 + i]) << " ";
               }
      break;

    case 0x24: cout << "\tClose File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << " Handle: " << hex << static_cast<uint32_t>(data[2]);
      break;

    case 0x30: cout << "\tMove / Copy / Rename File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << ((data[2] && 0x04)? " recursive" : " not recursive");
               cout << ((data[2] && 0x02)? " force" : " not force");
               cout << ((data[2] && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(data[4])<<8) | data[3];

               cout << " Source Path Name Length: "  << hex << i_pathNameLength;
               cout << " Source Path Name: ";
               for (int i = 0; i < i_pathNameLength; i++) {
                   cout << uint8_t(data[7 + i]);
               }

               i_destPathNameLength = (static_cast<uint16_t>(data[6])<<8) | data[5];

               cout << " Destination Path Name Length: " << hex << i_destPathNameLength;
               cout << " Destination Path Name: ";
               for (int i = 0; i < i_destPathNameLength; i++) {
                 cout << uint8_t(data[7 + i_pathNameLength + i]);
               }
      break;

    case 0x31: cout << "\tDelete File Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);

               cout << ((data[2] && 0x04)? " recursive" : " not recursive");
               cout << ((data[2] && 0x02)? " force" : " not force");
               cout << ((data[2] && 0x01)? " copy" : " not copy");

               i_pathNameLength = (static_cast<uint16_t>(data[4])<<8) | data[3];

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: ";
              for (uint16_t i = 0; i <  i_pathNameLength; i++)
              {
                 cout << uint8_t(data[5 + i]);
              }

      break;
    case 0x32: cout << "\tGet Files Attributes Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);

               i_pathNameLength = (static_cast<uint16_t>(data[3])<<8) | data[2];

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                  cout << uint8_t(data[4 + i]);
               }
      break;
    case 0x33: cout << "\tSet File Attributes Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);

               if (data[2] == 0x1C) cout <<" Clear read-only attribute";
               if (data[2] == 0x1D) cout <<" Set read-only attribute";
               if (data[2] == 0x13) cout <<" Clear hidden attribute";
               if (data[2] == 0x17) cout <<" Set hidden attribute";

               i_pathNameLength = (static_cast<uint16_t>(data[4])<<8) | data[3];

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                  cout << uint8_t(data[5 + i]);
               }
      break;
    case 0x34: cout << "\tGet File Date & File Time Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);

               i_pathNameLength = (static_cast<uint16_t>(data[3])<<8) | data[2];

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                  cout << uint8_t(data[4 + i]);
               }
      break;
    case 0x40: cout << "\tInitialize Volume Request TAN: " << setw(2) << hex << static_cast<uint32_t>(data[1]);
               cout << " Space : "        <<hex<< ((static_cast<uint32_t>(data[5]) << 24)
			                                    | (static_cast<uint32_t>(data[4]) << 16)
												| (static_cast<uint32_t>(data[3]) <<  8)
												| (static_cast<uint32_t>(data[2])      ));
               if (data[6] == 0x00) cout <<" create  a new volume using all available space";
               if (data[6] == 0x02) cout <<" create  a new volume using space specified";
               if (data[6] == 0x01) cout <<" Overwrite the existing volume";

               i_pathNameLength = static_cast<uint16_t>(data[8]<<8) | data[7];

               cout << " Path Name Length: " << hex << i_pathNameLength;
               cout << " Path Name: " ;
               for (int i = 0; i < i_pathNameLength; i++) {
                  cout << uint8_t(data[9 + i]);
               }
      break;
    default: return;
  }
}





void interpretePgn(uint32_t);

void interpretePgnAcknowledge()
{
}

void interpreteWheelBasedSpeedDist()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Wheel-based implement speed
      std::cout << std::dec << "WBISp: " << ((canGetData(1) << 8) | canGetData(0)) * 0.001 << " m/s ";

      // Wheel-based distance
      std::cout << "WBIDs: " << static_cast<uint32_t>( (static_cast<uint32_t>(canGetData(5)) << 24) 
                                                     | (static_cast<uint32_t>(canGetData(4)) << 16) 
                                                     | (static_cast<uint32_t>(canGetData(3)) << 8) 
                                                     |  static_cast<uint32_t>(canGetData(2))
                                                     ) * 0.001 << " m ";

      // Maximum time of tractor power
      std::cout << "MaxTimeTP: " << static_cast<uint16_t>(canGetData(6)) << " min ";

      // Operator direction reversed
      switch (canGetData(7) >> 6) {
          case 0x0: cout << "Operator direction: NotReversed "; break;
          case 0x1: cout << "Operator direction: Reversed "; break;
          case 0x2: cout << "Operator direction: Error "; break;
          case 0x3: cout << "Operator direction: Not available "; break;
      }
      
      // Start/Stop state
      switch ((canGetData(7) >> 4) & 0x03) {
        case 0x0: cout << "St/Sp state: stop "; break;
        case 0x1: cout << "St/Sp state: start "; break;
        case 0x2: cout << "St/Sp state: error "; break;
        case 0x3: cout << "St/Sp state: n/a "; break;
      }
      
      // Key switch state
      switch ((canGetData(7) >> 2) & 0x03) {
        case 0x0: cout << "Key sws: off "; break;
        case 0x1: cout << "Key sws: not off "; break;
        case 0x2: cout << "Key sws: error "; break;
        case 0x3: cout << "Key sws: n/a "; break;
      }

      // Ground based direction
      switch (canGetData(7) & 0x03) {
        case 0x0: cout << "WB Dir: Reverse "; break;
        case 0x1: cout << "WB Dir: Forward "; break;
        case 0x2: cout << "WB Dir: Error "; break;
        case 0x3: cout << "WB Dir: N/A "; break;
      }
  }
}

void interpreteGroundBasedSpeedDist()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Wheel-based implement speed
      std::cout << std::dec << "GBISp: " << ((canGetData(1) << 8) | canGetData(0)) * 0.001 << " m/s ";

      // Wheel-based distance
      std::cout << "GBIDs: " << static_cast<uint32_t>( (static_cast<uint32_t>(canGetData(5)) << 24) 
                                                     | (static_cast<uint32_t>(canGetData(4)) << 16) 
                                                     | (static_cast<uint32_t>(canGetData(3)) << 8) 
                                                     |  static_cast<uint32_t>(canGetData(2))
                                                     ) * 0.001 << " m ";

      // Byte 7 reserved.

      // Bit 8 to 3 reserved

      // Ground based direction
      switch (canGetData(7) & 0x03) {
        case 0x0: cout << "GB Dir: Reverse "; break;
        case 0x1: cout << "GB Dir: Forward "; break;
        case 0x2: cout << "GB Dir: Error "; break;
        case 0x3: cout << "GB Dir: N/A "; break;
      }
  }
}

void interpreteRearPTOstate()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear PTO output shaft speed
      std::cout << std::dec << "Rear PTO output shaft speed: " << static_cast<uint16_t>( (static_cast<uint16_t>(canGetData(1)) << 8) | static_cast<uint16_t>(canGetData(0)) ) * 0.125 << " 1/min ";

      // Rear PTO output shaft speed setpoint
      std::cout << std::dec << "Rear PTO output shaft speed setpoint: " << static_cast<uint16_t>( (static_cast<uint16_t>(canGetData(3)) << 8) | static_cast<uint16_t>(canGetData(2)) ) * 0.125 << " 1/min ";

      // Rear PTO engagement
      switch (canGetData(4) >> 6) {
          case 0x0: cout << "RPTO eng: disen "; break;
          case 0x1: cout << "RPTO eng: en "; break;
          case 0x2: cout << "RPTO eng: Error "; break;
          case 0x3: cout << "RPTO eng: n/a "; break;
      }
      
      // Rear PTO mode
      switch ((canGetData(4) >> 4) & 0x03) {
        case 0x0: cout << "RPTO mod: 540 "; break;
        case 0x1: cout << "RPTO mod: 1000 "; break;
        case 0x2: cout << "RPTO mod: Error "; break;
        case 0x3: cout << "RPTO mod: n/a "; break;
      }
      
      // Rear PTO economy mode
      switch ((canGetData(4) >> 2) & 0x03) {
        case 0x0: cout << "RPTO ec mod: disen "; break;
        case 0x1: cout << "RPTO ec mod: en "; break;
        case 0x2: cout << "RPTO ec mod: error "; break;
        case 0x3: cout << "RPTO ec mod: n/a "; break;
      }

      // Rear PTO mode request status
      switch (canGetData(4) & 0x03) {
        case 0x0: cout << "RPTO mod rq st: disen "; break;
        case 0x1: cout << "RPTO mod rq st: en "; break;
        case 0x2: cout << "RPTO mod rq st: Error "; break;
        case 0x3: cout << "RPTO mod rq st: N/A "; break;
      }

      // Rear PTO mode
      switch (canGetData(5) >> 6) {
          case 0x0: cout << "RPTO mode: external request "; break;
          case 0x1: cout << "RPTO mode: controll override "; break;
          case 0x2: cout << "RPTO mode: Error "; break;
          case 0x3: cout << "RPTO mode: n/a "; break;
      }
      
      // Rear PTO Economy mode request status
      switch ((canGetData(5) >> 4) & 0x03) {
        case 0x0: cout << "RPTO economy mode request status: external request "; break;
        case 0x1: cout << "RPTO economy mode request status: controll override "; break;
        case 0x2: cout << "RPTO economy mode request status: Error "; break;
        case 0x3: cout << "RPTO economy mode request status: n/a "; break;
      }
      
      // Rear PTO shaft speed limit status
      switch ((canGetData(5) >> 1) & 0x07) {
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

void interpreteRearHitch()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
      // Rear hitch position in %
      std::cout << std::dec << "RearHitchPos: " << canGetData(0) * 0.4 << "% ";

      // Rear hith in-work indication.
      switch (canGetData(1) >> 6) {
          case 0x0: cout << "RH in-wo ind: out-of-work "; break;
          case 0x1: cout << "RH in-wo ind: in-work "; break;
          case 0x2: cout << "RH in-wo ind: error "; break;
          case 0x3: cout << "RH in-wo ind: n/a "; break;
      }
      
      // Rear hitch position limit status
      switch ((canGetData(1) >> 3) & 0x07) {
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
      std::cout << std::dec << "Rear nom low link force: " << int8_t(canGetData(2)) * 0.8 << "% ";

      // Rear draft
      std::cout << std::dec << "Rear draft: " << static_cast<uint16_t>( (static_cast<uint16_t>(canGetData(4)) << 8) | static_cast<uint16_t>(canGetData(3)) ) * 10 - 320000 << "N ";

      // Bits 6 to 8 are reserved
  }
}

void interpretePgnLanguage()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Language Code
    cout << "LC:" << canGetAsciiData(0) << canGetAsciiData(1) << " ";

    // Decimal Point
    switch (canGetData(2) >> 6) {
      case 0x0: cout << "dp:, "; break;
      case 0x1: cout << "dp:. "; break;
      case 0x2: cout << "dp:R "; break;
      case 0x3: cout << "dp:N "; break;
    }
    // Time Format
    switch ((canGetData(2) >> 4) & 0x03) {
      case 0x0: cout << "t:24 "; break;
      case 0x1: cout << "t:12 "; break;
      case 0x2: cout << "t:R  "; break;
      case 0x3: cout << "t:N  "; break;
    }
    // Date Format
    switch (canGetData(3)) {
      case 0x0: cout << "d:ddmmyyyy "; break;
      case 0x1: cout << "d:ddyyyymm "; break;
      case 0x2: cout << "d:mmyyyydd "; break;
      case 0x3: cout << "d:mmddyyyy "; break;
      case 0x4: cout << "d:yyyymmdd "; break;
      case 0x5: cout << "d:yyyyddmm "; break;
      default:  cout << "d:UNSPECIF "; break;
    }
    // Distance
    switch ((canGetData(4) >> 6) & 0x3) {
      case 0x0: cout << "d:M  "; break;
      case 0x1: cout << "d:IU "; break;
      case 0x2: cout << "d:R  "; break;
      case 0x3: cout << "d:N  "; break;
    }
    // Area
    switch ((canGetData(4) >> 4) & 0x3) {
      case 0x0: cout << "a:M  "; break;
      case 0x1: cout << "a:IU "; break;
      case 0x2: cout << "a:R  "; break;
      case 0x3: cout << "a:N  "; break;
    }
    // Volume
    switch ((canGetData(4) >> 2) & 0x3) {
      case 0x0: cout << "v:M "; break;
      case 0x1: cout << "v:I "; break;
      case 0x2: cout << "v:U "; break;
      case 0x3: cout << "v:N "; break;
    }
    // Mass
    switch ((canGetData(4) >> 0) & 0x3) {
      case 0x0: cout << "m:M "; break;
      case 0x1: cout << "m:I "; break;
      case 0x2: cout << "m:U "; break;
      case 0x3: cout << "m:N "; break;
    }
    // Temperature
    switch ((canGetData(5) >> 6) & 0x3) {
      case 0x0: cout << "t:M  "; break;
      case 0x1: cout << "t:IU "; break;
      case 0x2: cout << "t:R  "; break;
      case 0x3: cout << "t:N  "; break;
    }
    // Pressure
    switch ((canGetData(5) >> 4) & 0x3) {
      case 0x0: cout << "p:M  "; break;
      case 0x1: cout << "p:IU "; break;
      case 0x2: cout << "p:R  "; break;
      case 0x3: cout << "p:N  "; break;
    }
    // Force
    switch ((canGetData(5) >> 2) & 0x3) {
      case 0x0: cout << "f:M  "; break;
      case 0x1: cout << "f:IU "; break;
      case 0x2: cout << "f:R  "; break;
      case 0x3: cout << "f:N  "; break;
    }
    // System
    switch ((canGetData(5) >> 0) & 0x3) {
      case 0x0: cout << "s:M "; break;
      case 0x1: cout << "s:I "; break;
      case 0x2: cout << "s:U "; break;
      case 0x3: cout << "s:N "; break;
    }
  }
}

void interpreteLightingCommand()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Tractor high-beam head lights
    switch ((canGetData(0) >> 6) & 0x3) {
      case 0x0: cout << "Tractor high-beam head lights: deactivate "; break;
      case 0x1: cout << "Tractor high-beam head lights: activate "; break;
      case 0x2: cout << "Tractor high-beam head lights: reserve "; break;
      case 0x3: cout << "Tractor high-beam head lights: don't care "; break;
    }
    // Tractor low-beam head lights
    switch ((canGetData(0) >> 4) & 0x3) {
      case 0x0: cout << "Tractor low-beam head lights: deactivate "; break;
      case 0x1: cout << "Tractor low-beam head lights: activate "; break;
      case 0x2: cout << "Tractor low-beam head lights: reserve "; break;
      case 0x3: cout << "Tractor low-beam head lights: don't care "; break;
    }
    // Tractor alternate head lights
    switch ((canGetData(0) >> 2) & 0x3) {
      case 0x0: cout << "Tractor alternate head lights: deactivate "; break;
      case 0x1: cout << "Tractor alternate head lights: activate "; break;
      case 0x2: cout << "Tractor alternate head lights: reserve "; break;
      case 0x3: cout << "Tractor alternate head lights: don't care "; break;
    }
    // Daytime running lights
    switch ((canGetData(0) >> 0) & 0x3) {
      case 0x0: cout << "Daytime running lights: deactivate "; break;
      case 0x1: cout << "Daytime running lights: activate "; break;
      case 0x2: cout << "Daytime running lights: reserve "; break;
      case 0x3: cout << "Daytime running lights: don't care "; break;
    }

    
    // Left-turn signal lights
    switch ((canGetData(1) >> 6) & 0x3) {
      case 0x0: cout << "Left-turn signal lights: deactivate "; break;
      case 0x1: cout << "Left-turn signal lights: activate "; break;
      case 0x2: cout << "Left-turn signal lights: reserve "; break;
      case 0x3: cout << "Left-turn signal lights: don't care "; break;
    }
    // Right-turn signal lights
    switch ((canGetData(1) >> 4) & 0x3) {
      case 0x0: cout << "Right-turn signal lights: deactivate "; break;
      case 0x1: cout << "Right-turn signal lights: activate "; break;
      case 0x2: cout << "Right-turn signal lights: reserve "; break;
      case 0x3: cout << "Right-turn signal lights: don't care "; break;
    }
    // Beacon lights
    switch ((canGetData(1) >> 2) & 0x3) {
      case 0x0: cout << "Beacon lights: deactivate "; break;
      case 0x1: cout << "Beacon lights: activate "; break;
      case 0x2: cout << "Beacon lights: reserve "; break;
      case 0x3: cout << "Beacon lights: don't care "; break;
    }
    // Front fog lights
    switch ((canGetData(1) >> 0) & 0x3) {
      case 0x0: cout << "Front fog: deactivate "; break;
      case 0x1: cout << "Front fog: activate "; break;
      case 0x2: cout << "Front fog: reserve "; break;
      case 0x3: cout << "Front fog: don't care "; break;
    }

    
    // Left stop lights
    switch ((canGetData(2) >> 6) & 0x3) {
      case 0x0: cout << "Left stop lights: deactivate "; break;
      case 0x1: cout << "Left stop lights: activate "; break;
      case 0x2: cout << "Left stop lights: reserve "; break;
      case 0x3: cout << "Left stop lights: don't care "; break;
    }
    // Right stop lights
    switch ((canGetData(2) >> 4) & 0x3) {
      case 0x0: cout << "Right stop lights: deactivate "; break;
      case 0x1: cout << "Right stop lights: activate "; break;
      case 0x2: cout << "Right stop lights: reserve "; break;
      case 0x3: cout << "Right stop lights: don't care "; break;
    }
    // Center stop lights
    switch ((canGetData(2) >> 2) & 0x3) {
      case 0x0: cout << "Center stop lights: deactivate "; break;
      case 0x1: cout << "Center stop lights: activate "; break;
      case 0x2: cout << "Center stop lights: reserve "; break;
      case 0x3: cout << "Center stop lights: don't care "; break;
    }
    // Back up light and alarm horn
    switch ((canGetData(2) >> 0) & 0x3) {
      case 0x0: cout << "Back up light and alarm horn: deactivate "; break;
      case 0x1: cout << "Back up light and alarm horn: activate "; break;
      case 0x2: cout << "Back up light and alarm horn: reserve "; break;
      case 0x3: cout << "Back up light and alarm horn: don't care "; break;
    }

    
    // Tractor marker lights
    switch ((canGetData(3) >> 6) & 0x3) {
      case 0x0: cout << "Tractor marker lights: deactivate "; break;
      case 0x1: cout << "Tractor marker lights: activate "; break;
      case 0x2: cout << "Tractor marker lights: reserve "; break;
      case 0x3: cout << "Tractor marker lights: don't care "; break;
    }
    // Implement marker lights
    switch ((canGetData(3) >> 4) & 0x3) {
      case 0x0: cout << "Implement marker lights: deactivate "; break;
      case 0x1: cout << "Implement marker lights: activate "; break;
      case 0x2: cout << "Implement marker lights: reserve "; break;
      case 0x3: cout << "Implement marker lights: don't care "; break;
    }
    // Tractor clearance lights
    switch ((canGetData(3) >> 2) & 0x3) {
      case 0x0: cout << "Tractor clearance lights: deactivate "; break;
      case 0x1: cout << "Tractor clearance lights: activate "; break;
      case 0x2: cout << "Tractor clearance lights: reserve "; break;
      case 0x3: cout << "Tractor clearance lights: don't care "; break;
    }
    // Implement clearance lights
    switch ((canGetData(3) >> 0) & 0x3) {
      case 0x0: cout << "Implement clearance lights: deactivate "; break;
      case 0x1: cout << "Implement clearance lights: activate "; break;
      case 0x2: cout << "Implement clearance lights: reserve "; break;
      case 0x3: cout << "Implement clearance lights: don't care "; break;
    }

    
    // Tractor rear hight mounted work lights
    switch ((canGetData(4) >> 6) & 0x3) {
      case 0x0: cout << "Tractor rear hight mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor rear hight mounted work lights: activate "; break;
      case 0x2: cout << "Tractor rear hight mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor rear hight mounted work lights: don't care "; break;
    }
    // Tractor rear low mounted work lights
    switch ((canGetData(4) >> 4) & 0x3) {
      case 0x0: cout << "Tractor rear low mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor rear low mounted work lights: activate "; break;
      case 0x2: cout << "Tractor rear low mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor rear low mounted work lights: don't care "; break;
    }
    // Tractor underside work lights
    switch ((canGetData(4) >> 2) & 0x3) {
      case 0x0: cout << "Tractor underside work lights: deactivate "; break;
      case 0x1: cout << "Tractor underside work lights: activate "; break;
      case 0x2: cout << "Tractor underside work lights: reserve "; break;
      case 0x3: cout << "Tractor underside work lights: don't care "; break;
    }
    // Rear fog light 
    switch ((canGetData(4) >> 0) & 0x3) {
      case 0x0: cout << "Rear fog light: deactivate "; break;
      case 0x1: cout << "Rear fog light: activate "; break;
      case 0x2: cout << "Rear fog light: reserve "; break;
      case 0x3: cout << "Rear fog light: don't care "; break;
    }

    
    // Tractor front high-mounted work lights
    switch ((canGetData(5) >> 6) & 0x3) {
      case 0x0: cout << "Tractor front high-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor front high-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor front high-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor front high-mounted work lights: don't care "; break;
    }
    // Tractor front low-mounted work lights
    switch ((canGetData(5) >> 4) & 0x3) {
      case 0x0: cout << "Tractor front low-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor front low-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor front low-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor front low-mounted work lights: don't care "; break;
    }
    // Tractor side high-mounted work lights
    switch ((canGetData(5) >> 2) & 0x3) {
      case 0x0: cout << "Tractor side high-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor side high-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor side high-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor side high-mounted work lights: don't care "; break;
    }
    // Tractor side low-mounted work lights
    switch ((canGetData(5) >> 0) & 0x3) {
      case 0x0: cout << "Tractor side low-mounted work lights: deactivate "; break;
      case 0x1: cout << "Tractor side low-mounted work lights: activate "; break;
      case 0x2: cout << "Tractor side low-mounted work lights: reserve "; break;
      case 0x3: cout << "Tractor side low-mounted work lights: don't care "; break;
    }

    
    // Implement left forward work lights
    switch ((canGetData(6) >> 6) & 0x3) {
      case 0x0: cout << "Implement left forward work lights: deactivate "; break;
      case 0x1: cout << "Implement left forward work lights: activate "; break;
      case 0x2: cout << "Implement left forward work lights: reserve "; break;
      case 0x3: cout << "Implement left forward work lights: don't care "; break;
    }
    // Implement right forward work lights
    switch ((canGetData(6) >> 4) & 0x3) {
      case 0x0: cout << "Implement right forward work lights: deactivate "; break;
      case 0x1: cout << "Implement right forward work lights: activate "; break;
      case 0x2: cout << "Implement right forward work lights: reserve "; break;
      case 0x3: cout << "Implement right forward work lights: don't care "; break;
    }
    // Implement OEM option 1 light
    switch ((canGetData(6) >> 2) & 0x3) {
      case 0x0: cout << "Implement OEM option 1 light: deactivate "; break;
      case 0x1: cout << "Implement OEM option 1 light: activate "; break;
      case 0x2: cout << "Implement OEM option 1 light: reserve "; break;
      case 0x3: cout << "Implement OEM option 1 light: don't care "; break;
    }
    // Implement OEM option 2 light
    switch ((canGetData(6) >> 0) & 0x3) {
      case 0x0: cout << "Implement OEM option 2 light: deactivate "; break;
      case 0x1: cout << "Implement OEM option 2 light: activate "; break;
      case 0x2: cout << "Implement OEM option 2 light: reserve "; break;
      case 0x3: cout << "Implement OEM option 2 light: don't care "; break;
    }

    
    // Implement rear work lights
    switch ((canGetData(7) >> 6) & 0x3) {
      case 0x0: cout << "Implement rear work lights: deactivate "; break;
      case 0x1: cout << "Implement rear work lights: activate "; break;
      case 0x2: cout << "Implement rear work lights: reserve "; break;
      case 0x3: cout << "Implement rear work lights: don't care "; break;
    }
    // Implement left-facing work lights
    switch ((canGetData(7) >> 4) & 0x3) {
      case 0x0: cout << "Implement left-facing work lights: deactivate "; break;
      case 0x1: cout << "Implement left-facing work lights: activate "; break;
      case 0x2: cout << "Implement left-facing work lights: reserve "; break;
      case 0x3: cout << "Implement left-facing work lights: don't care "; break;
    }
    // Implement right-facing work lights
    switch ((canGetData(7) >> 2) & 0x3) {
      case 0x0: cout << "Implement right-facing work lights: deactivate "; break;
      case 0x1: cout << "Implement right-facing work lights: activate "; break;
      case 0x2: cout << "Implement right-facing work lights: reserve "; break;
      case 0x3: cout << "Implement right-facing work lights: don't care "; break;
    }
    // Lighting data message request
    switch ((canGetData(7) >> 0) & 0x3) {
      case 0x0: cout << "Lighting data message request: deactivate "; break;
      case 0x1: cout << "Lighting data message request: activate "; break;
      case 0x2: cout << "Lighting data message request: reserve "; break;
      case 0x3: cout << "Lighting data message request: don't care "; break;
    }
  }
}

void interpreteEngineSpeedMsg()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Engine speed
    cout << "Engine speed:" << static_cast<uint16_t>( (static_cast<uint16_t>(canGetData(4)) << 8) | static_cast<uint16_t>(canGetData(3)) ) * 0.125 << " ";
  }
}

void interpreteVehicleSpeed()
{
  if (canGetBytes() != 8)
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
    cout << "Compass bearing:" << (static_cast<double>( (static_cast<uint16_t>(canGetData(1)) << 8) | static_cast<uint16_t>(canGetData(0)) ) / 128) << "° "
         << "Naviagion based vehicle speed:" << static_cast<double>( (static_cast<uint16_t>(canGetData(3)) << 8) | static_cast<uint16_t>(canGetData(2)) ) / 256 << " km/h "
         << "pitch:" << (static_cast<double>( (static_cast<uint16_t>(canGetData(5)) << 8) | static_cast<uint16_t>(canGetData(4)) ) / 128) - 200 << "° "
         << "altitude:" << (static_cast<double>( (static_cast<uint16_t>(canGetData(7)) << 8) | static_cast<uint16_t>(canGetData(6)) ) * 0.125 ) - 2500 << " m ";
  }
}

void interpreteVehiclePosition()
{
  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
  }
  else
  {
    // Vehicle position
    // J1939-71
    // 4 bytes Latitude  10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    // 4 bytes Longitude 10^-7 deg/bit, -210 deg offset range from 10^-7 deg/bit, -210 deg offset
    cout << "Latitude: " << (static_cast<uint32_t>( (static_cast<uint32_t>(canGetData(3)) << 24) 
                                                     | (static_cast<uint32_t>(canGetData(2)) << 16) 
                                                     | (static_cast<uint32_t>(canGetData(1)) << 8) 
                                                     |  static_cast<uint32_t>(canGetData(0))
                                                     ) * 0.0000001) - 210 << "° "
         << "Longitude: " << (static_cast<uint32_t>( (static_cast<uint32_t>(canGetData(7)) << 24) 
                                                     | (static_cast<uint32_t>(canGetData(6)) << 16) 
                                                     | (static_cast<uint32_t>(canGetData(5)) << 8) 
                                                     |  static_cast<uint32_t>(canGetData(4))
                                                     ) * 0.0000001) - 210 << "° ";
  }
}

void interpretePgnsCl2FS()
{
  analyzeCl2FS(canGetSa(), canGetDa(), 8, can_data);
}

void interpretePgnsFS2Cl()
{
  analyzeFS2Cl(canGetSa(), canGetDa(), 8, can_data);
}


void interpretePgnsTPETP()
{
  bool b_tpStreamEnd = false;
  bool b_etpStreamEnd = false;
  TransferCollection_c::Variant_e e_Variant = TransferCollection_c::variant_etp;

  if (canGetBytes() != 8)
  {
    cout << "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";
    return;
  }

  switch (canGetPgn()) {

  case TP_CONN_MANAGE_PGN:
    e_Variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_CONN_MANAGE_PGN:
    switch (canGetData(0)) {
    case 0x10: 
      cout << "RTS - Request to Send (TP)            "; 
      {
        size_t const ct_sizeTransferData = size_t(canGetData(2)) << 8 | canGetData(1);
        (void)gc_transferCollection.newSession(
            e_Variant,
            canGetSa(),
            canGetDa(),
            ct_sizeTransferData);
      }
      break;
    case 0x14:
      cout << "RTS - Request to Send (ETP)           ";
      {
        size_t const ct_sizeTransferData = (static_cast<uint32_t>(canGetData(4)) << 24) |
          (static_cast<uint32_t>(canGetData(3)) << 16) |
          (static_cast<uint32_t>(canGetData(2)) << 8 ) |
          (static_cast<uint32_t>(canGetData(1)));
        (void)gc_transferCollection.newSession(
            e_Variant,
            canGetSa(),
            canGetDa(),
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
          gc_transferCollection.getSession(
              e_Variant,
              canGetSa(),
              canGetDa());
        t_ptrSession->mui32_packetOffSet =
          ((static_cast<uint32_t>(canGetData(4)) << 16) |
           (static_cast<uint32_t>(canGetData(3)) << 8 ) |
           (static_cast<uint32_t>(canGetData(2))));
      }
      break;
    case 0x13: 
      cout << "EoMACK - End of Message Ack (TP)      ";
      b_tpStreamEnd = true;
      break;
    case 0x17: 
      cout << "EoMACK - End of Message Ack (ETP)     ";
      b_etpStreamEnd = true;

      break;
    case 0x20: 
      cout << "BAM - Broadcast Announce Msg (TP)     ";
      break;
    case 0xFF: 
      cout << "CONNABORT - Connection Abort (TP/ETP) ";           
      gc_transferCollection.deleteSession(e_Variant, canGetSa(), canGetDa());
      break;
    default: return;
    }
    if ((canGetPgn() == TP_CONN_MANAGE_PGN)) {
      TransferCollection_c::PtrSession_t t_ptrSession =
        gc_transferCollection.getSession(
            e_Variant,
            canGetSa(),
            canGetDa());
      t_ptrSession->mui32_embeddedPgn =
        (static_cast<uint32_t>(canGetData(7)) << 16) |
        (static_cast<uint32_t>(canGetData(6)) << 8 ) |
        (static_cast<uint32_t>(canGetData(5)));
      cout << " on " << setw(6) << setfill('0') << t_ptrSession->mui32_embeddedPgn << " (";
      interpretePgn ( t_ptrSession->mui32_embeddedPgn);
      cout << ")";
    } else if (canGetPgn() == ETP_CONN_MANAGE_PGN) {
      TransferCollection_c::PtrSession_t t_ptrSession =
        gc_transferCollection.getSession(
            e_Variant,
            canGetSa(),
            canGetDa());
      t_ptrSession->mui32_embeddedPgn =
        (static_cast<uint32_t>(canGetData(7)) << 16) |
        (static_cast<uint32_t>(canGetData(6)) << 8 ) |
        (static_cast<uint32_t>(canGetData(5)));
      cout << " on " << setw(6) << setfill('0') << t_ptrSession->mui32_embeddedPgn << " (";
      interpretePgn (t_ptrSession->mui32_embeddedPgn);
      cout << ")";
    }
    break;

  case TP_DATA_TRANSFER_PGN:
    e_Variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_DATA_TRANSFER_PGN:
    cout << "DATA - Data Packet #"<<setw(2)<<setfill(' ')<<dec<<uint16_t(canGetData(0));
    {
      TransferCollection_c::PtrSession_t t_ptrSession =
        gc_transferCollection.getSession(
            e_Variant,
            canGetSa(),
            canGetDa());
      if ((canGetPgn() == TP_DATA_TRANSFER_PGN)) {
        for (int i = 0; i < 7; i++) {
          if ((7 *  (canGetData(0) - 1)) + i >= int(t_ptrSession->mvec_data.size()))
            break;
          size_t t_index = (7 *  (canGetData(0) - 1)) + i;
          (t_ptrSession->mvec_data)[t_index] = canGetData(i + 1);
        }
      } else if (canGetPgn() == ETP_DATA_TRANSFER_PGN) {
        for (int i = 0; i < 7; i++) {
          size_t t_index = (t_ptrSession->mui32_packetOffSet * 7 + (7 *  (canGetData(0) - 1))) + i;
          if (t_index >= t_ptrSession->mvec_data.size())
            break;
          (t_ptrSession->mvec_data)[t_index] = canGetData(i + 1);
        }
      }
    }
    break;
  default:
    break;
  }

  if (b_tpStreamEnd)
  {
    TransferCollection_c::PtrSession_t t_ptrSession =
      gc_transferCollection.getSession(
          e_Variant,
          canGetDa(),
          canGetSa());
    uint32_t const cui32_tpembeddedPgn = t_ptrSession->mui32_embeddedPgn;

    switch (cui32_tpembeddedPgn) {
    case 0xab00:
      analyzeFS2Cl(
          canGetDa(),
          canGetSa(),
          t_ptrSession->mvec_data.size(),
          &(t_ptrSession->mvec_data)[0]);
      break;
    case 0xaa00:
      analyzeCl2FS(
          canGetDa(),
          canGetSa(),
          t_ptrSession->mvec_data.size(),
          &(t_ptrSession->mvec_data)[0]);
      break;
    default:
      analyzeRest(
          canGetDa(),
          canGetSa(),
          t_ptrSession->mvec_data.size(),
          &(t_ptrSession->mvec_data)[0],
          cui32_tpembeddedPgn);
      break;
    }

    gc_transferCollection.deleteSession(
        e_Variant,      
        canGetDa(),
        canGetSa());
  }

  if (b_etpStreamEnd)
  {
    TransferCollection_c::PtrSession_t t_ptrSession =
      gc_transferCollection.getSession(
          e_Variant,
          canGetDa(),
          canGetSa());
    uint32_t const cui32_etpembeddedPgn = t_ptrSession->mui32_embeddedPgn;

    switch (cui32_etpembeddedPgn) {
    case 0xab00:
      analyzeFS2Cl(
         canGetDa(),
         canGetSa(),
         t_ptrSession->mvec_data.size(),
         &(t_ptrSession->mvec_data)[0]);
      break;
    case 0xaa00:
      analyzeCl2FS(
         canGetDa(),
         canGetSa(),
         t_ptrSession->mvec_data.size(),
         &(t_ptrSession->mvec_data)[0]);
      break;
    default:
      analyzeRest(
         canGetDa(),
         canGetSa(),
         t_ptrSession->mvec_data.size(),
         &(t_ptrSession->mvec_data)[0],
         cui32_etpembeddedPgn);
      break;
    }

    gc_transferCollection.deleteSession(
        e_Variant,      
        canGetDa(),
        canGetSa());
  }
}

void interpretePgnData (uint32_t rui32_pgn)
{
  switch (rui32_pgn)
  {
    case VT_TO_ECU_PGN:                  interpretePgnsVtEcu(true); break; // to ECU
    case ECU_TO_VT_PGN:                  interpretePgnsVtEcu(false); break; // from ECU
    case ACKNOWLEDGEMENT_PGN:            interpretePgnAcknowledge(); break;
    case PROCESS_DATA_PGN:               break;
    case CLIENT_TO_FS_PGN:               interpretePgnsCl2Fs(); break;
    case FS_TO_CLIENT_PGN:               interpretePgnsFs2Cl(); break;
    case GUIDANCE_MACHINE_STATUS:        break;
    case GUIDANCE_SYSTEM_CMD:            break;
    case ISOBUS_CERTIFICATION_PGN:       break;
    case ETP_DATA_TRANSFER_PGN:          interpretePgnsTPETP(); break;
    case ETP_CONN_MANAGE_PGN:            interpretePgnsTPETP(); break;
    case REQUEST_PGN_MSG_PGN:            break;
    case TP_DATA_TRANSFER_PGN:           interpretePgnsTPETP(); break;
    case TP_CONN_MANAGE_PGN:             interpretePgnsTPETP(); break;
    case ADDRESS_CLAIM_PGN:              break;
    case PROPRIETARY_A_PGN:              break;
    case PROPRIETARY_A2_PGN:             break;
    case WORKING_SET_MEMBER_PGN:         break;
    case WORKING_SET_MASTER_PGN:         break;
    case LANGUAGE_PGN:                   interpretePgnLanguage(); break;
    case LIGHTING_DATA_PGN:              break;
    case LIGHTING_COMMAND_PGN:           interpreteLightingCommand(); break;
    case HITCH_PTO_COMMANDS:             break;
    case REAR_PTO_STATE_PGN:             interpreteRearPTOstate(); break;
    case FRONT_PTO_STATE_PGN:            break;
    case REAR_HITCH_STATE_PGN:           interpreteRearHitch(); break;
    case FRONT_HITCH_STATE_PGN:          break;
    case MAINTAIN_POWER_REQUEST_PGN:     break;
    case WHEEL_BASED_SPEED_DIST_PGN:     interpreteWheelBasedSpeedDist(); break;
    case GROUND_BASED_SPEED_DIST_PGN:    interpreteGroundBasedSpeedDist(); break;
    case SELECTED_SPEED_CMD:             break;
    case SELECTED_SPEED_MESSAGE:         break;
    case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE: interpreteEngineSpeedMsg(); break;
    case SOFTWARE_IDENTIFICATION_PGN:    break;
    case TIME_DATE_PGN:                  break;
    case VEHICLE_DIRECTION_SPEED:        interpreteVehicleSpeed(); break;
    case VEHICLE_POSITION:               interpreteVehiclePosition(); break;
    case PROPRIETARY_B_PGN:              break;
    case NMEA_GPS_POSITION_RAPID_UPDATE_PGN: break;
    case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:  break;
    case NMEA_GPS_POSITION_DATA_PGN:     break;
    case NMEA_GPS_DIRECTION_DATA_PGN:    break;
    case NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS: break;
    default:                             break;
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

void interpretePgn (uint32_t rui32_pgn)
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




int parseLogLine()
{
  int i_result=-1; // default to error, variable will always be set
  switch (logType)
  {
    case logTypeCanServer: i_result = parseLogLineCanServer();   break;
    case logTypeRte:       i_result = parseLogLineRte();         break;
    case logTypeCANMon:    i_result = parseLogLineCANMon();      break;
    case logTypeCANoe:     i_result = parseLogLineCANoe();       break;
    case logTypeA1ASCII:   i_result = parseLogLineA1ASCII();     break;
    case logTypeTrc:       i_result = parseLogLineTrc();         break;
    case logTypeJohnDeere: i_result = parseLogLineJohnDeere();   break;
    case logTypeRte2:      i_result = parseLogLineRte2();        break;
    case logTypeJrf:       i_result = parseLogLineJrf();         break;
    default:               exit_with_error("Unknown Log-Type!"); return false; // return just to satisfy compiler. exit_with_error will exit anyway ;)
  }
  if (i_result == 0) // no error
  { /// Printout interpreted line

    // Timestamp
    cout << setfill (' ') << dec << setw (10) << (can_time/1000) << "." << setfill('0')<<setw(3)<<(can_time%1000)<< "   ";

    // CAN-ID / number of bytes
    cout << setfill ('0') << hex << setw (8) << can_id << "  " << uint16_t(can_bytes) << " ";

    // Databytes (HEX)
    int i;
    for (i=0; i< canGetBytes(); i++) cout << " " << setw (2) <<uint16_t(canGetData(i));
    for (;    i<8;              i++) cout << "   ";
    cout << "  ";

    // Databytes (HEX)
    for (i=0; i< canGetBytes(); i++) cout << setw (1) << char(canGetAsciiData(i));
    for (;    i<8;              i++) cout << " ";

    // SA
    cout << "  "<<setw(2) << uint16_t(canGetSa()) << "->";
    // DA
    if (canIsPdu1()) cout << setw(2) << uint16_t(canGetPs());
    else cout << "FF";

    // Priority
    cout << " (" << uint16_t(canGetPrio()) << ")";

    // PGN
    cout << " " << setw(6) << canGetPgn() << " => ";
    // Interpreted PGN
    interpretePgn (canGetPgn());
    // Interpreted PGN-Data
    interpretePgnData (canGetPgn());
    cout << endl;
  }
  else
  {
    cout << "---- line missing - error in parsing!----"<<endl; /// @todo SOON-260: replace by the original line!
  }
  return i_result;
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



void checkAlives()
{
  if ( ((canGetPgn() == VT_TO_ECU_PGN) && (canGetData (0) == 0xFE))
       ||
       ((canGetPgn() == ECU_TO_VT_PGN) && (canGetData (0) == 0xFF))
     )
  {
    struct_messages[0].svec_alives [canGetSa()].push_back (canGetTime());
  }


  if ( ((canGetPgn() == PROCESS_DATA_PGN) && (canGetData (0) == 0x0E))
       ||
       ((canGetPgn() == PROCESS_DATA_PGN) && (canGetData (0) == 0x0F))
     )
  {
    struct_messages[1].svec_alives [canGetSa()].push_back (canGetTime());
  }

}



void checkSingles()
{
  if (canGetPgn() == ACKNOWLEDGEMENT_PGN)
  {
    struct_messages[2].svec_alives [canGetSa()].push_back (canGetTime());
  }
}



void checkHandshakingsVtCommands()
{
  int mode; // no need to initialize.

  if ((canGetData(0) >= 0x60) && (canGetData(0) < 0xF0))
  { // not a maintenance command!
    msgType_t msgType_direction;
    uint8_t ui8_saClient;
    if (canGetPgn() == VT_TO_ECU_PGN)
    {
      mode = 3;
      msgType_direction = msgTypeResponse;
      ui8_saClient = canGetPs();
    }
    else if (canGetPgn() == ECU_TO_VT_PGN)
    {
      mode = 3;
      msgType_direction = msgTypeCommand;
      ui8_saClient = canGetSa();
    }
    else return;
    struct_messages[mode].svec_response [ui8_saClient].push_back (msgType_direction);
    struct_messages[mode].svec_alives   [ui8_saClient].push_back (canGetTime());
    return;
  }
}


void checkHandshakingTP()
{
  msgType_t mtype;
  uint8_t ui8_saClient=0xFE; // default to avoid compiler warning. The code below should make sure that it is always initialized properly!
  if ((canGetPgn() == ETP_DATA_TRANSFER_PGN) || (canGetPgn() == TP_DATA_TRANSFER_PGN))
  {
    ui8_saClient = canGetSa();
    mtype = msgTypeDATA;
  }
  else if ((canGetPgn() == ETP_CONN_MANAGE_PGN) || (canGetPgn() == TP_CONN_MANAGE_PGN))
  {
    switch (canGetData (0))
    {
      case 0x10:
      case 0x14: mtype = msgTypeRTS; ui8_saClient = canGetSa(); break;
      case 0x11:
      case 0x15: mtype = msgTypeCTS; ui8_saClient = canGetDa(); break;
      case 0x16: mtype = msgTypeDPO; ui8_saClient = canGetSa(); break;
      case 0x13:
      case 0x17: mtype = msgTypeEOMACK; ui8_saClient = canGetDa();break;
      case 0xFF: mtype = msgTypeCONNABORT; ui8_saClient = canGetSa();
                 // this message is special case! This aborts SA->DA and also DA->SA!
                 struct_messages[4].svec_response [canGetDa()].push_back (mtype);
                 struct_messages[4].svec_alives   [canGetDa()].push_back (canGetTime());
                 break; // CONN ABORT BY SENDER!
      /// @todo SOON-260: Damn, can't we detect ConnAbort by Receiver here, too?
    }
  }
  else return;

  struct_messages[4].svec_response [ui8_saClient].push_back (mtype);
  struct_messages[4].svec_alives   [ui8_saClient].push_back (canGetTime());
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
  ifs_in.open (argv[1]);
  if (!ifs_in.is_open()) exit_with_error("Couldn't open file");

  if (argc >= 3)
    logType = (logType_t) atoi (argv[2]);
  else // default to can_server
    logType = logTypeCanServer;

  if (argc >= 4)
    i_multipacketWrap = atoi (argv[3]);
  else // default to can_server
    i_multipacketWrap = 32;

  while (!ifs_in.eof())
  {
    if (parseLogLine() == 0)
    {
      checkAlives();
      checkSingles();
      checkHandshakingsVtCommands();
      checkHandshakingTP();
    }
  }

  ifs_in.close();
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

void analyzeFS2Cl(uint8_t SA, uint8_t DA, uint16_t lengthData, uint8_t Data[])
{
  uint32_t i_pathNameLength = 0;
  uint16_t msg_count = 0;
  uint16_t msg_offset = 0;

  cout << "SA: " << setw(2)<<setfill('0') << uint32_t(SA) << " DA: " << setw(2)<<setfill('0') << uint32_t(DA) << " length Data: " << setw(4)<<setfill('0') << uint32_t(lengthData);

  switch (uint32_t(Data[0])) {
    case 0x00: 
      cout << "\tFile Server Status: ";
        (Data[1] & 0x2) ? cout << "Busy writing" : cout << "Not Busy writing";
        (Data[1] & 0x1) ? cout << " Busy reading" : cout << " Not Busy reading";
      cout << " Number of Open Files: " << setw(2) << hex << uint32_t(Data[2]);
      break; //File Server Status

    case 0x01: 

      cout << "\tGet File Server Properties Response Version Number:";
      switch (Data[1]) {
        case 0x00: cout << " Draft version of the Standard"; break;
        case 0x01: cout << " Final Draft version of the Standard"; break;
        case 0x02: cout << " First published version of the Standard"; break;
      }
      cout << " Maximum Number of Simuntaneously Open Files: " << hex << uint32_t(Data[2]);
      cout << " File Server Capabilities : ";
      (Data[3]) ? cout << " File server supports multiple volumes"
                : cout << " File server does not supports multiple volumes";
      break; //Get File Server Properties Response

    case 0x10: 

      cout << "\tGet Current Directory Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      cout << " Total Space: " << setw(8) <<  ((static_cast<uint32_t>(canGetData(6)) << 24) |
		                                      (static_cast<uint32_t>(canGetData(5)) << 16) |
		                                      (static_cast<uint32_t>(canGetData(4)) << 8 ) |
		                                      (static_cast<uint32_t>(canGetData(3)))) << "(* 512 bytes)";
      cout << " Free Space: " << setw(8) << ((static_cast<uint32_t>(canGetData(10)) << 24) |
		                                    (static_cast<uint32_t>(canGetData( 9)) << 16) |
		                                    (static_cast<uint32_t>(canGetData( 8)) << 8 ) |
		                                    (static_cast<uint32_t>(canGetData( 7)))) << "(* 512 bytes)";
      i_pathNameLength = (static_cast<uint16_t>(Data[12])<<8) | Data[11];
      cout << " Path Name Length: " << setw(4) << i_pathNameLength;

      cout << " Path Name: ";
      for (uint16_t i_count = 0; i_count < i_pathNameLength; i_count++)
      {
        cout << uint8_t(Data[13 + i_count]);
      }

      break; //Get Current Directory Response 
    case 0x11: 

      cout << "\tChange Current Directory Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);

      break; //Chanege Current Directory Response
    case 0x20:

      cout << "\tOpen File Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      cout << " Handle: " << setw(2) << hex << uint32_t(Data[3]);
      decodeAttributes(Data[4]);

      break; //Open File Response
    case 0x21: 

      cout << "\tSeek File Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      cout << " Position : " << setw(8) << ((static_cast<uint32_t>(canGetData(7)) << 24) |
		                                   (static_cast<uint32_t>(canGetData(6)) << 16) |
		                                   (static_cast<uint32_t>(canGetData(5)) << 8 ) |
		                                   (static_cast<uint32_t>(canGetData(4))));
      break; //Seek File Response
    case 0x22:

      cout << "\tRead File/Directory Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);

      msg_count = uint16_t((Data[4]<<8) | Data[3]);
      msg_offset = 5;

      cout << " Count: " << setw(4) << msg_count << " ";

      //case file-data
      if (lengthData == msg_count + 5) {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          cout << setw(2) << hex << uint32_t(Data[5 + loop]) << " ";
        }
      }
      //else directory entries
      else {
        for (uint16_t loop = 0; loop < msg_count; loop++) {
          uint8_t filenamelength = uint8_t(Data[msg_offset]);
          if (filenamelength == 0xff)
            break;
          cout << " Filename length: " << uint16_t(filenamelength) << " ";
          cout << " Filename: ";
          uint8_t loopfilename = 0;
          for (; loopfilename < filenamelength; loopfilename++) {
            cout << uint8_t(Data[msg_offset + 1 + loopfilename]);
          }
          decodeAttributes(Data[msg_offset + 1 + loopfilename]);
          decodeDate(uint16_t(Data[msg_offset + 3 + loopfilename] << 8 | uint16_t(Data[msg_offset + 2 + loopfilename])));
          decodeTime(uint16_t(Data[msg_offset + 5 + loopfilename] << 8 | uint16_t(Data[msg_offset + 4 + loopfilename])));
          cout << " FileSize: " << ((static_cast<uint32_t>(canGetData(msg_offset + 9 + loopfilename)) << 24) |
		                           (static_cast<uint32_t>(canGetData(msg_offset + 8 + loopfilename)) << 16) |
		                           (static_cast<uint32_t>(canGetData(msg_offset + 7 + loopfilename)) << 8 ) |
		                           (static_cast<uint32_t>(canGetData(msg_offset + 6 + loopfilename))));
          msg_offset = uint16_t(msg_offset + filenamelength + 10);
        }
      }

      break; //Read File/Directory Response
    case 0x23: 

      cout << "\tWrite File Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      cout << " Count: " << setw(4) << uint16_t((Data[4]<<8) | Data[3]);
      break; // Write File Response
    case 0x24: 

      cout << "\tClose File Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      break; // Close File Response
    case 0x30: 

      cout << "\tMove File Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      break; // Move File Response 
    case 0x31: 

      cout << "\tDelete File Repsonse TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      break; // Delete File Repsonse
    case 0x32: 

      cout << "\tGet File Attribute Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      decodeAttributes(Data[3]);
      cout << " Size: " << ((static_cast<uint32_t>(canGetData(7)) << 24) |
		                   (static_cast<uint32_t>(canGetData(6)) << 16) |
		                   (static_cast<uint32_t>(canGetData(5)) << 8 ) |
		                   (static_cast<uint32_t>(canGetData(4))));
      break; // Get File Attribute Response
    case 0x33: 

      cout << "\tSet File Attribute Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      break; // Set File Attribute Response
    case 0x34: 

      cout << "\tGet File Date & Time Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      decodeDate(uint16_t((Data[4]<<8) | Data[3]));
      decodeTime(uint16_t((Data[6]<<8) | Data[5]));
      break; // Get File Date & Time Response
    case 0x40: 

      cout << "\tInitialize Volume Response TAN: " << setw(2) << hex << uint32_t(Data[1]);
      decodeErrorCode(Data[2]);
      decodeAttributes(Data[3]);
      break; // Initialize Volume Response
    default: return;
  }
}

void analyzeRest(uint8_t SA, uint8_t DA, uint16_t lengthData, uint8_t Data[], uint32_t aui32_embeddedPgn)
{
  (void)aui32_embeddedPgn; // currently unused

  int local_multipacketWrap = i_multipacketWrap;
  if (local_multipacketWrap > lengthData)
    local_multipacketWrap = lengthData;

  cout << ")" << endl;

  cout << "              " << hex << setw(2)<<setfill('0') << uint32_t(SA) << "->" << setw(2)<<setfill('0') << uint32_t(DA) << " " << dec << setw(7)<<setfill(' ') << uint32_t(lengthData) << "  " << hex;

  for (int blockoffset=0; blockoffset < lengthData; blockoffset += local_multipacketWrap)
  {
    if (blockoffset > 0)
      cout << "                              ";

    for (int inblock = 0; inblock < i_multipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < lengthData)
        cout << setw(2)<<setfill('0') << uint32_t(Data[blockoffset+inblock]) << " ";
      else
        cout << "   ";
    }
    for (int inblock = 0; inblock < i_multipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < lengthData)
        cout << getAscii (Data[blockoffset+inblock]);
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
