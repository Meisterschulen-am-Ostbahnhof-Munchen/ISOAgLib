/***************************************************************************
                          canpkgext_c.h - header for extended CanPkg_c object,
                                          which calls data flag converting
                                          functions on assign operations
                             -------------------
    begin                : Tue Jan 11 2000
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#ifndef CAN_PKG_EXT_H
#define CAN_PKG_EXT_H

#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class IsoName_c;
class IsoItem_c;


/** structure which will be filled when an address is resolved */
class AddressResolveResults_c
{
  public:
    AddressResolveResults_c();
    void init( uint8_t* aui8_address );
    ~AddressResolveResults_c();

    // IMPORTANT:
    // the IsoName_c instance should be handled as if it would be a fixed
    // _part_ of the struct --> the constructor allocates an instance, the destructur frees it
    //                          and all other operations just operate on the allocated instance.
    IsoName_c* mpc_isoName;
    // IMPORTANT:
    // in contrast to IsoName_c, the MonitorItem_c _POINTER_ is always set to NULL or to an instance, that
    // is located somewhere else (thus do NEVER NEVER NEVER call new or delete for this entry!!!!!!!!)
    IsoItem_c* mpc_monitorItem;
    //can be source or destination address
    uint8_t* mpui8_address;
} ;


typedef enum Scope_en
{
  local  = 0,
  remote = 1
} Scope;


/** extended version of CanPkg_c which overwrites the
    assign and getData functions
    with call for data flag converting functions
  *@author Dipl.-Inform. Achim Spangler
  */
class CanPkgExt_c : public CanPkg_c
{
 private:
  CanPkgExt_c( const CanPkgExt_c&);           //!< prevent copy constructor use: do not code this method
  CanPkgExt_c& operator=(const CanPkgExt_c&); //!< prevent copy operator use: do not code this method
 public:
  /** default constructor, which has nothing to do */
  CanPkgExt_c( int ai_singletonVecKey = 0 );

  /** virtual default destructor, which has nothing to do */
  virtual ~CanPkgExt_c();

  /**
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    assign operator to insert informations from one CanPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CanPkgExt_c::operator=
    @see CanPkgExt_c::getData
    @param arc_right reference to the source CanPkg_c on the right
    @return reference to the source CanPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  virtual const CanPkg_c& operator=(const CanPkg_c& arc_right);
  */

  /**
    simply deliver a uint8_t from a specific position with operator[]
    @param ab_pos position of dellivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos ab_pos
  */
  uint8_t operator[](uint8_t aui8_pos) const {return msc_data[aui8_pos];}

  /**
    set an uint8_t value at specified position in string
    @param aui8_pos position [0..7]
    @param aui8_val uint8_t value to set
  */
  void setUint8Data( uint8_t aui8_pos, uint8_t aui8_val)
    { msc_data.setUint8Data(aui8_pos, aui8_val);}

  /**
    set an uint16_t value at specified position in string
    @param aui8_devClassInst position [0..6]
    @param aui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t aui8_pos, uint16_t aui16_val)
    { msc_data.setUint16Data(aui8_pos, aui16_val);}

  /**
    set an int16_t value at specified position in string
    @param aui8_devClassInst position [0..6]
    @param ai16_val int16_t value to set
  */
  void setInt16Data( uint8_t aui8_pos, int16_t ai16_val)
    { msc_data.setInt16Data(aui8_pos, ai16_val);}

  /**
    set an uint32_t value at specified position in string
    @param aui8_devClassInst position [0..4]
    @param aui32_val uint32_t value to set
  */
  void setUint32Data( uint8_t aui8_pos, uint32_t aui32_val)
    { msc_data.setUint32Data(aui8_pos, aui32_val);}

  /**
    set an int32_t value at specified position in string
    @param aui8_devClassInst position [0..4]
    @param ai32_val int32_t value to set
  */
  void setInt32Data( uint8_t aui8_pos, int32_t ai32_val)
    { msc_data.setInt32Data(aui8_pos, ai32_val);}

