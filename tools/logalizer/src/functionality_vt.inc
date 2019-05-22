std::string
interpreteAux2StatusValue(uint16_t u16Value1, uint16_t u16Value2)
{
    std::ostringstream out;
    out << " | Value 1: " << u16Value1 << " | Value 2: " << u16Value2;
    return out.str();
}

std::string
partiallyInterpretePgnsVtEcu( PtrDataFrame_t at_ptrFrame, bool response )
{
  if (at_ptrFrame->dataSize() < 8)
    return "*** ILLEGAL - THIS PGN *MUST* HAVE AT LEAST 8 DATABYTES ***";

  std::ostringstream out;

  if (at_ptrFrame->dataOctet(1-1) < 0x10)
    out << "Activation ";
  else if ((at_ptrFrame->dataOctet(1-1) >=0x20) && (at_ptrFrame->dataOctet(1-1) < 0x30))
    out << "Auxiliary ";
  else if ((at_ptrFrame->dataOctet(1-1) >=0x90) && (at_ptrFrame->dataOctet(1-1) < 0xF0))
    out << "Command ";

  int8_t i8_uidBytePos = -1;
  char str_uidBytePos[ 100 ] = " ObjectID=";

  switch (at_ptrFrame->dataOctet(1-1))
  {
    case 0x00:
      if ( ((uint16_t(at_ptrFrame->dataOctet(3 + 1 - 1) << 8)) | (uint16_t(at_ptrFrame->dataOctet(3 - 1)))) == 0xFFFF )
        out << "Alarm ACK";
      else
        out << "Soft Key";
      i8_uidBytePos = 3;
      switch (at_ptrFrame->dataOctet(2-1))
      {
        case 0: out << " - Released"; break;
        case 1: out << " - Pressed "; break;
        case 2: out << " - Held    "; break;
        case 3: out << " - Aborted "; break;
        default:                      break;
      }
      break;

    case 0x01: out << "Button"; i8_uidBytePos = 3;
      switch (at_ptrFrame->dataOctet(2-1))
      {
        case 0: out << " - Released"; break;
        case 1: out << " - Pressed "; break;
        case 2: out << " - Held    "; break;
        case 3: out << " - Aborted "; break;
        default:                      break;
      }
      break;

    case 0x02: out << "Pointing Event";
      out << " X:" << ((uint16_t(at_ptrFrame->dataOctet(2 + 1 - 1) << 8)) | (uint16_t(at_ptrFrame->dataOctet(2 - 1))));
      out << " Y:" << ((uint16_t(at_ptrFrame->dataOctet(4 + 1 - 1) << 8)) | (uint16_t(at_ptrFrame->dataOctet(4 - 1))));
      switch (at_ptrFrame->dataOctet(6-1) & 0x0F)
      {
        case 0: out << " - Released"; break;
        case 1: out << " - Pressed "; break;
        case 2: out << " - Held    "; break;
        default:                      break;
      }
      break;

    case 0x03: out << "VT Select Input Object"; i8_uidBytePos = 2; break;
    case 0x04: out << "VT ESC"; i8_uidBytePos = 2; break;
    case 0x05: out << "VT Change Numeric Value"; i8_uidBytePos = 2; break;
    case 0x06: out << "VT Change Active Mask"; i8_uidBytePos = 2; break;
    case 0x07: out << "VT Change Soft Key Mask"; i8_uidBytePos = 4; break;
    case 0x08: out << "Input String Value"; i8_uidBytePos = 2; break;

    case 0x11: out << "Object Pool Transfer"; break;
    case 0x12: out << "End of Object Pool"; 
        if (response)
        {
            if (at_ptrFrame->dataOctet(2 - 1) == 0)
                out << " (No Error) ";
            else
                out << " (Error(s)) ";
        }
        break;

    case 0x20: out << "Auxiliary Assignment Type 1"; i8_uidBytePos = 4; break;
    case 0x21: out << "Auxiliary Input Type 1 status"; break;

    case 0x22: out << "Preferred Assignment"; break;
    case 0x23: out << "Input Type 2 Maintenance"; i8_uidBytePos = 2; strcpy( str_uidBytePos, " MIC=" ); if (at_ptrFrame->dataOctet(4-1)) out << " -READY-"; else out << " -INIT'-"; break;
    case 0x24: out << "Assignment Type 2"; i8_uidBytePos = response ? 2 : 13; break;
    case 0x25: out << "Input Status Type 2 Enable"; i8_uidBytePos = 2; break;
    case 0x26: out << "Input Type 2 Status message"; 
        i8_uidBytePos = 2; 
        out << interpreteAux2StatusValue(at_ptrFrame->dataOctet(3) | (at_ptrFrame->dataOctet(4) << 8), 
                                         at_ptrFrame->dataOctet(5) | (at_ptrFrame->dataOctet(6) << 8)); 
        break;
    case 0x27: out << "Capabilities"; break;

    case 0x92: out << "ESC"; i8_uidBytePos = 2; break;

    case 0xC0: out << "Get Memory"; break;
    case 0xC2: out << "Get Number Of Soft Keys"; break;
    case 0xC3: out << "Get Text Font Data"; break;
    case 0xC7: out << "Get Hardware"; break;

    case 0xD0: out << "Store Version"; 
        if (response)
        {
            if (at_ptrFrame->dataOctet(6 - 1) == 0)
                out << " (No Error) ";
            else
                out << " (Error(s)) ";
        }
        break;
    case 0xD1: out << "Load Version"; break;
    case 0xD2: out << "Delete Version"; break;

    case 0xDF: // break left out intentionally
    case 0xE0: out << "Get Versions"; break;

    case 0xA0: out << "Hide/Show Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) out << " -show-"; else out << " -hide-"; break;
    case 0xA1: out << "Enable/Disable Object"; i8_uidBytePos = 2; if (at_ptrFrame->dataOctet(4-1)) out << " -enable-"; else out << " -disable-"; break;
    case 0xA2: out << "Select Input Object"; i8_uidBytePos = 2; break;
    case 0xA3: out << "Control Audio Device"; break;
    case 0xA4: out << "Set Audio Volume"; break;
    case 0xA5: out << "Change Child Location"; i8_uidBytePos = 4; break;
    case 0xA6: out << "Change Size"; i8_uidBytePos = 2; break;
    case 0xA7: out << "Change Background Colour"; i8_uidBytePos = 2; break;
    case 0xA8: out << "Change Numeric Value"; i8_uidBytePos = 2; break;
    case 0xA9: out << "Change End Point"; i8_uidBytePos = 2; break;
    case 0xAA: out << "Change Font Attributes"; i8_uidBytePos = 2; break;
    case 0xAB: out << "Change Line Attributes"; i8_uidBytePos = 2; break;
    case 0xAC: out << "Change Fill Attributes"; i8_uidBytePos = 2; break;
    case 0xAD: out << "Change Active Mask"; i8_uidBytePos = response ? 2 : 4; break;
    case 0xAE: out << "Change Soft Key Mask"; i8_uidBytePos = 5; break;
    case 0xAF: out << "Change Attribute"; i8_uidBytePos = 2; out << " AID="; out << uint16_t(at_ptrFrame->dataOctet(4-1)); break;

    case 0xB0: out << "Change Priority"; i8_uidBytePos = 2; break;
    case 0xB1: out << "Change List Item"; i8_uidBytePos = 2; break;
    case 0xB2: out << "Delete Object Pool"; break;
    case 0xB3: out << "Change String Value"; i8_uidBytePos = response ? 4 : 2; break;
    case 0xB4: out << "Change Child Position"; i8_uidBytePos = 4; break;

    case 0xBD:
        out << "Lock/Unlock Mask";
        if (at_ptrFrame->dataOctet(2-1))
            out << " -lock-";
        else
            out << " -unlock-";
        if (response)
        {
            if ((at_ptrFrame->dataOctet(3-1) & 0x70) == 0)
            {
                out << " Normal response,";
            }
            else
            {
                out << " Unsolicited unlock,";
            }
            out << " Error Codes: 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << unsigned(at_ptrFrame->dataOctet(3-1)) << std::setfill(' ') << std::setw(0) << std::nouppercase << std::dec << ",";
        }
        else
        {
            if (at_ptrFrame->dataOctet(2-1))
                out << " Lock timeout in ms: " << ((uint16_t(at_ptrFrame->dataOctet(5+1-1)<<8)) | (uint16_t(at_ptrFrame->dataOctet(5-1)))) << ",";
            i8_uidBytePos = 3;
        } 
        break;

    case 0xFE: out << "VT Status Message: Active WS-Master's SA: $"; out << std::hex << uint16_t(at_ptrFrame->dataOctet(2-1)); break;
    case 0xFF: out << "Working Set Maintenance"; break;
  }
  if (i8_uidBytePos > 0)
  {
    if ((unsigned)i8_uidBytePos < at_ptrFrame->dataSize())
      out << std::dec << str_uidBytePos << ((uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos + 1 - 1) << 8)) | (uint16_t(at_ptrFrame->dataOctet(i8_uidBytePos - 1))));
    else
      out << "*** ERROR: Command too short!!! ***";
  }

  return out.str();
}

