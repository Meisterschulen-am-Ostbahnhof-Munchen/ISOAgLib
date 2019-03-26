/*
  logalizer.cpp

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

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
#include <addresstracker.h>
#include <string>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <sstream>

#include <SimpleOpt.h>

// <DESTINATION> PGNs
#define TIM_SERVER_TO_CLIENT_PGN  0x002300LU
#define TIM_CLIENT_TO_SERVER_PGN  0x002400LU
#define AUTH_SERVER_TO_CLIENT_PGN 0x007000LU
#define AUTH_CLIENT_TO_SERVER_PGN 0x006F00LU
#define CLIENT_TO_FS_PGN          0x00AA00LU
#define FS_TO_GLOBAL_PGN          0x00ABFFLU
#define FS_TO_CLIENT_PGN          0x00AB00LU
#define GUIDANCE_MACHINE_STATUS   0x00AC00LU
#define GUIDANCE_SYSTEM_CMD       0x00AD00LU
#define ETP_DATA_TRANSFER_PGN     0x00C700LU
#define ETP_CONN_MANAGE_PGN       0x00C800LU
#define PROCESS_DATA_PGN          0x00CB00LU
#define CAB_MESSAGE_1             0x00E000LU
#define VT_TO_GLOBAL_PGN          0x00E6FFLU
#define VT_TO_ECU_PGN             0x00E600LU
#define ECU_TO_VT_PGN             0x00E700LU
#define ECU_TO_GLOBAL_PGN         0x00E7FFLU
/* we're NOT using ACK/NACK to Global address, we're directing directly to the sender! */
#define ACKNOWLEDGEMENT_PGN       0x00E800LU
#define REQUEST_PGN_MSG_PGN       0x00EA00LU
#define TP_DATA_TRANSFER_PGN      0x00EB00LU
#define TP_CONN_MANAGE_PGN        0x00EC00LU
#define ADDRESS_CLAIM_PGN         0x00EE00LU
#define PROPRIETARY_A_PGN         0x00EF00LU
#define PROPRIETARY_A2_PGN        0x01EF00LU

// <NO DESTINATION> PGNs
#define ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE 0x00F004LU
#define ELECTRONIC_TRANSMISSION_CONTROLLER_2 0x00F005LU
#define ELECTRONIC_AXLE_CONTROLLER_1 0x00F006LU
#define SELECTED_SPEED_MESSAGE      0x00F022LU
#define PRODUCT_IDENTIFICATION_PGN  0x00FC8DLU
#define CONTROL_FUNCTION_FUNCTIONALITIES_PGN 0x00FC8ELU
#define ALL_IMPLEMENTS_STOP_OPERATIONS_SWITCH_STATE_PGN 0x00FD02LU
#define ECU_DIAGNOSTIC_PROTOCOL_PGN 0x00FD32LU
#define ISOBUS_CERTIFICATION_PGN    0x00FD42LU
#define SELECTED_SPEED_CMD          0x00FD43LU
#define ECU_IDENTIFICATION_INFORMATION_PGN 0x00FDC5LU
#define OPERATORS_EXTERNAL_LIGHT_CONTROLS_MESSAGE 0x00FDCCLU
#define BASIC_JOYSTICK_MESSAGE_1    0x00FDD6LU
#define TRACTOR_FACILITIES_PGN      0x00FE09LU
#define WORKING_SET_MEMBER_PGN      0x00FE0CLU
#define WORKING_SET_MASTER_PGN      0x00FE0DLU
#define LANGUAGE_PGN                0x00FE0FLU
#define AUX_VALVE_0_ESTIMATED_FLOW  0x00FE10LU
#define AUX_VALVE_1_ESTIMATED_FLOW  0x00FE11LU
#define AUX_VALVE_2_ESTIMATED_FLOW  0x00FE12LU
#define AUX_VALVE_3_ESTIMATED_FLOW  0x00FE13LU
#define AUX_VALVE_4_ESTIMATED_FLOW  0x00FE14LU
#define AUX_VALVE_5_ESTIMATED_FLOW  0x00FE15LU
#define AUX_VALVE_6_ESTIMATED_FLOW  0x00FE16LU
#define AUX_VALVE_7_ESTIMATED_FLOW  0x00FE17LU
#define AUX_VALVE_8_ESTIMATED_FLOW  0x00FE18LU
#define AUX_VALVE_9_ESTIMATED_FLOW  0x00FE19LU
#define AUX_VALVE_10_ESTIMATED_FLOW 0x00FE1ALU
#define AUX_VALVE_11_ESTIMATED_FLOW 0x00FE1BLU
#define AUX_VALVE_12_ESTIMATED_FLOW 0x00FE1CLU
#define AUX_VALVE_13_ESTIMATED_FLOW 0x00FE1DLU
#define AUX_VALVE_14_ESTIMATED_FLOW 0x00FE1ELU
#define AUX_VALVE_15_ESTIMATED_FLOW 0x00FE1FLU
#define AUX_VALVE_0_MEASURED_FLOW   0x00FE20LU
#define AUX_VALVE_1_MEASURED_FLOW   0x00FE21LU
#define AUX_VALVE_2_MEASURED_FLOW   0x00FE22LU
#define AUX_VALVE_3_MEASURED_FLOW   0x00FE23LU
#define AUX_VALVE_4_MEASURED_FLOW   0x00FE24LU
#define AUX_VALVE_5_MEASURED_FLOW   0x00FE25LU
#define AUX_VALVE_6_MEASURED_FLOW   0x00FE26LU
#define AUX_VALVE_7_MEASURED_FLOW   0x00FE27LU
#define AUX_VALVE_8_MEASURED_FLOW   0x00FE28LU
#define AUX_VALVE_9_MEASURED_FLOW   0x00FE29LU
#define AUX_VALVE_10_MEASURED_FLOW  0x00FE2ALU
#define AUX_VALVE_11_MEASURED_FLOW  0x00FE2BLU
#define AUX_VALVE_12_MEASURED_FLOW  0x00FE2CLU
#define AUX_VALVE_13_MEASURED_FLOW  0x00FE2DLU
#define AUX_VALVE_14_MEASURED_FLOW  0x00FE2ELU
#define AUX_VALVE_15_MEASURED_FLOW  0x00FE2FLU
#define AUX_VALVE_0_COMMAND         0x00FE30LU
#define AUX_VALVE_1_COMMAND         0x00FE31LU
#define AUX_VALVE_2_COMMAND         0x00FE32LU
#define AUX_VALVE_3_COMMAND         0x00FE33LU
#define AUX_VALVE_4_COMMAND         0x00FE34LU
#define AUX_VALVE_5_COMMAND         0x00FE35LU
#define AUX_VALVE_6_COMMAND         0x00FE36LU
#define AUX_VALVE_7_COMMAND         0x00FE37LU
#define AUX_VALVE_8_COMMAND         0x00FE38LU
#define AUX_VALVE_9_COMMAND         0x00FE39LU
#define AUX_VALVE_10_COMMAND        0x00FE3ALU
#define AUX_VALVE_11_COMMAND        0x00FE3BLU
#define AUX_VALVE_12_COMMAND        0x00FE3CLU
#define AUX_VALVE_13_COMMAND        0x00FE3DLU
#define AUX_VALVE_14_COMMAND        0x00FE3ELU
#define AUX_VALVE_15_COMMAND        0x00FE3FLU
#define LIGHTING_DATA_PGN           0x00FE40LU
#define LIGHTING_COMMAND_PGN        0x00FE41LU
#define HITCH_PTO_COMMANDS          0x00FE42LU
#define REAR_PTO_STATE_PGN          0x00FE43LU
#define FRONT_PTO_STATE_PGN         0x00FE44LU
#define REAR_HITCH_STATE_PGN        0x00FE45LU
#define FRONT_HITCH_STATE_PGN       0x00FE46LU
#define MAINTAIN_POWER_REQUEST_PGN  0x00FE47LU
#define WHEEL_BASED_SPEED_DIST_PGN  0x00FE48LU
#define GROUND_BASED_SPEED_DIST_PGN 0x00FE49LU
#define VEHICLE_FLUIDS              0x00FE68LU
#define ELECTRONIC_TRANSMISSION_CONTROLLER_5 0x00FEC3LU

