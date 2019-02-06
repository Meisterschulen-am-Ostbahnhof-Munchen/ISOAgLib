// functionality_tim.inc
std::string
partiallyInterpreteTimServerStatus(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Tim Server Status | HB["; //Heartbeat
    out << std::setfill(' ') << std::right << std::setw(3) << std::dec << uint16_t(a_ptrFrame->dataOctet(1));
    out << "] [";
    switch (a_ptrFrame->dataOctet(2) >> 6)
    {
    case 0: out << "RE"; break; //Reserved
    case 1: out << "MA"; break; //Master
    case 2: out << "ER"; break; //Error
    case 3: out << "NA"; break; //Not available
    default: out << "  "; break;
    }

    out << "] OpAck[";  //Operator Acknowledgement
    switch ((a_ptrFrame->dataOctet(2) >> 4) & 0x3)
    {
    case 0: out << "NACK"; break; //Not acknowledged
    case 1: out << "ACK "; break; //Acknowledged
    case 2: out << "ERR "; break; //Error
    case 3: out << "NA  "; break; //Not available
    default: out << "    "; break;
    }

    out << "] SysSt[";  //System state
    switch (a_ptrFrame->dataOctet(2) & 0xF)
    {
    case 0x0: out << "NAU"; break; //No automation active
    case 0x1: out << "AUT"; break; //Automation active
    case 0xE: out << "ERR"; break; //Error
    case 0xF: out << "NA "; break; //Not available
    default:  out << "RES"; break; //Reserved
    }

    out << "] SrvSt[";  //Server state
    switch ((a_ptrFrame->dataOctet(3) >> 4))
    {
    case 0x0: out << "AutNA"; break; //Automation unavailable
    case 0x1: out << "AutNR"; break; //Automation not ready
    case 0x3: out << "AutEN"; break; //Automation enabled
    case 0x4: out << "Pend "; break; //Pending
    case 0x5: out << "AutAC"; break; //Automation active
    case 0xE: out << "ERR  "; break; //Error
    case 0xF: out << "NA   "; break; //Not available
    default:  out << "RES  "; break; //Reserved
    }

    out << "] SysStdStl[";  //System stand still
    switch ((a_ptrFrame->dataOctet(3) >> 2) & 0x3)
    {
    case 0: out << "NStl"; break; //Not standstill
    case 1: out << "Stl "; break; //Standstill
    case 2: out << "ERR "; break; //Error
    case 3: out << "NA  "; break; //Not available
    default: out << "    ";break;
    }

    out << "] SysStatOp[";  //System stationary operation
    switch (a_ptrFrame->dataOctet(3) & 0x3)
    {
    case 0: out << "NStaOp"; break; //No stationary Operation
    case 1: out << "StatOp"; break; //Stationary operation
    case 2: out << "ERR   "; break; //Error
    case 3: out << "NA    "; break; //Not available
    default: out << "      "; break;
    }
    out << "]";

    return (out.str());
}

std::string
partiallyInterpreteTimClientStatus(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Tim Client Status | HB["; //Heartbeat
    out << std::setfill(' ') << std::right << std::setw(3) << std::dec << uint16_t(a_ptrFrame->dataOctet(1));
    out << "] ClSt[";
    switch ((a_ptrFrame->dataOctet(2) >> 4))
    {
    case 0x0: out << "AutNA"; break; //Automation unavailable
    case 0x1: out << "AutNR"; break; //Automation not ready
    case 0x2: out << "AutRE"; break; // Automation ready to enable
    case 0x3: out << "AutEN"; break; //Pending
    case 0x5: out << "AutAc"; break; //Automation active
    case 0xE: out << "ERR  "; break; //Error
    case 0xF: out << "NA   "; break; //Not available
    default:  out << "RES  "; break; //Reserved
    }
    out << "]";

    return (out.str());
}

std::string
partiallyInterpreteConnectionVersionResponse(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Connection version response; Error code: ";
    switch (a_ptrFrame->dataOctet(1))
    {
    case 0x0: out << "No error; ";                                             break;
    case 0x1: out << "TIM server and TIM client versions are incompatible; ";  break;
    default:  out << "Reserved; ";                                             break;
    }
    out << "Connection version: ";
    switch (a_ptrFrame->dataOctet(2))
    {
    case 0xFB:
    case 0xFC:
    case 0xFD:
    case 0xFE: out << "Reserved";                                             break;
    case 0xFF: out << "TIM server and TIM client versions are incompatible";  break;
    default:  out << std::dec << uint16_t(a_ptrFrame->dataOctet(2));          break;
    }
    return (out.str());
}

std::string
interpreteFacility(uint8_t a_facility)
{
    std::ostringstream out;
    switch (a_facility & 0x3)
    {
    case 0x0: out << "Not supported.";  break;
    case 0x1: out << "Supported.";      break;
    case 0x2: out << "Error.";          break;
    case 0x3: out << "Not available.";  break;
    default:                            break;
    }
    return (out.str());
}

//// TODO: improve output here + replace tabs with setw()!
//std::string
//functionSupportResponse(PtrDataFrame_t a_ptrFrame)
//{
//    std::ostringstream out;
//    out << std::left << std::setfill(' ') << std::setw(113) << " " << "Function support response. Number of supported functions: " << std::dec << uint16_t(a_ptrFrame->data()[1]);
//    uint8_t functionNumber = 1;
//    for (uint8_t idx = 2; idx < a_ptrFrame->dataSize(); )
//    {
//        out << "\n";
//        out << std::left << std::setfill(' ') << std::setw(116) << " " << "Function " << std::dec << uint16_t(functionNumber) << ": ";
//        switch (a_ptrFrame->data()[idx])
//        {
//        case 0x00:
//        case 0x01:
//        case 0x02:
//        case 0x03:
//        case 0x04:
//        case 0x05:
//        case 0x06:
//        case 0x07:
//        case 0x08:
//        case 0x09:
//        case 0x0A:
//        case 0x0B:
//        case 0x0C:
//        case 0x0D:
//        case 0x0E:
//        case 0x0F:
//        case 0x10:
//        case 0x11:
//        case 0x12:
//        case 0x13:
//        case 0x14:
//        case 0x15:
//        case 0x16:
//        case 0x17:
//        case 0x18:
//        case 0x19:
//        case 0x1A:
//        case 0x1B:
//        case 0x1C:
//        case 0x1D:
//        case 0x1E:
//        case 0x1F:
//        case 0x20:
//        {
//            out << "Auxiliary valve " << std::dec << uint16_t(a_ptrFrame->data()[idx]);
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) <<"\n";
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << std::left << std::setfill(' ') << std::setw(120) << " " << "Valve State: ";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3) << "\n";
//                out << std::left << std::setfill(' ') << std::setw(120) << " " << "Valve Flow: ";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//            }
//        }
//        break;
//        case 0x40:
//        {
//            out << "Front PTO";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Disengagement:\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Engagement:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Speed counter clockwise:\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 2) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO speed clockwise:\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx]) & 0x3);
//            }
//        }
//        break;
//        case 0x41:
//        {
//            out << "Rear PTO";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Disengagement:\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Engagement:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO Speed counter clockwise:\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 2) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tPTO speed clockwise:\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx]) & 0x3);
//            }
//        }
//        break;
//        case 0x42:
//        {
//            out << "Front Hitch";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tHitch Motion:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tHitch Position:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//            }
//        }
//        break;
//        case 0x43:
//        {
//            out << "Rear Hitch";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tHitch Motion:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tHitch Position:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//            }
//        }
//        break;
//        case 0x44:
//        {
//            out << "Vehicle Speed";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tVehicle speed forward:\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tVehicle speed reverse:\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tStart vehicle:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 2) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tStop vehicle:\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx]) & 0x3);
//            }
//            if (neededBytes >= 2)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tForward direction:\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tReverse direction:\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 4) & 0x3);
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tChange direction:\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 2) & 0x3);
//            }
//        }
//        break;
//        case 0x46:
//        {
//            out << "External Guidance";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            out << "\tNeeded Bytes: " << std::dec << uint16_t(neededBytes);
//            if (neededBytes >= 1)
//            {
//                ++idx;
//                out << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCurvature:\t\t\t\t\t\t";
//                out << interpreteFacility((a_ptrFrame->data()[idx] >> 6) & 0x3);
//            }
//        }
//        break;
//        default:
//            out << "Unknown Function ID";
//            ++idx;
//            uint8_t neededBytes = a_ptrFrame->data()[idx];
//            idx += neededBytes;
//            break;
//        }
//        ++idx;
//        ++functionNumber;
//    }
//    return (out.str());
//}
//