bool auxDirectionToVt( uint8_t auxByte )
{
  switch( auxByte )
  {
    case 0x20: return false; // "Auxiliary Assignment Type 1";
    case 0x21: return false; // "Auxiliary Input Type 1 status"; break;
    
    case 0x22: return true;  // "Preferred Assignment"; break;
    case 0x23: return true;  // "Input Type 2 Maintenance"; i8_uidBytePos = 2; strcpy( str_uidBytePos, " MIC=" ); break;
    case 0x24: return false; // "Assignment Type 2"; i8_uidBytePos = 13; break;
    case 0x25: return false; // "Input Status Type 2 Enable"; i8_ui8BytePos = 2; break;
    case 0x26: return false; // "Input Type 2 Status message"; i8_uidBytePos = 2; break;
    case 0x27: return true;  // "Capabilities"; break;
    default:   return true;
  }
}

std::string
interpretePgnsVtFromEcu( PtrDataFrame_t at_ptrFrame )
{
  const uint8_t gfByte = at_ptrFrame->dataOctet(0);
  bool const cb_response = ((gfByte >= 0x20) && (gfByte <= 0x2F)) ? !auxDirectionToVt(gfByte) : (gfByte < 0x10);

  std::string result(
    partiallyInterpretePgnsVtEcu(at_ptrFrame, cb_response) );

  if (cb_response)
    result += " Response";

  if( gs_main.mb_storeIop && ( at_ptrFrame->data()[0] == 0x12 ) ) {
    iopStore( at_ptrFrame->sourceAddress() );
  }

  return result;
}


std::string
interpretePgnsVtToEcu( PtrDataFrame_t at_ptrFrame )
{
  const uint8_t gfByte = at_ptrFrame->dataOctet(0);
  bool const cb_response = ((gfByte >= 0x20) && (gfByte <= 0x2F)) ? auxDirectionToVt(gfByte) : ( (gfByte >=0x10) && (gfByte < 0xF0) );

  std::string result(
    partiallyInterpretePgnsVtEcu(at_ptrFrame, cb_response) );

  if (cb_response)
    result += " Response";

  return result;
}
