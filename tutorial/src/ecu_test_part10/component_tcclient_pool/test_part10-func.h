#include "test_part10-bytestream.inc"

IsoAgLib::iIsoName_c c_myIsoName (true, 0x2, 0x6, 0, 0x80, 0x165, 0x1b, 0x0, 0x0);

IsoAgLib::iProcDataLocal_c c_setpointVolumeRate(0x1, 0, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_actualVolumeRate(0x2, 0, c_myIsoName, false, 9);

IsoAgLib::iProcDataLocal_c c_workState(0x8d, 0, c_myIsoName, false, 8);

IsoAgLib::iProcDataLocal_c c_sectionControlState(0xa0, 0, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_defaultLogging(0xdfff, 0, c_myIsoName, false, 31);

IsoAgLib::iProcDataLocal_c c_workStateSection01(0x8d, 1, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection02(0x8d, 2, c_myIsoName, true, 9);

