/***************************************************************************
                          measureprogBase.cc - base class for measure
                                                 values and programs
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measureprogbase_c.h"
#include "../../impl/process_c.h"
#include "../../processdatachangehandler_c.h"
#include <algorithm>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
	#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_MeasureProgBaseTotal = 0;
static uint16_t sui16_printedMeasureProgBaseTotal = 0;
#endif

namespace __IsoAgLib {
/**
  initialise the measure prog instance, to set this instance to a well defined starting condition
  @param rrefc_processData optional reference to containing ProcDataBase_c instance (default NULL)
  @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
  @param ri32_val optional individual measure val for this program instance (can differ from master measure value)
  @param rc_gtp optional GETY_POS of partner member for this measure program
*/
void MeasureProgBase_c::init( ProcDataBase_c *const rpc_processData,
  Proc_c::progType_t ren_progType, int32_t ri32_val,
  GetyPos_c rc_gtp)
{ // set the dynamic list to a well defined cleared starting condition
	#ifdef DEBUG_HEAP_USEAGE
  static bool b_doPrint = true;
  if ( b_doPrint )
  {
    b_doPrint = false;
  	getRs232Instance()
	  	<< "sizeof(MeasureSubprog_c) ==  " << sizeof(MeasureSubprog_c)
	  	<< " Bytes\r\n";
  }
  if ( sui16_MeasureProgBaseTotal > 0 )
  {
    sui16_MeasureProgBaseTotal -= ( vec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  }
  #endif

	vec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  set(rpc_processData);
  // store the parameter init vals
  c_gtp = rc_gtp;
  i32_val = ri32_val;
  en_progType = ren_progType;

  // set the rest of element vals to defined init
  en_accumProp = Proc_c::AccumNone;
  en_doSend = Proc_c::DoNone;
  en_type = Proc_c::DistProp;

  i32_accel = i32_delta = i32_integ = i32_lastTime = i32_max = i32_min = 0;
}

/**
  assignment of MeasureProgBase_c objects
  @param rrefc_src source MeasureProgBase_c instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const MeasureProgBase_c& MeasureProgBase_c::operator=(const MeasureProgBase_c& rrefc_src){
  // call base class operator
  ProcessElementBase_c::operator=(rrefc_src);

  assignFromSource( rrefc_src );

  // return reference to source
  return rrefc_src;
}

/**
  copy constructor
  @param rrefc_src source MeasureProgBase_c instance
*/
MeasureProgBase_c::MeasureProgBase_c(const MeasureProgBase_c& rrefc_src)
 : ProcessElementBase_c(rrefc_src)  {
  assignFromSource( rrefc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void MeasureProgBase_c::assignFromSource( const MeasureProgBase_c& rrefc_src )
{ // copy element vars
  c_gtp = rrefc_src.c_gtp;
  en_accumProp =  rrefc_src.en_accumProp;
  en_doSend = rrefc_src.en_doSend;
  en_progType = rrefc_src.en_progType;
  i32_accel = rrefc_src.i32_accel;
  i32_delta = rrefc_src.i32_delta;
  i32_integ = rrefc_src.i32_integ;
  i32_lastTime = rrefc_src.i32_lastTime;
  i32_max = rrefc_src.i32_max;
  i32_min = rrefc_src.i32_min;
  i32_val = rrefc_src.i32_val;
  vec_measureSubprog = rrefc_src.vec_measureSubprog;

  if (vec_measureSubprog.size() < rrefc_src.vec_measureSubprog.size())
  { // not all items copied
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_MeasureProgBaseTotal += ( vec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  
	  if ( vec_measureSubprog.size() > 0 )
    {
      getRs232Instance()
		    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
    }
  }
  #endif
}

/** default destructor which has nothing to do */
MeasureProgBase_c::~MeasureProgBase_c(){
	#ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgBaseTotal -= ( vec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
	#endif
}

/**
  add an aditional subprog or update if one with same kind exist already

  possible errors:
      * Err_c::badAlloc not enough memory to add new subprog
  @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
  @param ri32_increment increment value
  @return always true; only relevant for overoaded methods in derived classes
*/
bool MeasureProgBase_c::addSubprog(Proc_c::type_t ren_type, int32_t ri32_increment){
  if (ren_type == Proc_c::TimeProp) en_accumProp = Proc_c::AccumTime;
  else if (ren_type == Proc_c::DistProp) en_accumProp = Proc_c::AccumDist;

  // if subprog with this type exist, update only value
  Vec_MeasureSubprog::iterator pc_subprog = std::find(vec_measureSubprog.begin(), vec_measureSubprog.begin(), ren_type);
  if (pc_subprog != vec_measureSubprog.end())
  { // subprog with same type found -> update val
    pc_subprog->setIncrement(ri32_increment);
  }
  else
  { // no subprog with same type exist -> insert new one
    const uint8_t b_oldSize = vec_measureSubprog.size();
    vec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ri32_increment));
    if (b_oldSize >= vec_measureSubprog.size())
    { // array didn't grow
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_MeasureProgBaseTotal += ( 1 * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  
	    getRs232Instance()
		    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
    }
    #endif
  }
  return true;
}

/**
  LBS+ uses positive values even for time proportional measure prog
  -> only the start cmd choose increment type
  -> search for forced increment type and set first to according type if needed
*/
void MeasureProgBase_c::forceSubprogType(Proc_c::type_t ren_type)
{
  Vec_MeasureSubprog::iterator pc_subprog;
  for (pc_subprog = vec_measureSubprog.begin(); pc_subprog != vec_measureSubprog.end(); pc_subprog++)
  {
    if (pc_subprog->type() == ren_type) break;
  }
  if ((pc_subprog == vec_measureSubprog.end()) && (!vec_measureSubprog.empty()))
  { // set increment type of first item
    vec_measureSubprog.begin()->setType(ren_type);
  }
}


/**
  start a measuring programm
  @param ren_progType wanted msg type for measure prog (Proc_c::Base, Proc_c::Target)
  @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
  @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg)
  @return always true; only relevant for overoaded methods in derived classes
*/
bool MeasureProgBase_c::start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend){
  // register values
  en_progType = ren_progType; // base or target msg
  en_doSend = (ren_doSend != Proc_c::DoNone)?ren_doSend : en_doSend;
  if (en_doSend == Proc_c::DoNone) en_doSend = Proc_c::DoVal;
  forceSubprogType(ren_type);
  en_type = ren_type;
  return true;
}

/**
  stop all running subprog
  @return always true; only relevant for overoaded methods in derived classes
*/
bool MeasureProgBase_c::stop(){
  // clear the array with all subprogs -> no trigger test is done on value set
	#ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgBaseTotal -= ( vec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );

  if ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    getRs232Instance()
    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
  }
  #endif
  vec_measureSubprog.clear();
  en_doSend = Proc_c::DoNone;
  return true;
}

/**
  deliver actual last received value
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return measure val for this prog (can differ from master measure val)
*/
int32_t MeasureProgBase_c::val(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 0, 0);
  return i32_val;
};
/**
  deliver integ val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return integral val for this measure prog
*/
int32_t MeasureProgBase_c::integ(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 3, 0);
  return i32_integ;
};

