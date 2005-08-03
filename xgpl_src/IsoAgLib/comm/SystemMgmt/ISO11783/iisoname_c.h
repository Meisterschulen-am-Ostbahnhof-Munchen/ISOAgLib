/***************************************************************************
                          iiso_name.h - handles the ISO 64bit NAME field
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
#ifndef IISO_NAME_H
#define IISO_NAME_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isoname_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  handle the 64bit ISO11783 NAME field
  with input/output from/to 8byte string
  and specific read/write access to single
  flags
  @author Dipl.-Inform. Achim Spangler
*/
class iISOName_c : public __IsoAgLib::ISOName_c {
private:
public:
  /**
    constructor which can read in initial data from uint8_t string
    @param rpb_src 64bit input data string
  */
  iISOName_c(const uint8_t* rpb_src = NULL) : ISOName_c( rpb_src ) {};
  /**
    constructor which format data string from series of input flags
    @param rb_selfConf true -> indicate sefl configuring ECU
    @param rui8_indGroup industry group of device (2 for agriculture)
    @param rb_devClass device class of ECU (equivalent to GETY in DIN)
    @param rb_devClassInst instance number of ECU with same devClass
          in the network (comparable to POS in DIN9684)
    @param rb_func function of the ECU (usual 25 for network interconnect)
    @param rui16_manufCode code of manufactor (11bit)
    @param rui32_serNo serial no of specific device (21bit)
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  iISOName_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_devClass, uint8_t rb_devClassInst,
        uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0)
        : ISOName_c( rb_selfConf, rui8_indGroup, rb_devClass, rb_devClassInst, rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst ) {};
  /**
    copy constructor for ISOName
    @param rrefc_src source ISOName_c instance
  */
  iISOName_c(const iISOName_c& rrefc_src) : ISOName_c( rrefc_src ) {};
  /**
    assign constructor for ISOName
    @param rrefc_src source ISOName_c object
  */
  const iISOName_c& operator=(const iISOName_c& rrefc_src) { return static_cast<const iISOName_c&>(ISOName_c::operator=( rrefc_src ));};

  /**
    set data string with all flags with one call
    @param rb_selfConf true -> indicate sefl configuring ECU
    @param rui8_indGroup industry group of device (2 for agriculture)
    @param rb_devClass device class of ECU (equivalent to GETY in DIN)
    @param rb_devClassInst instance number of ECU with same devClass
          in the network (comparable to POS in DIN9684)
    @param rb_func function of the ECU (usual 25 for network interconnect)
    @param rui16_manufCode code of manufactor (11bit)
    @param rui32_serNo serial no of specific device (21bit)
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void set(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_devClass, uint8_t rb_devClassInst,
        uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0)
    { ISOName_c::set( rb_selfConf, rui8_indGroup, rb_devClass, rb_devClassInst, rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst );};


  /**
    deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const uint8_t* outputString() const {return ISOName_c::outputString();};

  /**
    get self config mode
    @return self configuration adress state
  */
  uint8_t selfConf() const { return ISOName_c::selfConf();};
  /**
    get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const { return ISOName_c::indGroup();};
  /**
    get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const { return ISOName_c::devClassInst();};
  /**
    get device class code
    @return:device class
  */
  uint8_t devClass() const { return ISOName_c::devClass();};
  /**
    get function code
    @return function code
  */
  uint8_t func() const { return ISOName_c::func();};
  /**
    get function instance code
    @return function instance code
  */
  uint8_t funcInst() const { return ISOName_c::funcInst();};
  /**
    get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const { return ISOName_c::ecuInst();};
  /**
    get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const { return ISOName_c::manufCode();};

  /**
    get serial number
    @return serial number
  */
  uint32_t serNo() const { return ISOName_c::serNo();};


  /**
    set the NAME data from 8 uint8_t string
    @param rpb_src pointer to 8byte source string
  */
  void inputString(const uint8_t* rpb_src) { ISOName_c::inputString( rpb_src );};

  /**
    set self config mode
    @param rb_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool rb_selfConf) { ISOName_c::setSelfConf( rb_selfConf );};
  /**
    set industry group code
    @param rui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t rui8_indGroup) { ISOName_c::setIndGroup( rui8_indGroup );};
  /**
    set device class instance number
    @param rb_devClassInst instance number of ECU with same devClass
          in the network (comparable to POS in DIN9684)
  */
  void setDevClassInst(uint8_t rb_devClassInst) { ISOName_c::setDevClassInst( rb_devClassInst );};
  /**
    set device class code
    @param rb_devClass device class of ECU (equivalent to GETY in DIN)
  */
  void setDevClass(uint8_t rb_devClass) { ISOName_c::setDevClass( rb_devClass );};
  /**
    set function code
    @param rb_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t rb_func) { ISOName_c::setFunc( rb_func );};
  /**
    set function instance code
    @param rb_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t rb_funcInst) { ISOName_c::setFuncInst( rb_funcInst );};
  /**
    set ECU instance code
    @param rb_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t rb_ecuInst) { ISOName_c::setEcuInst( rb_ecuInst );};
  /**
    set manufactor code
    @param rui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t rui16_manufCode) { ISOName_c::setManufCode( rui16_manufCode );};

  /**
    set serial number (Identity Number)
    @param rui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t rui32_serNo) { ISOName_c::setSerNo( rui32_serNo );};

  /**
    check if this NAME has higher prio
    than the given NAME 8-uint8_t string
    @param rpb_compare
    @return 0 == equal; -1 == this has lower prio than par; +1 == this item has higher prio than par
  */
  int8_t higherPriThanPar(const uint8_t* rpb_compare) { return ISOName_c::higherPriThanPar( rpb_compare );};
};

}
#endif
