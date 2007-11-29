#include "DeviceDescription.xml-bytestream.inc"

IsoAgLib::iIdentItem_c c_myIdent(0x2, 0x5, 0, 0x19, 0x7ff, 0x1b, 0x80, 0xffff, 0,0,true
 #ifdef USE_ISO_TERMINAL 
 , 0, NULL
 #endif
);

static const uint16_t scui16_myOnoffElementNumber = 0;
const IsoAgLib::ElementDdi_s s_myOnoffElementDDI[] =
{
	{0x008D, true, IsoAgLib::ProcessCmd_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleMeasure_c c_myOnoff(s_myOnoffElementDDI,
scui16_myOnoffElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_myWorkWidthElementNumber = 0;
const IsoAgLib::ElementDdi_s s_myWorkWidthElementDDI[] =
{
	{0x0043, true, IsoAgLib::ProcessCmd_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleMeasure_c c_myWorkWidth(s_myWorkWidthElementDDI,
scui16_myWorkWidthElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

static const uint16_t scui16_myApplicationRateElementNumber = 0;
const IsoAgLib::ElementDdi_s s_myApplicationRateElementDDI[] =
{
	{0x0007, true, IsoAgLib::ProcessCmd_c::exactValue},
	// termination entry
	{0xFFFF, false, IsoAgLib::ProcessCmd_c::noValue}
};

IsoAgLib::iProcDataLocalSimpleMeasure_c c_myApplicationRate(s_myApplicationRateElementDDI,
scui16_myApplicationRateElementNumber, c_myIdent.isoName(), &c_myIdent.isoName(), false
#ifdef USE_EEPROM_IO
, 0xffff
#endif
);

