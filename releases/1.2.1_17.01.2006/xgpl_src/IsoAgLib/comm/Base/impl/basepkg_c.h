/***************************************************************************
                          basepkg_c.h  - header file for CAN telgram
                                        encapsulating object, which presents
                                        the base msg informations through
                                        easy flag access; manages base msg
                                        type 1,2 and lower priority
                                        msg like calendar
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef BASE_PKG_H
#define BASE_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

#define BABO_ARR_POS 0
#define SEND_ARR_POS 1

/**
  header file for CAN telgram
  encapsulating object, which presents
  the base msg informations through
  easy flag access; manages base msg
  type 1,2 and lower priority
  msg like calendar
  *@author Dipl.-Inform. Achim Spangler
  */
class BasePkg_c : public CANPkgExt_c  {
public:
  /** default constructor which has nothing to do */
  BasePkg_c();
  /** default destructor which has nothing to do */
  ~BasePkg_c();
  /**
    assign operator to insert informations from one CANPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CANPkgExt_c::operator=
    @see CANPkgExt_c::getData
    @param rrefc_right reference to the source CANPkg_c on the right
    @return reference to the source CANPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  */
  virtual const CANPkg_c& operator=(const CANPkg_c& rrefc_right);

  /* ******************** */
  /* retreiving of values */
  /* ******************** */

  /**
    deliver type of base msg BABO
    @return BABO code of base msg (bit 4-7 in identifier)
  */
  uint8_t babo()const {return pb_internalData[BABO_ARR_POS];};
  /**
    deliver sender nr
    @return SEND code of base msg (bit 8-11 in identifier)
  */
  uint8_t send() const {return pb_internalData[SEND_ARR_POS];};

  /**
    deliver value of i16_val12
    @return data bytes 1 and 2 as integer value
  */
  int16_t val12() const {return i16_val12;};
  /**
    deliver value of i16_val34
    @return data bytes 3 and 4 as integer value
  */
  int16_t val34() const {return i16_val34;};
  /**
    deliver value of i16_val56
    @return data bytes 5 and 6 as integer value
  */
  int16_t val56() const {return i16_val56;};
  /**
    deliver value of i16_val78
    @return data bytes 7 and 8 as integer value
  */
  int16_t val78() const {return i16_val78;};

  /**
    deliver value of b_val1
    @return data uint8_t 1 as uint8_t value
  */
  uint8_t val1() const {return pb_val[0];};
  /**
    deliver value of b_val2
    @return data uint8_t 2 as uint8_t value
  */
  uint8_t val2() const {return pb_val[1];};
  /**
    deliver value of b_val3
    @return data uint8_t 3 as uint8_t value
  */
  uint8_t val3() const {return pb_val[2];};
  /**
    deliver value of b_val4
    @return data uint8_t 4 as uint8_t value
  */
  uint8_t val4() const {return pb_val[3];};
  /**
    deliver value of b_val5
    @return data uint8_t 5 as uint8_t value
  */
  uint8_t val5() const {return pb_val[4];};
  /**
    deliver value of b_val6
    @return data uint8_t 6 as uint8_t value
  */
  uint8_t val6() const {return pb_val[5];};
  /**
    deliver value of b_val7
    @return data uint8_t 7 as uint8_t value
  */
  uint8_t val7() const {return pb_val[6];};
  /**
    deliver value of b_val8
    @return data uint8_t 8 as uint8_t value
  */
  uint8_t val8() const {return pb_val[7];};

  #ifdef USE_ISO_11783
  /**
    deliver int32_t value between Byte 3 to 6
    @return int32_t value of uint8_t 3 to 6
  */
  int32_t val36(){return i32_val36;};
  #endif
  /* ***************** */
  /* setting of values */
  /* ***************** */