  /**
    set an float value at specified position in string
    @param aui8_pos position [0..4]
    @param af_val float value to set
   */
  void setFloatData( uint8_t aui8_pos, float af_val)
  { msc_data.setFloatData(aui8_pos, af_val);}
  /**
    simply deliver a uint8_t from a specific position with
    @param ab_pos position of dellivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos ab_pos
  */
  uint8_t getUint8Data(uint8_t aui8_pos) const {return msc_data.getUint8Data(aui8_pos);}

  /**
    simply deliver a uint16_t from a specific starting position with
    @param ab_pos position of dellivered uint16_t [0..6]
    @return uint16_t balue in CAN data string at pos (ab_pos, ab_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t aui8_pos) const {return msc_data.getUint16Data(aui8_pos);}

  /**
    simply deliver a int16_t from a specific starting position with
    @param ab_pos position of dellivered int16_t [0..6]
    @return int16_t balue in CAN data string at pos (ab_pos, ab_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t aui8_pos) const {return msc_data.getInt16Data(aui8_pos);}

  /**
    simply deliver a uint32_t from a specific starting position with
    @param ab_pos position of dellivered uint32_t [0..4]
    @return uint32_t balue in CAN data string at pos (ab_pos, ab_pos+1) read Low/High order
  */
  uint32_t getUint32Data(uint8_t aui8_pos) const {return msc_data.getUint32Data(aui8_pos);}

  /**
    simply deliver a int32_t from a specific starting position with
    @param ab_pos position of dellivered int32_t [0..4]
    @return int32_t balue in CAN data string at pos (ab_pos, ab_pos+1) read Low/High order
  */
  int32_t getInt32Data(uint8_t aui8_pos) const {return msc_data.getInt32Data(aui8_pos);}

  /**
    simply deliver a float from a specific starting position with
    @param aui8_pos position [0..4]
    @return af_val float to return
   */
  float getFloatData( uint8_t aui8_pos) const { return msc_data.getFloatData(aui8_pos);}

  /**
    put data into given reference to BIOS related data structure with data, len
    @param rt_ident     reference where the ident is placed for send
    @param rui8_identType reference to the ident type val: 0==std, 1==ext
    @param rb_dlcTarget reference to the DLC field of the target
    @param pb_dataTarget pointer to the data string of the target
  */
  virtual void getData(uint32_t& rt_ident, uint8_t& rui8_identType,
                       uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget);

  /** is the currently handled message a network management message
      @return  true -> message is a network management message
     */
  virtual bool isNetworkMgmt() const { return false; }

  // begin of block with ISO 11783 CAN formating functions
  /**
    get the value of the ISO11783 ident field SA
    @return source adress
  */
  uint8_t isoSa() const;

  /**
    get the value of the ISO11783 ident field PGN
    @return parameter group number
  */
  uint32_t isoPgn() const {return ( ( ( (uint32_t)ident() >> 8) & 0x3FF00 ) | isoPs() );}

  /**
    get the value of the ISO11783 ident field DP
    @return data page
  */
  uint8_t isoDp() const {return (ident(3) & 0x1);}

  /**
    get the value of the ISO11783 ident field PF
    @return PDU Format
  */
  uint8_t isoPf() const {return ident(2);}

  /**
    get the value of the ISO11783 ident field PS
    @return PDU Specific
  */
  uint8_t isoPs() const;

  /**
    get the value of the ISO11783 ident field PRI
    @return priority
  */
  uint8_t isoPri() const {return (ident(3) >> 2);}

  /**
    set the value of the ISO11783 ident field SA
    @return source adress
  */
  void setIsoSa(uint8_t aui8_val);

  /**
    set the value of the ISO11783 ident field PGN
    @return parameter group number
  */
  void setIsoPgn(uint32_t aui32_val);

  bool resolveAddress(AddressResolveResults_c& arc_addressResolveResults);

