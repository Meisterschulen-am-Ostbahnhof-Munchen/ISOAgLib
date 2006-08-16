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
#ifdef USE_ISO_11783
  #include "idevpropertyhandler_c.h"
#endif

// Begin Namespace IsoAgLib::iProcess_c
namespace IsoAgLib {

/**
  Central managing instance for all process data
  informations in the system

  <b>Basic Rules for Matching of Received Messages)</b>
  1) primary match with LIS, DEVCLASS, ZAEHLNUM, WERT, INST
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

#if defined(USE_ISO_11783) && defined(USE_PROC_DATA_DESCRIPTION_POOL)
  iDevPropertyHandler_c& getDevPropertyHandlerInstance( void )
  {return static_cast<iDevPropertyHandler_c&>(Process_c::getDevPropertyHandlerInstance());};
#endif

  /**
    if the amount of created local process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param rui16_localProcCapacity
  */
  void localProcDataReserveCnt( uint16_t rui16_localProcCapacity )
  { Process_c::localProcDataReserveCnt( rui16_localProcCapacity );};
  /**
    if the amount of created remote process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param rui16_remoteProcCapacity
  */
  void remoteProcDataReserveCnt( uint16_t rui16_remoteProcCapacity )
  { Process_c::remoteProcDataReserveCnt( rui16_remoteProcCapacity );};
  /**
    checks if a suitable iProcessDataLocal_c item exist
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClass DEVCLASS code of searched local Process Data instance
    @param rui8_devClassInst optional POS
    @return true -> suitable instance found
  */
  bool existProcDataLocal(
#ifdef USE_ISO_11783
                          uint16_t rui16_DDI,
                          uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                          uint8_t rui8_dataDevClass,
                          uint8_t rui8_devClassInst,
                          uint8_t rui8_lis,
                          uint8_t rui8_wert,
                          uint8_t rui8_inst,
                          uint8_t rui8_zaehlnum,
#endif
                          uint8_t rui8_devClassReceiver)
  { return Process_c::existProcDataLocal(
#ifdef USE_ISO_11783
                          rui16_DDI,
                          rui16_element,
#endif
#ifdef USE_DIN_9684
                          rui8_dataDevClass,
                          rui8_devClassInst,
                          rui8_lis,
                          rui8_wert,
                          rui8_inst,
                          rui8_zaehlnum,
#endif
                          rui8_devClassReceiver
  );}
  /**
    checks if a suitable iProcessDataRemote_c item exist
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClass DEVCLASS code of searched remote Process Data instance
    @param rui8_devClassInst optional POS
    @return true -> suitable instance found
  */
  bool existProcDataRemote(
#ifdef USE_ISO_11783
                           uint16_t rui16_DDI,
                           uint16_t rui16_element,
                           uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                           uint8_t rui8_dataDevClass,
                           uint8_t rui8_devClassInst,
                           uint8_t rui8_lis,
                           uint8_t rui8_wert,
                           uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum,
#endif
                           uint8_t rui8_devClassReceiver)
  { return Process_c::existProcDataRemote(
#ifdef USE_ISO_11783
                           rui16_DDI,
                           rui16_element,
                           rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                           rui8_dataDevClass,
                           rui8_devClassInst,
                           rui8_lis,
                           rui8_wert,
                           rui8_inst,
                           rui8_zaehlnum,
#endif
                           rui8_devClassReceiver
  );};

  /**
    delivers count of local process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClass DEVCLASS code of searched local Process Data instance
    @return count of similar local process data entries
  */
  uint8_t procDataLocalCnt(
#ifdef USE_ISO_11783
                           uint16_t rui16_DDI,
                           uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                           uint8_t rui8_lis,
                           uint8_t rui8_wert,
                           uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum,
#endif
                           uint8_t rui8_devClass)
    {return Process_c::procDataLocalCnt(
#ifdef USE_ISO_11783
                                        rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                        rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                        rui8_devClass);
    };