#define ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN                0x00FECALU
#define PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN     0x00FECBLU
#define DIAGNOSTIC_DATA_CLEAR_PGN                          0x00FECCLU
#define SOFTWARE_IDENTIFICATION_PGN 0x00FEDALU
#define IDLE_OPERATION              0x00FEDCLU

#define VEHICLE_DISTANCE            0x00FEE0LU
#define SHUTDOWN                    0x00FEE4LU
#define ENGINE_HOURS_REVOLUTIONS    0x00FEE5LU
#define TIME_DATE_PGN               0x00FEE6LU
#define VEHICLE_HOURS               0x00FEE7LU
#define VEHICLE_DIRECTION_SPEED     0x00FEE8LU
#define FUEL_CONSUMPTION_LIQUID     0x00FEE9LU
#define VEHICLE_WEIGHT              0x00FEEALU
#define ENGINE_TEMPERATURE_1        0x00FEEELU
#define ENGINE_FLUID_LEVEL_PRESSURE_1 0x00FEEFLU
#define POWER_TAKEOFF_INFORMATION   0x00FEF0LU
#define CRUISE_CONTROL_VEHICLE_SPEED 0x00FEF1LU
#define FUEL_ECONOMY_LIQUID         0x00FEF2LU
#define VEHICLE_POSITION            0x00FEF3LU
#define AMBIENT_CONDITIONS          0x00FEF5LU
#define VEHICLE_ELECTRICAL_POWER    0x00FEF7LU
#define TRANSMISSION_FLUIDS         0x00FEF8LU
#define DASH_DISPLAY                0x00FEFCLU
// PROPRIETARY_B_PGN ranges from 0x00FF00 to 0x00FFFF !
#define PROPRIETARY_B_PGN           0x00FF00LU


// <NMEA 2000 GPS Messages>
#define NMEA_GPS_POSITION_RAPID_UPDATE_PGN        0x01F801LU
#define NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN         0x01F802LU
#define NMEA_GPS_HIGH_PRECISION_RAPID_UPDATE_PGN  0x01F803LU
#define NMEA_GPS_POSITION_DATA_PGN                0x01F805LU
#define NMEA_GPS_TIME_DATE_PGN                    0x01F809LU
#define NMEA_GPS_CROSS_TRACK_ERROR_PGN            0x01F903LU
#define NMEA_GPS_DIRECTION_DATA_PGN               0x01FE11LU
#define NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS    0x01FA06LU


namespace
{
typedef std::pair< int, PtrDataFrame_t > ParseLogLine_t(std::string const &acr_line);
typedef std::string Interprete_t(PtrDataFrame_t);


struct Main_s {
  Main_s();
  size_t mt_sizeMultipacketWrap; // default will be set when parsing parameters
  bool mb_storeIop;
  bool mb_storeDdop;
  TransferCollection_c mc_trans;
  ParseLogLine_t *pt_parseLogLine;
  FILE *m_gpxFile;
  AliveCollection_c m_alive;
  AddressTracker_c mc_tracker;
  std::map< uint8_t, std::vector<uint8_t> > m_vtupl;
  std::map< uint8_t, std::vector<uint8_t> > m_tcupl;
};

inline Main_s::Main_s() :
  mt_sizeMultipacketWrap(0),
  mb_storeIop(false),
  mb_storeDdop(false),
  mc_trans(),
  pt_parseLogLine(0),
  m_gpxFile( NULL ),
  m_alive(),
  mc_tracker()
{
}


struct Main_s gs_main;
} //namespace


enum { OPT_GPX, OPT_TYPE, OPT_WRAP, OPT_TC_SA, OPT_STORE_IOP, OPT_STORE_DDOP, OPT_HELP};

CSimpleOpt::SOption g_rgOptions[] = {
    { OPT_GPX, "-gpx", SO_REQ_SEP },
    { OPT_TYPE, "-t", SO_REQ_SEP },
    { OPT_WRAP, "-w", SO_REQ_SEP },
    { OPT_TC_SA, "-tc", SO_REQ_SEP},
    { OPT_STORE_IOP, "--iop", SO_NONE },
    { OPT_STORE_DDOP, "--ddop", SO_NONE },
    { OPT_HELP, "--help", SO_NONE },
    SO_END_OF_OPTIONS
};

void iopStore(uint8_t sa);
void ddopStore(uint8_t sa);

#include "functionality_vt.inc"
#include "functionality_fs.inc"
#include "functionality_tecu.inc"
#include "functionality_gps.inc"
#include "functionality_nw.inc"
#include "functionality_tc.inc"
#include "functionality_tim.inc"
#include "parsers.inc"


