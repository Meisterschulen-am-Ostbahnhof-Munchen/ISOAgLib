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
#include <IsoAgLib/comm/Part10_TaskController_Client/idevpropertyhandler_c.h>
#ifdef USE_PROC_REMOTE_STD
  #include "Remote/Std/iprocdataremote_c.h"
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_SETPOINT
  #include "Remote/SimpleSetpoint/iprocdataremotesimplesetpoint_c.h"
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE
  #include "Remote/SimpleMeasure/iprocdataremotesimplemeasure_c.h"
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT
  #include "Remote/SimpleMeasureSetpoint/iprocdataremotesimplesetpointsimplemeasure_c.h"
#endif
#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED
  #include "Remote/SimpleMeasureSetpoint/iprocdataremotesimplesetpointsimplemeasurecombined_c.h"
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

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  iDevPropertyHandler_c& getDevPropertyHandlerInstance( void )
  {return static_cast<iDevPropertyHandler_c&>(Process_c::getDevPropertyHandlerInstance());};
#endif

  /**
    if the amount of created local process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param aui16_localProcCapacity
  */
  void localProcDataReserveCnt( uint16_t aui16_localProcCapacity )
  { Process_c::localProcDataReserveCnt( aui16_localProcCapacity );}

  /**
    if the amount of created remote process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param aui16_remoteProcCapacity
  */
  void remoteProcDataReserveCnt( uint16_t aui16_remoteProcCapacity )
  { Process_c::remoteProcDataReserveCnt( aui16_remoteProcCapacity );}

  /**
    checks if a suitable iProcessDataLocal_c item exist
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param arcc_isoNameReceiver isoName of searched local Process Data instance
    @return true -> suitable instance found
  */
  bool existProcDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const iIsoName_c& arcc_isoNameReceiver)
  { return Process_c::existProcDataLocal(aui16_DDI, aui16_element, arcc_isoNameReceiver);}
  /**
    checks if a suitable iProcessDataRemote_c item exist
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param arcc_isoNameSender isoName of the sender (used for check against isoName())
    @param arcc_isoNameReceiver isoName code of searched local Process Data instance
    @return true -> suitable instance found
  */
  bool existProcDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                            const iIsoName_c& arcc_isoNameSender, const iIsoName_c& arcc_isoNameReceiver)
  { return Process_c::existProcDataRemote( aui16_DDI, aui16_element, arcc_isoNameSender, arcc_isoNameReceiver);}

  /**
    delivers count of local process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param arcc_isoName isoName code of searched local Process Data instance
    @return count of similar local process data entries
  */
  uint8_t procDataLocalCnt( uint16_t aui16_DDI, uint16_t aui16_element, const iIsoName_c& arcc_isoName)
  {return Process_c::procDataLocalCnt(aui16_DDI, aui16_element, arcc_isoName);}

  /**
    delivers count of remote process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param arcc_isoNameSender isoName of the sender (used for check against isoName())
    @param arcc_isoName isoName code of searched remote Process Data instance
    @return count of similar remote process data entries
  */
  uint8_t procDataRemoteCnt( uint16_t aui16_DDI,
                             uint16_t aui16_element,
                             const iIsoName_c& arcc_isoNameSender,
                             const iIsoName_c& arcc_isoName)
  { return Process_c::procDataRemoteCnt( aui16_DDI, aui16_element, arcc_isoNameSender, arcc_isoName); }

  // addDDI2ExistingProcData only possible for remote process data project (iProcDataRemote_c has to be defined)
  /** checks if a DDI can be added to a group and return ptr to proc data if successfully */
#ifdef USE_PROC_REMOTE_STD
  iProcDataRemote_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool rb_isSetpoint)
  { return static_cast<iProcDataRemote_c*>(Process_c::addDDI2ExistingProcData(aui16_DDI, aui_deviceElement, ac_isoName, ren_valueGroup, rb_isSetpoint));};
#endif

#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE
  iProcDataRemoteSimpleMeasure_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool rb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasure_c*>(Process_c::addDDI2ExistingProcData(aui16_DDI, aui_deviceElement, ac_isoName, ren_valueGroup, rb_isSetpoint));};
#endif

#ifdef USE_PROC_REMOTE_SIMPLE_SETPOINT
  iProcDataRemoteSimpleSetpoint_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool rb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleSetpoint_c*>(Process_c::addDDI2ExistingProcData(aui16_DDI, aui_deviceElement, ac_isoName, ren_valueGroup, rb_isSetpoint));};
#endif

#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT
  iProcDataRemoteSimpleMeasureSetpoint_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool rb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasureSetpoint_c*>(Process_c::addDDI2ExistingProcData(aui16_DDI, aui_deviceElement, ac_isoName, ren_valueGroup, rb_isSetpoint));};
#endif

#ifdef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED
  iProcDataRemoteSimpleMeasureSetpointCombined_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool rb_isSetpoint)
  { return static_cast<iProcDataRemoteSimpleMeasureSetpointCombined_c*>(Process_c::addDDI2ExistingProcData(aui16_DDI, aui_deviceElement, ac_isoName, ren_valueGroup, rb_isSetpoint));};
#endif

  /** checks if a DDI can be added to a group and if yes then add it! */
  bool checkAndAddMatchingDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName)
  { return Process_c::checkAndAddMatchingDDI2Group(aui16_DDI, aui_deviceElement, ac_isoName);}

  /** checks this DDI already exists in one ProcDataRemoteBase_c instance */
  bool check4DDIExisting(uint16_t aui16_DDI, uint16_t aui_deviceElement, const iIsoName_c& ac_isoName)
  { return Process_c::check4DDIExisting(aui16_DDI, aui_deviceElement, ac_isoName);}

  bool addProprietaryDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, bool b_isSetpoint, ProcessCmd_c::ValueGroup_t ddiType, const iIsoName_c& ac_isoName)
  { return Process_c::addProprietaryDDI2Group(aui16_DDI, aui_deviceElement, b_isSetpoint, ddiType, ac_isoName);}

  void registerWsmTaskMsgHandler(__IsoAgLib::ProcessWsmTaskMsgHandler_c* apc_processWsmTaskMsgHandler)
  { Process_c::registerWsmTaskMsgHandler(apc_processWsmTaskMsgHandler); }

  /** set the pointer to the handler class (used for callback when TC status message is processed)
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { Process_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); } ;

 private:
  /** allow getIProcessInstance() access to shielded base class.
      otherwise __IsoAgLib::getProcessInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iProcess_c& getIProcessInstance( uint8_t aui8_instance );
  #else
  friend iProcess_c& getIProcessInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iProcess_c& getIProcessInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  inline iProcess_c& getIProcessInstance( void )
  { return static_cast<iProcess_c&>(__IsoAgLib::getProcessInstance());};
#endif

} // end of namespace IsoAgLib::iProcess_c
#endif
