#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iISOName_c myISOName(0x5, 0x0);

IsoAgLib::iIdentItem_c c_myIdent(&myISOName, true, 0x2, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0x0, 0x0
 #ifdef USE_ISO_TERMINAL
 , 0, NULL
 #endif
);

static const uint16_t scui16_workStateElementNumber = 0;
const IsoAgLib::ElementDDI_s s_workStateElementDDI[] =
{
	{0x008D, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_workState(s_workStateElementDDI,
scui16_workStateElementNumber, myISOName, 0x2, myISOName, &myISOName, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_workWidthElementNumber = 0;
const IsoAgLib::ElementDDI_s s_workWidthElementDDI[] =
{
	{0x0043, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_workWidth(s_workWidthElementDDI,
scui16_workWidthElementNumber, myISOName, 0x2, myISOName, &myISOName, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_applicationRateElementNumber = 2;
const IsoAgLib::ElementDDI_s s_applicationRateElementDDI[] =
{
	{0x1, true, IsoAgLib::GeneralCommand_c::exactValue},
	{0x2, false, IsoAgLib::GeneralCommand_c::exactValue},
	{0x3, true, IsoAgLib::GeneralCommand_c::defaultValue},
	{0x4, true, IsoAgLib::GeneralCommand_c::minValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_applicationRate(s_applicationRateElementDDI,
scui16_applicationRateElementNumber, myISOName, 0x2, myISOName, &myISOName, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