void
exit_with_usage(const char* progname)
{
  std::cerr << "ISOBUS-Logalizer (c) 2007 - 2019 OSB AG." << std::endl << std::endl;
  std::cerr << "Usage: " << progname << " [-t logType] [-gpx gpxFile] [-w num] [-tc num] [--iop] [--ddop] logFile" << std::endl << std::endl;
  std::cerr << "-t:      0 -> can_server [DEFAULT]"<<std::endl;
  std::cerr << "         1 -> rte"<<std::endl;
  std::cerr << "         2 -> CANMon"<<std::endl;
  std::cerr << "         3 -> CANoe ASCII (.asc)"<<std::endl;
  std::cerr << "         4 -> A1ASCII"<<std::endl;
  std::cerr << "         5 -> PCAN-View (.trc)"<<std::endl;
  std::cerr << "         6 -> JohnDeere"<<std::endl;
  std::cerr << "         7 -> rte2"<<std::endl;
  std::cerr << "         8 -> JRF (.jrf)"<<std::endl;
  std::cerr << "         9 -> PCANExplorer (.trc)"<<std::endl;
  std::cerr << "        10 -> SocketCAN candump -l"<<std::endl;
  std::cerr << "        11 -> SocketCAN2 (different candump format)" << std::endl;
  std::cerr << "        12 -> WTK"<<std::endl;
  std::cerr << "        13 -> Kvaser Memorator CSV"<<std::endl;
  std::cerr << "        14 -> ?csv" << std::endl;
  std::cerr << "        15 -> Komodo" << std::endl;
  std::cerr << "        16 -> Vehicle Spy 3 Bus Traffic File" << std::endl;
  std::cerr << "        17 -> PCAN-View v4 (.trc)" << std::endl;
  std::cerr << "        18 -> PCAN-View v6 (.trc)" << std::endl;
  std::cerr << "        19 -> Viewtool Ginkgo (.csv)" << std::endl;
  std::cerr << "        20 -> RM CAN Device Monitor (.txt)" << std::endl;
  std::cerr << "        21 -> Unknown Format Ploeger(.txt)" << std::endl;    // TODO use correct name for format if known
  std::cerr << "        22 -> Unknown Format Ploeger 2(.txt)" << std::endl;    // TODO use correct name for format if known
  std::cerr << std::endl;
  std::cerr << "-w:      Number of data-bytes to display per line. Defaults to 32." << std::endl;
  std::cerr << "-tc:     Override TC SA manually. SA must be given as decimal integer." << std::endl;
  std::cerr << "--iop:   Store VT object pool transfers in iop format. Default: do not store" << std::endl;
  std::cerr << "--ddop:  Store TC object pool transfers in ddop format. Default: do not store" << std::endl;
  std::cerr << "logFile: filepath or - (dash, means standard input rather than a real file)" << std::endl;
  std::cerr << std::endl;

  std::cerr << "can_server:        '104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0'"<<std::endl;
  std::cerr << "rte:               '[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06'"<<std::endl;
  std::cerr << "                    (with OR without Channel-Nr. in []. This is being autodetected.)"<<std::endl;
  std::cerr << "CANMon:            'RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB'"<<std::endl;
  std::cerr << "CANoe:             '  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF'"<<std::endl;
#ifdef WIN32
  std::cerr << "A1ASCII:     'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff    '..." <<std::endl  << "             ...'   446270'"<<std::endl;
#else
  std::cerr << "A1ASCII:           'm e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270'"<<std::endl;
#endif
  std::cerr << "PCANView:          '    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15'"<<std::endl;
#ifdef WIN32
  std::cerr << "JohnDeere:   'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488  '..."<<std::endl;
  std::cerr << "             ...'    17920  ....... '"<<std::endl;
#else
  std::cout << "JohnDeere:         'r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....... '"<<std::endl;
#endif
  std::cerr << "JRF:               '41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF'"<<std::endl;
  std::cerr << "PCANExplorer:      '    13)       116.6 1  Rx     18EF808B 80 8  12 15 15 15 15 15 15 15'"<<std::endl;
  std::cerr << "SocketCAN:         '(1321953173.037244) can1 10B14D4C#FF7F0000FFFFFFFF'"<<std::endl;
  std::cerr << "SocketCAN2:        '(001.004013)  can1  18FEF326   [8]  34 06 30 95 A0 EF 95 3E'" << std::endl;
  std::cerr << "WTK:               '0000.376 can r 18E6FFF1  8  21 00 FF FF 00 00 00 FF  0'"<<std::endl;
  std::cerr << "KvaserM.CSV:       '0.33198,1,cfffff0,4,3,55,7d,7d,,,,,,1,2014-05-05 15:01:08'"<<std::endl;
  std::cerr << "?csv:              '0xCFE46F0*,54.6857,FF,FF,FF,FF,00,FF,FF,FF'" << std::endl;
  std::cerr << "Komodo:            '0:00.003.537,0x0cff05b4,0,8,01 00 01 00 01 00 C0 C0'" << std::endl;
  std::cerr << "Vehicle Spy 3      '2,28.2609260000172,0.01300800000899471,67371012,F,F,HS CAN $CCBFFF7,HS CAN,,CCBFFF7,F,T,FE,FF,FF,FF,00,00,00,00,,,'" << std::endl;
  std::cerr << "PCAN-View v4:      '    13)       116.6  DT  18EF808B RX 8  12 15 15 15 15 15 15 15'" << std::endl;
  std::cerr << "PCAN-View v6:      '    13)       116.6 1 Rx  18EF808B - 8  12 15 15 15 15 15 15 15'" << std::endl;
  std::cerr << "Viewtool Ginkgo:   'Device0,CH1,Extended Frame,Data Frame,0x18EFFFAD,Receive,8,40 FF FF FF FF FF FF FF ,Success,00:05:56.126.000'" << std::endl;
  std::cerr << "RM CAN Device:     '18EF808B 1RXE    8  12  15  15  15  15  15  15  15                        1234,5678" << std::endl;
  std::cerr << "Unknown by Ploeger:'0   419428883 X       8   2  98 255 255 255 255   1  54       0.018340 R'" << std::endl;    //TODO: use proper name for format if known
  std::cerr << "Unknown byPloeger2:'ID=99618844x,Type=D,Length=8,Data=77FF364656023415'" << std::endl;    //TODO: use proper name for format if known

  exit(0);
}


std::string
multiPacket_dump( std::ostringstream& out, PtrDataFrame_t at_ptrFrame )
{
  size_t local_multipacketWrap = gs_main.mt_sizeMultipacketWrap;
  if (local_multipacketWrap > at_ptrFrame->dataSize())
    local_multipacketWrap = at_ptrFrame->dataSize();

  out << ")" << std::endl;

  out << "              " << std::hex << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->sourceAddress()) << "->" << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->destinationAddress()) << " " << std::dec << std::setw(7)<<std::setfill(' ') << uint32_t(at_ptrFrame->dataSize()) << "  " << std::hex;

  for (size_t blockoffset=0; blockoffset < at_ptrFrame->dataSize(); blockoffset += local_multipacketWrap)
  {
    if (blockoffset > 0)
      out << "                              ";

    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        out << std::right << std::setw(2)<<std::setfill('0') << uint32_t(at_ptrFrame->dataOctet(blockoffset+inblock)) << " ";
      else
        out << "   ";
    }
    for (size_t inblock = 0; inblock < gs_main.mt_sizeMultipacketWrap; ++inblock)
    {
      if ((blockoffset + inblock) < at_ptrFrame->dataSize())
        out << getAscii (at_ptrFrame->dataOctet(blockoffset+inblock));
      else
        out << " ";
    }
    out << std::endl;
  }
  out << std::left; //set back to left aligned (no idea where this alignment is needed next)
  return out.str();
}

