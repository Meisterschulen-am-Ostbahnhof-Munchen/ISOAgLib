/***************************************************************************
                          isoaglib_err.h - central object for error management

                             -------------------
    begin                : Thu Jan 20 2000
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * This library is free software; you Can redistribute it and/or modify it *
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

#ifndef _LIB_ERR_H
#define _LIB_ERR_H

#include "../typedef.h"
#include <IsoAgLib/util/impl/singleton.h>
#include "compiler_adoption.h"
#include <bitset>


#define ERR_FIELD_COUNTER_SIZE 2

// Begin Namespace IsoAgLib
namespace IsoAgLib {
  class LibErr_c;
  /** C-style function, to get access to the unique LibErr_c singleton instance */
  LibErr_c& getLbsErrInstance( void );

/**
  Basic object for Error Management:
  define all error codes as Bit-Values combined of common and specific error informations
  some functions for state check and setting of error state;
  All objects which Can hold their own error state are derived from Err
  *@author Dipl.-Inform. Achim Spangler
*/
class LibErr_c : public Singleton<LibErr_c> {
public:
  /** enum to identify the different error types */
  typedef enum {
    Precondition=0,
    BadAlloc=1,
    ElNonexistent=2,
    Unspecified=3,
    HwConfig=4,
    Range=5,
    Busy=6,
    SystemOpen=7,
    SystemWatchdog=8,
    CanOff=9,
    CanWarn=10,
    CanBus=11,
    CanMsgObj=12,
    CanMem=13,
    CanOverflow=14,
    CanConflict=15,
    CanDiffSpeed=16,
    Rs232Overflow=17,
    Rs232Underflow=18,
    EepromSegment=19,
    EepromWriteError=20,
    LbsSysNoActiveLocalMember=21,
    LbsBaseSenderConflict=22,
    IsoTerminalOutOfMemory=23,
    LbsMultiSendBusy=24,
    AllErrTypes=25,
  } LbsLibErrTypes_t;

  /** enum to identify the different error locations */
  typedef enum {
    Lbs=0,
    LbsBase=1,
    LbsSystem=2,
    LbsProcess=3,
    LbsTerminal=4,
    IsoTerminal=5,
    LbsMultipacket=6,
    HwSystem=7,
    Can=8,
    Eeprom=9,
    Sensor=10,
    Actor=11,
    Rs232=12,
    AllErrLocations=13,
  } LbsLibErrLocations_t;

  /**
    Initialize the unique error state handler
    @return true -> everything without errors initialised
  */
  bool init( void );
  /** register an error
    * @param rt_errType type of occured error
    * @param rt_errLocation location of error
    */
  void registerError( LbsLibErrTypes_t rt_errType, LbsLibErrLocations_t rt_errLocation );
  
  /**
    clear all error information
  */
  void clear( void ) { clear( AllErrTypes, AllErrLocations ); };
  /**
    clear specific error information of a special location
    @param rt_errLocation select for which error locations the counter shall be reset
    @param rt_errType select for which error types the counter shall be reset (default reset all counters)
  */
  void clear( LbsLibErrTypes_t rt_errType, LbsLibErrLocations_t rt_errLocation );
  /**
    clear all error information of a special type
    @param rt_errType select for which error types the counter shall be reset
  */
  void clear( LbsLibErrTypes_t rt_errType );
  /**
    clear all error information of a special location
    @param rt_errLocation select for which error locations the counter shall be reset
  */
  void clear( LbsLibErrLocations_t rt_errLocation );

//  /** deliver the timestamp of last error occurence */
//  uint32_t getLastErrorTime( void ) const { return ui32_lastErrorTime;};

  /**
    deliver the count of registered errors
    @param rt_errType optional select the error-types to count (default all error types)
    @return count of registered error types
  */
  uint16_t getErrCnt( LbsLibErrTypes_t rt_errType = AllErrTypes ) const;
  /**
    deliver the count of registered errors with a special error location
    @param rt_errLocation location to check for errors
    @return count of registered error types
  */
  uint16_t getErrCnt( LbsLibErrLocations_t rt_errLocation ) const;

