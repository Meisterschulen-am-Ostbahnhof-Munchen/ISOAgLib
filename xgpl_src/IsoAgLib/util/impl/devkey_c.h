/***************************************************************************
                          devkey_c.h - class DevKey_c combines device class
                                      ( GETY_POS for DIN 9684 ) and instance
                                      number ( POS for DIN 9684 )
                             -------------------
    begin                 Sun Feb 23 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2006-02-16 22:58:43 +0100 (Do, 16 Feb 2006) $
    $LastChangedRevision: 1556 $
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
#ifndef DEVCLASSPOS_H
#define DEVCLASSPOS_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/SystemMgmt/impl/baseitem_c.h>
#ifdef USE_ISO_11783
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** class DevKey_c combines device class
  * ( GETY_POS for DIN 9684 ) and instance
  * number ( POS for DIN 9684 )
  *@author Dipl.-Inform. Achim Spangler
  */
class DevKey_c {
 public:
    /** constant for default parameters and initialization, where the device type is not yet spcified.
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const DevKey_c DevKeyUnspecified;
    /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const DevKey_c DevKeyInitialProcessData;
    /** default constructor
      * @param rui16_devClass optional initial DEVCLASS (device type)
      * @param rui16_pos optional initial device class instance
      */
    #ifdef USE_ISO_11783
    DevKey_c( uint8_t rui8_devClass = 0x7F, uint8_t rui8_pos = 0xF )
    : c_isoName( true, 2, rui8_devClass, rui8_pos, 0xFF, 0x7FF, 0x1FFFFF, 0x1F, 0x7 ) {};
    #else
    DevKey_c( uint16_t rui16_devClass = 0x7F, uint16_t rui16_pos = 0xF )
    : ui16_data( ( rui16_devClass << 8 ) | ( rui16_pos & 0xF ) ){};
    #endif

