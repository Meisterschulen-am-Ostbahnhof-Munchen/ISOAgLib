/***************************************************************************
                          processelementbase_c.h - base class for elements
                                                   of ProcessData
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
#ifndef PROCESS_ELEMENT_BASE_H
#define PROCESS_ELEMENT_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "procdatabase_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataBase_c;

/**
  base class for elements of ProcessData which has pointer to
  the containing ProcessData item, inherit pointer to Scheduler_c from Scheduler_Task_c
  @author Dipl.-Inform. Achim Spangler
*/
class ProcessElementBase_c : public ClientBase  {
private:
public: 
  /**
    constructor which initialse both pointers if given
    @param apc_processData optional pointer to containing ProcessData instance
  */
  ProcessElementBase_c( ProcDataBase_c *const apc_processData = NULL );
  /**
    constructor which initialse both pointers if given
    @param arc_processData optional reference to containing ProcessData instance
  */
  ProcessElementBase_c( ProcDataBase_c &arc_processData );
  /**
    copy constructor
    @param arcc_src source ProcessElementBase_c instance
  */
   ProcessElementBase_c(const ProcessElementBase_c& arcc_src);
  /**
    assignment operator for ProcessElementBase
    @param arcc_src source ProcessElementBase_c instance
  */
  const ProcessElementBase_c& operator=(const ProcessElementBase_c& arcc_src);
  /** default destructor which has nothing to do */
  ~ProcessElementBase_c();
  
  /**
    set the pointer to Scheduler_c and ProcessData by references to the object instances
    @param rc_processData optional reference to containing ProcessData instance
  */
  void set(ProcDataBase_c& rc_processData );
  /**
    set the pointer to Scheduler_c and ProcessData by pointer to the object instances
    @param apc_processData optional pointer to containing ProcessData instance
  */
  void set(ProcDataBase_c *const apc_processData );
  
  /**
    deliver a reference to the containing ProcessData item
    @return reference to containing ProcessData
  */
  ProcDataBase_c& processData()const{return *mpc_processData;};
  /**
    deliver the pointer to the containing ProcessData item
    @return pointer to containing ProcessData
  */
  ProcDataBase_c *const pprocessData()const{return mpc_processData;};
  /**
    deliver a reference to the containing ProcessData item
    @return reference to containing ProcessData
  */
  const ProcDataBase_c& processDataConst()const {return *mpc_processData;};
  /**
    deliver the pointer to the containing ProcessData item
    @return pointer to containing ProcessData
  */
  const ProcDataBase_c *const pprocessDataConst()const{return mpc_processData;};
  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  const proc_valType_t valType()const
  {return mpc_processData->valType();};
  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType)
    {mpc_processData->setValType(ren_procValType);};
  
private: // Private attributes
  /** pointer to the containing ProcessData item */
  ProcDataBase_c* mpc_processData;
};

}
#endif