/**
  deliver min val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return MIN val for this measure prog
*/
int32_t MeasureProgBase_c::min(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 1, 0);
  return i32_min;
};
/**
  deliver max val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return MAX val for this measure prog
*/
int32_t MeasureProgBase_c::max(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 2, 0);
  return i32_max;
};


/**
  init the element vars
  @param ri32_val initial measure val
*/
void MeasureProgBase_c::initVal(int32_t ri32_val){
	#ifdef DEBUG_HEAP_USEAGE
  if ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    getRs232Instance()
    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
  }
  #endif
  i32_val = i32_min = i32_max = i32_integ = ri32_val;
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  initialise the measure prog instance, to set this instance to a well defined starting condition
  @param rrefc_processData optional reference to containing ProcDataBase_c instance (default NULL)
  @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
  @param rf_val optional individual measure val for this program instance (can differ from master measure value)
  @param rc_gtp optional GETY_POS of partner member for this measure program
*/
void MeasureProgBase_c::init(
  ProcDataBase_c *const rpc_processData,
  Proc_c::progType_t ren_progType,
  float rf_val,
  GetyPos_c rc_gtp)
{ // set the dynamic list to a well defined cleared starting condition
	#ifdef DEBUG_HEAP_USEAGE
  static bool b_doPrint = true;
  if ( b_doPrint )
  {
    b_doPrint = false;
  	getRs232Instance()
	  	<< "sizeof(MeasureSubprog_c) ==  " << sizeof(MeasureSubprog_c)
	  	<< " Bytes\r\n";
  }

  sui16_MeasureProgBaseTotal -= ( vec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
	#endif

  vec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  set(rpc_processData);
  // store the parameter init vals
  c_gtp = rc_gtp;
  f_val = rf_val;
  en_progType = ren_progType;

	// set the rest of element vals to defined init
  en_accumProp = Proc_c::AccumNone;
  en_doSend = Proc_c::DoNone;
  en_type = Proc_c::DistProp;

  i32_accel = i32_delta = i32_integ = i32_lastTime = i32_max = i32_min = 0;
}
/**
  deliver actual last received value
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return measure val for this prog (can differ from master measure val)
*/
float MeasureProgBase_c::valFloat(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 0, 0);
  return f_val;
};
/**
  deliver integ val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return integral val for this measure prog
*/
float MeasureProgBase_c::integFloat(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 3, 0);
  return f_integ;
};

