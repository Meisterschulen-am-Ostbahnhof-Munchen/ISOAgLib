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