void
iopAppend( PtrDataFrame_t at_ptrFrame ) {
  if( ( ( at_ptrFrame->pgn() & 0x3FF00 ) == 0xE700 ) && ( at_ptrFrame->data()[0] == 0x11 ) ) {
    if( gs_main.m_vtupl.find( at_ptrFrame->sourceAddress() ) == gs_main.m_vtupl.end() ) {
      gs_main.m_vtupl[ at_ptrFrame->sourceAddress() ] = std::vector<uint8_t>();
    }
    gs_main.m_vtupl[ at_ptrFrame->sourceAddress() ].insert( gs_main.m_vtupl[ at_ptrFrame->sourceAddress() ].end(), at_ptrFrame->data().begin() + 1, at_ptrFrame->data().end() );
  }
}

void
ddopAppend(PtrDataFrame_t at_ptrFrame) {
  if (((at_ptrFrame->pgn() & 0x3FF00) == 0xCB00) && (at_ptrFrame->data()[0] == 0x61)) {
    if (gs_main.m_tcupl.find(at_ptrFrame->sourceAddress()) == gs_main.m_tcupl.end()) {
      gs_main.m_tcupl[at_ptrFrame->sourceAddress()] = std::vector<uint8_t>();
    }
    gs_main.m_tcupl[at_ptrFrame->sourceAddress()].insert(gs_main.m_tcupl[at_ptrFrame->sourceAddress()].end(), at_ptrFrame->data().begin() + 1, at_ptrFrame->data().end());
  }
}

void
iopStore( uint8_t sa ) {
  static unsigned n = 0;
  if( gs_main.m_vtupl.find( sa ) != gs_main.m_vtupl.end() ) {
    char fn[512];
    sprintf( fn, "%03d_0x%02x.iop", n++, sa );
    std::ofstream f( fn, std::ios::binary );
    if( f.is_open() ) {
      std::ostream_iterator<unsigned char> fi(f, "");
      std::copy( gs_main.m_vtupl[sa].begin(), gs_main.m_vtupl[sa].end(), fi );
    } else {
      std::cerr << "Could not open " << fn << " for writing...sorry." << std::endl;
      gs_main.mb_storeIop = false;
    }
    f.close();
    gs_main.m_vtupl.erase( gs_main.m_vtupl.find( sa ) );
  }
}

void
ddopStore(uint8_t sa) {
  static unsigned n = 0;
  if (gs_main.m_tcupl.find(sa) != gs_main.m_tcupl.end()) {
    char fn[512];
    sprintf(fn, "%03d_0x%02x.ddop", n++, sa);
    std::ofstream f(fn, std::ios::binary);
    if (f.is_open()) {
      std::ostream_iterator<unsigned char> fi(f, "");
      std::copy(gs_main.m_tcupl[sa].begin(), gs_main.m_tcupl[sa].end(), fi);
    }
    else {
      std::cerr << "Could not open " << fn << " for writing...sorry." << std::endl;
      gs_main.mb_storeDdop = false;
    }
    f.close();
    gs_main.m_tcupl.erase(gs_main.m_tcupl.find(sa));
  }
}


TransferCollection_c::PtrConnection_t
getTransferConnection(
    std::ostringstream& out,
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
    out << " (ERROR, no " <<
      (TransferCollection_c::variant_tp == ae_variant ? "TP" : "ETP") <<
      " " << std::hex << unsigned(aui8_transferSourceAddress) <<
      "->" << std::hex << unsigned(aui8_transferDestinationAddress) <<") " << std::dec;
  }
  return t_ptrConnection;
}




