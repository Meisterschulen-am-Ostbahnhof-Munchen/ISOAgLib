/***************************************************************************
                          measuresubprog_c.h - Every increment type of a 
                                              measure prog is managed by a
                                              MeasureSubprog_c instance
                             -------------------
    begin                : Fri Apr 07 2000
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
#ifndef MEASURE_SUBPROG_H
#define MEASURE_SUBPROG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include "../../impl/proc_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Every increment type of a measure prog is
  managed by a MeasureSubprog_c instance.
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureSubprog_c {
public:
  /**
    default constructor which can optionally set increment type, increment and running state
    @param ren_type optional increment type (default time proportional)
    @param ri32_increment optional increment value of ren_type (default 0)
    @param rb_started optional running state (default off)
    @param ri32_lastVal optional value of last trigger event (default 0)
  */
  MeasureSubprog_c(Proc_c::type_t ren_type = Proc_c::TimeProp, int32_t ri32_increment = 0, bool rb_started = false, int32_t ri32_lastVal = 0);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    default constructor which can optionally set increment type, increment and running state
    @param ren_type increment type
    @param rf_increment increment value of ren_type
    @param rb_started optional running state (default off)
    @param rf_lastVal optional value of last trigger event (default 0)
  */
  MeasureSubprog_c(Proc_c::type_t ren_type, float rf_increment, bool rb_started = false, float rf_lastVal = 0.0);
#endif
  /**
    operator= for Subprogs
    @param rrefc_src source instance
    @return reference to source instance for commands like "subp1 = subp2 = subp3;"
  */
  const MeasureSubprog_c& operator=(const MeasureSubprog_c& rrefc_src);
  /**
    copy constructor for Subprogs
    @param rrefc_src source instance
  */
   MeasureSubprog_c(const MeasureSubprog_c& rrefc_src);
  
  /** default destructor which has nothing to do */
  ~MeasureSubprog_c();

  /** 
    deliver subprog type
    @return type of this measure subprogram increment
  */
  Proc_c::type_t type(){return en_type;};

  /**
    set subprog type
    @param ren_type wanted increment type of this subprogram
  */
  void setType(Proc_c::type_t ren_type){en_type = ren_type;};
  /**
    deliver the increment value of this subprog
    @return increment value
  */
  int32_t increment() const {return i32_increment;};
  /**
    set increment value
    @param ri32_val wanted increment value
  */
  void setIncrement(int32_t ri32_val){i32_increment = ri32_val;};

  /**
    start a measuring subprogramm, potentially with increment and lastVal definition
    @param ri32_increment optional increment value (default old increment value used)
    @param ri32_lastVal last trigger value (default 0)
  */
  void start(int32_t ri32_increment = 0, int32_t ri32_lastVal = 0);
  /**
    delivers if given value forces trigger of send of registered informations
    @param ri32_val actual increment relevant value (time for TimeProp, distance for DistProp, ...)
    @return true -> this subprog triggers (e.g. send actual value)
  */
  bool updateTrigger(int32_t ri32_val);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the increment value of this subprog
    @return increment value
  */
  float incrementFloat() const {return f_increment;};
  /**
    set increment value
    @param ri32_val wanted increment value
  */
  void setIncrement(float rf_val){f_increment = rf_val;};

  /**
    start a measuring subprogramm, potentially with increment and lastVal definition
    @param rf_increment increment value
    @param rf_lastVal last trigger value (default 0)
  */
  void start(float rf_increment, float rf_lastVal = 0);
  /**
    delivers if given value forces trigger of send of registered informations
    @param rf_val actual increment relevant value (time for TimeProp, distance for DistProp, ...)
    @return true -> this subprog triggers (e.g. send actual value)
  */
  bool updateTrigger(float rf_val);
#endif
  /** stop a measuring subprogram  */
  void stop(){b_started = false;};

  /**
    compare two Subprogs with ==
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is equal to the other
  */
  bool operator==(const MeasureSubprog_c& rrefc_right)const;
  /**
    compare two Subprogs with !=
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is different to the other
  */
  bool operator!=(const MeasureSubprog_c& rrefc_right)const;
  /**
    compare two Subprogs with <
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is < than the other
  */
  bool operator<(const MeasureSubprog_c& rrefc_right)const;
  /**
    compare two Subprogs with <=
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is <= than the other
  */
  bool operator<=(const MeasureSubprog_c& rrefc_right)const;
  /**
    compare two Subprogs with >
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is > than the other
  */
  bool operator>(const MeasureSubprog_c& rrefc_right)const;
  /**
    compare two Subprogs with >=
    @param rrefc_right compared MeasureSubprog_c instance
    @return true -> this instance is >= than the other
  */
  bool operator>=(const MeasureSubprog_c& rrefc_right)const;

  /**
    compare two Subprogs with ==
    @param ren_type compared increment type
    @return true -> this instance is equal to the compared increment type
  */
  bool operator==(Proc_c::type_t ren_type)const;
  /**
    compare two Subprogs with !=
    @param ren_type compared increment type
    @return true -> this instance is different to the compared increment type
  */
  bool operator!=(Proc_c::type_t ren_type)const;
  /**
    compare two Subprogs with <
    @param ren_type compared increment type
    @return true -> this instance is < than the compared increment type
  */
  bool operator<(Proc_c::type_t ren_type)const;
  /**
    compare two Subprogs with <=
    @param ren_type compared increment type
    @return true -> this instance is <= than the compared increment type
  */
  bool operator<=(Proc_c::type_t ren_type)const;
  /**
    compare two Subprogs with >
    @param ren_type compared increment type
    @return true -> this instance is > than the compared increment type
  */
  bool operator>(Proc_c::type_t ren_type)const;
  /**
    compare two Subprogs with >=
    @param ren_type compared increment type
    @return true -> this instance is >= than the compared increment type
  */
  bool operator>=(Proc_c::type_t ren_type)const;


private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  /** last value for the increment test of this instance */
  union {
  int32_t i32_lastVal;
  float f_lastVal;
  };
  /** increment of this subprog item */
  union {
  int32_t i32_increment;
  float f_increment;
  };
#else
  /** last value for the increment test of this instance */
  int32_t i32_lastVal;
  /** increment of this subprog item */
  int32_t i32_increment;
#endif
  /** states if this subprog is started */
  bool b_started;
  /** type of this subprog instance 
    (the managed increment type)
  */
  Proc_c::type_t en_type;
private: // Private methods
  /**
    calculate a single identifying value for easier compare of Subprogs
    @return single value for comparison of Subprogs
  */
  int32_t calcCompVal()const;
};

}
#endif
