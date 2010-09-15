/***************************************************************************
                          adrvect_c.h  - header file to AdrVect_c for easy
                                        access on adress vector
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
#ifndef ADR_VECT_H
#define ADR_VECT_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/* **************************************** */
/* import Shared namespaces for easy access */
/* **************************************** */


/**
  handles the Scheduler_c adress vector for easy
  access to single adress numbers
  @author Dipl.-Inform. Achim Spangler
  */
class AdrVect_c {
public:
  /**
    constructor which can optional set the adress vector to a given value
    @param rui16_adrvect adress vector from a ISO11783 or DIN9684 system msg
  */
  AdrVect_c(const uint16_t rui16_adrvect = 0);
  /**
    copy constructor, whcih takes its information from another AdrVect_c instance
    @param rrefc_src other AdrVect_c instance
  */
  AdrVect_c(const AdrVect_c& rrefc_src);
  /** default destructor which has nothing to do */
  ~AdrVect_c();

   /**
     delivers value of ui16_adrvect
     @return adress vector as uint16_t value
   */
   uint16_t adrvect() const {return ui16_adrvect;};
   
  /**
    read the adress vector into the adress vector field of a ISO11783 or DIN9684 system msg 
    (two uint8_t string must be referenced)
    @param rrefb_data pointer to the beginning of the adress vector field
  */
  inline void getCanString(uint8_t *const rrefb_data) const;
  
  /**
    write the adress vector from the adress vector field of a ISO11783 or DIN9684 system msg
    @param rpb_data pointer to the beginning of the source adress vector field
  */
  inline void set(uint8_t *const rpb_data);
  
   /**
     check if one special adress is used
     @param rui8_nr adress number to be checked, if it's used
     @return true -> adress rui8_nr is used
   */
   bool isAdrUsed(uint8_t rui8_nr) const {return ((ui16_adrvect & (1 << rui8_nr)) > 0);};
   
   /**
     set one specific adress as used - answer false if adress was already used
     @param rui8_nr adress number to set as used
     @return true -> the adress was free before, and could be engaged
   */
   bool setUsedAdr(uint8_t rui8_nr);
  
  /**
    clear one specific adress
    @param rui8_nr adress to free in the adress vector
    @return true -> an before used adress was cleared
  */
  bool clearUsedAdr(uint8_t rui8_nr);
   
   /**
     reserve the first free adress and answer it
     @return a) !=0xFF -> adress number which was free and could be angaged; b) 0xFF -> no adress number free
   */
   uint8_t reserveFreeAdress();
   
   /**
     check, if one free adress is available -> when all adresses are used, the val is 0x7FFF
     @return true -> at least one adress number is free
   */
   bool freeNrAvailable() const {return (ui16_adrvect < 0x7FFF)? true:false;};

  /**
    check for equality with an adress vector unsigned 16int value
    @param rui16_adrvect comparison uint16 adress vector value
    @return true -> the 16bit value represents the same adress vector
  */
  bool operator==(const uint16_t rui16_adrvect) const {return ui16_adrvect == rui16_adrvect;};
  /**
    check for equality with another AdrVect_c instance
    @param rrefc_cmp comparison AdrVect_c instance
    @return true -> the other AdrVect_c instance is the same
  */
  bool operator==(const AdrVect_c& rrefc_cmp) const {return ui16_adrvect == rrefc_cmp.ui16_adrvect;};

  /**
    check if own adress vector value is greater than adress vector unsigned 16int value
    @param rui16_adrvect comparison uint16 adress vector value
    @return true -> the own adress vector value is greater than the other value
  */
  bool operator>(const uint16_t rui16_adrvect) const {return ui16_adrvect > rui16_adrvect;};
  /**
    check if own adress vector value is greater than another AdrVect_c instance
    @param rrefc_cmp comparison AdrVect_c instance
    @return true -> the own adress vector value is greater than the other value
  */
  bool operator>(const AdrVect_c& rrefc_cmp) const {return ui16_adrvect > rrefc_cmp.ui16_adrvect;};

  /**
    check if own adress vector value is smaller than adress vector unsigned 16int value
    @param rui16_adrvect comparison uint16 adress vector value
    @return true -> the own adress vector value is smaller than the other value
  */
  bool operator<(const uint16_t rui16_adrvect) const {return ui16_adrvect < rui16_adrvect;};
  /**
    check if own adress vector value is smaller than another AdrVect_c instance
    @param rrefc_cmp comparison AdrVect_c instance
    @return true -> the own adress vector value is smaller than the other value
  */
  bool operator<(const AdrVect_c& rrefc_cmp) const {return ui16_adrvect < rrefc_cmp.ui16_adrvect;};

