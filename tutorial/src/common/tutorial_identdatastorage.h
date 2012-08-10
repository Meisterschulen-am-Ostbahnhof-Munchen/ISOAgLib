/** \example tutorial_identdatastorage.cpp
 * Example implementation for storing identdata with the eeprom tutorial.
 */

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminal_c.h>

#include "tutorial_eeprom.h"

namespace IsoAgLibTutorial {

  class tutorialDataStorage_c: public IsoAgLib::iIdentDataStorage_c,
                               public IsoAgLib::iVtClientDataStorage_c {
    public:
      /** constructor with base address for eeprom usage
          @param a_baseAddr base address in eeprom to use
        */
      tutorialDataStorage_c(const uint16_t a_baseAddr);

      //! method called by IsoAgLib to retrive a formerly stored source address
      uint8_t loadSa() {
        return _loadSa();
      }

      //! method called by IsoAgLib to hand over a claimed source address for
      //! non volatile storage
      void storeSa(const uint8_t a_sa) {
        _storeSa(a_sa);
      }

      void loadDtcs( __IsoAgLib::DtcContainer_c &arc_dtcContainer )
      {
        _loadDtcs(arc_dtcContainer);
      }
      void storeDtcs( const __IsoAgLib::DtcContainer_c &arc_dtcContainer )
      {
        _storeDtcs(arc_dtcContainer);
      }
      
      void loadPreferredVt( IsoAgLib::iIsoName_c &arc_isoname, uint8_t &arui8_boottime_s )
      {
        _loadPreferredVt(arc_isoname, arui8_boottime_s);
      }
      void storePreferredVt( const IsoAgLib::iIsoName_c &arc_isoname, uint8_t aui8_bootTime )
      {
        _storePreferredVt(arc_isoname, aui8_bootTime);
      }


    private:
      static const uint32_t m_eepromMagicPattern = 0x150a61b;
      static const uint8_t m_defaultSa = 0x80;
      // block size: remember, that the eeprom tutorial allocates eeprom space
      // with the size of  pattern + block size + m_eepromBlockSize
      static const uint16_t m_eepromBlockSize = 1; // one byte payload needed for sa
      static const uint16_t m_eepromSaOffset = 0;

      //! protected implementation for loading the source address
      uint8_t _loadSa();
      //! protected implementation for storing the source address
      void _storeSa(const uint8_t a_sa);

      void _loadDtcs( __IsoAgLib::DtcContainer_c &arc_dtcContainer );
      void _storeDtcs( const __IsoAgLib::DtcContainer_c &arc_dtcContainer );

      void _loadPreferredVt( IsoAgLib::iIsoName_c &arc_isoname, uint8_t &arui8_boottime_s );
      void _storePreferredVt( const IsoAgLib::iIsoName_c &arc_isoname, uint8_t aui8_bootTime );
     
#ifdef USE_EEPROM_IO
      IsoAgLibTutorial::TutorialEeprom_c mc_eeprom;
#endif
  };
}
// eof