/**
  deliver min val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return MIN val for this measure prog
*/
float MeasureProgBase_c::minFloat(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 1, 0);
  return f_min;
};
/**
  deliver max val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return MAX val for this measure prog
*/
float MeasureProgBase_c::maxFloat(bool rb_sendRequest) const
{
  if(rb_sendRequest)processData().sendDataRawCmdGtp(2, gtp(), 3, 2, 0);
  return f_max;
};


/**
  init the element vars
  @param rf_val initial measure val
*/
void MeasureProgBase_c::initVal(float rf_val){
	#ifdef DEBUG_HEAP_USEAGE
  if ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    getRs232Instance()
    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
  }
  #endif
  f_val = f_min = f_max = f_integ = rf_val;
}
#endif


/**
  process a message;
  MeasureProgBase_c::processMsg is responsible for measure prog
  controlling commands
  @return true -> message was already edited complete
*/
bool MeasureProgBase_c::processMsg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  uint8_t b_mod = c_pkg.mod(),
      b_pd = c_pkg.pd();

	#ifdef DEBUG_HEAP_USEAGE
  if ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    getRs232Instance()
    << "MPBase T: " << sui16_MeasureProgBaseTotal << ", Node: " << ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) << "\r\n";
  }
  #endif
  bool b_edited = false;

  // check if PD==0 -> SET increment message
  if (b_pd == 0)
  { // mark that msg already edited
    b_edited = true;

    // programm controlling command
    if ((b_mod == 4) || (b_mod == 5))
    { // increment
      processIncrementMsg();
    }
    else if ( b_mod == 6 )
    { // program controlling with start, stop or reset
      uint8_t b_cmd = c_pkg.data(0);
      #ifdef LAV_COMMENT_MEASUREPROG_CMD
      if ((b_cmd & 0x80) > 0)
      #else
      if ( ((b_cmd & 0xF) > 0) && ((b_cmd & 0xF) < 7))
      #endif
      { // start command
        start(static_cast<Proc_c::progType_t>(c_pkg.pri()),
             static_cast<Proc_c::type_t>(b_cmd & 0x7),
             static_cast<Proc_c::doSend_t>((b_cmd >> 4) & 0x7));
      }
      else
      {
        #ifdef LAV_COMMENT_MEASUREPROG_CMD
        if ((b_cmd & 0x8) > 0)
        #else
        if ((b_cmd & 0xF) == 0x8)
        #endif
        { // reset command
          reset(b_cmd);
          // call handler function if handler class is registered
          if ( processData().getProcessDataChangeHandler() != NULL )
            processData().getProcessDataChangeHandler()->processMeasurementReset( pprocessData(), 0, c_pkg.memberSend().gtp() );
        }
        else
        { // stop command
          stop();
        }
      }
    }
  }
  return b_edited;
}

/**
  perform periodic actions
  @return true -> all planned activities performed in available time
*/
bool MeasureProgBase_c::timeEvent( void )
{return true;}