std::string
interpretePgn( uint32_t rui32_pgn )
{
  std::ostringstream out;

  switch (rui32_pgn)
  {
  case TIM_SERVER_TO_CLIENT_PGN:                  out << "TIM_SERV_TO_CLIENT ";        break;
  case TIM_CLIENT_TO_SERVER_PGN:                  out << "TIM_CLIENT_TO_SERV ";        break;
  case AUTH_SERVER_TO_CLIENT_PGN:                 out << "AUTH_SERV_TO_CLIENT ";       break;
  case AUTH_CLIENT_TO_SERVER_PGN:                 out << "AUTH_CLIENT_TO_SERV ";       break;
  case VT_TO_ECU_PGN:                             out << "VT_TO_ECU         ";         break;
  case ECU_TO_VT_PGN:                             out << "ECU_TO_VT         ";         break;
  case ACKNOWLEDGEMENT_PGN:                       out << "ACKNOWLEDGEMENT   ";         break;
  case CAB_MESSAGE_1:                             out << "CAB_MESSAGE_1     ";         break;
  case PROCESS_DATA_PGN:                          out << "PROCESS_DATA      ";         break;
  case CLIENT_TO_FS_PGN:                          out << "CLIENT_TO_FS      ";         break;
  case FS_TO_CLIENT_PGN:                          out << "FS_TO_CLIENT      ";         break;
  case GUIDANCE_MACHINE_STATUS:                   out << "GUIDANCE_MACH_ST  ";         break;
  case GUIDANCE_SYSTEM_CMD:                       out << "GUIDANCE_SYS_CMD  ";         break;
  case ISOBUS_CERTIFICATION_PGN:                  out << "ISOBUS_CERTIFICAT.";         break;
  case ETP_DATA_TRANSFER_PGN:                     out << "ETP_DATA_TRANSFER ";         break;
  case ETP_CONN_MANAGE_PGN:                       out << "ETP_CONN_MANAGE   ";         break;
  case REQUEST_PGN_MSG_PGN:                       out << "REQUEST_MSG       ";         break;
  case TP_DATA_TRANSFER_PGN:                      out << "TP_DATA_TRANSFER  ";         break;
  case TP_CONN_MANAGE_PGN:                        out << "TP_CONN_MANAGE    ";         break;
  case ADDRESS_CLAIM_PGN:                         out << "ADDRESS_CLAIM     ";         break;
  case PROPRIETARY_A_PGN:                         out << "PROPRIETARY_A     ";         break;
  case PROPRIETARY_A2_PGN:                        out << "PROPRIETARY_A2    ";         break;
  case OPERATORS_EXTERNAL_LIGHT_CONTROLS_MESSAGE: out << "OPERATORS_EXTERNAL_LIGHT_CONTROLS_MESSAGE "; break;
  case BASIC_JOYSTICK_MESSAGE_1:                  out << "BASIC_JOYSTICK_MESSAGE_1 "; break;
  case WORKING_SET_MEMBER_PGN:                    out << "WORKING_SET_MEMBER "; break;
  case WORKING_SET_MASTER_PGN:                    out << "WORKING_SET_MASTER "; break;
  case LANGUAGE_PGN:                              out << "LANGUAGE          "; break;
  case LIGHTING_DATA_PGN:                         out << "LIGHTING_DATA     "; break;
  case LIGHTING_COMMAND_PGN:                      out << "LIGHTING_COMMAND  "; break;
  case HITCH_PTO_COMMANDS:                        out << "HITCH_PTO_COMMANDS "; break;
  case REAR_PTO_STATE_PGN:                        out << "REAR_PTO_STATE    "; break;
  case FRONT_PTO_STATE_PGN:                       out << "FRONT_PTO_STATE   "; break;
  case REAR_HITCH_STATE_PGN:                      out << "REAR_HITCH_STATE  "; break;
  case FRONT_HITCH_STATE_PGN:                     out << "FRONT_HITCH_STATE "; break;
  case MAINTAIN_POWER_REQUEST_PGN:                out << "MAINTAIN_POWER_REQ "; break;
  case WHEEL_BASED_SPEED_DIST_PGN:                out << "WHEEL_BASED_SPEED_DIST "; break;
  case GROUND_BASED_SPEED_DIST_PGN:               out << "GROUND_BASED_SPEED_DIST "; break;
  case ELECTRONIC_TRANSMISSION_CONTROLLER_5:      out << "ELECTRONIC_TRANSMISSION_CONTROLLER_5 "; break;
  case VEHICLE_FLUIDS:                            out << "VEHICLE_FLUIDS    "; break;
  case SELECTED_SPEED_CMD:                        out << "SELECTED_SPEED_CMD "; break;
  case SELECTED_SPEED_MESSAGE:                    out << "SELECTED_SPEED_MESSAGE "; break;
  case ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE:    out << "ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE "; break;
  case ELECTRONIC_TRANSMISSION_CONTROLLER_2:      out << "ELECTRONIC_TRANSMISSION_CONTROLLER_2 "; break;
  case ELECTRONIC_AXLE_CONTROLLER_1:              out << "ELECTRONIC_AXLE_CONTROLLER_1 "; break;
  case PRODUCT_IDENTIFICATION_PGN:                out << "PRODUCT_IDENTIFICATION "; break;
  case CONTROL_FUNCTION_FUNCTIONALITIES_PGN:      out << "CONTROL_FUNCTION_FUNCTIONALITIES "; break;
  case ALL_IMPLEMENTS_STOP_OPERATIONS_SWITCH_STATE_PGN: out << "ALL_IMPLEMENTS_STOP_OPERATIONS_SWITCH_STATE "; break;
  case ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN:     out << "ACTIVE_DIAGNOSTIC_TROUBLE_CODES "; break;
  case SOFTWARE_IDENTIFICATION_PGN:             out << "SOFTWARE_IDENTIFICATION "; break;
  case IDLE_OPERATION:                          out << "IDLE_OPERATION    "; break;
  case VEHICLE_DISTANCE:                        out << "VEHICLE_DISTANCE  "; break;
  case SHUTDOWN:                                out << "SHUTDOWN          "; break;
  case ENGINE_HOURS_REVOLUTIONS:                out << "ENGINE_HOURS_REVOLUTIONS "; break;
  case TIME_DATE_PGN:                           out << "TIME_DATE         "; break;
  case VEHICLE_HOURS:                           out << "VEHICLE_HOURS     "; break;
  case VEHICLE_DIRECTION_SPEED:                 out << "VEHICLE_DIRECTION_SPEED "; break;
  case FUEL_CONSUMPTION_LIQUID:                 out << "FUEL_CONSUMPTION_LIQUID "; break;
  case VEHICLE_WEIGHT:                          out << "VEHICLE_WEIGHT    "; break;
  case ENGINE_TEMPERATURE_1:                    out << "ENGINE_TEMPERATURE_1 "; break;
  case ENGINE_FLUID_LEVEL_PRESSURE_1:           out << "ENGINE_FLUID_LEVEL_PRESSURE_1 "; break;
  case POWER_TAKEOFF_INFORMATION:               out << "POWER_TAKEOFF_INFORMATION "; break;
  case CRUISE_CONTROL_VEHICLE_SPEED:            out << "CRUISE_CONTROL_VEHICLE_SPEED "; break;
  case FUEL_ECONOMY_LIQUID:                     out << "FUEL_ECONOMY_LIQUID "; break;
  case VEHICLE_POSITION:                        out << "VEHICLE_POSITION  "; break;
  case AMBIENT_CONDITIONS:                      out << "AMBIENT_CONDITIONS "; break;
  case VEHICLE_ELECTRICAL_POWER:                out << "VEHICLE_ELECTRICAL_POWER "; break;
  case TRANSMISSION_FLUIDS:                     out << "TRANSMISSION_FLUIDS "; break;
  case DASH_DISPLAY:                            out << "DASH_DISPLAY      "; break;
  case PROPRIETARY_B_PGN:                       out << "PROPRIETARY_B(1of) "; break;
  case NMEA_GPS_POSITION_RAPID_UPDATE_PGN:      out << "NMEA_GPS_POSITION_RAPID_UPDATE "; break;
  case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:       out << "NMEA_GPS_COG_SOG_RAPID_UPDATE "; break;
  case NMEA_GPS_HIGH_PRECISION_RAPID_UPDATE_PGN:out << "NMEA_GPS_HIGH_PRECISION_RAPID_UPDATE "; break;
  case NMEA_GPS_POSITION_DATA_PGN:              out << "NMEA_GPS_POSITION_DATA "; break;
  case NMEA_GPS_TIME_DATE_PGN:                  out << "NMEA_GPS_TIME_DATE "; break;
  case NMEA_GPS_CROSS_TRACK_ERROR_PGN:          out << "NMEA_GPS_CROSS_TRACK_ERROR "; break;
  case NMEA_GPS_DIRECTION_DATA_PGN:             out << "NMEA_GPS_DIRECTION_DATA "; break;
  case NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS:  out << "GNSS Pseudorange Noise Statistics "; break;

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
    out << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_ESTIMATED_FLOW << "_ESTIMATED_FLOW ";
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
    out << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_MEASURED_FLOW << "_MEASURED_FLOW ";
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
    out << "AUX_VALVE_" << std::dec << rui32_pgn-AUX_VALVE_0_COMMAND << "_COMMAND ";
    break;
  default:
    if ((rui32_pgn & 0x00FF00) == PROPRIETARY_B_PGN) {
      out << "PROPRIETARY_B (0x" << std::setfill('0') << std::hex << std::setw(2) << (rui32_pgn - PROPRIETARY_B_PGN) << ") ";
    } else {
      out << std::hex << "0x" << rui32_pgn << std::dec;
    }
    break;
  }

  return out.str();
}

std::string
interpretePgnAcknowledge(PtrDataFrame_t at_ptrFrame)
{
    std::string TypeOfAck;

    switch (at_ptrFrame->dataOctet(0))
    {
    case 0: TypeOfAck = "POSITIVE";
        break;
    case 1: TypeOfAck = "NEGATIVE (NACK)";
        break;
    case 2: TypeOfAck = "ACCESS DENIED";
        break;
    case 3: TypeOfAck = "CANNOT RESPOND NOW (BUSY)";
        break;
    default: TypeOfAck = "undefined";//this should not happen, only 0-3 are defined in standard
        break;
    }

    uint32_t cui32_requestedPgn =
        (uint32_t(at_ptrFrame->dataOctet(7)) << 16) |
        (uint32_t(at_ptrFrame->dataOctet(6)) << 8) |
        (uint32_t(at_ptrFrame->dataOctet(5)));

    std::ostringstream out;
    out << "(0x" << std::right << std::setfill('0') << std::hex << std::setw(5) << cui32_requestedPgn << ", " << interpretePgn(cui32_requestedPgn) << ") " << TypeOfAck << "; GF Value ="
        << " 0x" << std::right << std::setfill('0') << std::hex << std::setw(2) << unsigned(at_ptrFrame->dataOctet(1));
    return out.str();
}


