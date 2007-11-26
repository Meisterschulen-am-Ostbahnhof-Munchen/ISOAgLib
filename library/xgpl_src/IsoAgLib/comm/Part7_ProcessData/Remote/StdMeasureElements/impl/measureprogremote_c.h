/***************************************************************************
                          measureprogremote_c.h - object for managing of
                                                  remote measure programs
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
#ifndef MEASUREPROG_REMOTE_H
#define MEASUREPROG_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../../StdMeasureElements/impl/measureprogbase_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataRemoteBase_c;

/**
  managing of remote measure programs -> starting of remote programs;
  requesting of remote values;
  storing values sent from remote
  @see MeasureProgLocal
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgRemote_c : public MeasureProgBase_c  {
public:
  /**
    constructor of MeasureProgRemote_c which can needed pointers to containing
    Scheduler_c and Process_c instances
    @param apc_processData optional pointer to Process_c
  */
  MeasureProgRemote_c( ProcDataBase_c *const apc_processData = NULL );
  /**
    initialise this MeasureProgRemote_c instance to well defined initial condition
    Scheduler_c and Process_c instances
    @param apc_processData optional pointer to Process_c
  */
  void init( ProcDataBase_c *const apc_processData );
  /**
    assignment of MeasureProgRemote_c objects
    @param arc_src source MeasureProgRemote_c instance
    @return reference to the source instance for commands like "prog1 = prog2 = prog3 ...;
  */
  const MeasureProgRemote_c& operator=(const MeasureProgRemote_c& arc_src);
  /**
    copy constructor for MeasureProgRemote
    @param arc_src source MeasureProgRemote_c instance
  */
   MeasureProgRemote_c(const MeasureProgRemote_c& arc_src);
  /** default destructor which has nothing to do */
  ~MeasureProgRemote_c();

  /**
    deliver actual measurement value as long
    @param ab_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterMeasurementVal(bool ab_sendRequest = false) { return val( ab_sendRequest );};

  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal() { resetVal();};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver actual measurement value as float
    @param ab_sendRequest true -> request for new value is sent (optional, default false)
  */
  float masterValFloat(bool ab_sendRequest = false) { return valFloat( ab_sendRequest );};
  #endif

  /**
    start a measuring programm

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO

    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  bool start(Proc_c::doSend_t ren_doSend);

  /**
    start a measuring programm

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO

    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  virtual bool start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend);
  /**
    send the stop command to the remote owner of data

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @param b_deleteSubProgs is only used for ISO
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
    @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  virtual bool stop(bool b_deleteSubProgs = true, Proc_c::type_t ren_type = Proc_c::NullType,
                    Proc_c::doSend_t ren_doSend = Proc_c::DoVal);


  /**
    deliver med val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return actual medium value
  */
  int32_t med(bool ab_sendRequest = false) const;
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver med val as float
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return actual medium value
  */
  float medFloat(bool ab_sendRequest = false) const;
#endif
  /**
    deliver time of last receive of new measurement value
    (val, min, max, integ or med)
    @return timestamp of last receive [msec.]
  */
  int32_t lastMeasurementReceive() const {return mi32_lastMeasureReceive;};

  /**
    process msg;
    @return true -> msg is already complete edited
  */
  virtual bool processMsg();
  /**
    set if this MeasureProgRemote_c instance should store
    target/partner process data messages not direct addressed
    to a local member (default store not)
    @param ab_useForeign wanted mode (default true)
  */
  void receiveForeignMeasurement(bool ab_useForeign = true);

  /**
    set according to values of the package the accoring value
    (uses function, wich converts if needed)
  */
  void setValFromPkg();


  /**
    set a new measure val
    @param ai32_val new val received from remote system
  */
  virtual void setVal(int32_t ai32_val);

  /**
    init element values
    @param ai32_val starting measuring value
  */
  virtual void initVal(int32_t ai32_val);
  /**
    send reset command for measure value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @param ai32_val reset measure value to this value (ISO only)
    @return true -> command successful sent
  */
  virtual bool resetVal(int32_t ai32_val = 0);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set a new measure val
    @param af_val new val received from remote system
  */
  virtual void setVal(float af_val);

  /**
    init element values
    @param af_val starting measuring value
  */
  virtual void initVal(float af_val);
#endif
  /**
    send reset command for medium value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @return true -> command successful sent
  */
  virtual bool resetMed();
  /**
    send reset command for integral value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @return true -> command successful sent
  */
  virtual bool resetInteg();
  /**
    send reset command for minimum value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @return true -> command successful sent
  */
  virtual bool resetMin();
  /**
    send reset command for maximum value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @return true -> command successful sent
  */
  virtual bool resetMax();

  /**
    perform periodic actions --> stop measuring prog if isoName isn't active any more
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );
private: // Private methods
  /**
    verify the stored ISOName code of the remote system
    and set Err_c::elNonexistent if this system isn't registered as ative;
    if isoName is undefied yet, retrieve it by the stored ident DEVCLASS of this process data

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
    @return true -> everything o.k.
  */
  bool verifySetRemoteISOName();

  /**
    set medium val
    @param ai32_val new medium value
  */
  void setMed(int32_t ai32_val){mi32_med = ai32_val;};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set medium val
    @param af_val new medium value
  */
  void setMed(float af_val){f_med = af_val;};
#endif
  /**
    deliver a reference to ProcDataRemote_c
    (the base function only delivers ProcDataBase_c)
    @return reference of containing ProcDataRemote_c
  */
  ProcDataRemoteBase_c& processData()
  {
    return *((ProcDataRemoteBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataRemote_c
    (the base function only delivers ProcDataBase_c)
    @return pointer to containing ProcDataRemote_c
  */
  ProcDataRemoteBase_c *const pprocessData()const
  {
    return ((ProcDataRemoteBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /** medium value of the remote process data */
    int32_t mi32_med;
    /** medium value of the remote process data */
    float f_med;
  };
#else
  /** medium value of the remote process data */
  int32_t mi32_med;
#endif
  /** time of last receive of measurement value update */
  int32_t mi32_lastMeasureReceive;
  /**
    control if measurement values addressed to other than a local
    member should be stored (default not)
  */
  bool mb_receiveForeignMeasurement;
};

}
#endif