  /**
    set the value of the ISO11783 ident field DP
    @return data page
  */
  void setIsoDp(uint8_t aui8_val) {setIdent( ((ident(3)& 0x1C) | (aui8_val & 0x03)), 3, Ident_c::ExtendedIdent);}

  /**
    set the value of the ISO11783 ident field PF
    @return PDU Format
  */
  void setIsoPf(uint8_t aui8_val) {setIdent(aui8_val, 2, Ident_c::ExtendedIdent); }

  /** combined setting of DP and PF field in identifier as 10Bit part of a 16-Bit parameter
      @param aui16_dpPf DP and PF for the PGN
    */
  void setIsoDpPf( uint16_t aui16_dpPf ) { setIdentWord( (((ident(3)& 0x1C)<<8) | (aui16_dpPf & 0x03FF)), 1, Ident_c::ExtendedIdent);}

  /**
    set the value of the ISO11783 ident field PS
    @return PDU Specific
  */
  void setIsoPs(uint8_t aui8_val);

  /**
    set the value of the ISO11783 ident field PRI
    @return priority
  */
  void setIsoPri(uint8_t aui8_val){setIdent( ((ident(3)&1) | (aui8_val << 2)), 3, Ident_c::ExtendedIdent);}

  void setExtCanPkg(uint8_t pri, uint8_t dp, uint8_t pf, uint8_t ps, uint8_t sa, uint8_t len) {
    CanPkg_c::setIdentType(Ident_c::ExtendedIdent);
    setIsoPri(pri);
    setIsoDp(dp);
    setIsoPf(pf);
    setIsoPs(ps);
    setIsoSa(sa);
    setLen (len);
    msb_runFlag2String = false;
  }

  void setExtCanPkg3(uint8_t pri, uint8_t dp, uint8_t pf, uint8_t ps, uint8_t sa, uint8_t d0, uint8_t d1, uint8_t d2) {
    CanPkg_c::setIdentType(Ident_c::ExtendedIdent);
    setIsoPri(pri);
    setIsoDp(dp);
    setIsoPf(pf);
    setIsoPs(ps);
    setIsoSa(sa);
    setUint8Data (0, d0);
    setUint8Data (1, d1);
    setUint8Data (2, d2);
    setLen (3);
    msb_runFlag2String = false;
  }

  // This function sets the DLC to 8 and fills up
  // the databytes AFTER lenActualData with 0xFF
  // Example: WS-Master has only first databyte used, rest is filled with 0xFFs
  //          call setLen8FillUpWithReserved(1)
  void setLen8FillUpWithReserved (uint8_t lenActualData)
  {
    for (; lenActualData < 8; lenActualData++)
      setUint8Data(lenActualData, 0xFF);
    setLen(8);
  }

  void setExtCanPkg8(uint8_t pri, uint8_t dp, uint8_t pf, uint8_t ps, uint8_t sa, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    CanPkg_c::setIdentType(Ident_c::ExtendedIdent);
    setIsoPri(pri);
    setIsoDp(dp);
    setIsoPf(pf);
    setIsoPs(ps);
    setIsoSa(sa);
    setUint8Data (0, d0);
    setUint8Data (1, d1);
    setUint8Data (2, d2);
    setUint8Data (3, d3);
    setUint8Data (4, d4);
    setUint8Data (5, d5);
    setUint8Data (6, d6);
    setUint8Data (7, d7);
    setLen (8);
    msb_runFlag2String = false;
  }

  /** check if source and destination address are valid
      @see     FilterBox_c::processMsg()
      @pre     we want to process a message
      @return  Valid -> both addresses are valid
               Invalid -> one or both addresses are invalid
               OnlyNetworkMgmt -> one or both addresses are only useable for network management
    */
  MessageState_t resolveReceivingInformation();

  /** check if source and destination address are valid
      @see     CanPkgExt_c::operator<<
      @pre     we want to send a message
      @return  Valid -> both addresses are valid
               Invalid -> one or both addresses are invalid
               OnlyNetworkMgmt -> one or both addresses are only useable for network management
    */
  bool resolveSendingInformation();