/**
  deliver to rb_mod according measure val type

  possible errors:
      * Err_c:range MOD is not in allowed range [0..6]
  @param rb_mod MOD of wanted subtype
  @return value of specified subtype
*/
int32_t MeasureProgBase_c::valMod(uint8_t rb_mod) const {
  int32_t i32_value = val();
  if (rb_mod < 5)
  { // correct range
    switch (rb_mod)
    {
      case 0:
        // set val with function, to calc delta and accel
        // i32_value = val();
        break;
      case 1:
        i32_value = min();
        break;
      case 2:
        i32_value = max();
        break;
      case 3:
        i32_value = integ();
        break;
      case 4:
        i32_value = med();
        break;
      case 5:
        i32_value = processData().internalUnit();
        break;
      case 6:
        i32_value = processData().sendConversion();
        break;
    }
  }
  else
  { // wrong range
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsProcess );
  }
  return i32_value;
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver to rb_mod according measure val type
  as float value

  possible errors:
      * Err_c:range MOD is not in allowed range [0..6]
  @param rb_mod MOD of wanted subtype
  @return value of specified subtype
*/
float MeasureProgBase_c::valModFloat(uint8_t rb_mod) const
{
  float f_value = valFloat();
  if (rb_mod < 5)
  { // correct range
    switch (rb_mod)
    {
      case 0:
        // set val with function, to calc delta and accel
        // i32_value = val();
        break;
      case 1:
        f_value = minFloat();
        break;
      case 2:
        f_value = maxFloat();
        break;
      case 3:
        f_value = integFloat();
        break;
      case 4:
        f_value = medFloat();
        break;
      case 5:
        f_value = processData().internalUnit();
        break;
      case 6:
        f_value = processData().sendConversion();
        break;
    }
  }
  else
  { // wrong range
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsProcess );
  }
  return f_value;
}
#endif


/**
  reset according to the process msg command the appropriate value type
  @param rb_comm command from Scheduler_c reset message
*/
void MeasureProgBase_c::reset(uint8_t rb_comm){
  if ((rb_comm & 0x8) > 0)
  {
    if ((rb_comm & 0xF0) > 0)
    {
      for (uint8_t b_mask = 1; b_mask <= 4; b_mask *= 2)
      {
        if (( (rb_comm & 0xF0) & (b_mask << 4) ) > 0)
        { // bit of test b_mask set in reset cmd
          switch (b_mask)
          {
            case 1: // value reset
              resetVal();
              break;
            case 2: // medium reset
              resetMed();
              break;
            case 4: // integer reset
              resetInteg();
              break;
          } // switch
        } // if
      } // for
    } // if > 0
    else
    { // all values reset
      resetMin();
      resetMax();
      resetMed();
      resetVal();
      resetInteg();
    }
  }
}

/**
  process a message with an increment for a measuring program

  possible errors:
      * Err_c::badAlloc not enough memory to add new subprog
*/
void MeasureProgBase_c::processIncrementMsg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

  // set c_gtp to caller of prog
  c_gtp = c_pkg.memberSend().gtp();

  if ((c_pkg.mod() == 4) || (c_pkg.mod() == 5))
  { // time or distance proportional
    // get the int32_t data val without conversion
    int32_t i32_val = c_pkg.dataRawCmdLong();

    if (i32_val < 0)
    { // time proportional
      addSubprog(Proc_c::TimeProp, CNAMESPACE::labs(i32_val));
    }
    else
    { // distance proportional
      addSubprog(Proc_c::DistProp, i32_val);
    }
  }
}

/**
  reset according to the MOD the appropriate value type

  possible errors:
      * Err_c:range MOD is not in allowed range [0..4]
  @param rb_mod MOD of wanted subtype
*/
void MeasureProgBase_c::resetValMod(uint8_t rb_mod){
  if (rb_mod < 5)
  { // correct range
    switch (rb_mod)
    {
      case 0:
        // set val with function, to calc delta and accel
        resetVal();
        break;
      case 1:
        resetMin();
        break;
      case 2:
        resetMax();
        break;
      case 3:
        resetInteg();
        break;
      case 4:
        resetMed();
        break;
    }
  }
  else
  { // wrong range
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsProcess );
  }
}

} // end of namespace __IsoAgLib
