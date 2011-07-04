/*
  ident_c.h: header file for Ident

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDENT_H
#define IDENT_H

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/hal/hal_can.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** class for management of CAN ident with
  ident type dpendent compare and setting operations
  @see CAN_IO
  @see FilterBox
  @see MSgObj
  @author Dipl.-Inform. Achim Spangler
  */
class Ident_c {
public:
  /** enum type for defining the CAN ident length to S (11Bit)
    or extended (29Bit)
  */

enum identType_t {StandardIdent = 0, ExtendedIdent = 1};

  /** constructor which can set the values
    @param at_ident new ident setting
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
  */
  Ident_c(MASK_TYPE at_ident, identType_t ren_identType);

  /** constructor which gets its values from other instance
    @param acrc_src source Ident_c instance
  */
  Ident_c(const Ident_c& acrc_src);

  /** destructor which has nothing to do */
  ~Ident_c();

  /** compare this ident setting with another
    (use memory operation for max speed)
    @param acrc_src compared ident
    @return true -> referenced ident has same setting and type
  */
  bool operator==(const Ident_c& acrc_src) const;

  /** compare this ident setting with another
    (use memory operation for max speed)
    @param acrc_src compared ident
    @return true -> referenced ident has different setting and type
  */
  bool operator!=(const Ident_c& acrc_src) const
    {return (!(operator==(acrc_src)));}

  /** compare this ident value and ident type
    @param at_ident compared ident setting
    @param ren_identType type of compared ident
    @return true -> given setting and type are equal
  */
  bool equal(const MASK_TYPE at_ident, identType_t ren_identType) const
    {return ((mt_ident == at_ident)&&(mt_type == ren_identType));}

  /** deliver ident value masked by given ident
    @param at_mask mask value
    @return ident setting masked by at_mask (only '1' where mask and ident has '1')
  */
  MASK_TYPE masked(MASK_TYPE at_mask = ~0) const
    {return (mt_type == StandardIdent)?(at_mask & mt_ident & 0x7FF):(at_mask & mt_ident);}

  /** deliver ident value masked by given ident
    @param acrc_mask mask value
    @return ident setting masked by at_mask (only '1' where mask and ident has '1')
  */
  MASK_TYPE masked(const Ident_c& acrc_mask) const
    {return (mt_type == StandardIdent) ?(acrc_mask.mt_ident & mt_ident & 0x7FF):(acrc_mask.mt_ident & mt_ident);}

  /** update the ident with bitwise AND with given ident setting
    @param rc_bitAnd Ident_c variable with ident to bit_AND
  */
  void ident_bitAnd(const Ident_c& rc_bitAnd)
    {if (rc_bitAnd.mt_type == mt_type) mt_ident &= rc_bitAnd.mt_ident;}

  /** deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t bit_diff(const Ident_c& acrc_ident) const;
  /**
    deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @param at_mask
    @return amount of different bits
  */
  uint8_t bitDiffWithMask(const Ident_c& acrc_ident, MASK_TYPE at_mask, unsigned int& ui_lsbFromDiff) const;
  /**
    deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t bitDiff(const Ident_c& acrc_ident, unsigned int& ui_lsbFromDiff) const;
  /** update the ident value with the given mask --> clear any bit in ident, which are not set in given mask.
      Update the mask only, when the ident type of the referenced mask is the same.
    */
  void updateWithMask( const Ident_c& acrc_mask )
    { if ( mt_type == acrc_mask.mt_type ) mt_ident &= acrc_mask.mt_ident;}
  /**
    deliver the ident type
    @return Ident_c::S for 11bit ident or Ident_c::E for 29bit
  */
  identType_t identType() const {return mt_type;}

  /** deliver the ident setting
    @return ident value
  */
  MASK_TYPE ident() const {return mt_ident;}

  /** deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_pos
    @return ident value
  */
  uint8_t ident(uint8_t ab_pos) const {return static_cast<uint8_t>((mt_ident >> (ab_pos*8)) & 0xFF);}

  /** set this ident
    @param at_ident new ident setting
    @param ren_identType new ident type (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
  */
  void set(MASK_TYPE at_ident, identType_t ren_identType);

  /** set this ident with access to single unsigned char
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_val new val for ident at wanted position
    @param ab_pos position in ident, where ident should be placed in
  */
  void setByte(uint8_t ab_val, uint8_t ab_pos);

  /**
    set specific uint16_t of this ident
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param aui16_val value for ident at wanted position for the telegram
    @param aui8_pos position [0..1] for wanted value for ident for the telegram (pos0==byte0, pos1==byte2)
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  void setWord(uint16_t aui16_val, uint8_t aui8_pos);

  /** set type of ident
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  void setIdentType(Ident_c::identType_t at_type){mt_type = at_type;}

  /** set this ident according to other Ident
    @param acrc_src source Ident_c instance
  */
  const Ident_c& operator=(const Ident_c& acrc_src) {
    mt_ident = acrc_src.mt_ident;
    mt_type = acrc_src.mt_type;
    return acrc_src;
  }

private:
  MASK_TYPE mt_ident;
  identType_t mt_type;
};
}

#endif
