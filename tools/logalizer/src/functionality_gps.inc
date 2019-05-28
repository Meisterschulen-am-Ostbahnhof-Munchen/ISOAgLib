std::string
gps_xteMode(unsigned data)
{
  switch (data & 0x3u) {
  case 0u:
    return "0=Autonomous";
  case 1u:
    return "1=Differential_enhanced";
  case 2u:
    return "2=Estimated";
  case 3u:
    return "3=Simulator";
  case 4u:
    return "4=Manual";
  default:
    return "(protocol error)";
  }
}

std::string
gps_navTerm(unsigned data)
{
  switch (data & 0x3u) {
  case 0u:
    return "0=No";
  case 1u:
    return "1=Yes";
  default:
    return "(protocol error)";
  }
}


std::string
interpreteCrossTrackError(PtrDataFrame_t at_ptrFrame)
{
  std::ostringstream out;

  int32_t xte = int32_t( uint32_t(at_ptrFrame->dataOctet(2)) | uint32_t(at_ptrFrame->dataOctet(3))<<8 | uint32_t(at_ptrFrame->dataOctet(4))<<16 | uint32_t(at_ptrFrame->dataOctet(5))<<24 );

  // for different valves resp. different values of at_ptrFrame->pgn()
  out <<
    "SID: " << unsigned(at_ptrFrame->dataOctet(0)) << " " <<
    "XTE Mode: " << gps_xteMode(at_ptrFrame->dataOctet(1) >> 4) << " " <<
    "NavTerm: " << gps_navTerm(at_ptrFrame->dataOctet(1) & 0x3) << " " <<
    "XTE: " << double(xte)*0.01 << "m ";

  return out.str();
}

std::string
interpreteCogSogRapidUpdate(PtrDataFrame_t at_ptrFrame)
{
    std::ostringstream out;

    const uint16_t cu16CogValueRaw = at_ptrFrame->dataOctet(2) | static_cast<uint16_t>(at_ptrFrame->dataOctet(3) << 8);
    const uint16_t cu16SogValueRaw = at_ptrFrame->dataOctet(4) | static_cast<uint16_t>(at_ptrFrame->dataOctet(5) << 8);
    const float_t cf32CogValue = cu16CogValueRaw * 0.0057f;
    const float_t cf32SogValue = cu16SogValueRaw * 0.01f;
    out << "COG: " << cf32CogValue << "° " << " | SOG: " << cf32SogValue << "m/s";

    return out.str();
}

std::string
interpretePositionDataRapidUpdate(PtrDataFrame_t at_ptrFrame)
{
    std::ostringstream out;
    
    const int32_t ci32LatitudeRaw =  static_cast<uint32_t>(at_ptrFrame->dataOctet(0)        |
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(1)) <<   8|
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(2)) << 2*8|
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(3)) << 3*8);
    const int32_t ci32LongitudeRaw = static_cast<uint32_t>(at_ptrFrame->dataOctet(4)        |
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(5)) <<   8|
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) << 2*8|
                                     static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 3*8);

    const float_t cf32Latitude  = ci32LatitudeRaw  * 0.0000001f;
    const float_t cf32Longitude = ci32LongitudeRaw * 0.0000001f;
    out << "Latitude: " << cf32Latitude << "° " << " | Longitude: " << cf32Longitude << "° ";
    return out.str();
}