  /**
    delivers count of remote process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param rui16_DDI
    @param rui16_element
    @param rui8_devClassSender devClass of the sender (used for check against ownerDevKey().getDevClass())

    DIN parameter
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClass DEVCLASS code of searched local Process Data instance
    @return count of similar remote process data entries
  */
  uint8_t procDataRemoteCnt(
#ifdef USE_ISO_11783
                            uint16_t rui16_DDI,
                            uint16_t rui16_element,
                            uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                            uint8_t rui8_lis,
                            uint8_t rui8_wert,
                            uint8_t rui8_inst,
                            uint8_t rui8_zaehlnum,
#endif
                            uint8_t rui8_devClass)
    {return Process_c::procDataRemoteCnt(
#ifdef USE_ISO_11783
                                         rui16_DDI, rui16_element, rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                                         rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                         rui8_devClass);
    };

#ifdef USE_ISO_11783
  // addDDI2ExistingProcData only possible for remote process data project (iProcDataRemote_c has to be defined)
  /** checks if a DDI can be added to a group and return ptr to proc data if successfully */
#ifdef USE_PROC_REMOTE_STD
  iProcDataRemote_c* addDDI2ExistingProcData(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey, GeneralCommand_c::ValueGroup_t& ren_valueGroup, bool refb_isSetpoint)
  { return static_cast<iProcDataRemote_c*>(Process_c::addDDI2ExistingProcData(rui16_DDI, rui_deviceElement, rc_devKey, ren_valueGroup, refb_isSetpoint));};
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE
  iProcDataRemoteSimpleMeasure_c* addDDI2ExistingProcData(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey, GeneralCommand_c::ValueGroup_t& ren_valueGroup, bool refb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasure_c*>(Process_c::addDDI2ExistingProcData(rui16_DDI, rui_deviceElement, rc_devKey, ren_valueGroup, refb_isSetpoint));};
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_SETPOINT
  iProcDataRemoteSimpleSetpoint_c* addDDI2ExistingProcData(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey, GeneralCommand_c::ValueGroup_t& ren_valueGroup, bool refb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleSetpoint_c*>(Process_c::addDDI2ExistingProcData(rui16_DDI, rui_deviceElement, rc_devKey, ren_valueGroup, refb_isSetpoint));};
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT
  iProcDataRemoteSimpleMeasureSetpoint_c* addDDI2ExistingProcData(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey, GeneralCommand_c::ValueGroup_t& ren_valueGroup, bool refb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasureSetpoint_c*>(Process_c::addDDI2ExistingProcData(rui16_DDI, rui_deviceElement, rc_devKey, ren_valueGroup, refb_isSetpoint));};
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED
  iProcDataRemoteSimpleMeasureSetpointCombined_c* addDDI2ExistingProcData(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey, GeneralCommand_c::ValueGroup_t& ren_valueGroup, bool refb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasureSetpointCombined_c*>(Process_c::addDDI2ExistingProcData(rui16_DDI, rui_deviceElement, rc_devKey, ren_valueGroup, refb_isSetpoint));};
#endif

  /** checks if a DDI can be added to a group and if yes then add it! */
  bool checkAndAddMatchingDDI2Group(uint16_t rui16_DDI, uint16_t rui_deviceElement, const iDevKey_c& rc_devKey)
  { return Process_c::checkAndAddMatchingDDI2Group(rui16_DDI, rui_deviceElement, rc_devKey);};

  bool addProprietaryDDI2Group(uint16_t rui16_DDI, uint16_t rui_deviceElement, bool b_isSetpoint, GeneralCommand_c::ValueGroup_t ddiType, const iDevKey_c& rc_devKey)
  { return Process_c::addProprietaryDDI2Group(rui16_DDI, rui_deviceElement, b_isSetpoint, ddiType, rc_devKey);};
#endif


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