struct TSASupportedFunctions {
    uint16_t sourceAddress; //uint16 for easier cout output
    std::vector<uint8_t> supportedFunctionsDataBytes;
};

std::vector<TSASupportedFunctions> vecListOfSupportedFunctions;

std::string
functionSupportResponse(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    TSASupportedFunctions SaAndFunctions;
    out << std::left << std::setfill(' ') << "Function support response. Number of supported functions: " << std::dec << uint16_t(a_ptrFrame->data()[1]);
    SaAndFunctions.sourceAddress = a_ptrFrame->sourceAddress();
    SaAndFunctions.supportedFunctionsDataBytes = a_ptrFrame->data();

    //check if there is already data stored for this SA
    for (uint8_t i = 0; i < vecListOfSupportedFunctions.size(); i++)
    {
        if (vecListOfSupportedFunctions[i].sourceAddress == a_ptrFrame->sourceAddress())
        {
            if (vecListOfSupportedFunctions[i].supportedFunctionsDataBytes == a_ptrFrame->data())
            {
                return (out.str());
            }
        }
    }
    //store data in list to print out summary at end of file
    vecListOfSupportedFunctions.push_back(SaAndFunctions);

    return (out.str());
}


void printFullListOfSupportedFunctions(std::ostream& out)
{
    using std::hex;
    out << "\nList of Source Addresses and their supported TIM functions: \n\n";
    out << std::left << std::setfill(' ') << std::setw(10) << "SA:" << "Function List:\n";
    for (uint8_t i = 0; i < vecListOfSupportedFunctions.size(); ++i)
    {
        uint8_t functionNumber = 1;
        out << std::hex << vecListOfSupportedFunctions[i].sourceAddress;
        for (uint8_t idx = 2; idx < vecListOfSupportedFunctions[i].supportedFunctionsDataBytes.size(); )
        {
            if (idx == 2)
            {
                out << std::left << std::setfill(' ') << std::setw(8) << " ";
            }
            else
            {
                out << std::left << std::setfill(' ') << std::setw(10) << " ";
            }
            out << "Function " << std::dec << uint16_t(functionNumber) << ": ";
            switch (vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx])
            {
            case 0x00:
            {
                out << "Auxiliary valve Superset";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Valve State: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Valve Flow: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3);
                }
            }
            break;
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            {
                out << "Auxiliary valve " << std::dec << uint16_t(vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx]);
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Valve State: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Valve Flow: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3);
                }
            }
            break;
            case 0x40:
            {
                out << "Front PTO";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Disengagement: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Engagement counter-clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Engagement clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 2) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Speed counter clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx]) & 0x3);
                }
                if (neededBytes >= 2)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO speed clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3);

                }
            }
            break;
            case 0x41:
            {
                out << "Rear PTO";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Disengagement: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Engagement counter-clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Engagement clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 2) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO Speed counter clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx]) & 0x3);
                }
                if (neededBytes >= 2)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "PTO speed clockwise: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3);

                }
            }
            break;
            case 0x42:
            {
                out << "Front Hitch";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Hitch Motion: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Hitch Position: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3);
                }
            }
            break;
            case 0x43:
            {
                out << "Rear Hitch";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Hitch Motion: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Hitch Position: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3);
                }
            }
            break;
            case 0x44:
            {
                out << "Vehicle Speed";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Vehicle speed forward: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Vehicle speed reverse: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Start vehicle: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 2) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Stop vehicle: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx]) & 0x3);
                }
                if (neededBytes >= 2)
                {
                    ++idx;
                    out << "\n";
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Forward direction: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Reverse direction: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 4) & 0x3) << "\n";

                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Change direction: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 2) & 0x3);
                }
            }
            break;
            case 0x46:
            {
                out << "External Guidance";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                out << " Needed Bytes: " << std::dec << uint16_t(neededBytes) << "\n";
                if (neededBytes >= 1)
                {
                    ++idx;
                    out << std::left << std::setfill(' ') << std::setw(14) << " " << std::setw(34) << "Curvature: ";
                    out << interpreteFacility((vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx] >> 6) & 0x3);
                }
            }
            break;
            default:
                out << "Reserved";
                ++idx;
                uint8_t neededBytes = vecListOfSupportedFunctions[i].supportedFunctionsDataBytes[idx];
                idx += neededBytes;
                break;
            }
            ++idx;
            ++functionNumber;
            out << "\n";
        }
        out << "\n";
    }
}

