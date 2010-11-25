/** \example tutorial_eeprom.cpp
 * This is a generic eeprom manager for the IsoAgLib tutorial
 *
 * Each instance allocates a block of eeprom data that has the size:
 *
 * header + blockSize
 *
 * The header consists of:
 * 4 byte magic pattern
 * 2 byte block length
 *
 * There's no protection that several instances of TutorialEeprom_c
 * write to conflicting areas - please take care about yourself.
 */

#include <IsoAgLib/isoaglib_config.h>
#ifdef USE_EEPROM_IO

#include <IsoAgLib/hal/hal_typedef.h>

namespace IsoAgLibTutorial {
  class TutorialEeprom_c {
    public:
      TutorialEeprom_c() :
          m_valid(false), m_initialized(false), m_baseAddr(0xffff),
          m_blockSize(0x0), m_magicPattern(0) {}

      /** Initialize eeprom handler with parameters
          @param a_eepromAddr   starting address for this instance
          @param a_blockSize    size allocated by this instance
          @param a_magicPattern pattern to check for in the first 4 bytes for validation.
                                The pattern is written to the eeprom each time a invalid
                                pattern was read a some data is written to the block
       */
      bool init(const uint16_t a_eepromAddr, const uint16_t a_blockSize, const uint32_t a_magicPattern);

      /** retrive eeprom state
          @return bool  true if magic patter was found at the beginning of the block
       */
      bool valid() const {
        return m_valid;
      }

      /** set block content valid and write magic pattern */
      bool setValid();

      /** write a_data at offset a_offet to block */
      template <class T>
      bool write(const uint16_t a_offset, const T& a_data) {
        return _write(a_offset + getHeaderSize(), reinterpret_cast<const void*>(&a_data), sizeof(T));
      }

      /** read data at offset a_offet from block and write to a_data */
      template <class T>
      bool read(const uint16_t a_offset, T& a_data) {
        return _read(a_offset + getHeaderSize(), reinterpret_cast<void*>(&a_data), sizeof(T));
      }

    private:
      //! have a valid eeprom
      bool m_valid;
      //! initialized in the mean, that we've got a valid configuration
      bool m_initialized;
      //! configured eeprom base address
      uint16_t m_baseAddr;
      //! configured block size
      uint16_t m_blockSize;
      //! magic code
      uint32_t m_magicPattern;

      //! write a_size bytes from a_data to position eeprom_base + a_offset
      bool _write(const uint16_t a_offset, const void* a_data, uint16_t a_size);
      //! copy a_size bytes to a_data from position eeprom_base + a_offset
      bool _read(const uint16_t a_offset, void* a_data, uint16_t a_size);

      //! deliver size of block header
      uint16_t getHeaderSize() const {
        return (sizeof(m_magicPattern) + sizeof(m_blockSize));
      }

      /** initialize header- write magic pattern and blocksize to eeprom
          @return true on success
       */
      bool writeHeader();
      /** check header for a valid pattern and blocksize entry
          @return true when header information is valid
       */
      bool checkHeader();
  };
}
#endif
// eof
