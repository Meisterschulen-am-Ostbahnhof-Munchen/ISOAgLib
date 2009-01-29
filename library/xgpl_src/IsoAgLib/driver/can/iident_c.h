/***************************************************************************
                          iident.h  -  header file for iIdent
                             -------------------
    begin                : Sun Dec 24 2000
    copyright            : (c) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IIDENT_H
#define IIDENT_H

#include "impl/ident_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** class for management of CAN ident with 
  ident type dpendent compare and setting operations
  @see iCAN_IO
  @see FilterBox
  @see MSgObj
  *@author Dipl.-Inform. Achim Spangler
  */
class iIdent_c : public __IsoAgLib::Ident_c {
public: 
  /**
    @brief enum type for defining the CAN ident length to S (11Bit)
    or extended (29Bit)
  */
  typedef Ident_c::identType_t identType_t;
    
  /** 
    @brief constructor which can set the values
    @param at_ident new ident setting
    @param ren_identType new ident type 
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default: DEFAULT_IDENT_TYPE set in isoaglib_config)
  */
  iIdent_c(MASK_TYPE at_ident = 0, identType_t ren_identType = DEFAULT_IDENT_TYPE)
    : Ident_c(at_ident, ren_identType) {};
  /**
    @brief constructor which gets its values from other instance
    @param acrc_src source Ident_c instance
  */
  iIdent_c(const Ident_c& acrc_src) : Ident_c(acrc_src) {};
  
  /**
    @brief compare this ident setting with another
    (use memory operation for max speed)
    @param acrc_src compared ident
    @return true -> referenced ident has same setting and type
  */
  bool operator==(const Ident_c& acrc_src) const
    {return Ident_c::operator==(acrc_src);};

  /**
    @brief compare this ident setting with another
    (use memory operation for max speed)
    @param acrc_src compared ident
    @return true -> referenced ident has different setting and type
  */
  bool operator!=(const Ident_c& acrc_src) const
    {return Ident_c::operator!=(acrc_src);};
  /**
    @brief compare this ident value and ident type
    @param at_ident compared ident setting
    @param ren_identType type of compared ident
    @return true -> given setting and type are equal
  */
  bool equal(const MASK_TYPE at_ident, identType_t ren_identType) const
    {return Ident_c::equal(at_ident, ren_identType);};
  /**
    @brief:deliver ident value masked by given ident
    @param at_mask mask value
    @return ident setting masked by at_mask (only '1' where mask and ident has '1')
  */
  MASK_TYPE masked(MASK_TYPE at_mask = ~0) const
    {return Ident_c::masked(at_mask);};
  /**
    @brief:deliver ident value masked by given ident
    @param acrc_mask mask value
    @return ident setting masked by at_mask (only '1' where mask and ident has '1')
  */
  MASK_TYPE masked(const Ident_c& acrc_mask = ~0) const
    {return Ident_c::masked(acrc_mask);};
  /**
    @brief update the ident with bitwise AND with given ident setting
    @param rc_bitAnd Ident_c variable with ident to bit_AND
  */
  void ident_bitAnd(const Ident_c& rc_bitAnd)
    {Ident_c::ident_bitAnd(rc_bitAnd);};
    
  /**
    @brief:deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t bit_diff(const Ident_c& acrc_ident) const
    {return Ident_c::bit_diff(acrc_ident);};
  /**
    @brief:deliver the ident type
    @return Ident_c::S for 11bit ident or Ident_c::E for 29bit
  */
  identType_t identType() const {return Ident_c::identType();};
  /**
    @brief:deliver the ident setting
    @return ident value
  */
  MASK_TYPE ident() const {return Ident_c::ident();};
  /**
    @brief:deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_pos
    @return ident value
  */
  MASK_TYPE ident(uint8_t ab_pos) const {return Ident_c::ident(ab_pos);};
  /**
    @brief check if Ident_c is set as empty (needed for MsgObj)
  */
  bool empty() const {return Ident_c::empty();};
  /**
    @brief set the Ident_c to empty state
    @param ab_empty set empty state (default = true)
  */
  void setEmpty(bool ab_empty = true) {Ident_c::setEmpty(ab_empty);};
  /**
    @brief set this ident
    @param at_ident new ident setting
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default defined in isoaglib_config.h)
  */
  void set(MASK_TYPE at_ident = 0, identType_t ren_identType = DEFAULT_IDENT_TYPE)
    {Ident_c::set(at_ident, ren_identType);};
  /**
    @brief set this ident with access to single unsigned char
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_val new val for ident at wanted position
    @param ab_pos position in ident, where ident should be placed in
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default defined in isoaglib_config.h)
  */
  void set(uint8_t ab_val = 0, uint8_t ab_pos = 0, identType_t ren_identType = DEFAULT_IDENT_TYPE)
    {Ident_c::set(ab_val, ab_pos, ren_identType);};
  /**
    @brief set type of ident
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  void setIdentType(iIdent_c::identType_t at_type)
    {Ident_c::setIdentType(at_type);};
  /**
    @brief set this ident according to other Ident
    @param acrc_src source Ident_c instance
  */
  const iIdent_c& operator=(const iIdent_c& acrc_src)
    {return ((const iIdent_c&)Ident_c::operator=(((const Ident_c&)acrc_src)));};
};

}
#endif