std::string
partiallyInterpreteTimServerAuthenticationStatus(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Tim Server Authentication Status\n";
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Error Code : " << std::setw(2) << std::setfill(' ') << std::dec << uint16_t(a_ptrFrame->dataOctet(1)) << "\n";

    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Type: ";
    switch (a_ptrFrame->dataOctet(2) >> 4)
    {
    case 0x0: out << "TIM Authentication\n";  break;
    case 0xE: out << "Error\n";               break;
    case 0xF: out << "Not available\n";       break;
    default:  out << "Reserved\n";            break;
    }
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Status: ";
    switch (a_ptrFrame->dataOctet(2) & 0xF)
    {
    case 0x0: out << "Not authenticated\n"; break;
    case 0x1: out << "Authenticated\n";     break;
    case 0xE: out << "Error\n";             break;
    case 0xF: out << "Not available\n";     break;
    default:  out << "Reserved\n";          break;
    }
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Substatus:\n";
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "(Re)Start authentication: " <<             (a_ptrFrame->dataOctet(3) & 0x80 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "LwA possible: " <<                         (a_ptrFrame->dataOctet(3) & 0x40 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Testlab Certificate valid: " <<            (a_ptrFrame->dataOctet(3) & 0x20 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Manufacturer Certificate valid: " <<       (a_ptrFrame->dataOctet(3) & 0x10 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Manufacturer Series Certificate valid: " << (a_ptrFrame->dataOctet(3) & 0x8 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Device Certificate valid: " <<              (a_ptrFrame->dataOctet(3) & 0x4 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Challenge signed: " <<                      (a_ptrFrame->dataOctet(3) & 0x2 ? "Yes"   : "No");
    return (out.str());
}

std::string
partiallyInterpreteTimClientAuthenticationStatus(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Tim Client Authentication Status\n";
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Error Code : " << std::setw(2) << std::setfill(' ') << std::dec << uint16_t(a_ptrFrame->dataOctet(1)) << "\n";
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Type: ";

    switch (a_ptrFrame->dataOctet(2) >> 4)
    {
    case 0x0: out << "TIM Authentication\n";  break;
    case 0xE: out << "Error\n";               break;
    case 0xF: out << "Not available\n";       break;
    default:  out << "Reserved\n";            break;
    }
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Status: ";
    switch (a_ptrFrame->dataOctet(2) & 0xF)
    {
    case 0x0: out << "Not authenticated\n"; break;
    case 0x1: out << "Authenticated\n";     break;
    case 0xE: out << "Error\n";             break;
    case 0xF: out << "Not available\n";     break;
    default:  out << "Reserved\n";          break;
    }
    out << std::setfill(' ') << std::left << std::setw(116) << " " << "Authentication Substatus:\n";
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "(Re)Start authentication: " <<              (a_ptrFrame->dataOctet(3) & 0x80 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "LwA possible: " <<                          (a_ptrFrame->dataOctet(3) & 0x40 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Testlab Certificate valid: " <<             (a_ptrFrame->dataOctet(3) & 0x20 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Manufacturer Certificate valid: " <<        (a_ptrFrame->dataOctet(3) & 0x10 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Manufacturer Series Certificate valid: " <<  (a_ptrFrame->dataOctet(3) & 0x8 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Device Certificate valid: " <<               (a_ptrFrame->dataOctet(3) & 0x4 ? "Yes\n" : "No\n");
    out << std::setfill(' ') << std::left << std::setw(120) << " " << std::setw(40) << "Challenge signed: " <<                       (a_ptrFrame->dataOctet(3) & 0x2 ? "Yes"   : "No");
    return (out.str());
}

// noch nicht getestet
std::string
CertificateRequest(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << " Certificate Request - ";
    switch (a_ptrFrame->dataOctet(4))
    {
    case 0x0: out << "Root Certificate";                break;
    case 0x1: out << "Testlab Certificate";             break;
    case 0x2: out << "Manufacturer Certificate";        break;
    case 0x3: out << "Manufacturer Series Certificate"; break;
    case 0x4: out << "Device Certificate";              break;
    default:  out << "Unknown Certificate";             break;
    }
    return (out.str());
}

std::string
interpreteAuxValveID(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    out << "Auxiliary valve " << std::dec << uint16_t(a_ptrFrame->dataOctet(0)) << " flow request: ";
    return (out.str());
}

// noch nicht getestet
std::string
interpreteAuxValveCommand(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    uint16_t slotInfo = a_ptrFrame->dataOctet(5) << 8 | a_ptrFrame->dataOctet(4);
    if (slotInfo == 0x0000)
    {
        out << "Valve fully opened (Extend)";
    }
    else if (slotInfo >= 0x0001 && slotInfo <= 0x7D7F)
    {
        out << "Extend flow at " << std::dec << uint16_t((0x7D80 - slotInfo) * 0.004) << " %";
    }
    else if (slotInfo == 0x7D80)
    {
        out << "Valve blocked";
    }
    else if (slotInfo >= 0x7D81 && slotInfo <= 0xFAFF)
    {
        out << "Retract flow at " << std::dec << uint16_t((slotInfo - 0x7D80) * 0.004) << " %";
    }
    else if (slotInfo == 0xFB00)
    {
        out << "Valve fully opened (Retract)";
    }
    else if (slotInfo == 0xFB01)
    {
        out << "Float";
    }
    else if (slotInfo == 0xFBFD)
    {
        out << "Release control";
    }
    else if (slotInfo == 0xFBFF)
    {
        out << "Ready to control";
    }
    else
    {
        out << "Reserved";
    }
    return (out.str());
}

std::string
interpretePTOCommand(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    uint16_t slotInfo = a_ptrFrame->dataOctet(5) << 8 | a_ptrFrame->dataOctet(4);
    if (slotInfo == 0x0000)
    {
        out << "Speed counter clockwise set by TIM Server";
    }
    else if (slotInfo >= 0x0001 && slotInfo <= 0x7D7F)
    {
        out << "Rotate counter clockwise with a speed of " << std::dec << uint16_t((0x7D80 - slotInfo) * 0.125) << " 1/min";
    }
    else if (slotInfo == 0x7D80)
    {
        out << "Off";
    }
    else if (slotInfo >= 0x7D81 && slotInfo <= 0xFAFF)
    {
        out << "Rotate clockwise with a speed of " << std::dec << uint16_t((slotInfo - 0x7D80) * 0.125) << " 1/min";
    }
    else if (slotInfo == 0xFB00)
    {
        out << "Speed clockwise set by TIM Server";
    }
    else if (slotInfo == 0xFBFD)
    {
        out << "Release control";
    }
    else if (slotInfo == 0xFBFF)
    {
        out << "Ready to control";
    }
    else
    {
        out << "Reserved";
    }
    return (out.str());
}

std::string
interpreteHitchCommand(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    uint16_t slotInfo = a_ptrFrame->dataOctet(5) << 8 | a_ptrFrame->dataOctet(4);
    if (slotInfo == 0x000)
    {
        out << "Full lower";
    }
    else if (slotInfo >= 0x0001 && slotInfo <= 0x270F)
    {
        out << "Raise up to " << std::dec << uint16_t(slotInfo / 100) << "." << uint16_t(slotInfo % 100) << " %";
    }
    else if (slotInfo == 0x2710)
    {
        out << "Full Raise";
    }
    else if (slotInfo == 0xFB00)
    {
        out << "Float";
    }
    else if (slotInfo == 0xFBFD)
    {
        out << "Release control";
    }
    else if (slotInfo == 0xFBFF)
    {
        out << "Ready to control";
    }
    else
    {
        out << "Reserved";
    }
    return (out.str());
}

std::string
interpreteVehicleSpeedCommand(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    uint16_t slotInfo = a_ptrFrame->dataOctet(5) << 8 | a_ptrFrame->dataOctet(4);
    if (slotInfo == 0x0000)
    {
        out << "Reverse speed set by TIM server";
    }
    else if (slotInfo >= 0x0001 && slotInfo <= 0x7D7F)
    {
        out << "Reverse with a speed of " << std::dec << uint16_t((0x7D80 - slotInfo) * 0.001) << " m/s = " << uint16_t((0x7D80 - slotInfo) * 0.0036) << " km/h";
    }
    else if (slotInfo == 0x7D80)
    {
        out << "Stop";
    }
    else if (slotInfo >= 0x7D81 && slotInfo <= 0xFAFF)
    {
        out << "Forward with a speed of " << std::dec << uint16_t((slotInfo - 0x7D80) * 0.001) << " m/s = " << uint16_t((slotInfo - 0x7D80) * 0.0036) << " km/h";
    }
    else if (slotInfo == 0xFB00)
    {
        out << "Forward speed set by TIM server";
    }
    else if (slotInfo == 0xFBFD)
    {
        out << "Release control requesting operator awareness";
    }
    else if (slotInfo == 0xFBFE)
    {
        out << "Release control without operator awareness";
    }
    else if (slotInfo == 0xFBFF)
    {
        out << "Ready to control";
    }
    else
    {
        out << "Reserved";
    }
    return (out.str());
}

std::string
interpreteExternalGuidanceCommand(PtrDataFrame_t a_ptrFrame)
{
    std::ostringstream out;
    uint16_t slotInfo = (a_ptrFrame->dataOctet(5) << 8) | a_ptrFrame->dataOctet(4);
    if (slotInfo >= 0x0000 && slotInfo <= 0x7D7F)
    {
        // Left
        out << "Turning left with " << std::dec << uint16_t((0x7D80 - slotInfo) / 4) << " 1/km ==> radius = " << uint16_t(4000 / (0x7D80 - slotInfo)) << " m";
    }
    else if (slotInfo == 0x7D80)
    {
        // Straight
        out << "Straight";
    }
    else if (slotInfo >= 0x7D81 && slotInfo <= 0xFAFF)
    {
        // Right
        out << "Turning right with " << std::dec << uint16_t((slotInfo - 0x7D80) / 4) << " 1/km ==> radius = " << uint16_t(4000 / (slotInfo - 0x7D80)) << " m";
    }
    else if (slotInfo == 0xFBFD)
    {
        // Release Control
        out << "Release control";
    }
    else if (slotInfo == 0xFBFF)
    {
        out << "Ready to control";
    }
    else
    {
        // Reserved
        out << "Reserved";
    }
    return (out.str());
}

std::string
interpretePgnsTimServerToClient(PtrDataFrame_t a_ptrFrame)
{
    std::string result(" - ");
    switch (a_ptrFrame->dataOctet(0))
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
        result.append(interpreteAuxValveID(a_ptrFrame));
        result.append(" flow status:  ");
        result.append(interpreteAuxValveCommand(a_ptrFrame));
        break;
    case 0x40:
        result.append("Front PTO Status:  ");
        result.append(interpretePTOCommand(a_ptrFrame));
        break;
    case 0x41:
        result.append("Rear PTO Status:  ");
        result.append(interpretePTOCommand(a_ptrFrame));
        break;
    case 0x42:
        result.append("Front Hitch Status:  ");
        result.append(interpreteHitchCommand(a_ptrFrame));
        break;
    case 0x43:
        result.append("Rear Hitch Status:  ");
        result.append(interpreteHitchCommand(a_ptrFrame));
        break;
    case 0x44:
        result.append("Vehicle Speed Status:  ");
        result.append(interpreteVehicleSpeedCommand(a_ptrFrame));
        break;
    case 0x46:
        result.append("External Guidance Status:  ");
        result.append(interpreteExternalGuidanceCommand(a_ptrFrame));
        break;
    case 0xF3:
        result.append("Function assignment status response.");
        break;
    case 0xF4:
        result.append(functionSupportResponse(a_ptrFrame));
        break;
    case 0xF5:
        result.append("Function assignment response");
        break;
    case 0xF6:
        result.append(partiallyInterpreteConnectionVersionResponse(a_ptrFrame));
        break;
    case 0xF7:
        result.append("Client version request");
        break;
    case 0xF8:
        result.append("Server version response");
        break;
    case 0xFA:
        result.append(partiallyInterpreteTimServerStatus(a_ptrFrame));
        break;
    default:
        result.append("Unknown command. Upgrade logalizer in interpretePgnsTimServerToClient()");
        break;
    }

    return result;
}

std::string
interpretePgnsTimClientToServer(PtrDataFrame_t a_ptrFrame)
{
    std::string result(" - ");
    switch (a_ptrFrame->dataOctet(0))
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
        result.append(interpreteAuxValveID(a_ptrFrame));
        result.append(" flow request: ");
        result.append(interpreteAuxValveCommand(a_ptrFrame));
        break;
    case 0x40:
        result.append("Front PTO Request: ");
        result.append(interpretePTOCommand(a_ptrFrame));
        break;
    case 0x41:
        result.append("Rear PTO Request: ");
        result.append(interpretePTOCommand(a_ptrFrame));
        break;
    case 0x42:
        result.append("Front Hitch Request: ");
        result.append(interpreteHitchCommand(a_ptrFrame));
        break;
    case 0x43:
        result.append("Rear Hitch Request: ");
        result.append(interpreteHitchCommand(a_ptrFrame));
        break;
    case 0x44:
        result.append("Vehicle Speed Request: ");
        result.append(interpreteVehicleSpeedCommand(a_ptrFrame));
        break;
    case 0x46:
        result.append("External Guidance Request: ");
        result.append(interpreteExternalGuidanceCommand(a_ptrFrame));
        break;
    case 0xF3:
        result.append("Function assignment status request");
        break;
    case 0xF4:
        result.append("Function support request");
        break;
    case 0xF5:
        result.append("Function assignment request");
        break;
    case 0xF6:
        result.append("Connection version request");
        break;
    case 0xF7:
        result.append("Client version request");
        break;
    case 0xF8:
        result.append("Server version request");
        break;
    case 0xF9:
        result.append(partiallyInterpreteTimClientStatus(a_ptrFrame));
        break;
    default:
        result.append("Unknown command. Upgrade logalizer in interpretePgnsTimClientToServer()");
        break;
    }

    return result;
}

std::string
interpreteTimAuthenticationServerToClient(PtrDataFrame_t a_ptrFrame)
{
    std::string result(" - ");
    switch (a_ptrFrame->dataOctet(0))
    {
    case 0x00:
        result.append("Server stored certificates (bitmask) response.");
        break;
    case 0x01:
        result.append("Client stored certificates (bitmask) request.");
        break;
    case 0x02:
        result.append("Server certificate response.");
        break;
    case 0x03:
        result.append("Client ");
        result.append(CertificateRequest(a_ptrFrame));
        break;
    case 0x04:
        result.append("Server random challenge response.");
        break;
    case 0x05:
        result.append("Client random challenge request.");
        break;
    case 0x06:
        result.append("Server signed challenge response.");
        break;
    case 0x07:
        result.append("Client signed challenge request.");
        break;
    case 0x08:
        result.append("Server authentication info response.");
        break;
    case 0x09:
        result.append("Client authentication info request.");
        break;
    case 0xF9:
        result.append(partiallyInterpreteTimServerAuthenticationStatus(a_ptrFrame));
        break;
    default:
        break;
    }

    return result;
}

std::string
interpreteTimAuthenticationClientToServer(PtrDataFrame_t a_ptrFrame)
{
    std::string result(" - ");
    switch (a_ptrFrame->dataOctet(0))
    {
    case 0x00:
        result.append("Server stored certificates (bitmask) request.");
        break;
    case 0x01:
        result.append("Client stored certificates (bitmask) response.");
        break;
    case 0x02:
        result.append("Server ");
        result.append(CertificateRequest(a_ptrFrame));
        break;
    case 0x03:
        result.append("Client certificate response.");
        break;
    case 0x04:
        result.append("Server random challenge request.");
        break;
    case 0x05:
        result.append("Client random challenge response.");
        break;
    case 0x06:
        result.append("Server signed challenge request.");
        break;
    case 0x07:
        result.append("Client signed challenge response.");
        break;
    case 0x08:
        result.append("Server authentication info request.");
        break;
    case 0x09:
        result.append("Client authentication info response.");
        break;
    case 0xFA:
        result.append(partiallyInterpreteTimClientAuthenticationStatus(a_ptrFrame));
        break;
    default:
        break;
    }

    return result;
}

