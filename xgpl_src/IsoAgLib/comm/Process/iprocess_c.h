/***************************************************************************
                          iprocess_c.h  - central managing instance for
                                           all process data informations in
                                           the system
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
#ifndef IPROCESS_H
#define IPROCESS_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/process_c.h"
#include "proc_c.h"


// Begin Namespace IsoAgLib::iProcess_c
namespace IsoAgLib {

/**
  Central managing instance for all process data
  informations in the system

  <b>Basic Rules for Matching of Received Messages)</b>
  1) primary match with LIS, GETY, ZAEHLNUM, WERT, INST
  2) distinguishing of more hits by POS
  \n
  <b>Acceptance Guidlines for Received Messages)</b>
  1) EMPF must fit to local member, SEND must be of existing member with claimed address
  2) special addition for Remote Process Data:
       alternatively SEND fit to owner of some Remote Data used to
       detect setpoints sent by owner of Remote Process Data
       -> possible to detect if local commander gets master or not
       (measure values are ignored by rule 2 unless MeasureProgRemote_c::receiveForeignMeasurement()
        was called for this ProcessDataRemote instance)
  \n
  The most functions of (i)Process_c are only relevant for the internal
  implementation, so that the interface to the API is quite small.
  Some of the internal tasks are:
  - handle list of pointers to all process data instances in the application
  - distribute the periodic timeEvent() call to all local process data instances,
    so that %e.g. value can be sent for measurement programs
  - forward incoming CAN messages to the appropriate process data to update values
    or to register measure progs, setpoints, etc.

  An overall description of Process Data management in IsoAgLib can be found in
  \ref ProcDataPage .


  @author Dipl.-Inform. Achim Spangler
*/
class iProcess_c : private __IsoAgLib::Process_c  {
public:
  /** initialisation for Process_c
  */
  void init( void ) { Process_c::init();};
  /**
    checks if a suitable iProcessDataLocal_c item exist
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_gety GETY code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> suitable instance found
  */
  bool existProcDataLocal(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert,
                             uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rb_pos = 0xFF, uint8_t rui8_pri = 2)
    {return iProcess_c::existProcDataLocal(rui8_lis, rui8_gety, rui8_wert,
                             rui8_inst, rui8_zaehlnum, rb_pos, rui8_pri);};
  /**
    checks if a suitable iProcessDataRemote_c item exist
    @param rui8_lis LIS code of searched remote Process Data instance
    @param rui8_gety GETY code of searched remote Process Data instance
    @param rui8_wert WERT code of searched remote Process Data instance
    @param rui8_inst INST code of searched remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched remote Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> suitable instance found
  */
  bool existProcDataRemote(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert,
                              uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rb_pos = 0xFF, uint8_t rui8_pri = 2)
    {return iProcess_c::existProcDataRemote(rui8_lis, rui8_gety, rui8_wert,
                             rui8_inst, rui8_zaehlnum, rb_pos, rui8_pri);};
  /**
    delivers count of local process data entries with similar ident
    (which differs only in POS of owner)
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_gety GETY code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return count of similar local process data entries
  */
  uint8_t procDataLocalCnt(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum, uint8_t rui8_pri = 2)
    {return Process_c::procDataLocalCnt(rui8_lis, rui8_gety, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri);};
  /**
    delivers count of remote process data entries with similar ident
    (which differs only in POS of owner)
    @param rui8_lis LIS code of searched remote Process Data instance
    @param rui8_gety GETY code of searched remote Process Data instance
    @param rui8_wert WERT code of searched remote Process Data instance
    @param rui8_inst INST code of searched remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched remote Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return count of similar remote process data entries
  */
  uint8_t procDataRemoteCnt(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                            uint8_t rui8_zaehlnum, uint8_t rui8_pri = 2)
    {return Process_c::procDataRemoteCnt(rui8_lis, rui8_gety, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri);};
 private:
  /** allow getIProcessInstance() access to shielded base class.
      otherwise __IsoAgLib::getProcessInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iProcess_c& getIProcessInstance( uint8_t rui8_instance );
  #else
  friend iProcess_c& getIProcessInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iProcess_c& getIProcessInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  inline iProcess_c& getIProcessInstance( void )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance());};
#endif

} // end of namespace IsoAgLib::iProcess_c
#endif
