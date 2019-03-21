//filled up till entry 557 (= "test")
const char* ddis[] = {
"Data Dictionary Version",
"Setpoint Volume Per Area Application Rate",
"Actual Volume Per Area Application Rate",
"Default Volume Per Area Application Rate",
"Minimum Volume Per Area Application Rate",
"Maximum Volume Per Area Application Rate",
"Setpoint Mass Per Area Application Rate",
"Actual Mass Per Area Application Rate",
"Default Mass Per Area Application Rate",
"Minimum Mass Per Area Application Rate",
"Maximum Mass Per Area Application Rate",
"Setpoint Count Per Area Application Rate",
"Actual Count Per Area Application Rate",
"Default Count Per Area Application Rate",
"Minimum Count Per Area Application Rate",
"Maximum Count Per Area Application Rate",
"Setpoint Spacing Application Rate",
"Actual Spacing Application Rate",
"Default Spacing Application Rate",
"Minimum Spacing Application Rate",
"Maximum Spacing Application Rate",
"Setpoint Volume Per Volume Application Rate",
"Actual Volume Per Volume Application Rate",
"Default Volume Per Volume Application Rate",
"Minimum Volume Per Volume Application Rate",
"Maximum Volume Per Volume Application Rate",
"Setpoint Mass Per Mass Application Rate",
"Actual Mass Per Mass Application Rate",
"Default Mass Per Mass Application Rate",
"Minimum Mass Per Mass Application Rate",
"MaximumMass Per Mass Application Rate",
"Setpoint Volume Per Mass Application Rate",
"Actual Volume Per Mass Application Rate",
"Default Volume Per Mass Application Rate",
"Minimum Volume Per Mass Application Rate",
"Maximum Volume Per Mass Application Rate",
"Setpoint Volume Per Time Application Rate",
"Actual Volume Per Time Application Rate",
"Default Volume Per Time Application Rate",
"Minimum Volume Per Time Application Rate",
"Maximum Volume Per Time Application Rate",
"Setpoint Mass Per Time Application Rate",
"Actual Mass Per Time Application Rate",
"Default Mass Per Time Application Rate",
"Minimum Mass Per Time Application Rate",
"Maximum Mass Per Time Application Rate",
"Setpoint Count Per Time Application Rate",
"Actual Count Per Time Application Rate",
"Default Count Per Time Application Rate",
"Minimum Count Per Time Application Rate",
"Maximum Count Per Time Application Rate",
"Setpoint Tillage Depth",
"Actual Tillage Depth",
"Default Tillage Depth",
"Minimum Tillage Depth",
"Maximum Tillage Depth",
"Setpoint Seeding Depth",
"Actual Seeding Depth",
"Default Seeding Depth",
"Minimum Seeding Depth",
"Maximum Seeding Depth",
"Setpoint Working Height",
"Actual Working Height",
"Default Working Height",
"Minimum Working Height",
"Maximum Working Height",
"Setpoint Working Width",
"Actual Working Width",
"Default Working Width",
"Minimum Working Width",
"Maximum Working Width",
"Setpoint Volume Content",
"Actual Volume Content",
"Maximum Volume Content",
"Setpoint Mass Content",
"Actual Mass Content",
"Maximum Mass Content",
"Setpoint Count Content",
"Actual Count Content",
"Maximum Count Content",
"Application Total Volume in [L]",
"Application Total Mass in [kg]",
"Application Total Count",
"Volume Per Area Yield",
"Mass Per Area Yield",
"Count Per Area Yield",
"Volume Per Time Yield",
"Mass Per Time Yield",
"Count Per Time Yield",
"Yield Total Volume",
"Yield Total Mass",
"Yield Total Count",
"Volume Per Area Crop Loss",
"Mass Per Area Crop Loss",
"Count Per Area Crop Loss",
"Volume Per Time Crop Loss",
"Mass Per Time Crop Loss",
"Count Per Time Crop Loss",
"Percentage Crop Loss",
"Crop Moisture",
"Crop Contamination",
"Setpoint Bale Width",
"Actual Bale Width",
"Default Bale Width",
"Minimum Bale Width",
"Maximum Bale Width",
"Setpoint Bale Height",
"ActualBaleHeight",
"Default Bale Height",
"Minimum Bale Height",
"Maximum Bale Height",
"Setpoint Bale Size",
"Actual Bale Size",
"Default Bale Size",
"Minimum Bale Size",
"Maximum Bale Size",
"Total Area",
"Effective Total Distance",
"Ineffective Total Distance",
"Effective Total Time",
"Ineffective Total Time",
"Product Density Mass Per Volume",
"Product Density Mass PerCount",
"Product Density Volume Per Count",
"Auxiliary Valve Scaling Extend",
"Auxiliary Valve Scaling Retract",
"Auxiliary Valve Ramp Extend Up",
"Auxiliary Valve Ramp Extend Down",
"Auxiliary Valve Ramp Retract Up",
"Auxiliary Valve Ramp Retract Down",
"Auxiliary Valve Float Threshold",
"Auxiliary Valve Progressivity Extend",
"Auxiliary Valve Progressivity Retract",
"Auxiliary Valve Invert Ports",
"Device Element Offset X",
"Device Element Offset Y",
"Device Element Offset Z",
"Device Volume Capacity",
"Device Mass Capacity",
"Device Count Capacity",
"Setpoint Percentage Application Rate",
"Actual Work State",
"Physical Setpoint Time Latency",
"Physical Actual Value Time Latency",
"Yaw Angle",
"Roll Angle",
"Pitch Angle",
"Log Count",
"Total Fuel Consumption",
"Instantaneous Fuel Consumption per Time",
"Instantaneous Fuel Consumption per Area",
"Instantaneous Area Per Time Capacity",
"152 unused",
"Actual Normalized Difference Vegetative Index (NDVI)",
"Physical Object Length",
"Physical Object Width",
"Physical Object Height",
"Connector Type",
"Prescription Control State",
"Number of Sub-Units per Section",
"Section Control State",
"Actual   Condensed Work State (1-16)   ",  //align lenths (also with setpoint condensed work state ddis)
"Actual   Condensed Work State (17-32)  ",
"Actual   Condensed Work State (33-48)  ",
"Actual   Condensed Work State (49-64)  ",
"Actual   Condensed Work State (65-80)  ",
"Actual   Condensed Work State (81-96)  ",
"Actual   Condensed Work State (97-112) ",
"Actual   Condensed Work State (113-128)",
"Actual   Condensed Work State (129-144)",
"Actual   Condensed Work State (145-160)",
"Actual   Condensed Work State (161-176)",
"Actual   Condensed Work State (177-192)",
"Actual   Condensed Work State (193-208)",
"Actual   Condensed Work State (209-224)",
"Actual   Condensed Work State (225-240)",
"Actual   Condensed Work State (241-256)",
"Actual length of cut",
"Element Type Instance",
"Actual Cultural Practice",
"Device Reference Point (DRP) to Ground distance",
"Dry Mass Per Area Yield",
"Dry Mass Per Time Yield",
"Yield Total Dry Mass",
"Reference Moisture For Dry Mass",
"Seed Cotton Mass Per Area Yield",
"Lint Cotton Mass Per Area Yield",
"Seed Cotton Mass Per Time Yield",
"Lint Cotton Mass Per Time Yield",
"Yield Total Seed Cotton Mass",
"Yield Total Lint Cotton Mass",
"Lint Turnout Percentage ",
"Ambient temperature",
"Setpoint Product Pressure",
"Actual Product Pressure",
"Minimum Product Pressure",
"Maximum Product Pressure",
"Setpoint Pump Output Pressure",
"Actual Pump Output Pressure",
"Minimum Pump Output Pressure",
"Maximum Pump Output Pressure",
"Setpoint Tank Agitation Pressure",
"Actual Tank Agitation Pressure",
"Minimum Tank Agitation Pressure",
"Maximum Tank Agitation Pressure",
"SC Turn On Time",
"SC Turn Off Time",
"Wind speed",
"Wind direction",
"Air Humidity",
"Sky conditions",
"Last Bale Flakes per Bale",
"Last Bale Average Moisture",
"Last Bale Average Strokes per Flake",
"Lifetime Bale Count",
"Lifetime Working Hours",
"Actual Bale Hydraulic Pressure",
"Last Bale Average Hydraulic Pressure",
"Setpoint Bale Compression Plunger Load",
"Actual Bale Compression Plunger Load",
"Last Bale Average Bale Compression Plunger Load",
"Last Bale Applied Preservative",
"Last Bale Tag Number",
"Last Bale Mass",
"Delta T",
"Setpoint Working Length",
"Actual Working Length",
"Minimum Working Length",
"Maximum Working Length",
"Actual Net Weight",
"Net Weight State",
"Setpoint Net Weight",
"Actual Gross Weight",
"Gross Weight State",
"Minimum Gross Weight",
"Maximum Gross Weight",
"Thresher Engagement Total Time",
"Actual Header Working Height Status",
"Actual Header Rotational Speed Status",
"Yield Hold Status",
"Actual (Un)Loading System Status",
"Crop Temperature",
"Setpoint Sieve Clearance",
"Actual Sieve Clearance",
"Minimum Sieve Clearance",
"Maximum Sieve Clearance",
"Setpoint Chaffer Clearance",
"Actual Chaffer Clearance",
"Minimum Chaffer Clearance",
"Maximum Chaffer Clearance",
"Setpoint Concave Clearance",
"Actual Concave Clearance",
"Minimum Concave Clearance",
"Maximum Concave Clearance",
"Setpoint Separation Fan Rotational Speed",
"Actual Separation Fan Rotational Speed",
"Minimum Separation Fan Rotational Speed",
"Maximum Separation Fan Rotational Speed",
"Hydraulic Oil Temperature",
"Yield Lag Ignore Time",
"Yield Lead Ignore Time",
"Average Yield Mass Per Time",
"Average Crop Moisture",
"Average Yield Mass Per Area",
"Connector Pivot X-Offset",
"Remaining Area",
"Lifetime Application Total Mass",
"Lifetime Application Total Count",
"Lifetime Yield Total Volume",
"Lifetime Yield Total Mass",
"Lifetime Yield Total Count",
"Lifetime Total Area",
"Lifetime Effective Total Distance",
"Lifetime Ineffective Total Distance",
"Lifetime Effective Total Time",
"Lifetime Ineffective Total Time",
"Lifetime Fuel Consumption",
"Lifetime Average Fuel Consumption per Time",
"Lifetime Average Fuel Consumption per Area",
"Lifetime Yield Total Dry Mass",
"Lifetime Yield Total Seed Cotton Mass",
"Lifetime Yield Total Lint Cotton Mass",
"Lifetime Threshing Engagement Total Time",
"Precut Total Count",
"Uncut Total Count",
"Lifetime Precut Total Count",
"Lifetime Uncut Total Count",
"Setpoint Prescription Mode",
"Actual Prescription Mode",
"Setpoint Work State",
"Setpoint Condensed Work State (1-16)   ",
"Setpoint Condensed Work State (17-32)  ",
"Setpoint Condensed Work State (33-48)  ",
"Setpoint Condensed Work State (49-64)  ",
"Setpoint Condensed Work State (65-80)  ",
"Setpoint Condensed Work State (81-96)  ",
"Setpoint Condensed Work State (97-112) ",
"Setpoint Condensed Work State (113-128)",
"Setpoint Condensed Work State (129-144)",
"Setpoint Condensed Work State (145-160)",
"Setpoint Condensed Work State (161-176)",
"Setpoint Condensed Work State (177-192)",
"Setpoint Condensed Work State (193-208)",
"Setpoint Condensed Work State (209-224)",
"Setpoint Condensed Work State (225-240)",
"Setpoint Condensed Work State (241-256)",
"True Rotation Point  X-Offset ",
"True Rotation Point Y-Offset",
"Actual Percentage Application Rate",
"Minimum Percentage Application Rate",
"Maximum Percentage Application Rate",
"Relative Yield Potential",
"Minimum Relative Yield Potential",
"Maximum Relative Yield Potential",
"Actual Percentage Crop Dry Matter",
"Average Percentage Crop Dry Matter ",
"Effective Total Fuel Consumption",
"Ineffective Total Fuel Consumption",
"Effective Total Diesel Exhaust Fluid Consumption",
"Ineffective Total Diesel Exhaust Fluid Consumption",
"Last loaded Weight",
"Last unloaded Weight",
"Load Identification Number",
"Unload Identification Number",
"Chopper Engagement Total Time",
"Lifetime Application Total Volume",
"Setpoint Header Speed",
"Actual Header Speed",
"Minimum Header Speed",
"Maximum Header Speed",
"Setpoint Cutting drum speed",
"Actual Cutting drum speed",
"Minimum Cutting drum speed",
"Maximum Cutting drum speed",
"Operating Hours Since Last Sharpening",
"Front PTO hours",
"Rear PTO hours",
"Lifetime Front PTO hours",
"Lifetime Rear PTO Hours",
"Effective Total Loading Time",
"Effective Total Unloading Time",
"Setpoint Grain Kernel Cracker Gap",
"Actual Grain Kernel Cracker Gap",
"Minimum Grain Kernel Cracker Gap",
"Maximum Grain Kernel Cracker Gap",
"Setpoint Swathing Width",
"Actual Swathing Width",
"Minimum Swathing Width",
"Maximum Swathing Width",
"Nozzle Drift Reduction",
"Function Type",
"Application Total Volume in [ml]",
"Application Total Mass in gram [g]",
"Total Application of Nitrogen",
"Total Application of Ammonium",
"Total Application of Phosphor",
"Total Application of Potassium",
"Total Application of Dry Matter",
"Average Dry Yield Mass Per Time",
"Average Dry Yield Mass Per Area",
"Last Bale Size",
"Last Bale Density",
"Total Bale Length",
"Last Bale Dry Mass",
"Actual Flake Size",
"Setpoint Downforce Pressure",
"Actual Downforce Pressure",
"Condensed Section Override State (1-16)",
"Condensed Section Override State (17-32) ",
"Condensed Section Override State (33-48) ",
"Condensed Section Override State (49-64) ",
"Condensed Section Override State (65-80) ",
"Condensed Section Override State (81-96) ",
"Condensed Section Override State (97-112) ",
"Condensed Section Override State (113-128) ",
"Condensed Section Override State (129-144) ",
"Condensed Section Override State (145-160) ",
"Condensed Section Override State (161-176) ",
"Condensed Section Override State (177-192) ",
"Condensed Section Override State (193-208) ",
"Condensed Section Override State (209-224) ",
"Condensed Section Override State (225-240) ",
"Condensed Section Override State (241-256) ",
"Apparent Wind Direction",
"Apparent Wind Speed",
"MSL Atmospheric Pressure ",
"Actual Atmospheric Pressure",
"Total Revolutions in Fractional Revolutions",
"Total Revolutions in Complete Revolutions",
"Setpoint Revolutions specified as count per time",
"Actual Revolutions Per Time",
"Default Revolutions Per Time",
"Minimum Revolutions Per Time",
"Maximum Revolutions Per Time",
"Actual Fuel Tank Content",
"Actual Diesel Exhaust Fluid Tank Content",
"Setpoint Speed",
"Actual Speed",
"Minimum Speed",
"Maximum Speed",
"Speed Source",
"Actual Application of Nitrogen",
"Actual application of Ammonium",
"Actual application of Phosphor",
"Actual application of Potassium",
"Actual application of Dry Matter",
"Actual Protein Content ",
"Average Protein Content",
"Average Crop Contamination",
"Total Diesel Exhaust Fluid Consumption",
"Instantaneous Diesel Exhaust Fluid Consumption per Time",
"Instantaneous Diesel Exhaust Fluid Consumption per Area",
"Lifetime Diesel Exhaust Fluid Consumption",
"Lifetime Average Diesel Exhaust Fluid Consumption per Time",
"Lifetime Average Diesel Exhaust Fluid Consumption per Area",
"Actual Seed Singulation Percentage",
"Average Seed Singulation Percentage",
"Actual Seed Skip Percentage",
"Average Seed Skip Percentage",
"Actual Seed Multiple Percentage",
"Average Seed Multiple Percentage",
"Actual Seed Spacing Deviation",
"Average Seed Spacing Deviation",
"Actual Coefficient of Variation of Seed Spacing Percentage ",
"Average Coefficient of Variation of Seed Spacing Percentage",
"Setpoint Maximum Allowed Seed Spacing Deviation",
"Setpoint Downforce as Force",
"Actual Downforce as Force",
"Loaded Total Mass",
"Unloaded Total Mass",
"Lifetime Loaded Total Mass",
"Lifetime Unloaded Total Mass",
"Setpoint Application Rate of Nitrogen",
"Actual  Application Rate of Nitrogen",
"Minimum Application Rate of Nitrogen",
"Maximum  Application Rate of Nitrogen",
"Setpoint  Application Rate of Ammonium",
"Actual  Application Rate of Ammonium",
"Minimum  Application Rate of Ammonium",
"Maximum  Application Rate of Ammonium",
"Setpoint  Application Rate of Phosphor",
"Actual  Application Rate of Phosphor",
"Minimum  Application Rate of Phosphor",
"Maximum  Application Rate of Phosphor",
"Setpoint  Application Rate of Potassium",
"Actual  Application Rate of Potassium",
"Minimum Application Rate of Potassium",
"Maximum Application Rate of Potassium",
"Setpoint Application Rate of Dry Matter",
"Actual  Application Rate of Dry Matter",
"Minimum Application Rate of Dry Matter",
"Maximum Application Rate of Dry Matter",
"Loaded Total Volume",
"Unloaded Total Volume",
"Lifetime loaded Total Volume",
"Lifetime Unloaded Total Volume",
"Last loaded Volume",
"Last unloaded Volume",
"Loaded Total Count",
"Unloaded Total Count",
"Lifetime Loaded Total Count",
"Lifetime Unloaded Total Count",
"Last loaded Count",
"Last unloaded Count",
"Haul Counter",
"Lifetime Haul Counter",
"Actual relative connector angle",
"Actual Percentage Content",
"468 unused",
"469 unused",
"470 unused",
"471 unused",
"Setpoint Length of Cut",
"Minimum length of cut",
"Maximum Length of Cut",
"Setpoint Bale Hydraulic Pressure",
"Minimum Bale Hydraulic Pressure",
"Maximum Bale Hydraulic Pressure",
"Setpoint Flake Size",
"Minimum Flake Size",
"Maximum Flake Size",
"Setpoint Number of Subbales",
"Last Bale Number of Subbales",
"Setpoint Engine Speed",
"Actual Engine Speed",
"Minimum Engine Speed",
"Maximum Engine Speed",
"unused",
"Diesel Exhaust Fluid Tank Percentage Level",
"Maximum Diesel Exhaust Fluid Tank Content",
"Maximum Fuel Tank Content",
"Fuel Percentage Level",
"Total Engine Hours",
"Lifetime Engine Hours",
"Last Event Partner ID (Byte 1-4)",
"Last Event Partner ID (Byte 5-8)",
"Last Event Partner ID (Byte 9-12)",
"Last Event Partner ID (Byte 13-16)",
"Last Event Partner ID Type",
"Last Event Partner ID Manufacturer ID Code",
"Last Event Partner ID Device Class",
"Setpoint Engine Torque",
"Actual Engine Torque",
"Minimum Engine Torque",
"Maximum Engine Torque",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"unused",
"Tramline Control Level",
"test",
"test",
//"PGN Based Data",
//"Request Default Process Data",
//"65534 Proprietary DDI Range",
//"Reserved"
};

