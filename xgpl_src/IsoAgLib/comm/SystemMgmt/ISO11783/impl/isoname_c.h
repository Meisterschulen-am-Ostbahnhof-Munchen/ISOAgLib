/***************************************************************************
                          isoname_c.h - handles the ISO 64bit NAME field
                             -------------------
    begin                : Wed Jan 03 2001
    copyright            : (C) 2001 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef ISO_NAME_H
#define ISO_NAME_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/canpkg_c.h>

namespace IsoAgLib {
  class iISOName_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** handle the 64bit ISO11783 NAME field
  with input/output from/to 8byte string
  and specific read/write access to single
  flags
  @author Dipl.-Inform. Achim Spangler
*/
class ISOName_c {
private:
public:
  /** constant for default parameters and initialization, where the device type is not yet spcified.
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const ISOName_c ISONameUnspecified;

  /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const ISOName_c ISONameInitialProcessData;

  /** default constructor
    using "explicit" to avoid WRONG implicit cast from SA to ISONAME!
    @param rui8_devClass     initial DEVCLASS (device type)
    @param rui8_devClassInst initial DEVCLASSINST (instance). Defaults to "unknown" (=0xF)
  */
  explicit ISOName_c( uint8_t rui8_devClass, uint8_t rui8_devClassInst=0xF )
  { set( true, 2, rui8_devClass, rui8_devClassInst, 0xFF, 0x7FF, 0x1FFFFF, 0x1F, 0x7 ); }

  /** constructor which can read in initial data from uint8_t string
    @param rpb_src 64bit input data string
  */
  ISOName_c(const uint8_t* rpb_src = NULL);

  /** constructor which can read in initial data from uint8_t string
    @param rpb_src 64bit input data string
  */
  ISOName_c(const Flexible8ByteString_c* rpu_src);

  /** constructor which format data string from series of input flags
    @param rb_selfConf true -> indicate sefl configuring ECU
    @param rui8_indGroup industry group of device (2 for agriculture)
    @param rui8_devClass device class of ECU
    @param rui8_devClassInst instance number of ECU with same devClass in the network
    @param rb_func function of the ECU (usual 25 for network interconnect)
    @param rui16_manufCode code of manufactor (11bit)
    @param rui32_serNo serial no of specific device (21bit)
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  ISOName_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
        uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0);
  /** copy constructor for ISOName
    @param rrefc_src source ISOName_c instance
  */
  ISOName_c(const ISOName_c& rrefc_src);

  /** assign constructor for ISOName
    @param rrefc_src source ISOName_c object
  */
  const ISOName_c& operator=(const ISOName_c& rrefc_src);

  /** default destructor */
  ~ISOName_c();

  /** set data string with all flags with one call
    @param rb_selfConf true -> indicate sefl configuring ECU
    @param rui8_indGroup industry group of device (2 for agriculture)
    @param rui8_devClass device class of ECU
    @param rui8_devClassInst instance number of ECU with same devClass in the network
    @param rb_func function of the ECU (usual 25 for network interconnect)
    @param rui16_manufCode code of manufactor (11bit)
    @param rui32_serNo serial no of specific device (21bit)
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void set(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
        uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0);

  /** set device class & instance with two seperate parameters */
  void set( uint8_t rui8_devClass, uint8_t rui8_devClassInst );

  /** set this instance to indicator for unspecified value */
  void setUnspecified( void ) { setDevClass( 0x7F ); setDevClassInst( 0xF );}

  /** check if this instance has specified value (different from default) */
  bool isSpecified( void ) const { return ( ( devClass() != 0x7F ) || ( devClassInst() != 0xF ) )?true:false;}

  /** check if this instance has unspecified value (match default) */
  bool isUnspecified( void ) const { return ( ( devClass() == 0x7F ) && ( devClassInst() == 0xF ) )?true:false;}

  /** IsoAgLib-specific enum for often used types of "functions" of IsoNames */
  enum ecuType_t {
    ecuTypeANYOTHER, // not one of the following
    ecuTypeTractorECU,
    ecuTypeTaskControl,
    ecuTypeVirtualTerminal,
    ecuTypeFileServerOrPrinter,
    ecuTypeNavigation
  };

  /** deliver type of ECU: Tractor, VT, Navigation, etc. */
  ecuType_t getEcuType() const;


  /** deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const uint8_t* outputString() const{return u_data.getUint8DataConstPointer();}

  /** deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const Flexible8ByteString_c* outputUnion() const{return &u_data;}

  /** get self config mode
    @return self configuration adress state
  */
  uint8_t selfConf() const;

  /** get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const;

  /** get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const;

  /** get device class code
    @return:device class
  */
  uint8_t devClass() const;

  /** get function code
    @return function code
  */
  uint8_t func() const;

  /** get function instance code
    @return function instance code
  */
  uint8_t funcInst() const;

  /** get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const;

  /** get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const;

  /** get serial number
    @return serial number
  */
  uint32_t serNo() const;

  /** set the NAME data from 8 uint8_t string
    @param rpb_src pointer to 8byte source string
  */
  void inputString(const uint8_t* rpb_src);

  /** set the NAME data from 8 uint8_t string
    @param rpb_src pointer to 8byte source string
  */
  void inputUnion(const Flexible8ByteString_c* rpu_src);

  /** set self config mode
    @param rb_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool rb_selfConf);

  /** set industry group code
    @param rui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t rui8_indGroup);

  /** set device class instance number
    @param rui8_devClassInst instance number of ECU with same devClass in the network
  */
  void setDevClassInst(uint8_t rui8_devClassInst);

  /** set device class code
    @param rui8_devClass device class of ECU
  */
  void setDevClass(uint8_t rui8_devClass);

  /** set function code
    @param rb_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t rb_func);

  /** set function instance code
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t rb_funcInst);

  /** set ECU instance code
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t rb_ecuInst);

  /** set manufactor code
    @param rui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t rui16_manufCode);

  /** set serial number (Identity Number)
    @param rui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t rui32_serNo);

  /** check if this NAME has higher prio
    than the given NAME 8-uint8_t string
    @param rpb_compare
    @return 0 == equal; -1 == this has lower prio than par; +1 == this item has higher prio than par
  */
  int8_t higherPriThanPar(const Flexible8ByteString_c* rpu_compare) const;

  /** Check if all Non-Instance fields of both ISONames match
    @return true if equal, false if one non-inst field differs!
  */
  bool isEqualRegardingNonInstFields (const ISOName_c& rrefc_isoName) const;

  /** compare two ISOName_c values with operator== */
  bool operator==( const ISOName_c& refc_right ) const
    { return (higherPriThanPar( refc_right.outputUnion() ) == 0)?true:false;}

  /** compare two ISOName_c values with operator!= */
  bool operator!=( const ISOName_c& refc_right ) const
    { return (higherPriThanPar( refc_right.outputUnion() ) != 0)?true:false;}

  /** compare ISOName_c value and Flexible8ByteString_c with operator!= */
  bool operator!=( const Flexible8ByteString_c& rrefc_right ) const
    { return (higherPriThanPar( &rrefc_right ) != 0)?true:false;}

  /** compare two ISOName_c values with operator< */
  bool operator<( const ISOName_c& refc_right ) const
    { return (higherPriThanPar( refc_right.outputUnion() ) == -1)?true:false;}

  /** convert function */
  IsoAgLib::iISOName_c& toIisoName_c();

  /** convert function */
  const IsoAgLib::iISOName_c& toConstIisoName_c() const;

private:
  /** ISO 8-uint8_t NAME field */
  Flexible8ByteString_c u_data;
};

}
#endif
