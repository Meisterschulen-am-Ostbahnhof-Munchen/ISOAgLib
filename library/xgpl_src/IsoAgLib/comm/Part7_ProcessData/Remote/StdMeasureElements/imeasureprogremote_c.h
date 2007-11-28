/***************************************************************************
                          imeasure_prog_remote.h - object for managing of
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
#ifndef IMEASURE_PROG_REMOTE_H
#define IMEASURE_PROG_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/measureprogremote_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  managing of remote measure programs -> starting of remote programs;
  requesting of remote values;
  storing values sent from remote
  @see MeasureProgLocal
  @author Dipl.-Inform. Achim Spangler
*/
class iMeasureProgRemote_c : private __IsoAgLib::MeasureProgRemote_c {
private:
  // private typedef alias names
  typedef __IsoAgLib::Proc_c Proc_c;
public:
  /**
    start a measuring programm

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO

    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  bool start(Proc_c::doSend_t ren_doSend)
        {return MeasureProgRemote_c::start(ren_doSend);};

  /**
    start a measuring programm

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO

    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  bool start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend)
        {return MeasureProgRemote_c::start(ren_type, ren_doSend);};

  /**
    set if this MeasureProgRemote_c instance should store
    target/partner process data messages not direct addressed
    to a local member (default store not)
    @param ab_useForeign wanted mode (default true)
  */
  void receiveForeignMeasurement(bool ab_useForegin = true)
    {MeasureProgRemote_c::receiveForeignMeasurement(ab_useForegin);};

  /**
    check if this measure prog is running
    @return true -> program is running
  */
  bool started(){return MeasureProgRemote_c::started();};

  /**
    send the stop command to the remote owner of data

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @param b_deleteSubProgs is only used for ISO
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
    @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> command successful sent
  */
  bool stop(bool b_deleteSubProgs = true, Proc_c::type_t ren_type = Proc_c::NullType, Proc_c::doSend_t ren_doSend = Proc_c::DoVal)
       {return MeasureProgRemote_c::stop(b_deleteSubProgs, ren_type, ren_doSend);};
  /**
    deliver time of last receive of new measurement value
    (val, min, max, integ or med)
    @return timestamp of last receive [msec.]
  */
  int32_t lastMeasurementReceive() const {return MeasureProgRemote_c::lastMeasurementReceive();};

  /**
    send reset command for measure value

    possible errors:
        * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
        * dependant error in CAN_IO
    @param ai32_val reset measure value to this value (ISO only)
    @return true -> command successful sent
  */
  bool resetVal(int32_t ai32_val = 0) {return MeasureProgRemote_c::resetVal(ai32_val);};

  /**
    add an aditional subprog or update if one with same kind exist already

    possible errors:
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment increment value
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overoaded methods in derived classes
  */
  bool addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend = Proc_c::DoVal)
      {return MeasureProgRemote_c::addSubprog(ren_type, ai32_increment, ren_doSend);};

  /**
    deliver value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val(bool ab_sendRequest = false){return MeasureProgRemote_c::val(ab_sendRequest);};
  /**
    deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  int32_t min(bool ab_sendRequest = false){return MeasureProgRemote_c::min(ab_sendRequest);};
  /**
    deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  int32_t max(bool ab_sendRequest = false){return MeasureProgRemote_c::max(ab_sendRequest);};
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  int32_t delta(){return MeasureProgRemote_c::delta();};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  int32_t accel(){return MeasureProgRemote_c::accel();};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  float valFloat(bool ab_sendRequest = false){return MeasureProgRemote_c::valFloat(ab_sendRequest);};
  /**
    deliver integ val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
  float integFloat(bool ab_sendRequest = false){return MeasureProgRemote_c::integFloat(ab_sendRequest);};
  /**
    deliver med val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return actual medium value
  */
  float medFloat(bool ab_sendRequest = false) const {return MeasureProgRemote_c::medFloat(ab_sendRequest);};
  /**
    deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  float minFloat(bool ab_sendRequest = false){return MeasureProgRemote_c::minFloat(ab_sendRequest);};
  /**
    deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  float maxFloat(bool ab_sendRequest = false){return MeasureProgRemote_c::maxFloat(ab_sendRequest);};
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  float deltaFloat(){return MeasureProgRemote_c::deltaFloat();};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  float accelFloat(){return MeasureProgRemote_c::accelFloat();};
#endif
  /**
    return the c_isoName code for this measureProg
    @return ISOName of this measureProg
  */
  const iIsoName_c& isoName(){return MeasureProgRemote_c::isoName().toConstIisoName_c();};
  /**
    deliver the type of the active increment types
    @return actual Bit-OR combined increment types
  */
  Proc_c::type_t type(){return MeasureProgRemote_c::type();};
  /**
    check if the given increment type is active
    @param ren_type tested increment type
    @return true -> given increment type is set
  */
  bool checkType(Proc_c::type_t ren_type)
    {return MeasureProgRemote_c::checkType(ren_type);};
  /**
    check if given send type is activated
    @param ren_doSend tested sended value
    @return true -> given value is sent on trigger
  */
  bool checkDoSend(Proc_c::doSend_t ren_doSend)
    {return MeasureProgRemote_c::checkDoSend(ren_doSend);};

  /**
    set the type of the active increment types
    @param ren_type Bit-OR combined increment type(s)
  */
  void setType(Proc_c::type_t ren_type) {MeasureProgRemote_c::setType(ren_type);};
  /**
    set the c_isoName code for this measureProg
    @param ac_isoName ISOName for exact specification of partner system
  */
  void setISOName(const iIsoName_c& ac_isoName) {MeasureProgRemote_c::setISOName(ac_isoName);};
 private:
  friend class iProcDataRemote_c;
  friend class iProcDataRemoteSimpleMeasure_c;
  friend class iProcDataRemoteSimpleSetpoint_c;
  friend class iProcDataRemoteSimpleSetpointSimpleMeasure_c;
};

}
#endif