  /**
    check if own adress vector value is greater or equal than adress vector unsigned 16int value
    @param rui16_adrvect comparison uint16 adress vector value
    @return true -> the own adress vector value is greater or equal than the other value
  */
  bool operator>=(const uint16_t rui16_adrvect) const {return ui16_adrvect >= rui16_adrvect;};
  /**
    check if own adress vector value is greater or equal than another AdrVect_c instance
    @param rrefc_cmp comparison AdrVect_c instance
    @return true -> the own adress vector value is greater or equal than the other value
  */
  bool operator>=(const AdrVect_c& rrefc_cmp) const {return ui16_adrvect >= rrefc_cmp.ui16_adrvect;};

  /**
    check if own adress vector value is smaller than adress vector unsigned 16int value
    @param rui16_adrvect comparison uint16 adress vector value
    @return true -> the own adress vector value is smaller or equal than the other value
  */
  bool operator<=(const uint16_t rui16_adrvect) const {return ui16_adrvect <= rui16_adrvect;};
  /**
    check if own adress vector value is smaller than another AdrVect_c instance
    @param rrefc_cmp comparison AdrVect_c instance
    @return true -> the own adress vector value is smaller or equal than the other value
  */
  bool operator<=(const AdrVect_c& rrefc_cmp) const {return ui16_adrvect <= rrefc_cmp.ui16_adrvect;};

  /**
    merge the own adress vector value with another uint16_t value by OR
    (set all numbers as used, which are used in the parameter value)
    @param rui16_adrvect source uint16 adress vector value
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator|=(const uint16_t rui16_adrvect){ui16_adrvect |= rui16_adrvect; return *this;};
  /**
    merge the own adress vector value with another AdrVect_c instance by OR
    (set all numbers as used, which are used in the parameter value)
    @param rrefc_src source AdrVect_c instance
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator|=(const AdrVect_c& src){ui16_adrvect |= src.ui16_adrvect; return *this;};
  /**
    merge the own adress vector value with another uint16_t value by AND
    (set only those numbers as used, which are used in both vectors)
    @param rui16_adrvect source uint16 adress vector value
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator&=(const uint16_t rui16_adrvect){ui16_adrvect &= rui16_adrvect; return *this;};
  /**
    merge the own adress vector value with another AdrVect_c instance by AND
    (set only those numbers as used, which are used in both vectors)
    @param rrefc_src source AdrVect_c instance
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator&=(const AdrVect_c& rrefc_src){ui16_adrvect &= rrefc_src.ui16_adrvect; return *this;};
  /**
    assign another adress vector value (uint16) to the own adr vect value
    @param rui16_adrvect source uint16 adress vector value
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator=(const uint16_t rui16_adrvect){ui16_adrvect = rui16_adrvect; return *this;};
  /**
    assign the value of another AdrVect_c instance to the own adr vect value
    @param rrefc_src source AdrVect_c instance
    @return reference to the resulting own AdrVect_c instance (for assigning chains like "vect1 |= vect2 |= vect3;)
  */
  AdrVect_c& operator=(const AdrVect_c& rrefc_src){ui16_adrvect = rrefc_src.ui16_adrvect; return *this;};

private: // Private attributes
  /** store the adress verctor */
  uint16_t ui16_adrvect;
};

/* ++++++++++++++++++++++++++++++++++++++++++ */
/* ++++++++ Inline Implementations ++++++++++ */
/* ++++++++++++++++++++++++++++++++++++++++++ */

/**
  read the adress vector into the adress vector field of a ISO11783 or DIN9684 system msg
  (two uint8_t string must be referenced)
  @param rrefb_data pointer to the beginning of the adress vector field
*/
inline void AdrVect_c::getCanString(uint8_t *const rrefb_data) const
{
  *rrefb_data = (ui16_adrvect >> 8);
  *(rrefb_data+1) = (ui16_adrvect & 0xFF);
};
  
/**
  write the adress vector from the adress vector field of a ISO11783 or DIN9684 system msg
  @param rpb_data pointer to the beginning of the source adress vector field
*/
inline void AdrVect_c::set(uint8_t *const rpb_data)
{  // the adr vect must be stored in order low high to send by CAN
  ui16_adrvect = *(rpb_data+1);
  ui16_adrvect |= static_cast<uint16_t>(*rpb_data) << 8;
};

}
#endif
