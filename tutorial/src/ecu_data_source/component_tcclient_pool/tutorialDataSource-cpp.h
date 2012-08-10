#include "tutorialDataSource-definition.inc"

namespace tutorialDataSource {

void initProcData( IsoAgLib::iIdentItem_c& myident ) {

  c_setpointVolumeRate.init( myident, 0x1, 0, true, 9);
  c_actualVolumeRate.init( myident, 0x2, 0, false, 9);
  c_workState.init( myident, 0x8d, 0, false, 8);
  c_sectionControlState.init( myident, 0xa0, 0, true, 9);
  c_defaultLogging.init( myident, 0xdfff, 0, false, 15);
  c_workStateSection01.init( myident, 0x8d, 1, true, 9);
  c_workStateSection02.init( myident, 0x8d, 2, true, 9);

}

}