	/**
	  deliver the type of the nth error
	  @param rui8_ind index of the requested error ( [0..(ErrCnt-1)] )
	  @return LbsLibErrTypes_t
	*/
	LbsLibErrTypes_t getErrIndType( uint8_t rui8_ind ) const;
	/**
	  deliver the location of the nth error
	  @param rui8_ind index of the requested error ( [0..(ErrCnt-1)] )
	  @return LbsLibErrTypes_t
	*/
	LbsLibErrLocations_t getErrIndLocation( uint8_t rui8_ind ) const;

  /**
    check if the error value reports correct error free state
    @param rt_errType optional select the error-types to count (default all error types)
    @return true -> the defined error locations report no error
  */
  bool good( LbsLibErrTypes_t rt_errType = AllErrTypes ) const;

  /**
    check if the error value reports correct error free state
    @param rt_errLocation location to check for errors
    @return true -> the defined error locations report no error
  */
  bool good( LbsLibErrLocations_t rt_errLocation ) const;

  /**
    check if the object has a local error
  */
  inline bool operator!(){ return (good(AllErrTypes))?false:true;};

  /** default destructor which has nothing to do */
  virtual ~LibErr_c();
  
private:
  friend class Singleton< LibErr_c >;

  /** default constructor which sets the error value to noErr */
  LibErr_c();
  /** copy constructor which sets the error value to the err value of the source */
  LibErr_c(const LibErr_c& rrefc_src);

  std::bitset<AllErrTypes> errTypeAtLoc [AllErrLocations];
  
/*
  struct errTypeBitfield {
    unsigned int Precondition : ERR_FIELD_COUNTER_SIZE;
    unsigned int BadAlloc : ERR_FIELD_COUNTER_SIZE;
    unsigned int ElNonexistent : ERR_FIELD_COUNTER_SIZE;
    unsigned int Unspecified : ERR_FIELD_COUNTER_SIZE;
    unsigned int HwConfig : ERR_FIELD_COUNTER_SIZE;
    unsigned int Range : ERR_FIELD_COUNTER_SIZE;
    unsigned int Busy : ERR_FIELD_COUNTER_SIZE;
    unsigned int SystemOpen : ERR_FIELD_COUNTER_SIZE;
    unsigned int SystemWatchdog : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanOff : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanWarn : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanBus : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanMsgObj : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanMem : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanOverflow : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanConflict : ERR_FIELD_COUNTER_SIZE;
    unsigned int CanDiffSpeed : ERR_FIELD_COUNTER_SIZE;
    unsigned int Rs232Overflow : ERR_FIELD_COUNTER_SIZE;
    unsigned int Rs232Underflow : ERR_FIELD_COUNTER_SIZE;
    unsigned int EepromSegment : ERR_FIELD_COUNTER_SIZE;
    unsigned int EepromWriteError : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsSysNoActiveLocalMember : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsBaseSenderConflict : ERR_FIELD_COUNTER_SIZE;
    unsigned int IsoTerminalOutOfMemory : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsMultiSendBusy : ERR_FIELD_COUNTER_SIZE;
  } errTypeBitfield;
  struct errLocBitfield {
    unsigned int Lbs : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsBase : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsSystem : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsProcess : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsTerminal : ERR_FIELD_COUNTER_SIZE;
    unsigned int IsoTerminal : ERR_FIELD_COUNTER_SIZE;
    unsigned int LbsMultipacket : ERR_FIELD_COUNTER_SIZE;
    unsigned int HwSystem : ERR_FIELD_COUNTER_SIZE;
    unsigned int Can : ERR_FIELD_COUNTER_SIZE;
    unsigned int Eeprom : ERR_FIELD_COUNTER_SIZE;
    unsigned int Sensor : ERR_FIELD_COUNTER_SIZE;
    unsigned int Actor : ERR_FIELD_COUNTER_SIZE;
    unsigned int Rs232 : ERR_FIELD_COUNTER_SIZE;
  } errLocBitfield;
*/

///// No more Last Error Time, as long as not requested by anyone...
//  uint32_t ui32_lastErrorTime;
};

} // end of namespace IsoAgLib

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
  /** C-style function, to get access to the unique LibErr_c singleton instance */
  IsoAgLib::LibErr_c& getLbsErrInstance( void );
  using IsoAgLib::LibErr_c;
}
#endif