    #ifdef USE_ISO_11783
    /** default constructor
      * @param rui16_devClass optional initial DEVCLASS (device type)
      * @param rui16_pos optional initial device class instance
      */
    DevKey_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
        uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0)
    : c_isoName( rb_selfConf, rui8_indGroup, rui8_devClass, rui8_devClassInst,
        rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst) {};
    /** default constructor
      * @param rui16_devClass optional initial DEVCLASS (device type)
      * @param rui16_pos optional initial device class instance
      */
    DevKey_c( const ISOName_c& rrefc_src )
    : c_isoName(rrefc_src) {};
    DevKey_c( const uint8_t* rpui8_dataName ) : c_isoName( rpui8_dataName ) {};
    #endif

    /** default constructor
      * @param rui16_devClass optional initial DEVCLASS (device type)
      * @param rui16_pos optional initial device class instance
      */
    DevKey_c( const DevKey_c& rrefc_src )
    #ifdef USE_ISO_11783
    : c_isoName(rrefc_src.c_isoName) {};
    #else
    : ui16_data(rrefc_src.ui16_data) {};
    #endif


    /** set device class & instance with two seperate parameters */
    #ifdef USE_ISO_11783
    void set( uint8_t rui8_devClass, uint8_t rui8_pos )
      { setDevClass( rui8_devClass); setDevClassInst( rui8_pos ); };
    #else
    void set( uint16_t rui16_devClass, uint16_t rui16_pos )
      { ui16_data = ( ( rui16_devClass << 8 ) | ( rui16_pos & 0xF ) ); };
    #endif

    #ifdef USE_ISO_11783
    /** set device class & instance with two seperate parameters */
    void set( const ISOName_c& rrefc_isoName ) { c_isoName = rrefc_isoName;};
    /** set device class & instance with two seperate parameters */
    void set( const uint8_t* rpui8_dataName ){ c_isoName.inputString(rpui8_dataName);};
    #endif

    /** set DEVCLASS (device type ) */
    #ifdef USE_ISO_11783
    void setDevClass( uint8_t rui8_devClass )
      { c_isoName.setDevClass( rui8_devClass );};
    #else
    void setDevClass( uint16_t rui16_devClass )
      { ui16_data = ( ( rui16_devClass << 8 ) | ( ui16_data & 0xF ) );};
    #endif
    /** set device class instance */
    #ifdef USE_ISO_11783
    void setDevClassInst( uint8_t rui8_pos )
      { c_isoName.setDevClassInst( rui8_pos );};
    #else
    void setDevClassInst( uint16_t rui16_pos )
      { ui16_data = ( ( ui16_data & 0xFF00 ) | ( rui16_pos & 0xF ) );};
    #endif


    /** deliver DEVCLASS (device type ) */
    uint8_t getDevClass( void ) const
    #ifdef USE_ISO_11783
      { return c_isoName.devClass();};
    #else
      { return ( ui16_data >> 8 );};
    #endif
    /** deliver _instance_ ( device type instance number ) */
    uint8_t getDevClassInst( void ) const
    #ifdef USE_ISO_11783
      { return c_isoName.devClassInst();};
    #else
      { return ( ui16_data & 0xF );};
    #endif

    #ifdef USE_ISO_11783
    /** provide pointer to second level compare NAME */
    ISOName_c& getName() { return c_isoName;};
    /** provide pointer to second level compare NAME */
    const ISOName_c& getConstName() const { return c_isoName;};
    #endif


    /** assign value from another DevKey_c instance */
    const DevKey_c& operator=( const DevKey_c& refc_src )
    #ifdef USE_ISO_11783
      {c_isoName = refc_src.c_isoName;return refc_src;};
    #else
      { ui16_data = refc_src.ui16_data; return *this;};
    #endif
    /** compare two DevKey_c values with operator== */
    bool operator==( const DevKey_c& refc_right ) const
    #ifdef USE_ISO_11783
      { return ( c_isoName == refc_right.c_isoName )?true:false;};
    #else
      { return ui16_data == refc_right.ui16_data;};
    #endif
    /** compare two DevKey_c values with operator!= */
    bool operator!=( const DevKey_c& refc_right ) const
    #ifdef USE_ISO_11783
      { return ( c_isoName != refc_right.c_isoName )?true:false;};
    #else
      { return ui16_data != refc_right.ui16_data;};
    #endif
    /** compare two DevKey_c values with operator< */
    bool operator<( const DevKey_c& refc_right ) const
    #ifdef USE_ISO_11783
      { return ( c_isoName < refc_right.c_isoName )?true:false;};
    #else
      { return ui16_data < refc_right.ui16_data;};
    #endif

    /** set this instance to indicator for unspecified value */
    void setUnspecified( void )
    #ifdef USE_ISO_11783
      { c_isoName.setDevClass( 0x7F ); c_isoName.setDevClassInst( 0xF );};
    #else
      { ui16_data = 0x7F0F;};
    #endif
    /** check if this instance has specified value (different from default */
    bool isSpecified( void )   const
    #ifdef USE_ISO_11783
      { return ( ( getDevClass() != 0x7F ) || ( getDevClassInst() != 0xF ) )?true:false;};
    #else
      { return ( ui16_data != 0x7F0F)?true:false;};
    #endif
    bool isUnspecified( void ) const
    #ifdef USE_ISO_11783
      { return ( ( getDevClass() == 0x7F ) && ( getDevClassInst() == 0xF ) )?true:false;};
    #else
      { return ( ui16_data == 0x7F0F)?true:false;};
    #endif
    bool isSpecifiedDevClassInst( void ) const
    #ifdef USE_ISO_11783
      { return ( getDevClassInst()  == 0xF  )?true:false;};
    #else
      { return ( ( ui16_data & 0xF ) != 0xF )?true:false;};
    #endif
    bool isUnspecifiedDevClassInst( void ) const
    #ifdef USE_ISO_11783
      { return ( getDevClass() == 0x7F )?true:false;};
    #else
      { return ( ( ui16_data & 0x7F00 ) == 0x7F00 )?true:false;};
    #endif
 private:
    #ifdef USE_ISO_11783
    ISOName_c c_isoName;
    #else
    uint16_t ui16_data;
    #endif
};

} // end namespace __IsoAgLib
#endif
