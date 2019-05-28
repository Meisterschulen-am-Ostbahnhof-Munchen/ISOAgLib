
std::string
interpreteTimeDate(PtrDataFrame_t at_ptrFrame)
{
    std::ostringstream out;
    const int8_t i8HourOffset = static_cast<int8_t>(at_ptrFrame->dataOctet(7)) -125;
    bool boApplyLocalOffsets = false;
    bool boPrintTimeAndDate = true;

    if (i8HourOffset < -23)
        out << "Time Standard Unknown";
    else if (i8HourOffset > -22 && i8HourOffset < 24)
    {
        out << "UTC Time/Date, Local Time Offsets";
        boApplyLocalOffsets = true;
    }
    else if (i8HourOffset > 23 && i8HourOffset < 124)
        out << "Time standard Unknown";
    else if (i8HourOffset == 124)
        out << "UTC Time/Date, no Offset provided";
    else if (i8HourOffset == 125)
        out << "Local Time/Date";
    else if (i8HourOffset > 125)
        out << "Time Standard Unknown";

    uint16_t u16Years, u16Months, u16Days, u16Hours, u16Minutes, u16Seconds; //uint16 for better text output

    //interprete the Time/Date Format
    u16Years = at_ptrFrame->dataOctet(5) + 1985;
    u16Months = at_ptrFrame->dataOctet(3);
    u16Days   = at_ptrFrame->dataOctet(4) / 4;   //skip the 0.25 day parts
    u16Hours = at_ptrFrame->dataOctet(2);
    u16Minutes = at_ptrFrame->dataOctet(1);
    u16Seconds = at_ptrFrame->dataOctet(0) / 4;

    if (boApplyLocalOffsets)
    {
        //ignore the minute offset
        //apply hours offset:
        if (i8HourOffset > 0 || i8HourOffset + static_cast<int8_t>(u16Hours) >= 0)
            u16Hours = u16Hours + i8HourOffset;
        else //this offset leads to negative hours -> go 1 day back and adjust hours according to previous day (24 -x), if previous day is first of month, go back to month before, the same applies for the year => proper and clean interpretation to complicated for logalizer purpose? -> keep this simplified version
            out << " | Offset not handled yet! Time/Date without offset";
    }

    if (u16Hours   > 24 ||
        u16Minutes > 59 ||
        u16Seconds > 59)
        boPrintTimeAndDate = false; //invalid format

    if (u16Days   > 31 ||
        u16Months > 12)
        boPrintTimeAndDate = false; //invalid format

    if (boPrintTimeAndDate)
    {
        out << " -> " << u16Days << "." << u16Months << "." << u16Years << "   ";
        out << std::setfill('0') << std::right;
        out << std::setw(2) << u16Hours << ":" << std::setw(2) << u16Minutes << ":" << std::setw(2) << std::setfill('0') << u16Seconds;     //skip the ms
        out << std::left;
    }
    else
        out << " -> Error in Time/Date Format!";

    return out.str();
}
