#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iDevKey_c myDeviceDevKey(0x5, 0x0);

IsoAgLib::iIdentItem_c c_myIdent(&myDeviceDevKey, true, 0x2, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0x0, 0x0
 #ifdef USE_ISO_TERMINAL 
 , 0, NULL
 #endif
);

static const uint16_t scui16_myOnoffElementNumber = 0;
const IsoAgLib::ElementDDI_s s_myOnoffElementDDI[] =
{
	{0x008D, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleSetpoint_c c_myOnoff(s_myOnoffElementDDI,
scui16_myOnoffElementNumber, myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_myWorkWidthElementNumber = 0;
const IsoAgLib::ElementDDI_s s_myWorkWidthElementDDI[] =
{
	{0x0042, true, IsoAgLib::GeneralCommand_c::exactValue},
	{0x0043, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleSetpoint_c c_myWorkWidth(s_myWorkWidthElementDDI,
scui16_myWorkWidthElementNumber, myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_myApplicationRateElementNumber = 0;
const IsoAgLib::ElementDDI_s s_myApplicationRateElementDDI[] =
{
	{0x0001, true, IsoAgLib::GeneralCommand_c::exactValue},
	{0x0002, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleSetpoint_c c_myApplicationRate(s_myApplicationRateElementDDI,
scui16_myApplicationRateElementNumber, myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

