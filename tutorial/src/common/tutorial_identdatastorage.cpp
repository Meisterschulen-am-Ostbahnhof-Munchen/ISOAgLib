/** \example tutorial_identdatastorage.cpp
 * Example implementation for storing identdata with the eeprom tutorial.
 */

#include <IsoAgLib/isoaglib_config.h>
#include "tutorial_identdatastorage.h"

namespace IsoAgLibTutorial {

  tutorialDataStorage_c::tutorialDataStorage_c(const uint16_t a_baseAddr) {
#ifdef USE_EEPROM_IO
    mc_eeprom.init(a_baseAddr, m_eepromBlockSize, m_eepromMagicPattern);
#else
(void)a_baseAddr;
#endif
  }

  uint8_t tutorialDataStorage_c::_loadSa() {
#ifdef USE_EEPROM_IO
    uint8_t sa = m_defaultSa;
    if (mc_eeprom.valid() && mc_eeprom.read(m_eepromSaOffset, sa)) {
      return sa;
    }
#endif
    return m_defaultSa;
  }

  void tutorialDataStorage_c::_storeSa(const uint8_t a_sa) {
#ifdef USE_EEPROM_IO
    mc_eeprom.write(m_eepromSaOffset, a_sa);
    mc_eeprom.setValid();
#else
(void)a_sa;
#endif
  }

  void tutorialDataStorage_c::_loadDtcs( __IsoAgLib::DtcContainer_c& /*arc_dtcContainer*/ )
  {
    // @TODO load DTCs
  }

  void tutorialDataStorage_c::_storeDtcs( const __IsoAgLib::DtcContainer_c& /*arc_dtcContainer*/ )
  {
    // @TODO save DTCs
  }

  void tutorialDataStorage_c::_loadPreferredVt( IsoAgLib::iIsoName_c& /*arc_isoname*/, uint8_t& /*arui8_boottime_s*/ )
  {
    // @TODO load Preferred VT infos
  }

  void tutorialDataStorage_c::_storePreferredVt( const IsoAgLib::iIsoName_c& /*arc_isoname*/, uint8_t /*aui8_bootTime*/ )
  {
    // @TODO save Preferred VT infos
  }
}

// eof
