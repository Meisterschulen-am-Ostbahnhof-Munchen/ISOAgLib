/***************************************************************************
                          icanpkgext_c.h - header for extended iCANPkg_c object,
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
#ifndef iCAN_PKG_EXT_H
#define iCAN_PKG_EXT_H


#include "impl/canpkgext_c.h"

// for forward declarations
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#include <IsoAgLib/driver/can/iident_c.h>



// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** extended version of CANPkg_c which overwrites the
    assign and getData functions
    with call for data flag converting functions
  *@author Dipl.-Inform. Achim Spangler
  */

class iCANPkgExt_c : private __IsoAgLib::CANPkgExt_c {
public:
  /**
    deliver the ident
    @return ident setting as MASK_TYPE
  */
  MASK_TYPE ident() const {return CANPkgExt_c::ident(); }
  /**
    deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param rb_pos
    @return ident value
  */
  MASK_TYPE ident(uint8_t rb_pos) const {return CANPkgExt_c::ident(rb_pos); }

  void setIdent(MASK_TYPE rt_ident, IsoAgLib::iIdent_c::identType_t rt_type = DEFAULT_IDENT_TYPE) { CANPkgExt_c::setIdent(rt_ident, rt_type); }

  /**
    set type of ident
    @param rt_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  void setIdentType(IsoAgLib::iIdent_c::identType_t rt_type){CANPkgExt_c::setIdentType(rt_type);}

  /**
    deliver type of Ident_c: 11bit standard or 29bit extended
    @return: Ident_c::S or Ident_c::E
  */
  IsoAgLib::iIdent_c::identType_t identType() {return CANPkgExt_c::identType();}

  /** retrieve len of last received CAN message */
  uint8_t getLen() { return CANPkgExt_c::getLen(); }

  /**
    simply deliver a uint8_t from a specific position with operator[]
    @param rb_pos position of dellivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rb_pos
  */
  uint8_t operator[](uint8_t rb_pos) const {return CANPkgExt_c::operator[](rb_pos); }

  /**
    set an uint8_t value at specified position in string
    @param rui8_pos position [0..7]
    @param rui8_val uint8_t value to set
  */
  void setUint8Data( uint8_t rui8_pos, uint8_t rui8_val) { CANPkgExt_c::setUint8Data( rui8_pos, rui8_val ); }
  /**
    set an uint16_t value at specified position in string
    @param rui8_pos position [0..6]
    @param rui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t rui8_pos, uint16_t rui16_val) { CANPkgExt_c::setUint16Data( rui8_pos, rui16_val ); }
  /**
    set an int16_t value at specified position in string
    @param rui8_pos position [0..6]
    @param ri16_val int16_t value to set
  */
  void setInt16Data( uint8_t rui8_pos, int16_t ri16_val) { CANPkgExt_c::setInt16Data( rui8_pos, ri16_val ); }
  /**
    set an uint32_t value at specified position in string
    @param rui8_pos position [0..4]
    @param rui32_val uint32_t value to set
  */
  void setUint32Data( uint8_t rui8_pos, uint32_t rui32_val) { CANPkgExt_c::setUint32Data( rui8_pos, rui32_val ); }
  /**
    set an int32_t value at specified position in string
    @param rui8_pos position [0..4]
    @param ri32_val int32_t value to set
  */
  void setInt32Data( uint8_t rui8_pos, int32_t ri32_val) { CANPkgExt_c::setInt32Data( rui8_pos, ri32_val ); }

  /**
    simply deliver a uint8_t from a specific position with
    @param rb_pos position of dellivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rb_pos
  */
  uint8_t getUint8Data(uint8_t rb_pos) const {return CANPkgExt_c::getUint8Data( rb_pos ); }
  /**
    simply deliver a uint16_t from a specific starting position with
    @param rb_pos position of dellivered uint16_t [0..6]
    @return uint16_t balue in CAN data string at pos (rb_pos, rb_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t rb_pos) const {return CANPkgExt_c::getUint16Data( rb_pos ); }
  /**
    simply deliver a int16_t from a specific starting position with
    @param rb_pos position of dellivered int16_t [0..6]
    @return int16_t balue in CAN data string at pos (rb_pos, rb_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t rb_pos) const {return CANPkgExt_c::getInt16Data( rb_pos ); }
  /**
    simply deliver a uint32_t from a specific starting position with
    @param rb_pos position of dellivered uint32_t [0..4]
    @return uint32_t balue in CAN data string at pos (rb_pos, rb_pos+1) read Low/High order
  */
  uint32_t getUint32Data(uint8_t rb_pos) const {return CANPkgExt_c::getUint32Data( rb_pos ); }
  /**
    simply deliver a int32_t from a specific starting position with
    @param rb_pos position of dellivered int32_t [0..4]
    @return int32_t balue in CAN data string at pos (rb_pos, rb_pos+1) read Low/High order
  */
  int32_t getInt32Data(uint8_t rb_pos) const {return CANPkgExt_c::getInt32Data( rb_pos ); }

  /**
    deliver time
    @return timestamp of the CAN telegram in [msec.] since system start
  */
  int32_t time() { return CANPkgExt_c::time(); }

  void setExtCanPkg3(uint8_t pri, uint8_t dp, uint8_t pf, uint8_t ps, uint8_t sa, uint8_t d0, uint8_t d1, uint8_t d2)
  { CANPkgExt_c::setExtCanPkg3(pri, dp, pf, ps, sa, d0, d1, d2); }