std::string
interpreteGpsPositionData(PtrDataFrame_t at_ptrFrame)
{
    std::ostringstream out;
    bool boResetValues = false;
    const uint8_t cu8FastPacketNrLowerNibble = at_ptrFrame->dataOctet(0) & 0xFU;  //TODO: (Wenn Zeit) Higher Nibble auch überprüfen, ob das zusammenpasst für die einzelnen Nachrichten
    static bool bo1stPartOfMsgReceived = false;
    static bool bo2ndPartOfMsgReceived = false;
    static bool bo3rdPartOfMsgReceived = false;
    static bool bo4thPartOfMsgReceived = false;
    static bool bo5thPartOfMsgReceived = false;
    static uint8_t u8TypeOfSystem = 0;
    static int64_t i64EntireLatitude = 0;
    static int64_t i64EntireLongitude = 0;
    static int64_t i64EntireAltitude = 0;
    static uint16_t u16PosDateRaw = 0;
    static uint32_t u32PosTimeRaw = 0;
    switch (cu8FastPacketNrLowerNibble)
    {
    case 0: //Bytes 1-7
        if (bo1stPartOfMsgReceived)
        {
            out << "Error! Should receive 2nd part of Msg now";
            break;
        }
        // First Msg Part: get Date, Time and first / last two Bytes of Latitude
        u16PosDateRaw = static_cast<uint16_t> (static_cast<uint16_t>(at_ptrFrame->dataOctet(3)) |
                                               static_cast<uint16_t>(at_ptrFrame->dataOctet(4)) << 8);

        u32PosTimeRaw = static_cast<uint32_t> (static_cast<uint32_t>(at_ptrFrame->dataOctet(5))        |
                                               static_cast<uint32_t>(at_ptrFrame->dataOctet(6)) <<   8 |
                                               static_cast<uint32_t>(at_ptrFrame->dataOctet(7)) << 2*8);

        bo1stPartOfMsgReceived = true;
        break;

    case 1: //Bytes 8-15
        if (bo2ndPartOfMsgReceived || !bo1stPartOfMsgReceived)
        {
            out << "Error! Wrong sequence! Ignoring rest of message";
            boResetValues = true;
        }
        // Second Msg Part: get rest of time and build beginning of latitude value
        u32PosTimeRaw |= static_cast<uint32_t> (static_cast<uint32_t>(at_ptrFrame->dataOctet(1)) << 3 * 8);

        i64EntireLatitude = static_cast<uint64_t>( static_cast<uint64_t>(at_ptrFrame->dataOctet(2)) << 0*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(3)) << 1*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(4)) << 2*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(5)) << 3*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(6)) << 4*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(7)) << 5*8);
        bo2ndPartOfMsgReceived = true;
        break;

    case 2:
        if (bo3rdPartOfMsgReceived || !bo1stPartOfMsgReceived || !bo2ndPartOfMsgReceived)
        {
            out << "Error! Wrong sequence! Ignoring rest of message";
            boResetValues = true;
        }
        //3rd Part of Msg: get rest of latitude and beginning of Longitude
        i64EntireLatitude |= static_cast<uint64_t>(static_cast<uint64_t>(at_ptrFrame->dataOctet(1)) << 6 * 8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(2)) << 7 * 8);

        i64EntireLongitude = static_cast<uint64_t>(static_cast<uint64_t>(at_ptrFrame->dataOctet(3)) << 0*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(4)) << 1*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(5)) << 2*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(6)) << 3*8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(7)) << 4*8);
        bo3rdPartOfMsgReceived = true;
        break;

    case 3:
        if (bo4thPartOfMsgReceived || !bo3rdPartOfMsgReceived || !bo1stPartOfMsgReceived || !bo2ndPartOfMsgReceived)
        {
            out << "Error! Wrong sequence! Ignoring rest of message";
            boResetValues = true;
        }
        //4th Part of Msg Received: get the rest of Longitude and the beginning of Altitude
        i64EntireLongitude |= static_cast<uint64_t>(static_cast<uint64_t>(at_ptrFrame->dataOctet(1)) << 5 * 8 |
                                                    static_cast<uint64_t>(at_ptrFrame->dataOctet(2)) << 6 * 8 |
                                                    static_cast<uint64_t>(at_ptrFrame->dataOctet(3)) << 7 * 8);

        i64EntireAltitude = static_cast<uint64_t>(static_cast<uint64_t>(at_ptrFrame->dataOctet(4)) << 0*8 |
                                                  static_cast<uint64_t>(at_ptrFrame->dataOctet(5)) << 1*8 |
                                                  static_cast<uint64_t>(at_ptrFrame->dataOctet(6)) << 2*8 |
                                                  static_cast<uint64_t>(at_ptrFrame->dataOctet(7)) << 3*8);
        bo4thPartOfMsgReceived = true;
        break;

    case 4:
        if (bo5thPartOfMsgReceived || !bo4thPartOfMsgReceived || !bo3rdPartOfMsgReceived || !bo1stPartOfMsgReceived || !bo2ndPartOfMsgReceived)
        {
            out << "Error! Wrong sequence! Ignoring rest of message";
            boResetValues = true;
        }
        //5th Part Of Msg Received(for now this is all we're interested in): get 2 MSB of Altitude and the Type of System
        i64EntireAltitude |= static_cast<uint64_t>(static_cast<uint64_t>(at_ptrFrame->dataOctet(1)) << 4 * 8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(2)) << 5 * 8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(3)) << 6 * 8 |
                                                   static_cast<uint64_t>(at_ptrFrame->dataOctet(4)) << 7 * 8);

        u8TypeOfSystem = at_ptrFrame->dataOctet(5) & 0xFU;
        bo5thPartOfMsgReceived = true;
        break;

    default: 
        break;
    }

    if (bo1stPartOfMsgReceived && bo2ndPartOfMsgReceived && bo3rdPartOfMsgReceived && bo4thPartOfMsgReceived && bo5thPartOfMsgReceived)
    {
        // we got the entire message now, so print it:
        out << "Summary: " << "Seconds since Midnight: " << u32PosTimeRaw * 0.0001F << " (~" << u32PosTimeRaw * 0.0001F / 3600 << "h)" << " | Date (Daycount since 01/01/1970): " << u16PosDateRaw << " | Latitude: " << (i64EntireLatitude / 1e12) * 0.0001 << "°"  << " | Longitude: " << (i64EntireLongitude / 1e12) * 0.0001 << "°" << " | Altitude: " << (i64EntireAltitude / 1e3) * 0.001 << "m";
        out << " | Type of System: ";
        switch (u8TypeOfSystem)
        {
        case 0:
            out << "GPS";
            break;
        case 1:
            out << "GLONASS";
            break;
        case 2:
            out << "GPS+GLONASS";
            break;
        case 3:
            out << "GPS+SBAS(WAAS)";
            break;
        case 4:
            out << "GPS+SBAS+GLONASS";
            break;
        default:
            out << "RESERVED";
            break;
        }
        //reset all values:
        boResetValues = true;
    }

    if (boResetValues)
    {
        bo1stPartOfMsgReceived = false;
        bo2ndPartOfMsgReceived = false;
        bo3rdPartOfMsgReceived = false;
        bo4thPartOfMsgReceived = false;
        bo5thPartOfMsgReceived = false;
        u8TypeOfSystem = 0;
        i64EntireLatitude = 0;
        i64EntireLongitude = 0;
        i64EntireAltitude = 0;
        u16PosDateRaw = 0;
        u32PosTimeRaw = 0;
    }

    return out.str();
}