  /**
    set type of base msg BABO
    @param rb_val uint8_t value to write into BABO code of ident in base msg (bit 4-7)
  */
  void setBabo(uint8_t rb_val){pb_internalData[BABO_ARR_POS] = rb_val; };
  /**
    set sender nr
    @param rb_val uint8_t value to write into SEND code of ident in base msg (bit 8-11)
  */
  void setSend(uint8_t rb_val){pb_internalData[SEND_ARR_POS] = rb_val;
    setIdentType(Ident_c::StandardIdent);
    };

  /**
    set value of i16_val12
    @param ri16_val integer value to write into data bytes 1,2
  */
  void setVal12(int16_t ri16_val){i16_val12 = ri16_val;};
  /**
    set value of i16_val34
    @param ri16_val integer value to write into data bytes 3,4
  */
  void setVal34(int16_t ri16_val){i16_val34 = ri16_val;};
  /**
    set value of i16_val56
    @param ri16_val integer value to write into data bytes 5,6
  */
  void setVal56(int16_t ri16_val){i16_val56 = ri16_val;};
  /**
    set value of i16_val78
    @param ri16_val integer value to write into data bytes 7,8
  */
  void setVal78(int16_t ri16_val){i16_val78 = ri16_val;};

  /**
    set value of b_val1
    @param rb_val uint8_t value to write into data uint8_t 1
  */
  void setVal1(uint8_t rb_val){pb_val[0] = rb_val;};
  /**
    set value of b_val2
    @param rb_val uint8_t value to write into data uint8_t 2
  */
  void setVal2(uint8_t rb_val){pb_val[1] = rb_val;};
  /**
    set value of b_val3
    @param rb_val uint8_t value to write into data uint8_t 3
  */
  void setVal3(uint8_t rb_val){pb_val[2] = rb_val;};
  /**
    set value of b_val4
    @param rb_val uint8_t value to write into data uint8_t 4
  */
  void setVal4(uint8_t rb_val){pb_val[3] = rb_val;};
  /**
    set value of b_val5
    @param rb_val uint8_t value to write into data uint8_t 5
  */
  void setVal5(uint8_t rb_val){pb_val[4] = rb_val;};
  /**
    set value of b_val6
    @param rb_val uint8_t value to write into data uint8_t 6
  */
  void setVal6(uint8_t rb_val){pb_val[5] = rb_val;};
  /**
    set value of b_val7
    @param rb_val uint8_t value to write into data uint8_t 7
  */
  void setVal7(uint8_t rb_val){pb_val[6] = rb_val;};
  /**
    set value of b_val8
    @param rb_val uint8_t value to write into data uint8_t 8
  */
  void setVal8(uint8_t rb_val){pb_val[7] = rb_val;};
  #ifdef USE_ISO_11783
  /**
    set int32_t value between Byte 3 to 6
    @param ri32_val int32_t value to set
  */
  void setVal36(int32_t ri32_val){i32_val36 = ri32_val;};
  #endif
  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CANPkg_c or CANPkgExt
    @see CANPkg_c::operator=
    @see CANPkgExt_c::operator=
  */
  virtual void string2Flags();
private:
// Private methods

  /**
    overloaded virtual function to translate flag values to data string;
    needed for sending informations from this object via CANIO_c on CAN BUS,
    because CANIO_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
  */
  virtual void flags2String();

  #ifdef USE_ISO_11783
  /**
    translate ISO11783 extended ident string data to flags
  */
  void isoString2Flags();
  /**
    translate ISO11783 flag data to extended CAN string
  */
  void isoFlags2String();
  #endif

private:
// Private attributes
  #ifdef USE_ISO_11783
  /** int32_t value in Byte 3 to 6 */
  int32_t i32_val36;
  int16_t i16_val45;
  #endif

  /** uint8_t data string for BABO and SEND */
  uint8_t pb_internalData[2];

  /** integer values in Byte 1 and 2 */
  int16_t i16_val12;
  /** integer values in Byte 3 and 4 */
  int16_t i16_val34;
  /** integer values in Byte 5 and 6 */
  int16_t i16_val56;
  /** integer values in Byte 7 and 8 */
  int16_t i16_val78;

  /** array for uint8_t values */
  uint8_t pb_val[9];
};

}
#endif