  void setExtCanPkg8(uint8_t pri, uint8_t dp, uint8_t pf, uint8_t ps, uint8_t sa, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  { CANPkgExt_c::setExtCanPkg8(pri, dp, pf, ps, sa, d0, d1, d2, d3, d4, d5, d6, d7); }

  /**
    put data into given reference to BIOS related data structure with data, len
    @param reft_ident     reference where the ident is placed for send
    @param refui8_identType reference to the ident type val: 0==std, 1==ext
    @param refb_dlcTarget reference to the DLC field of the target
    @param pb_dataTarget pointer to the data string of the target
  */
  virtual void getData(MASK_TYPE& reft_ident, uint8_t& refui8_identType,
                       uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
    {CANPkgExt_c::getData(reft_ident, refui8_identType, refb_dlcTarget, pb_dataTarget); }

  /**
    get the value of the ISO11783 ident field SA
    @return source adress
  */
  uint8_t isoSa() const {return CANPkgExt_c::isoSa(); }
  /**
    get the value of the ISO11783 ident field PGN
    @return parameter group number
  */
  uint32_t isoPgn() const {return CANPkgExt_c::isoPgn(); }
  /**
    get the value of the ISO11783 ident field DP
    @return data page
  */
  uint8_t isoDp() const {return CANPkgExt_c::isoDp(); }
  /**
    get the value of the ISO11783 ident field PF
    @return PDU Format
  */
  uint8_t isoPf() const {return CANPkgExt_c::isoPf(); }
  /**
    get the value of the ISO11783 ident field PS
    @return PDU Specific
  */
  uint8_t isoPs() const {return CANPkgExt_c::isoPs(); }
  /**
    get the value of the ISO11783 ident field PRI
    @return priority
  */
  uint8_t isoPri() const {return CANPkgExt_c::isoPri(); }

  /**
    set the value of the ISO11783 ident field SA
    @return source adress
  */
  void setIsoSa(uint8_t rb_val){CANPkgExt_c::setIsoSa(rb_val); }
  /**
    set the value of the ISO11783 ident field PGN
    @return parameter group number
  */
  void setIsoPgn(uint32_t rui32_val){CANPkgExt_c::setIsoPgn(rui32_val); }
  /**
    set the value of the ISO11783 ident field DP
    @return data page
  */
  void setIsoDp(uint8_t rb_val){CANPkgExt_c::setIsoDp(rb_val); }
  /**
    set the value of the ISO11783 ident field PF
    @return PDU Format
  */
  void setIsoPf(uint8_t rb_val){CANPkgExt_c::setIsoPf(rb_val); }
  /**
    set the value of the ISO11783 ident field PS
    @return PDU Specific
  */
  void setIsoPs(uint8_t rb_val){CANPkgExt_c::setIsoPs(rb_val); }
  /**
    set the value of the ISO11783 ident field PRI
    @return priority
  */
  void setIsoPri(uint8_t rb_val){CANPkgExt_c::setIsoPri(rb_val); }

  /**
    set the data len of CAN pkg (if rb_val is greater than 8, 8 is stored)
    @param rb_val length/amount of uint8_t in data string
  */
  void setLen(uint8_t rb_val){CANPkgExt_c::setLen(rb_val); }


  /** set the monitoritem for resolve SA
      @param pc_monitorItem  needed monitoritem
    */
  void setMonitorItemForSA (iISOItem_c* pc_monitorItem) { CANPkgExt_c::setMonitorItemForSA (pc_monitorItem); }

  /** set the isoName for resolve SA
      @param p_isoName        needed isoName
    */
  void setISONameForSA (const iISOName_c& refc_isoName) { CANPkgExt_c::setISONameForSA (refc_isoName); }

  /** set the monitoritem for resolve SA
      @param pc_monitorItem  needed monitoritem
    */
  void setMonitorItemForDA (iISOItem_c* pc_monitorItem) { CANPkgExt_c::setMonitorItemForDA (pc_monitorItem); }

  /** set the isoName for resolve SA
      @param p_isoName        needed isoName
    */
  void setISONameForDA (const iISOName_c& refc_isoName) { CANPkgExt_c::setISONameForDA (refc_isoName); }

  /** short inline function for setting the Destination address (PS) to global (0xFF)
    */
  void setGlobalDA() { CANPkgExt_c::setGlobalDA(); }

  /** get the monitoritem for resolved SA
    */
  const iISOItem_c* getMonitorItemForSA() { return static_cast<const iISOItem_c*>(CANPkgExt_c::getMonitorItemForSA()); }

  /** set the isoName for resolved SA
    */
  const iISOName_c& getISONameForSA() { return CANPkgExt_c::getISONameForSA().toConstIisoName_c(); }

  /** set the monitoritem for resolved SA
    */
  const iISOItem_c* getMonitorItemForDA() { return static_cast<const iISOItem_c*>(getMonitorItemForDA()); }

  /** set the isoName for resolved SA
    */
  const iISOName_c& getISONameForDA() { return CANPkgExt_c::getISONameForDA().toConstIisoName_c(); }

private:
  friend class iCANIO_c;
  friend class iCANCustomer_c;
};

} // namespace
#endif
