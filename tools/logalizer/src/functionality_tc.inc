std::string
interpreteProcessData( PtrDataFrame_t at_ptrFrame )
{
  if (at_ptrFrame->dataSize() < 8)
    return "*** ILLEGAL - THIS PGN *MUST* HAVE AT LEAST 8 DATABYTES ***";

  std::ostringstream out;

  if (at_ptrFrame->dataSize() > 8)
  { // can only be device description upload
  }
  else
  {
    bool valueAndMeasurementCommand = false;

    switch (at_ptrFrame->dataOctet(0) & 0x0F)
    {
    case 0x00: out << "Command Technical Capabilities"; break;
    case 0x01: out << "Command Device Description (Transfer and Management)"; break;
    case 0x02: out << "Command Request Value  "; valueAndMeasurementCommand = true; break;
    case 0x03: out << "Command Value          "; valueAndMeasurementCommand = true; break;
    case 0x04: out << "Command Measur. Time   "; valueAndMeasurementCommand = true; break;
    case 0x05: out << "Command Measur. Dist.  "; valueAndMeasurementCommand = true; break;
    case 0x06: out << "Command Measur. Min.   "; valueAndMeasurementCommand = true; break;
    case 0x07: out << "Command Measur. Max.   "; valueAndMeasurementCommand = true; break;
    case 0x08: out << "Command Measur. Change "; valueAndMeasurementCommand = true; break;
    case 0x09: out << "Command Peer Control Assignment"; break;
    case 0x0A: out << "Command Set Value and Acknowledge"; break;
    case 0x0D: out << "Message Process Data Acknowledge (PDACK)"; break;
    case 0x0E: out << "Message Task Controller Status"; break;
    case 0x0F: out << "Message Working Set Task"; break;
    default:   out << "RESERVED."; break;
    }

    if( valueAndMeasurementCommand )
    {
      uint16_t element = (at_ptrFrame->dataOctet(0) >> 4) | (uint16_t(at_ptrFrame->dataOctet(1)) << 4);
      uint32_t ddi = uint16_t(at_ptrFrame->dataOctet(2)) | (uint16_t(at_ptrFrame->dataOctet(3)) << 8);
      uint32_t value = uint32_t(at_ptrFrame->dataOctet(4)) | uint32_t(at_ptrFrame->dataOctet(5))<<8 | uint32_t(at_ptrFrame->dataOctet(6))<<16 | uint32_t(at_ptrFrame->dataOctet(4))<<24;

      out << "E=" << std::setw(4) << std::dec << element << " DDI=";
      if( ddi >= 0xdfff )
        out << "$" << std::setw(4) << std::hex << ddi << std::dec;
      else
        out << std::setw(5) << std::dec << ddi;
      out << " => "<< value;
    }
  }

  return out.str();
}
