/***************************************************************************
                          imeasure_prog_local.h - object for managing local
                                                 measure progs and values
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
#ifndef IMEASURE_PROG_LOCAL_H
#define IMEASURE_PROG_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/measureproglocal_c.h"
#include <IsoAgLib/util/igetypos_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  managing of local measure progs and values;
  sending values for running programs,
  answering value requests,
  starting and stopping measure programs by remote
  @see MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class iMeasureProgLocal_c : private __IsoAgLib::MeasureProgLocal_c {
private:
  // private typedef alias names
  typedef __IsoAgLib::Proc_c Proc_c;
public:
  /**
    deliver med val
    @return actual medium value
  */
  virtual int32_t med(bool rb_sendRequest = false){return MeasureProgLocal_c::med(rb_sendRequest);};


  /**
    deliver value
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val(){return MeasureProgLocal_c::val();};
  /**
    deliver integ val
    @return integral val for this measure prog
  */
  int32_t integ(){return MeasureProgLocal_c::integ();};
  /**
    deliver min val
    @return MIN val for this measure prog
  */
  int32_t min(){return MeasureProgLocal_c::min();};
  /**
    deliver max val
    @return MAX val for this measure prog
  */
  int32_t max(){return MeasureProgLocal_c::max();};
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  int32_t delta(){return MeasureProgLocal_c::delta();};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  int32_t accel(){return MeasureProgLocal_c::accel();};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver med val
    @return actual medium value
  */
  virtual float medFloat(bool rb_sendRequest = false)
    {return MeasureProgLocal_c::medFloat(rb_sendRequest);};


  /**
    deliver value
    @return measure val for this prog (can differ from master measure val)
  */
  float valFloat(){return MeasureProgLocal_c::valFloat();};
  /**
    deliver integ val
    @return integral val for this measure prog
  */
  float integFloat(){return MeasureProgLocal_c::integFloat();};
  /**
    deliver min val
    @return MIN val for this measure prog
  */
  float minFloat(){return MeasureProgLocal_c::minFloat();};
  /**
    deliver max val
    @return MAX val for this measure prog
  */
  float maxFloat(){return MeasureProgLocal_c::maxFloat();};
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  float deltaFloat(){return MeasureProgLocal_c::deltaFloat();};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  float accelFloat(){return MeasureProgLocal_c::accelFloat();};
#endif
  /**
    return the c_gtp code for this measureProg
    @return GETY_POS of this measureProg
  */
  iGetyPos_c gtp(){return MeasureProgLocal_c::gtp();};
  /**
    deliver the type of the active increment types
    @return actual Bit-OR combined increment types
  */
  Proc_c::type_t type(){return MeasureProgLocal_c::type();};
  /**
    check if the given increment type is active
    @param ren_type tested increment type
    @return true -> given increment type is set
  */
  bool checkType(Proc_c::type_t ren_type)
    {return MeasureProgLocal_c::checkType(ren_type);};
  /**
    return the program type of the item
    @return ProgType: Proc_c::UndefinedProg, Proc_c::Base, Proc_c::Target
  */
  uint8_t progType(){return MeasureProgLocal_c::progType();};
  /**
    check if given progType (base, target) is active
    @param ren_progType tested Prog-Type
    @return true -> given Prog-Type is set
  */
  bool checkProgType(Proc_c::progType_t ren_progType)
    {return MeasureProgLocal_c::checkProgType(ren_progType);};
  /**
    check if given send type is activated
    @param ren_doSend tested sended value
    @return true -> given value is sent on trigger
  */
  bool checkDoSend(Proc_c::doSend_t ren_doSend)
    {return MeasureProgLocal_c::checkDoSend(ren_doSend);};
 private:
  friend class iProcDataLocal_c;
  friend class iProcDataLocalSimpleMeasure_c;
  friend class iProcDataLocalSimpleSetpoint_c;
  friend class iProcDataLocalSimpleSetpointSimpleMeasure_c;
};

}
#endif
