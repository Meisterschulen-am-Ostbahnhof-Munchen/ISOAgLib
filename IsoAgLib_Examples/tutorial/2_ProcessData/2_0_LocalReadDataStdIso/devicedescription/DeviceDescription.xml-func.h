#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iDevKey_c myDeviceDevKey(0x5, 0x0);

IsoAgLib::iIdentItem_c c_myIdent(&myDeviceDevKey, true, 0x2, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0x0, 0x0
 #ifdef USE_ISO_TERMINAL 
 , 0, NULL
 #endif
);

#if defined(USE_ISO_11783)
const IsoAgLib::ElementDDI_s s_myOnoffElementDDI[] =
{
	{0x008D, 2, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, 0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_myOnoff(
#ifdef USE_ISO_11783
s_myOnoffElementDDI, 
#endif
#ifdef USE_DIN_9684
0x0, 0x1, 0x0, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

#if defined(USE_ISO_11783)
const IsoAgLib::ElementDDI_s s_myWorkWidthElementDDI[] =
{
	{0x0043, 2, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, 0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_myWorkWidth(
#ifdef USE_ISO_11783
s_myWorkWidthElementDDI, 
#endif
#ifdef USE_DIN_9684
0x0, 0x3, 0x1, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

#if defined(USE_ISO_11783)
const IsoAgLib::ElementDDI_s s_myApplicationRateElementDDI[] =
{
	{0x0007, 2, true, IsoAgLib::GeneralCommand_c::exactValue},
	// termination entry
	{0xFFFF, 0xFFFF, false, IsoAgLib::GeneralCommand_c::noValue}
};
#endif

IsoAgLib::iProcDataLocal_c c_myApplicationRate(
#ifdef USE_ISO_11783
s_myApplicationRateElementDDI, 
#endif
#ifdef USE_DIN_9684
0x0, 0x5, 0x0, 0xff, 
#endif
myDeviceDevKey, 0x2, myDeviceDevKey, &myDeviceDevKey, false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

