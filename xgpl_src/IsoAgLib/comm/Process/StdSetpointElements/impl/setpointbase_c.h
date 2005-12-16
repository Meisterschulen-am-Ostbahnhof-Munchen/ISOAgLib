/***************************************************************************
                          setpointbase_c.h - base class for management 
                                            of setpoints
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
#ifndef SETPOINT_BASE_H
#define SETPOINT_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Process/impl/processelementbase_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SetpointRegister_c;

/**
  base class for management of setpoints for storing, changing, reading the setpoint values;
  special actions are implemented by SetpointLocal_c or SetpointRemote
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointBase_c : public ProcessElementBase_c {
public: 
  /**
    default constructor which can set all member vars
    @param rpc_processData optional pointer to containing ProcessData instance
  */
  SetpointBase_c( ProcDataBase_c *const rpc_processData = NULL );
  /**
    default constructor which can set all member vars
    @param rrefc_processData optional reference to containing ProcessData instance
  */
  SetpointBase_c( ProcDataBase_c &rrefc_processData );
  /**
    assignement from another object
    @param rrefc_src source SetpointBase_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointBase_c& operator=( const SetpointBase_c& rrefc_src );
  /**
    copy constructor for SetpointBase
    @param rrefc_src source SetpointBase_c instance
  */
   SetpointBase_c( const SetpointBase_c& rrefc_src);
  /** default destructor which has nothing to do */
  virtual ~SetpointBase_c();
  /**
    init this item after the containing object item 
    was inserted in a list completely
    @param rpc_data pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c* pc_data = 0 );
  /**
    perform periodic actions
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );
  /**  process a setpoint message */
  void processMsg();
  /**  process a setpoint set command (pure virtual method)  */
  virtual void processSet() = 0;
  /** process a setpoint request command (pure virtual method) */
  virtual void processRequest() const = 0;

protected: // Protected methods
  /**
    send the values of an setpoint entry; if wanted 
    the values can be overridden with a special value
  
    possible errors:
        * dependant error in ProcDataBase_c commander of this setpoint isn't found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @param rrefc_src reference to SetpointRegister_c with registered setpoints
    @param rb_override true -> override registered setpoint with ri32_overrideVal
    @param ri32_overrideVal value which can override registered setpoint on rb_override == true
  */
  void sendSetpointVals( const SetpointRegister_c& rrefc_src,
                          bool rb_override = false, int32_t ri32_overrideVal = 0)const;
};

}
#endif
