#include "tutorialSimpleSectionControl-definition.inc"

namespace tutorialSimpleSectionControl {

void initProcData( IsoAgLib::iIdentItem_c& myident ) {

  c_xOffset.init( myident, 0x86, 0, false, 8);
  c_actualWidth.init( myident, 0x43, 0, false, 8);
  c_maxWidth.init( myident, 0x46, 0, false, 8);
  c_setpointVolumeRate.init( myident, 0x1, 0, true, 9);
  c_actualVolumeRate.init( myident, 0x2, 0, false, 9);
  c_workState.init( myident, 0x8d, 0, false, 9);
  c_sectionControlState.init( myident, 0xa0, 0, true, 8);
  c_condensedWorkState.init( myident, 0xa1, 0, true, 9);
  c_workStateSection01.init( myident, 0x8d, 1, true, 9);
  c_workStateSection02.init( myident, 0x8d, 2, true, 9);
  c_workStateSection03.init( myident, 0x8d, 3, true, 9);
  c_workStateSection04.init( myident, 0x8d, 4, true, 9);
  c_workStateSection05.init( myident, 0x8d, 5, true, 9);
  c_workStateSection06.init( myident, 0x8d, 6, true, 9);
  c_workStateSection07.init( myident, 0x8d, 7, true, 9);
  c_workStateSection08.init( myident, 0x8d, 8, true, 9);

}

}

