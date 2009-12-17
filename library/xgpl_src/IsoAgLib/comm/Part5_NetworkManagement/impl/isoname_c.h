/*
  isoname_c.h: handles the ISO 64bit NAME field

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_NAME_H
#define ISO_NAME_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>

namespace IsoAgLib {
  class iIsoName_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** handle the 64bit ISO11783 NAME field
  with input/output from/to 8byte string
  and specific read/write access to single
  flags
  @author Dipl.-Inform. Achim Spangler
*/
class IsoName_c {
private:
public:
  /** constant for default parameters and initialization, where the device type is not yet spcified.
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const IsoName_c& IsoNameUnspecified();

  /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
      the instantiation of this constant variable is located in the module cancustomer_c.cpp
    */
  static const IsoName_c& IsoNameInitialProcessData();

  /** default constructor
    using "explicit" to avoid WRONG implicit cast from SA to ISONAME!
    @param aui8_devClass     initial DEVCLASS (device type)
    @param aui8_devClassInst initial DEVCLASSINST (instance). Defaults to "unknown" (=0xF)
  */
  explicit IsoName_c( uint8_t aui8_devClass, uint8_t aui8_devClassInst=0xF )
  { set( true, 2, aui8_devClass, aui8_devClassInst, 0xFF, 0x7FF, 0x1FFFFF, 0x1F, 0x7 ); }

  /** constructor which can read in initial data from uint8_t string
    @param apb_src 64bit input data string, mustn't be NULL.
                   (if NULL, currently all fields are set to 0, but future behaviour is not defined.)
  */
  IsoName_c(const uint8_t* apb_src);

  /** (default) constructor which will set the IsoName to Unspecified */
  IsoName_c();

  /** constructor which can read in initial data from uint8_t string
    @param apu_src 64bit input data string
  */
  IsoName_c(const Flexible8ByteString_c* apu_src);

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
  IsoName_c(bool ab_selfConf, uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
        uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0);
  /** copy constructor for ISOName
    @param acrc_src source IsoName_c instance
  */
  IsoName_c(const IsoName_c& acrc_src);

  /** assign constructor for ISOName
    @param acrc_src source IsoName_c object
  */
  const IsoName_c& operator=(const IsoName_c& acrc_src);

  /** default destructor */
  ~IsoName_c();

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
        uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0);

  /** set device class & instance with two seperate parameters */
  void set( uint8_t aui8_devClass, uint8_t aui8_devClassInst );

  /** set this instance to indicator for unspecified value */
  void setUnspecified( void ) { setDevClass( 0x7F ); setDevClassInst( 0xF );}

  /** check if this instance has specified value (different from default) */
  bool isSpecified( void ) const { return ( ( devClass() != 0x7F ) || ( devClassInst() != 0xF ) );}

  /** check if this instance has unspecified value (match default) */
  bool isUnspecified( void ) const { return ( ( devClass() == 0x7F ) && ( devClassInst() == 0xF ) );}

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
  const uint8_t* outputString() const{return mu_data.getUint8DataConstPointer();}

  /** deliver the data NAME string as pointer to 8byte string
    @return const pointer to 8 uint8_t string with NAME
  */
  const Flexible8ByteString_c* outputUnion() const{return &mu_data;}

  /** get self config mode
    @return self configuration adress state
  */
  bool selfConf() const;

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
    @param apb_src pointer to 8byte source string
  */
  void inputString(const uint8_t* apb_src);

  /** set the NAME data from 8 uint8_t string
    @param apu_src pointer to 8byte source string
  */
  void inputUnion(const Flexible8ByteString_c* apu_src);

  /** set self config mode
    @param ab_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool ab_selfConf);

  /** set industry group code
    @param aui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t aui8_indGroup);

  /** set device class instance number
    @param aui8_devClassInst instance number of ECU with same devClass in the network
  */
  void setDevClassInst(uint8_t aui8_devClassInst);

  /** set device class code
    @param aui8_devClass device class of ECU
  */
  void setDevClass(uint8_t aui8_devClass);

  /** set function code
    @param ab_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t ab_func);

  /** set function instance code
    @param ab_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t ab_funcInst);

  /** set ECU instance code
    @param ab_ecuInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t ab_ecuInst);

  /** set manufactor code
    @param aui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t aui16_manufCode);

  /** set serial number (Identity Number)
    @param aui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t aui32_serNo);

  /** check if this NAME has higher prio
    than the given NAME 8-uint8_t string
    @param apu_compare
    @return 0 == equal; -1 == this has lower prio than par; +1 == this item has higher prio than par
  */
  int8_t higherPriThanPar(const Flexible8ByteString_c* apu_compare) const;

  /** Check if all Non-Instance fields of both ISONames match
    @return true if equal, false if one non-inst field differs!
  */
  bool isEqualRegardingNonInstFields (const IsoName_c& acrc_isoName) const;

  /** compare two IsoName_c values with operator== */
  bool operator==( const IsoName_c& rc_right ) const
    { return (higherPriThanPar( rc_right.outputUnion() ) == 0);}

  /** compare two IsoName_c values with operator!= */
  bool operator!=( const IsoName_c& rc_right ) const
    { return (higherPriThanPar( rc_right.outputUnion() ) != 0);}

  /** compare IsoName_c value and Flexible8ByteString_c with operator!= */
  bool operator!=( const Flexible8ByteString_c& acrc_right ) const
    { return (higherPriThanPar( &acrc_right ) != 0);}

  /** compare two IsoName_c values with operator< */
  bool operator<( const IsoName_c& rc_right ) const
    { return (higherPriThanPar( rc_right.outputUnion() ) == -1);}

  /** convert function */
  IsoAgLib::iIsoName_c& toIisoName_c();

  /** convert function */
  const IsoAgLib::iIsoName_c& toConstIisoName_c() const;

private:
  /** ISO 8-uint8_t NAME field */
  Flexible8ByteString_c mu_data;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoName_c ISOName_c;

}
#endif
