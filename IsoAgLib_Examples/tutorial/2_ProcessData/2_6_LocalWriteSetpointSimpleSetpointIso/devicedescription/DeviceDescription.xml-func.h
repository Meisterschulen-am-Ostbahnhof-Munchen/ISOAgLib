#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iDevKey_c myDeviceDevKey(0x5, 0x0);

IsoAgLib::iIdentItem_c c_myIdent(&myDeviceDevKey, true, 0x2, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0x0, 0x0
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

IsoAgLib::iProcDataLocalSimpleSetpoint_c c_workState(s_workStateElementDDI,
scui16_workStateElementNumber, myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_applicationRateElementNumber = 0;
const IsoAgLib::ElementDDI_s s_applicationRateElementDDI[] =
{
	{0x0001, true, IsoAgLib::GeneralCommand_c::exactValue},
	{0x0002, false, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleSetpoint_c c_applicationRate(s_applicationRateElementDDI,
scui16_applicationRateElementNumber, myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

