/***************************************************************************
                          getypos_c.h - class GetyPos_c combines device type
                                      ( GETY for DIN 9684 ) and instance
                                      number ( POS for DIN 9684 )
                             -------------------
    begin                 Sun Feb 23 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2003 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#ifndef GETYPOS_H
#define GETYPOS_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** class GetyPos_c combines device type
  * ( GETY for DIN 9684 ) and instance
  * number ( POS for DIN 9684 )
  *@author Dipl.-Inform. Achim Spangler
  */
class GetyPos_c {
 public:
  /** default constructor
    * @param rui8_gety optional initial GETY (device type)
    * @param rui8_pos optional initial POS (device number)
    */
  GetyPos_c( uint8_t rui8_gety = 0xF, uint8_t rui8_pos = 0xF )
  : ui8_data( ( rui8_gety << 4 ) | ( rui8_pos & 0xF ) ){};
 /** set GETY and POS with two seperate parameters */
 void set( uint8_t rui8_gety, uint8_t rui8_pos )
  { ui8_data = ( ( rui8_gety << 4 ) | ( rui8_pos & 0xF ) );};
 /** deliver GETY (device type ) */
 uint8_t getGety( void ) const { return ( ui8_data >> 4 );};
 /** deliver POS ( device type instance number ) */
 uint8_t getPos( void ) const { return ( ui8_data & 0xF );};
 /** set GETY (device type ) and POS */
 void setGtp( uint8_t rui8_gety, uint8_t rui8_pos )
  { ui8_data = ( ( rui8_gety << 4 ) | ( rui8_pos & 0xF ) );};
 /** set GETY (device type ) */
 void setGety( uint8_t rui8_gety )
  { ui8_data = ( ( rui8_gety << 4 ) | ( ui8_data & 0xF ) );};
 /** set POS ( device type instance number ) */
 void setPos( uint8_t rui8_pos )
  { ui8_data = ( ( ui8_data & 0xF0 ) | ( rui8_pos & 0xF ) );};
 /** set data with one combined value of GET and POS from DIN 9684 format
   * (DIN 90684 uses 3 bit POS, whereas ISO 11783 uses 4 bits)
   */
 void setCombinedDin( uint8_t rui8_dinData )
  { ui8_data = ( ( ( rui8_dinData << 1 ) & 0xF0 ) | ( rui8_dinData & 0x7 ) );};
 /** set data with one combined value of GET and POS from ISO 11783 format
   * (DIN 90684 uses 3 bit POS, whereas ISO 11783 uses 4 bits)
   */
 void setCombinedIso( uint8_t rui8_isoData ) { ui8_data = rui8_isoData;};
 /** deliver data as one combined value of GET and POS from DIN 9684 format
   * (DIN 90684 uses 3 bit POS, whereas ISO 11783 uses 4 bits)
   */
 uint8_t getCombinedDin( void ) const { return ( ( getGety() << 3 ) | getPos() );};
 /** deliver data as one combined value of GET and POS from ISO 11783 format
   * (DIN 90684 uses 3 bit POS, whereas ISO 11783 uses 4 bits)
   */
 uint8_t getCombinedIso( void ) const { return ui8_data;};
 /** assign value from another GetyPos_c instance */
 const GetyPos_c& operator=( const GetyPos_c& refc_src )
  { ui8_data = refc_src.ui8_data; return *this;};
 /** compare two GetyPos_c values with operator== */
 bool operator==( const GetyPos_c& refc_right ) const
  { return ui8_data == refc_right.ui8_data;};
 /** compare two GetyPos_c values with operator!= */
 bool operator!=( const GetyPos_c& refc_right ) const
  { return ui8_data != refc_right.ui8_data;};
 /** compare two GetyPos_c values with operator< */
 bool operator<( const GetyPos_c& refc_right ) const
  { return ui8_data < refc_right.ui8_data;};
 /** set this instance to indicator for unspecified value */
 void setUnspecified( void ) { ui8_data = 0xFF;};
 /** check if this instance has specified value (different from default */
 bool isSpecified( void ) const { return ( ui8_data != 0xFF)?true:false;};
 bool isUnspecified( void ) const { return ( ui8_data == 0xFF)?true:false;};
 bool isSpecifiedPos( void ) const { return ( ( ui8_data & 0xF ) != 0xF )?true:false;};
 bool isUnspecifiedPos( void ) const { return ( ( ui8_data & 0xF ) == 0xF )?true:false;};
 private:
  uint8_t ui8_data;
};

} // end namespace __IsoAgLib
#endif
