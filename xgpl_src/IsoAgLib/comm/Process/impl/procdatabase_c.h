/***************************************************************************
                          procdatabase_c.h  - base class for local
                                                     or remote process data
                                                     object
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
#ifndef PROCDATA_BASE_H
#define PROCDATA_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/singleton.h>
#ifdef USE_ISO_11783
  #include <IsoAgLib/comm/SystemMgmt/impl/istate_c.h>
#endif
#include "procident_c.h"
#include "processpkg_c.h"

namespace IsoAgLib {
  class  ProcessDataChangeHandler_c;
  class EventSource_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
/**
  base class for local or remote process data object;
  manages identity of a process data object -> comparing identity;
  has standard functions in mostly abstract way for derived classes
  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataBase_c : public ProcIdent_c, public ClientBase {
protected:
public:
  /** allow explicit SetpointBase_c the access to private elements */
  friend class SetpointBase_c;
  /** allow explicit MeasureProgBase_c the access to private elements */
  friend class MeasureProgBase_c;
  /** allow explicit MeasureProgRemote_c the access to private elements */
  friend class MeasureProgRemote_c;
  /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;
  friend class SimpleManageSetpointRemote_c;
  friend class SimpleManageMeasureProgRemote_c;
  friend class SimpleManageSetpointLocal_c;
  friend class SetpointLocal_c;
  friend class SetpointRemote_c;

  /**
    constructor which can set all element vars
    @param rpc_lbs optional pointer to central Scheduler_c instance
    @param rui8_lis optional LIS code of this instance
    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_gtp pointer to updated GETY_POS variable of owner
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataBase_c( uint8_t rui8_lis = 0, GetyPos_c rc_gtp = GetyPos_c(0, 0xF),
                        uint8_t rui8_wert = 0, uint8_t rui8_inst = 0, uint8_t rui8_zaehlnum = 0xFF,
                        uint8_t rui8_pri = 2, GetyPos_c rc_ownerGtp = GetyPos_c(0xF, 0xF), GetyPos_c *rpc_gtp = NULL,
                        ::IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                        int ri_singletonVecKey = 0)
  : ProcIdent_c(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_gtp),
    ClientBase(ri_singletonVecKey) {init(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_gtp,
                                         rpc_processDataChangeHandler, ri_singletonVecKey);};
  /**
    initialise this ProcDataBase_c instance to a well defined initial state
    @param rpc_lbs optional pointer to central Scheduler_c instance
    @param rui8_lis optional LIS code of this instance
    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_gtp pointer to updated GETY_POS variable of owner
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
  void init( uint8_t rui8_lis = 0, GetyPos_c rc_gtp = GetyPos_c(0, 0xF),
                        uint8_t rui8_wert = 0, uint8_t rui8_inst = 0, uint8_t rui8_zaehlnum = 0xFF,
                        uint8_t rui8_pri = 2, GetyPos_c rc_ownerGtp = GetyPos_c(0xF, 0xF), GetyPos_c *rpc_gtp = NULL,
                        IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                        int ri_singletonVecKey = 0);
  /**
    assignment operator for this base object
    @param rrefc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataBase_c& operator=(const ProcDataBase_c& rrefc_src);
  /**
    copy constructor for ProcDataBase_c
    @param rrefc_src source instance
  */
  ProcDataBase_c(const ProcDataBase_c& rrefc_src);
  /** default destructor which has nothing to do */
  virtual ~ProcDataBase_c();

  /** set the poitner to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { pc_processDataChangeHandler = rpc_processDataChangeHandler; } ;
  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return pc_processDataChangeHandler; } ;

  /**
    deliver the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @return internal unit
  */
  uint16_t internalUnit()const{return ui16_internalUnit;};
  /**
    set the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @param rui16_internalUnit new vaue for internal unit
  */
  void setInternalUnit(uint16_t rui16_internalUnit){ui16_internalUnit = rui16_internalUnit;};

  /**
    deliver the send conversion, which is used to send measuring values
    @return conversion value
  */
  const int32_t& sendConversion()const{return i32_sendConversion;};
  /**
    deliver the send conversion, which is used to send measuring values
    @return conversion value
  */
  void setSendConversion(int32_t ri32_sendConversion){i32_sendConversion = ri32_sendConversion;};

  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  const proc_valType_t valType()const{return en_procValType;};
  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType){en_procValType = ren_procValType;};

  /**
    deliver the pkg data value as int32_t;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has float val 4.2 the result of this
    function is 4
    @return data value as int32_t value (converted if needed)
  */
  int32_t pkgDataLong()const;
