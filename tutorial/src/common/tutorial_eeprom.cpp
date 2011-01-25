/** \example tutorial_eeprom.cpp
 * This is a generic eeprom manager for the IsoAgLib tutorial
 */

#include <IsoAgLib/isoaglib_config.h>

#ifdef USE_EEPROM_IO

#include <supplementary_driver/driver/eeprom/ieepromio_c.h>

#include "tutorial_eeprom.h"

namespace IsoAgLibTutorial {

  bool TutorialEeprom_c::init(const uint16_t a_eepromAddr, const uint16_t a_blockSize, const uint32_t a_magicPattern) {
    // base address
    if (HAL::getEepromSize() < (a_eepromAddr + getHeaderSize() + a_blockSize)) {
      // invalid size and block size combination
      return false;
    }

    m_baseAddr = a_eepromAddr;
    m_blockSize = a_blockSize + getHeaderSize();
    m_magicPattern = a_magicPattern;
    m_initialized = true;

    // check magic pattern
    m_valid = checkHeader();

    return true;
  }


  bool TutorialEeprom_c::setValid() {

    if (! m_initialized)
      return false;

    // write pattern if not already preset
    if (! m_valid) {
      m_valid = writeHeader();
    }
    return m_valid;
  }


  bool TutorialEeprom_c::_write(const uint16_t a_offset, const void* a_data, uint16_t a_size) {
    if (! m_initialized)
      return false;

    // parameter check
    if ((a_offset + a_size) > m_blockSize) {
      return false;
    }

    IsoAgLib::iEepromIo_c& rc_eeprom = IsoAgLib::getIeepromInstance();
    rc_eeprom.setp(m_baseAddr + a_offset);
    rc_eeprom.writeString(static_cast<const uint8_t*>(a_data), a_size);
    return true;
  }


  bool TutorialEeprom_c::_read(const uint16_t a_offset, void* a_data, uint16_t a_size) {
    if (! m_initialized)
      return false;

    // parameter check
    if ((a_offset + a_size) > m_blockSize) {
      return false;
    }

    IsoAgLib::iEepromIo_c& rc_eeprom = IsoAgLib::getIeepromInstance();
    rc_eeprom.setg(m_baseAddr + a_offset);
    rc_eeprom.readString(static_cast<uint8_t*>(a_data), a_size);
    return true;
  }


  bool TutorialEeprom_c::writeHeader() {
    bool r = _write(0, &m_magicPattern, sizeof(m_magicPattern));
    r &= _write(4, &m_blockSize, sizeof(m_blockSize));
    return r;
  }


  bool TutorialEeprom_c::checkHeader() {
    uint32_t p = 0;
    uint16_t s = 0;
    if ((_read(0, &p, sizeof(p))) && (_read(4, &s, sizeof(s)))) {
      return ((p == m_magicPattern) && (s == m_blockSize));
    }
    return false;
  }

}
#endif // USE_EEPROM_IO
// eof