std::string
decodeTechnicalCapabilities( uint8_t cmd )
{
  std::ostringstream out;

  switch( cmd )
  {
  case 0x0: out << "Version Request"; break;
  case 0x1: out << "Version"; break;
  default:  out << "Not specified Request/Answer!!!"; break;
  }

  return out.str();
}

std::string
decodeDevDescTransMgmt( uint8_t cmd )
{
  std::ostringstream out;

  switch( cmd )
  {
  case 0x0: out << "Structure Label Request"; break;
  case 0x1: out << "Structure Label"; break;
  case 0x2: out << "Localization Label Request"; break;
  case 0x3: out << "Localization Label"; break;
  case 0x4: out << "Request Object-pool Transfer"; break;
  case 0x5: out << "Request Object-pool Transfer Response"; break;
  case 0x6: out << "Object-pool Transfer"; break;
  case 0x7: out << "Object-pool Transfer Response"; break;
  case 0x8: out << "Object-pool Activate"; break;
  case 0x9: out << "Object-pool Activate Response"; break;
  case 0xA: out << "Object-pool Delete"; break;
  case 0xB: out << "Object-pool Delete Reponse"; break;
  case 0xC: out << "Change Designator"; break;
  case 0xD: out << "Change Designator Response"; break;
  default:  out << "Not specified Request/Answer!!!"; break;
  }

  return out.str();
}

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
    bool commandRequest = false;
    bool isMeasurementTime = false;
    bool isMeasurementDist = false;
    bool isMeasurementMin = false;
    bool isMeasurementMax = false;
    bool isMeasurementChange = false;
    bool boDdiIsAcws = false;
    bool boDdiIsScws = false;
    bool boDdiIsPcsOrScs = false;
    bool boIsTaskControllerStatus = false;

    switch (at_ptrFrame->dataOctet(0) & 0x0F)
    {
    case 0x00: out << "Command Technical Capabilities " << decodeTechnicalCapabilities( at_ptrFrame->dataOctet(0) >> 4 ); break;
    case 0x01: out << "Command Device Descriptor " << decodeDevDescTransMgmt( at_ptrFrame->dataOctet(0) >> 4 ); break;
    case 0x02: out << "Request  Value         "; valueAndMeasurementCommand = true; commandRequest = true;  break;
    case 0x03: boSaIsTc(at_ptrFrame->sourceAddress()) ? 
               out << "Setpoint Value         ": 
               out << "Measrmnt Value         "; valueAndMeasurementCommand = true;                         break;
    case 0x04: out << "Measurement Time  [ms] "; valueAndMeasurementCommand = true; isMeasurementTime  = true;  break;
    case 0x05: out << "Measurement Dist. [mm] "; valueAndMeasurementCommand = true; isMeasurementDist  = true;  break;
    case 0x06: out << "Measurement Minimum    "; valueAndMeasurementCommand = true; isMeasurementMin  = true;  break;
    case 0x07: out << "Measurement Maximum    "; valueAndMeasurementCommand = true; isMeasurementMax  = true;  break;
    case 0x08: out << "Measurement Change     "; valueAndMeasurementCommand = true; isMeasurementChange  = true;  break;
    case 0x09: out << "Command Peer Control Assignment"; break;
    case 0x0A: out << "Command Set Value and Acknowledge"; break;
    case 0x0D: out << "Message Process Data Acknowledge (PDACK)"; break;
    case 0x0E: out << "Message Task Controller Status"; boIsTaskControllerStatus = true; break;
    case 0x0F: out << "Message Working Set Task"; 
        if (at_ptrFrame->dataOctet(4) & 0x01)
        {
            out << " | Task Active";
        }
        break;
    default:   out << "RESERVED."; break;
    }

    if( valueAndMeasurementCommand )
    {
      uint16_t element = (uint16_t(at_ptrFrame->dataOctet(0) >> 4)) | uint16_t(uint16_t(at_ptrFrame->dataOctet(1)) << 4);
      uint32_t ddi = uint16_t(at_ptrFrame->dataOctet(2)) | (uint16_t(at_ptrFrame->dataOctet(3)) << 8);
      int32_t value = int32_t( uint32_t(at_ptrFrame->dataOctet(4)) | uint32_t(at_ptrFrame->dataOctet(5))<<8 | uint32_t(at_ptrFrame->dataOctet(6))<<16 | uint32_t(at_ptrFrame->dataOctet(7))<<24 );

      if (ddi >= 161 && ddi <= 176)
      {
          boDdiIsAcws = true;
      }
      else if (ddi >= 290 && ddi <= 305)
      {
          boDdiIsScws = true;
      }

      out << "E=" << std::setw(4) << std::dec << element << " DDI=";
      if( ddi >= 0xdfff )
        out << "$" << std::setw(4) << std::hex << ddi << std::dec;
      else
        out << std::setw(5) << std::dec << ddi;
     
      if (commandRequest)
          //do not print values for requests!! only leads to confusion
          out << " => " << std::setw(11) << std::left << std::dec << "/";
      else if (isMeasurementTime)
      {
          out << " => t=" << std::setw(9) << std::left << std::dec << value;
      }
      else if (isMeasurementDist)
      {
          out << " => d=" << std::setw(9) << std::left << std::dec << value;
      }
      else if (isMeasurementMin)
      {
          out << " => m=" << std::setw(9) << std::left << std::dec << value;
      }
      else if (isMeasurementMax)
      {
          out << " => M=" << std::setw(9) << std::left << std::dec << value;
      }
      else if (isMeasurementChange)
      {
          out << " => c=" << std::setw(9) << std::left << std::dec << value;
      }
      else if (boDdiIsAcws || boDdiIsScws)
          {
          //Condensed work state DDIs, use hex presentation
          out << " => 0x" << std::setw(9) << std::left << std::hex << (uint32_t)value;
          }
      else if (ddi == 158 || ddi == 160)
      {
          //prescription or section control state
          boDdiIsPcsOrScs = true;
          out << " => 0x" << std::setw(9) << std::left << std::hex << (uint32_t)value;
      }
      else
          out << " => " << std::setw(11) << std::left << std::dec << value;

      if( ddi < (sizeof(ddis) / sizeof(ddis[0])) )
        out << " (" << ddis[ ddi ] << ")";
      else if( ddi == 0xdffe )
        out << " (PGN Based Data)";
      else if( ddi == 0xdfff )
        out << " (Request Default Process Data)";
      else if( ddi == 0xffff )
        out << " (Reserved)";
      else if (ddi >= 0xe00)
        out << " (Proprietary DDI)";

      //print on off presentation for CWS sections
      if ((boDdiIsAcws || boDdiIsScws) && !(commandRequest))
      {
          out << " Sections";
          if (boDdiIsAcws)
              out << " == ";
          else
              out << " -> ";
          for (uint8_t i = 16; i > 0;--i)
          {
              int bitPair = (((uint32_t)value >> ((i-1 )*2)) & 0x3);
              switch (bitPair)
              {
              case 0:
                  out << "off" << " ";
                  break;
              case 1:
                  out << "ON " << " ";
                  break;
              case 2:
                  out << "err" << " ";
                  break;
              case 3:
                  out << "na " << " ";
                  break;
              default:
                  break;
              }
          }
      }

      //print presentation of SCS or PCS
      if (boDdiIsPcsOrScs)
      {
          int bitPair = (uint32_t)value & 0x3;
          out << "Control State: ";
          switch (bitPair)
          {
          case 0:
              out << "MANUAL" << " ";
              break;
          case 1:
              out << "AUTO  " << " ";
              break;
          case 2:
              out << "err   " << " ";
              break;
          case 3:
              out << "ndf   " << " ";
              break;
          }
      }
    }

    //print Task Controller Status Message Information
    if (boIsTaskControllerStatus)
    {
        if (at_ptrFrame->data()[4] & 0x01)
        {
            // Bit 1 = 1
            out << " | Task started";
        }
        if (at_ptrFrame->data()[4] & 0x02)
        {
            // Bit 2 = 1
            out << " | TC/DL busy saving data";
        }
        if (at_ptrFrame->data()[4] & 0x04)
        {
            // Bit 3 = 1
            out << " | TC/DL busy reading data";
        }
        if (at_ptrFrame->data()[4] & 0x08)
        {
            // Bit 4 = 1
            out << " | TC/DL busy executing a device descriptor command";
        }
        if (at_ptrFrame->data()[4] & 0x80)
        {
            // Bit 8 = 1
            out << " | TC/DL out of memory";
        }
    }

    if( gs_main.mb_storeDdop && ( at_ptrFrame->data()[0] == 0x81 ) ) {
      ddopStore( at_ptrFrame->sourceAddress() );
    }
  }

  return out.str();
}