#ifdef USE_ISO_11783
  /**
    deliver the pkg data value as uint32_t;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has float val 4.2 the result of this
    function is 4
    @return data value as uint32_t value (converted if needed)
  */
  uint32_t pkgDataUlong()const;
#endif
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the pkg data value as float;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has int32_t 4 the result of this function
    is float 4.0
    @return data value as float value (converted if needed)
  */
  float pkgDataFloat()const;
#endif

  /**
    process a message, which is adressed for this process data item;
    ProcDataBase_c::processMsg() is responsible to delegate the
    processing of setpoint and measurement messages to the appripriate
    functions processSetpoint and processProg;
    both functions are virtual, so that depending on loacl or remote
    process data the suitable reaction can be implemented
  */
  void processMsg();

  /**
    perform periodic acoins
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );

protected: // Protected methods
  /**
    send the given int32_t command value with variable GETY_POS rc_varGtp;
    set the cmd value without conversion in message string and set data
    format flags corresponding to central data type of this process data
    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems
    @param rui8_pri PRI code for the msg
    @param rc_varGtp variable GETY_POS
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @param ri32_cmdVal int32_t command value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  bool sendDataRawCmdGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, int32_t ri32_val) const;
  /**
    send the given int32_t value with variable GETY_POS rc_varGtp;
    set the int32_t value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data

    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems
    @param rui8_pri PRI code for the msg
    @param rc_varGtp variable GETY_POS
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @param ri32_val int32_t value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  bool sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, int32_t ri32_val = 0) const;
#ifdef USE_ISO_11783
  /**
    send the given uint32_t value with variable GETY_POS rc_varGtp;
    set the uint32_t value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data

    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems
    @param rui8_pri PRI code for the msg
    @param rc_varGtp variable GETY_POS
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @param ri32_val uint32_t value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  bool sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, uint32_t rulVal = 0) const;
#endif
#ifdef USE_FLOAT_DATA_TYPE
  /**
    send the given float value with variable GETY_POS rc_varGtp;
    set the float value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data

    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems
    @param rui8_pri PRI code for the msg
    @param rc_varGtp variable GETY_POS
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @param ri32_val float value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  bool sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, float rf_val = 0.0F) const;
#endif

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataBase_c& rrefc_src );
  /** helper function to get reference to process data pkg very quick */
  ProcessPkg_c& getProcessPkg( void ) const;
  /**
    resolv SEND|EMPF dependent on GETY_POS rc_varGtp
    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)
    and set basic value independent flags in ProcessPkg

    the PRI code of the message is set dependent of the ProcIdent_c PRI
    code for this process data instance, if no basic proc data (rui8_pri == 1)
    is wanted

    @param rui8_pri PRI code for the msg
    @param rc_varGtp variable GETY_POS
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @return true -> resolvSendGtp successfully resolved EMPF and SEND
  */
  bool resolvGtpSetBasicSendFlags(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod) const;
  /**
    virtual function which check dependent on remote/local
    if send action with given var parameter and address claim state of owner is
    allowed and resolves the appropriate numbers for sender and receiver (empf)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
    @param rui8_pri PRI code of message
    @param rb_var variable number (local: empf; remote: send)
    @param b_empf refernce to EMPF variable which is update dependent on type local/remote
    @param b_send refernce to SEND variable which is update dependent on type local/remote
    @param en_msgProto protocol type to use for the message
        IState_c::Din or IState_c::Iso (only compiled and used if USE_ISO_11783 is
        configured) (default: IState_c::Din)
    @return true -> owner of process data registered as active in Monitor-List
  */
  virtual bool var2empfSend(uint8_t rui8_pri, uint8_t rb_var, uint8_t &b_empf, uint8_t &b_send
  #ifdef USE_ISO_11783
    , IState_c::itemState_t &en_msgProto
  #endif
    ) const = 0;

  /**
    process a measure prog message
    -> fully dependent on children type local/remote
  */
  virtual void processProg();
  /**
    virtual base for processing of a setpoint message
  */
  virtual void processSetpoint();

private: // Private attributes
  friend class IsoAgLib::EventSource_c;
  /** central data type to use for messages: i32_val, ui32_val, float_val */
  proc_valType_t en_procValType;
  /** code of the internal used unit for the process data value */
  uint16_t ui16_internalUnit;
  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* pc_processDataChangeHandler;
  /** conversion factor for sending values */
  int32_t i32_sendConversion;
};

}
#endif
