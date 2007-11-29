#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iIdentItem_c c_myIdent(0x2, 0x5, 0, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0,0,true
 #ifdef USE_ISO_TERMINAL 
 , 0, NULL
 #endif
);

static const uint16_t scui16_workStateElementNumber = 0;
const IsoAgLib::ElementDdi_s s_workStateElementDDI[] =
{
	{0x008D, true, IsoAgLib::ProcessCmd_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_workState(s_workStateElementDDI,
scui16_workStateElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_applicationRateElementNumber = 0;
const IsoAgLib::ElementDdi_s s_applicationRateElementDDI[] =
{
	{0x0001, true, IsoAgLib::ProcessCmd_c::exactValue},
	{0x0002, false, IsoAgLib::ProcessCmd_c::exactValue},
	{0x0003, true, IsoAgLib::ProcessCmd_c::defaultValue},
	{0x0004, true, IsoAgLib::ProcessCmd_c::minValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_applicationRate(s_applicationRateElementDDI,
scui16_applicationRateElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_defaultLoggingElementNumber = 0;
const IsoAgLib::ElementDdi_s s_defaultLoggingElementDDI[] =
{
	{0xDFFF, false, IsoAgLib::ProcessCmd_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocal_c c_defaultLogging(s_defaultLoggingElementDDI,
scui16_defaultLoggingElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