std::string
interpreteRequestPgnMsg(PtrDataFrame_t at_ptrFrame)
{
  uint32_t cui32_requestedPgn =
      (uint32_t(at_ptrFrame->dataOctet(2)) << 16) |
      (uint32_t(at_ptrFrame->dataOctet(1)) << 8) |
      (uint32_t(at_ptrFrame->dataOctet(0)));

  switch (cui32_requestedPgn) {
  case ADDRESS_CLAIM_PGN:
    (void)gs_main.mc_tracker.requestForAddressClaimed(at_ptrFrame);
    break;
  }

  std::ostringstream out;
  out << "(0x" << std::right << std::setfill('0') << std::hex << std::setw(5) << cui32_requestedPgn << ", " << interpretePgn(cui32_requestedPgn) << ")";
  return out.str();
}


std::string
interpreteAddressClaimed(PtrDataFrame_t at_ptrFrame)
{
  readOutName(std::cout, at_ptrFrame);
  return gs_main.mc_tracker.addressClaimed(at_ptrFrame);
}


// forward declaration needed by "interpretePgnsTPETP"
void endOfTransfer( std::ostringstream& out, PtrDataFrame_t at_ptrFrame, TransferCollection_c::Variant_e ae_variant);

std::string
interpretePgnsTPETP(PtrDataFrame_t at_ptrFrame)
{
  if (at_ptrFrame->dataSize() != 8)
    return "*** ILLEGAL - THIS PGN *MUST* HAVE 8 DATABYTES ***";

  std::ostringstream out;

  bool b_streamEnd = false;
  bool boEtpRts = false;
  bool boEtpCts = false;
  bool boEtpDpo = false;
  bool boEtpEoMack = false;
  bool boTpRts = false;
  bool boTpCts = false;
  bool boTpEoMack = false;

  TransferCollection_c::Variant_e e_variant = TransferCollection_c::variant_etp;
  uint8_t sourceAddress = at_ptrFrame->sourceAddress();
  uint8_t destinationAddress = at_ptrFrame->destinationAddress();

  switch (at_ptrFrame->pgn()) {

  case TP_CONN_MANAGE_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_CONN_MANAGE_PGN:
    switch (at_ptrFrame->dataOctet(0)) {
    case 0x10:
      out << "RTS - Request to Send (TP)            ";
      boTpRts = true;
      {
        size_t const ct_sizeTransferData = size_t(at_ptrFrame->dataOctet(2)) << 8 | at_ptrFrame->dataOctet(1);
        (void)gs_main.mc_trans.newConnection(
            e_variant,
            sourceAddress,
            destinationAddress,
            ct_sizeTransferData);
      }
      break;
    case 0x14:
        boEtpRts = true;
      out << "RTS - Request to Send (ETP)           ";
      {
        size_t const ct_sizeTransferData = (static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 24) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(2)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(1)));
        (void)gs_main.mc_trans.newConnection(
            e_variant,
            sourceAddress,
            destinationAddress,
            ct_sizeTransferData);
      }
      break;
    case 0x11:
        boTpCts = true;
      out << "CTS - Clear to Send (TP)              ";
      sourceAddress = at_ptrFrame->destinationAddress();
      destinationAddress = at_ptrFrame->sourceAddress();    //switch SA and DA as the CTS comes from the receiving node, but the connections was saved from the sender's point of view
      break;
    case 0x15:
        boEtpCts = true;
        sourceAddress = at_ptrFrame->destinationAddress();
        destinationAddress = at_ptrFrame->sourceAddress();    //switch SA and DA as the CTS comes from the receiving node, but the connections was saved from the sender's point of view
      out << "CTS - Clear to Send (ETP)             ";
      break;

    case 0x16:
        boEtpDpo = true;
      out << "DPO - Data Packet Offset (ETP)        ";
      {
        TransferCollection_c::PtrConnection_t t_ptrConnection =
          getTransferConnection( out,
              e_variant,
              sourceAddress,
              destinationAddress);
        if (t_ptrConnection)
          t_ptrConnection->mui32_packetOffSet =
            ((static_cast<uint32_t>(at_ptrFrame->dataOctet(4)) << 16) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 8 ) |
             (static_cast<uint32_t>(at_ptrFrame->dataOctet(2))));
      }
      break;
    case 0x13:
      out << "EoMACK - End of Message Ack (TP)      ";
      boTpEoMack = true;
      b_streamEnd = true;
      sourceAddress = at_ptrFrame->destinationAddress();
      destinationAddress = at_ptrFrame->sourceAddress();    //switch SA and DA as the EoMACK comes from the receiving node, but the connections was saved from the sender's point of view
      break;
    case 0x17:
      out << "EoMACK - End of Message Ack (ETP)     ";
      boEtpEoMack = true;
      b_streamEnd = true;
      sourceAddress = at_ptrFrame->destinationAddress();
      destinationAddress = at_ptrFrame->sourceAddress();    //switch SA and DA as the EoMACK comes from the receiving node, but the connections was saved from the sender's point of view
      break;
    case 0x20:
      out << "BAM - Broadcast Announce Msg (TP)     ";
      break;
    case 0xFF:
      out << "CONNABORT - Connection Abort (TP/ETP) ";
      gs_main.mc_trans.deleteConnection(e_variant, at_ptrFrame->sourceAddress(), at_ptrFrame->destinationAddress());
      gs_main.mc_trans.deleteConnection(e_variant, at_ptrFrame->destinationAddress(), at_ptrFrame->sourceAddress());
      break;
    default:
      return out.str();
    } // TP/ETP CONN_MANAGE
    {
      TransferCollection_c::PtrConnection_t t_ptrConnection =
        getTransferConnection( out,
            e_variant,
            sourceAddress,
            destinationAddress);
      if (t_ptrConnection) {
        t_ptrConnection->mui32_embeddedPgn =
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 16) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 8 ) |
          (static_cast<uint32_t>(at_ptrFrame->dataOctet(5)));
        out << " on " << std::setw(5) << std::hex << std::setfill('0') << t_ptrConnection->mui32_embeddedPgn << " (" << std::dec;
        out << interpretePgn(t_ptrConnection->mui32_embeddedPgn);
        out << ")";
      }
    }
    if (boEtpCts)
        out << " #Packets:          " << std::left << std::dec << std::setfill(' ') << std::setw(10) << (uint16_t)at_ptrFrame->dataOctet(1) << " | #NextPacket: " << (uint32_t)((at_ptrFrame->dataOctet(4) << 2 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(3) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(2));
    else if (boEtpDpo)
        out << " #PacketsForOffset: " << std::left << std::dec << std::setfill(' ') << std::setw(10) << (uint16_t)at_ptrFrame->dataOctet(1) << " | DPO        : " << (uint32_t)((at_ptrFrame->dataOctet(4) << 2 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(3) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(2));
    else if (boEtpRts)
        out << " #Bytes:            " << std::left << std::dec  << std::setfill(' ') << std::setw(10) << (uint32_t)((at_ptrFrame->dataOctet(4) << 3 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(3) << 2 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(2) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(1));
    else if (boEtpEoMack)
        out << " #TotalBytes: " << std::left << std::dec << std::setfill(' ') << std::setw(10) << (uint32_t)((at_ptrFrame->dataOctet(4) << 3 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(3) << 2 * BITS_PER_BYTE) | (at_ptrFrame->dataOctet(2) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(1));
    else if (boTpCts)
        out << " #Packets:      " << std::left << std::dec << std::setfill(' ') << std::setw(5) << (uint16_t)at_ptrFrame->dataOctet(1)                                                  << " | #NextPacket:   " <<                 (uint16_t)at_ptrFrame->dataOctet(2);
    else if (boTpRts)
        out << " #TotalPackets: " << std::left << std::dec << std::setfill(' ') << std::setw(5) << (uint16_t)at_ptrFrame->dataOctet(3)                                                  << " | #TotalBytes:   " << std::setw(5) << (uint16_t)((at_ptrFrame->dataOctet(2) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(1)) << " | #MaxPacketsResponse: " << (uint16_t)at_ptrFrame->dataOctet(4);
    else if (boTpEoMack)
        out << " #TotalBytes:   " << std::left << std::dec << std::setfill(' ') << std::setw(5) << (uint16_t)((at_ptrFrame->dataOctet(2) << BITS_PER_BYTE) | at_ptrFrame->dataOctet(1)) << " | #TotalPackets: " <<                 (uint16_t)at_ptrFrame->dataOctet(3);
    break;

  case TP_DATA_TRANSFER_PGN:
    e_variant = TransferCollection_c::variant_tp;
    /* fall through */
  case ETP_DATA_TRANSFER_PGN:
    out << "DATA - Data Packet #"<<std::setw(2)<<std::setfill(' ')<<std::dec<<uint16_t(at_ptrFrame->dataOctet(0));
    {
      TransferCollection_c::PtrConnection_t t_ptrConnection =
        getTransferConnection( out,
            e_variant,
            sourceAddress,
            destinationAddress);

      if (t_ptrConnection)
      {
          if ((at_ptrFrame->pgn() == TP_DATA_TRANSFER_PGN)) {
              for (int i = 0; i < 7; i++) {
                  if ((7 * (at_ptrFrame->dataOctet(0) - 1)) + i >= int(t_ptrConnection->mvec_data.size()))
                      break;
                  size_t t_index = (7 * (at_ptrFrame->dataOctet(0) - 1)) + i;
                  (t_ptrConnection->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
              }
          }
          else if (at_ptrFrame->pgn() == ETP_DATA_TRANSFER_PGN) {
              for (int i = 0; i < 7; i++) {
                  size_t t_index = (t_ptrConnection->mui32_packetOffSet * 7 + (7 * (at_ptrFrame->dataOctet(0) - 1))) + i;
                  if (t_index >= t_ptrConnection->mvec_data.size())
                      break;
                  (t_ptrConnection->mvec_data)[t_index] = at_ptrFrame->dataOctet(i + 1);
              }
          }
      }
    }
    break;
  default:
    break;
  }

  if (b_streamEnd) { //instead of obsolete endOfTransfer(e_variant);
    TransferCollection_c::PtrConnection_t t_ptrConnection =
      getTransferConnection( out,
          e_variant,
          sourceAddress,        //source and destination address were already switch above
          destinationAddress);
    if (t_ptrConnection) {
      endOfTransfer( out, at_ptrFrame, e_variant );
    }
  }

  return out.str();
}


Interprete_t *
getPgnDataInterpreter( PtrDataFrame_t at_ptrFrame )
{
    uint32_t pgn = at_ptrFrame->pgn();
    if ((pgn & PROPRIETARY_B_PGN) == PROPRIETARY_B_PGN)
        pgn = PROPRIETARY_B_PGN;
  switch (pgn)
  {
  case TIM_SERVER_TO_CLIENT_PGN:                return interpretePgnsTimServerToClient;
  case TIM_CLIENT_TO_SERVER_PGN:                return interpretePgnsTimClientToServer;
  case AUTH_SERVER_TO_CLIENT_PGN:               return interpreteTimAuthenticationServerToClient;
  case AUTH_CLIENT_TO_SERVER_PGN:               return interpreteTimAuthenticationClientToServer;
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
  case AUX_VALVE_15_ESTIMATED_FLOW:             return interpreteValveEstimatedFlow;
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
  case AUX_VALVE_15_MEASURED_FLOW:              return interpreteValveMeasuredFlow;
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
  case AUX_VALVE_15_COMMAND:                    return interpreteValveCommand;
  case REQUEST_PGN_MSG_PGN:                     return interpreteRequestPgnMsg;
  case ADDRESS_CLAIM_PGN:                       return interpreteAddressClaimed;
  case PROCESS_DATA_PGN:                        return interpreteProcessData;
  case MAINTAIN_POWER_REQUEST_PGN:              return interpreteMaintainPower;
  case NMEA_GPS_CROSS_TRACK_ERROR_PGN:          return interpreteCrossTrackError;
  case GUIDANCE_MACHINE_STATUS:                 return interpreteGuidanceMachineStatus;
  case GUIDANCE_SYSTEM_CMD:
  case ISOBUS_CERTIFICATION_PGN:
  case PROPRIETARY_A_PGN:
  case PROPRIETARY_A2_PGN:
  case WORKING_SET_MEMBER_PGN:
  case WORKING_SET_MASTER_PGN:
  case LIGHTING_DATA_PGN:
  case HITCH_PTO_COMMANDS:
  case FRONT_PTO_STATE_PGN:
  case FRONT_HITCH_STATE_PGN:
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


std::string
interpretePgnData( PtrDataFrame_t at_ptrFrame )
{
  Interprete_t *p_f = getPgnDataInterpreter(at_ptrFrame);
  if (p_f)
    return p_f(at_ptrFrame);
  else
    return "";
}


void
endOfTransfer( std::ostringstream& out, PtrDataFrame_t at_ptrFrame, TransferCollection_c::Variant_e ae_variant )
{
  TransferCollection_c::PtrConnection_t t_ptrConnection =
    getTransferConnection( out,
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  if (t_ptrConnection) {
    PtrDataFrame_t t_ptrArtificialFrame = new DataFrame_c(
        t_ptrConnection->mvec_data,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress(),
        t_ptrConnection->mui32_embeddedPgn
        );

    // output in some ways, dump (raw), interprete and maybe store to a file
    multiPacket_dump( out, t_ptrArtificialFrame);
    out << interpretePgnData(t_ptrArtificialFrame);
    if (gs_main.mb_storeIop) iopAppend(t_ptrArtificialFrame);
    if (gs_main.mb_storeDdop) ddopAppend(t_ptrArtificialFrame);
   
    gs_main.mc_trans.deleteConnection(
        ae_variant,
        at_ptrFrame->destinationAddress(),
        at_ptrFrame->sourceAddress());
  }
}


ParseLogLine_t *
getLogLineParser( size_t at_choice )
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
    parseLogLineTrc2,
    parseLogLineSocketCAN,
    parseLogLineSocketCAN2,
    parseLogLineWTK,
    parseLogLineKvaserMemorator,
    parseLogLineCsv,
    parseLogLineKomodo,
    parseLogLineVehicleSpy,
    parseLogLineTrc3,
    parseLogLineTrc4,
    parseLogLineViewtoolGinkgoCsv,
    parseLogLineRmCanDeviceMonitor,
    parseLogLinePloegerUnknownFormat,
    parseLogLinePloegerUnknownFormat_2,
    defaultParseLogLine
  };

  size_t t_sizeLogLine = sizeof carrpt_parseLogLine / sizeof carrpt_parseLogLine[0];
  return carrpt_parseLogLine[
      at_choice < t_sizeLogLine ? at_choice : t_sizeLogLine - 1 ];
}


std::pair< int, PtrDataFrame_t >
parseLogLine( std::ostream& out, std::string const &acr_line )
{
#if DEBUG
  out << "Reading " << acr_line << std::endl;
#endif
  std::pair< int, PtrDataFrame_t > result = gs_main.pt_parseLogLine(acr_line);
  if (result.first == 0) // no error
  { /// Printout interpreted line
    PtrDataFrame_t t_ptrFrame = result.second;

    // Timestamp
    out << std::setfill (' ') << std::dec << std::setw (10) << (t_ptrFrame->time()/1000) << "." << std::setfill('0')<<std::setw(3)<<(t_ptrFrame->time()%1000)<< "   ";

    if (t_ptrFrame->isExtendedFrameFormat()) {
        // CAN-ID / number of bytes
        out << std::setfill('0') << std::hex << std::setw(8) << t_ptrFrame->identifier() << "  " << uint16_t(t_ptrFrame->dataSize()) << " ";
    }
    else
    {
        out << std::setfill(' ') << std::hex << std::setw(8) << t_ptrFrame->identifier() << "  " << uint16_t(t_ptrFrame->dataSize()) << " " << std::setfill('0');
    }

    // Databytes (HEX)
    size_t i;
    for (i=0; i< t_ptrFrame->dataSize(); i++) out << " " << std::setw (2) <<uint16_t(t_ptrFrame->dataOctet(i));
    for (;    i<8;              i++) out << "   ";
    out << "  ";

    // Databytes (HEX)
    for (i=0; i< t_ptrFrame->dataSize(); i++) out << std::setw (1) << char(t_ptrFrame->asciiDataOctet(i));
    for (;    i<8;              i++) out << " ";

    if (t_ptrFrame->isExtendedFrameFormat()) {
      // SA
      out << "  "<<std::setw(2) << uint16_t(t_ptrFrame->sourceAddress()) << "->";
      // DA
      if (t_ptrFrame->isPdu1()) out << std::setw(2) << uint16_t(t_ptrFrame->ps());
      else out << "FF";

      // Priority
      out << " (" << uint16_t(t_ptrFrame->prio()) << ")";

      // PGN
      out << " " << std::setw(5) << t_ptrFrame->pgn() << " => ";

      // Interpreted PGN
      out << interpretePgn(t_ptrFrame->pgn());

      // Interpreted PGN-Data
      out << interpretePgnData (t_ptrFrame);

      gs_main.mc_tracker.checkTimeouts( out, t_ptrFrame->time() );
      out << std::endl;
    }
    else  //standard Identifier
    {
        // SA
        out << "  " << std::setw(2) << uint16_t(t_ptrFrame->sourceAddress()) << "->??";
        //DA not available for standard identifier
        //printout the highest 3 Bits of Identifier
        out << " 3 MSB of Identifier: " << ((t_ptrFrame->identifier() >> 1*BITS_PER_BYTE ) & 0xF);
        out << std::endl;
    }
  }
  else
  {
    // report original line:
    out << "(" << acr_line << ")" <<std::endl;
  }
  return result;
}


#include "checks.inc"
#include "gpx_writer.inc"


int main (int argc, char** argv)
{
  gs_main.pt_parseLogLine = parseLogLineCanServer;
  gs_main.mt_sizeMultipacketWrap = 32; // CAN server default
  CSimpleOpt args(argc, argv, g_rgOptions);
  // while there are arguments left to process
  while (args.Next()) {
    if(args.LastError() == SO_SUCCESS) {
        switch ( args.OptionId() ) {
          case OPT_TYPE:
            gs_main.pt_parseLogLine = getLogLineParser( atoi( args.OptionArg() ) );
            break;
          case OPT_GPX:
            gs_main.m_gpxFile = gpxInit( args.OptionArg() );
            break;
          case OPT_WRAP:
            gs_main.mt_sizeMultipacketWrap = atoi( args.OptionArg() );
            break;
          case OPT_TC_SA:
              // "-tc 89": override SA of TC server manually
              vSetOverrideTcSa(atoi(args.OptionArg()));
              break;
          case OPT_STORE_IOP:
            gs_main.mb_storeIop = true;
            break;
          case OPT_STORE_DDOP:
            gs_main.mb_storeDdop = true;
            break;
          case OPT_HELP:
            exit_with_usage( argv[0] );
            return 0;
        }
    } else {
      printf( "Invalid argument: %s\n", args.OptionText());
      return 1;
    }
  }

  if( args.FileCount() != 1 ) {
    exit_with_usage( argv[0] );
  }

  PtrInputStream_t t_ptrIn = PtrInputStream_t( new InputStream_c( args.Files()[0] ) );

  if (!t_ptrIn->isOpen())
    exit_with_error( ( std::string( "Couldn't open file: " ) + std::string( args.Files()[0] ) ).c_str() );

  std::string str_line;
  for (;;) {
    getline(t_ptrIn->raw(), str_line);
    if( t_ptrIn->raw().fail() ) {
      break;
    }
    std::pair< int, PtrDataFrame_t > parse_result = parseLogLine(std::cout, str_line);
    if (0 == parse_result.first)
    {
      checkAlives(parse_result.second);
      checkSingles(parse_result.second);
      checkHandshakingsVtCommands(parse_result.second);
      checkHandshakingTP(parse_result.second);
      checkHandshakingsProcData(parse_result.second);
    }
    if (t_ptrIn->raw().eof()) {
      break;
    }
  }

  t_ptrIn = PtrInputStream_t(0);

  gs_main.m_alive.report( std::cout );
  printFullListOfSupportedFunctions(std::cout);
  printFullNameAndAddressList(std::cout);
  if( gs_main.m_gpxFile )
    gpxClose();
}
