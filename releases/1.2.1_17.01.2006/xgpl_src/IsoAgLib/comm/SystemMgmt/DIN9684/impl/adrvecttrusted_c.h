/***************************************************************************
                          adrvecttrusted_c.h  - from AdrVect_c derived object
                                                which holds a trusted
                                                adress vector version for
                                                own identities
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  * 
 ***************************************************************************/ 

 /**************************************************************************
 *                                                                         * 
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   * 
 * Each software module, which accesses directly elements of this file,    * 
 * is considered to be an extension of IsoAgLib and is thus covered by the * 
 * GPL license. Applications must use only the interface definition out-   * 
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  * 
 * and __HAL namespaces from applications which shouldnt be affected by    * 
 * the license. Only access their interface counterparts in the IsoAgLib   * 
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- * 
 * ion really needs access to a part of an internal namespace, so that the * 
 * interface might be extended if your request is accepted.                * 
 *                                                                         * 
 * Definition of direct access:                                            * 
 * - Instantiation of a variable with a datatype from internal namespace   * 
 * - Call of a (member-) function                                          * 
 * Allowed is:                                                             * 
 * - Instatiation of a variable with a datatype from interface namespace,  * 
 *   even if this is derived from a base class inside an internal namespace* 
 * - Call of member functions which are defined in the interface class     * 
 *   definition ( header )                                                 * 
 *                                                                         * 
 * Pairing of internal and interface classes:                              * 
 * - Internal implementation in an :impl: subdirectory                     * 
 * - Interface in the parent directory of the corresponding internal class * 
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      * 
 *   __IsoAgLib::Foo_c                                                     * 
 *                                                                         * 
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef ADR_VECT_TRUSTED_H
#define ADR_VECT_TRUSTED_H

#include "adrvect_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  from AdrVect_c derived object which holds a trusted
  adress vector version for own entities (f.e. for sending)
  *@author Dipl.-Inform. Achim Spangler
  */
class AdrVectTrusted_c : public AdrVect_c  {
public: 
  /**
    constructor which gets it's initial value from an uint16
    @param rui16_adrvect source adress vector uint16 value
  */
  AdrVectTrusted_c(const uint16_t rui16_adrvect = 0);
  /** default destructor which has nothing to do */
  ~AdrVectTrusted_c();

   /**
     delivers value of the trusted adrvect
     (this function is mainly called of internal identities,
      which should use the trusted version)
      @return value of the trusted adress vector as uint16
   */
   uint16_t adrvect() const {return c_trustedAdrvect.adrvect();};

  /**
    read the trusted adress vector into the adress vector field of a ISO11783 or DIN9684 system msg
    @param rref_data pointer to the beginning of the adress vector field
  */
  void getCanString(uint8_t *const rref_data) const
  { // use for CAN send the trusted version
    c_trustedAdrvect.getCanString(rref_data);
  };
  
   /**
     set the adress vector from another AdrVect_c as trusted vector
     @param rrefc_src reference to the source AdrVect_c instance
   */
   void setTrusted(AdrVect_c& rrefc_src){c_trustedAdrvect = rrefc_src;};
   
   /**
     check if one special adress is free in the trusted vector
     @param rui8_nr adress number to check for free state in trusted vector
     @return true -> number is free in trusted vector
   */
   bool isAdrUsedTrusted(uint8_t rui8_nr) const {return c_trustedAdrvect.isAdrUsed(rui8_nr);};
   
  /**
    sets an adr number used in trusted adrvect
    (sets adr used in public version too)
    @param rui8_nr adress number to set used
    @return true -> the number was free in trusted vector, and is now engaged
  */
  inline bool setUsedAdrTrusted(uint8_t rui8_nr)
  { // call for pulic
    AdrVect_c::setUsedAdr(rui8_nr);
    // call for trusted
    return c_trustedAdrvect.setUsedAdr(rui8_nr);
  };
   
  /**
    clear an adress for trusted and public
    @param rui8_nr adress number to clear in both vector versions
    @return true -> adress was used and was cleared with this method call
  */
  bool clearUsedAdr(uint8_t rui8_nr)
  { // call for pulic
    AdrVect_c::clearUsedAdr(rui8_nr);
    bool b_result = isAdrUsedTrusted(rui8_nr);
    // call for trusted
    c_trustedAdrvect.clearUsedAdr(rui8_nr);
    return b_result;
  };

   /**
     search for the first adress which is free in the untrusted version,
     reserve it in both adress vector versions and answer the free value
     @return free adress number (0xFF means nothing free)
   */
   inline uint8_t reserveFreeAdress();
private: // Private attributes
  /** trusted version of adrvect */
  AdrVect_c c_trustedAdrvect;
};


/**
   search for the first adress which is free in the untrusted version,
   reserve it in both adress vector versions and answer the free value
   @return free adress number (0xFF means nothing free)
*/
inline uint8_t AdrVectTrusted_c::reserveFreeAdress()
{
   /** search a free number  in the untrusted vector an reserve it */
   uint8_t result = AdrVect_c::reserveFreeAdress();
   /** reserve the resulting number also in the trusted vector */
   c_trustedAdrvect.setUsedAdr(result);
   return result;
};

}
#endif