  /** set the monitoritem for resolve SA
      @param apc_monitorItem  needed monitoritem
    */
  void setMonitorItemForSA( IsoItem_c* apc_monitorItem );

  /** set the isoName for resolve SA
      @param arc_isoName        needed isoName
    */
  void setISONameForSA( const IsoName_c& arc_isoName );

  /** set the monitoritem for resolve SA
      @param apc_monitorItem  needed monitoritem
    */
  void setMonitorItemForDA( IsoItem_c* apc_monitorItem );

  /** set the isoName for resolve SA
      @param arc_isoName        needed isoName
    */
  void setISONameForDA( const IsoName_c& arc_isoName );

  uint8_t checkMonitorItemISOName( const AddressResolveResults_c& arc_addressResolveResults ) const;

  /** short inline function for setting the Destination address (PS) to global (0xFF)
    */
  void setGlobalDA() { setIsoPs (0xFF); }

  /** get the monitoritem for resolved SA
    */
  IsoItem_c* getMonitorItemForSA() { return mc_addrResolveResSA.mpc_monitorItem; }

  /** get the isoName for resolved SA
    */
  const IsoName_c& getISONameForSA() { return *mc_addrResolveResSA.mpc_isoName; }

  /** set the monitoritem for resolved SA
    */
  IsoItem_c* getMonitorItemForDA() { return mc_addrResolveResDA.mpc_monitorItem; }

  /** set the isoName for resolved DA
    */
  const IsoName_c& getISONameForDA() { return *mc_addrResolveResDA.mpc_isoName; }


  #ifdef ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
    /** this virtual function can be used to detect CAnCustomer_c derived CAN message handlers, which
        want to send/receive proprietary messages on a CanIo_c instance, which is used to transport
        standardized protocol ISO 11783.
        Any proprietary derived CAN message handler should overload this function to return true in this function.
    */
    virtual bool isProprietaryMessageOnStandardizedCan() const { return false;}
  #endif // end of ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL

  /**
    abstract function to transform the string data into flag values
    => derived class must implement suitable data conversion function

    needed for assigning informations from another CanPkg_c or CANPkgExt
    @see CanPkgExt_c::operator=
  */
  virtual void string2Flags();

private:
// Private methods
  /** report if the combination of address and scope is valid in context of message processing
      @return  true -> address, scope combination is valid
    */
  MessageState_t address2IdentRemoteSa();

  /** report if the combination of address and scope is valid in context of message processing
      @return  true -> address, scope combination is valid
    */
  MessageState_t address2IdentLocalDa();

  /** set address in context of sending a message
      @param  addressResolveResults  source or destination address
      @param  scope                  local or remote
      @return  true -> monitoritem_c, isoName_c is a valid combination
    */
  MessageState_t setAddress(AddressResolveResults_c& arc_addressResolveResults, Scope scope);

  /** resolve a given monitoritem and get address if possible
      @param  addressResolveResults  address to resolve
      @return true -> monitoritem could be resolved
              false -> nothing more to be done
    */
  bool resolveMonitorItem( AddressResolveResults_c& arc_addressResolveResults );

  /**
    abstract transform flag values to data string
    => derived class must implement suitable data converting function

    needed for sending informations from this object via CanIo_c on CAN BUS,
    because CanIo_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CanPkgExt_c::getData
    @see __IsoAgLib::CanIo_c::operator<<
  */
  virtual void flags2String();

  /** flag to decide if flags2String has to be executed during send.
      Normally this is needed, but after each call of setExtCanPkg?? all bytes of the stream are already setup.
    */
  static bool msb_runFlag2String;

  /** variable which holds the results for a resolved source address */
  static AddressResolveResults_c mc_addrResolveResSA;

  /** variable which holds the results for a resolved destination address */
  static AddressResolveResults_c mc_addrResolveResDA;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef CanPkgExt_c CANPkgExt_c;

}
#endif
