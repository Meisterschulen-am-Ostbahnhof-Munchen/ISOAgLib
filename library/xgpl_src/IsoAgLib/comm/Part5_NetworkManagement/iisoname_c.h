/***************************************************************************
                          iiso_name.h - handles the ISO 64bit NAME field
                             -------------------
    begin                : Wed Jan 03 2001
    copyright            : (C) 2001 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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

/// The following define is only to be used in Variable-definitions when
/// "IsoAgLib::iIsoName_c(IsoAgLib::iIsoName_c::iIsoNameUnspecified)"
/// does NOT work as "iIsoNameUnspecified" itself is (probably!!) not yet
/// constructed. This may be the case if the variable-definition order
/// indicates that it gets initialized later then it's being used.
#define IISONAME_UNSPECIFIED_CONSTRUCTOR IsoAgLib::iIsoName_c(0x7F, 0xF)

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isoname_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{ // forward declarations
  class ProprietaryMessageHandler_c;
  class ProprietaryMessageClient_c;
  class TimePosGps_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {

// forward declarations
class iEepromIo_c;
class iProprietaryMessageHandler_c;
class iProprietaryMessageClient_c;

/** handle the 64bit ISO11783 NAME field
  with input/output from/to 8byte string
  and specific read/write access to single
  flags
  @author Dipl.-Inform. Achim Spangler
*/
#ifdef __IAR_SYSTEMS_ICC__
// Using IAR it is not allowed to static_cast from private or protected base.
class iIsoName_c : public __IsoAgLib::IsoName_c
#else
class iIsoName_c : private __IsoAgLib::IsoName_c
#endif
{
public:
  /** constant for default parameters and initialization, where the device type is not yet spcified.
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const iIsoName_c& iIsoNameUnspecified() { return IsoNameUnspecified().toConstIisoName_c(); }

  /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const iIsoName_c& iIsoNameInitialProcessData() { return IsoNameInitialProcessData().toConstIisoName_c(); }

  /** constant for default parameters and initialization, where the device type is not yet spcified.
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
      THIS VARIANT IS ONLY FOR BACKWARD COMPATIBILITY AT API LEVEL
    */
  static const iIsoName_c& iISONameUnspecified() { return iIsoNameUnspecified(); }

  /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
      THIS VARIANT IS ONLY FOR BACKWARD COMPATIBILITY AT API LEVEL
    */
  static const iIsoName_c& iISONameInitialProcessData() { return iIsoNameInitialProcessData(); }

  /** default constructor
    using "explicit" to avoid WRONG implicit cast from SA to ISONAME!
    @param aui8_devClass     initial DEVCLASS (device type)
    @param aui8_devClassInst initial DEVCLASSINST (instance). Defaults to "unknown" (=0xF)
    */
  explicit iIsoName_c( uint8_t aui8_devClass, uint8_t aui8_devClassInst=0xF)
  : IsoName_c( aui8_devClass, aui8_devClassInst ) {}

  /** constructor which format data string from series of input flags
    @param ab_selfConf true -> indicate sefl configuring ECU
    @param aui8_indGroup industry group of device (2 for agriculture)
    @param aui8_devClass device class of ECU
    @param aui8_devClassInst instance number of ECU with same devClass in the network
    @param ab_func function of the ECU (usual 25 for network interconnect)
    @param aui16_manufCode code of manufactor (11bit)
    @param aui32_serNo serial no of specific device (21bit)
    @param ab_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param ab_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  iIsoName_c(bool ab_selfConf, uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
        uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0)
        : IsoName_c( ab_selfConf, aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo, ab_funcInst, ab_ecuInst ) {}

  /** constructor which can read in initial data from uint8_t string
    @param apb_src 64bit input data string
  */
  iIsoName_c(const uint8_t* apb_src = NULL) : IsoName_c(apb_src) {}

  /** copy constructor for ISOName
    @param acrc_src source IsoName_c instance
  */
  iIsoName_c(const iIsoName_c& acrc_src) : IsoName_c( acrc_src ) {}

  /** assign constructor for ISOName
    @param acrc_src source IsoName_c object
  */
  const iIsoName_c& operator=(const iIsoName_c& acrc_src)
    { return IsoName_c::operator=( acrc_src ).toConstIisoName_c();}

  /** compare two iIsoName_c values with operator< */
  bool operator<( const iIsoName_c& rc_right ) const
    { return IsoName_c::operator < (rc_right);}

  /** compare two IsoName_c values with operator== */
  bool operator==( const iIsoName_c& rc_right ) const
    { return IsoName_c::operator==(rc_right);}

  /** compare two IsoName_c values with operator!= */
  bool operator!=( const iIsoName_c& rc_right ) const
    { return IsoName_c::operator!=(rc_right);}

  /** set data string with all flags with one call
    @param ab_selfConf true -> indicate sefl configuring ECU
    @param aui8_indGroup industry group of device (2 for agriculture)
    @param aui8_devClass device class of ECU
    @param aui8_devClassInst instance number of ECU with same devClass in the network
    @param ab_func function of the ECU (usual 25 for network interconnect)
    @param aui16_manufCode code of manufactor (11bit)
    @param aui32_serNo serial no of specific device (21bit)
    @param ab_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
    @param ab_funcInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void set(bool ab_selfConf, uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
        uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0)
    { IsoName_c::set( ab_selfConf, aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo, ab_funcInst, ab_ecuInst );}


  /** deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const uint8_t* outputString() const {return IsoName_c::outputString();}

  /** get self config mode
    @return self configuration adress state
  */
  bool selfConf() const { return IsoName_c::selfConf();}

  /** get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const { return IsoName_c::indGroup();}

  /** get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const { return IsoName_c::devClassInst();}

  /** get device class code
    @return:device class
  */
  uint8_t devClass() const { return IsoName_c::devClass();}

  /** get function code
    @return function code
  */
  uint8_t func() const { return IsoName_c::func();}

  /** get function instance code
    @return function instance code
  */
  uint8_t funcInst() const { return IsoName_c::funcInst();}

  /** get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const { return IsoName_c::ecuInst();}

  /** get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const { return IsoName_c::manufCode();}

  /** get serial number
    @return serial number
  */
  uint32_t serNo() const { return IsoName_c::serNo();}

  /** set the NAME data from 8 uint8_t string
    @param apb_src pointer to 8byte source string
  */
  void inputString(const uint8_t* apb_src) { IsoName_c::inputString( apb_src );}

  /** set self config mode
    @param ab_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool ab_selfConf) { IsoName_c::setSelfConf( ab_selfConf );}

  /** set industry group code
    @param aui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t aui8_indGroup) { IsoName_c::setIndGroup( aui8_indGroup );}

  /** set device class instance number
    @param aui8_devClassInst instance number of ECU with same devClass
          in the network
  */
  void setDevClassInst(uint8_t aui8_devClassInst) { IsoName_c::setDevClassInst( aui8_devClassInst );}

  /** set device class code
    @param aui8_devClass device class of ECU
  */
  void setDevClass(uint8_t aui8_devClass) { IsoName_c::setDevClass( aui8_devClass );}

  /** set function code
    @param ab_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t ab_func) { IsoName_c::setFunc( ab_func );}

  /** set function instance code
    @param ab_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t ab_funcInst) { IsoName_c::setFuncInst( ab_funcInst );}

  /** set ECU instance code
    @param ab_ecuInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t ab_ecuInst) { IsoName_c::setEcuInst( ab_ecuInst );}

  /** set manufactor code
    @param aui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t aui16_manufCode) { IsoName_c::setManufCode( aui16_manufCode );}

  /** set serial number (Identity Number)
    @param aui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t aui32_serNo) { IsoName_c::setSerNo( aui32_serNo );}

  /** set this instance to indicator for unspecified value */
  void setUnspecified( void ) { IsoName_c::setUnspecified( );}

  /** check if this instance has specified value (different from default) */
  bool isSpecified( void ) const { return IsoName_c::isSpecified( );}

  /** check if this instance has unspecified value (match default) */
  bool isUnspecified( void ) const { return IsoName_c::isUnspecified( );}

private:
  friend class iIsoItem_c;
  friend class iIdentItem_c;
  friend class iCanPkgExt_c;
  friend class iIsoMonitor_c;
  friend class iMultiSend_c;
  friend class iMultiReceive_c;
  friend class iTimePosGps_c;
  friend class iTracMove_c;
  friend class iTracMoveSetPoint_c;
  friend class iTracGeneral_c;
  friend class iTracPTO_c;
  friend class iTracPTOSetPoint_c;
  friend class iTracLight_c;
  friend class iTracFacilities_c;
  friend class iTracAux_c;
  friend class iTracCert_c;
  friend class iTracGuidance_c;
  friend class iTracGuidanceCommand_c;
  friend class IsoName_c;
  friend class iProcess_c;
  friend class iProcDataLocal_c;
  friend class iProcDataRemote_c;
  friend class iProcDataLocalSimpleMeasure_c;
  friend class iProcDataRemoteSimpleMeasure_c;
  friend class iMeasureProgLocal_c;
  friend class iSetpointRegister_c;
  friend class iProcDataLocalSimpleSetpoint_c;
  friend class iProcDataRemoteSimpleSetpoint_c;
  friend class iProcDataLocalSimpleSetpointSimpleMeasure_c;
  friend class iProcDataRemoteSimpleSetpointSimpleMeasure_c;
  friend class iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c;
  friend class iMeasureProgRemote_c;
  friend iEepromIo_c& operator<<(iEepromIo_c& rc_stream, const iIsoName_c& rc_data );
  friend iEepromIo_c& operator>>(iEepromIo_c& rc_stream, iIsoName_c& rc_data );
  friend struct iIsoFilter_s;
  friend class iProprietaryMessageHandler_c;
  friend class iProprietaryMessageClient_c;
  friend class __IsoAgLib::ProprietaryMessageClient_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
  friend class __IsoAgLib::TimePosGps_c;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iIsoName_c iISOName_c;


}
#endif
