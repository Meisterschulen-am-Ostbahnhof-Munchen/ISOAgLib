#include "tutorialDataSource-bytestream.inc"

IsoAgLib::iIsoName_c c_myIsoName (true, 0x2, 0x6, 0, 0x80, 0x165, 0x1b, 0x0, 0x0);

IsoAgLib::iProcDataLocal_c c_setpointVolumeRate(0x1, 0, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_actualVolumeRate(0x2, 0, c_myIsoName, false, 9);

IsoAgLib::iProcDataLocal_c c_workState(0x8d, 0, c_myIsoName, false, 8);

IsoAgLib::iProcDataLocal_c c_sectionControlState(0xa0, 0, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_defaultLogging(0xdfff, 0, c_myIsoName, false, 31);

IsoAgLib::iProcDataLocal_c c_workStateSection01(0x8d, 1, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection02(0x8d, 2, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection03(0x8d, 3, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection04(0x8d, 4, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection05(0x8d, 5, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection06(0x8d, 6, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection07(0x8d, 7, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection08(0x8d, 8, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection09(0x8d, 9, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection10(0x8d, 10, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection11(0x8d, 11, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection12(0x8d, 12, c_myIsoName, true, 9);

IsoAgLib::iProcDataLocal_c c_workStateSection13(0x8d, 13, c_myIsoName, true, 9);

