/** \example tutorial_identdatastorage.cpp
 * Example implementation for storing identdata with the eeprom tutorial.
 */

#include <IsoAgLib/isoaglib_config.h>
#include "tutorial_identdatastorage.h"

namespace IsoAgLibTutorial {

  tutorialIdentDataStorage_c::tutorialIdentDataStorage_c(const uint16_t a_baseAddr) {
#ifdef USE_EEPROM_IO
    mc_eeprom.init(a_baseAddr, m_eepromBlockSize, m_eepromMagicPattern);
#endif
  }

  uint8_t tutorialIdentDataStorage_c::_loadSa() {
#ifdef USE_EEPROM_IO
    uint8_t sa = m_defaultSa;
    if (mc_eeprom.valid() && mc_eeprom.read(m_eepromSaOffset, sa)) {
      return sa;
    }
#endif
    return m_defaultSa;
  }

  void tutorialIdentDataStorage_c::_storeSa(const uint8_t a_sa) {
#ifdef USE_EEPROM_IO
    mc_eeprom.write(m_eepromSaOffset, a_sa);
    mc_eeprom.setValid();
#endif
  }
}

// eof
